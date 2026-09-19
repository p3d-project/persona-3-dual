#!/usr/bin/env python3
"""Audio -> QOA conversion: ffmpeg (resample, low-pass, attenuate) -> vendored qoaconv.

Settings come from (lowest to highest priority): defaults, <name>.build.json, CLI flags.
Keys: frequency, channels, lowpass_hz (None = auto 40% of frequency, 0 = off), gain_db.
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

MAX_SAMPLERATE = 16777215  # QOA stores the sample rate in 24 bits
DEFAULTS = {"frequency": 32000, "channels": 1, "lowpass_hz": None, "gain_db": -3.0}


def load_config(input_path: str) -> dict:
    base = Path(input_path)
    for config_path in [
        base.with_suffix(".build.json"),
        base.parent.parent / f"{base.stem}.build.json",
    ]:
        if config_path.exists():
            with config_path.open("r", encoding="utf-8") as fh:
                data = json.load(fh)
            return data if isinstance(data, dict) else {}
    return {}


def normalize_config(config: dict) -> dict:
    cfg = dict(DEFAULTS)
    cfg.update({k: v for k, v in (config or {}).items() if v is not None})
    cfg["frequency"] = int(cfg["frequency"])
    cfg["channels"] = int(cfg["channels"])
    cfg["gain_db"] = float(cfg["gain_db"])
    cfg["lowpass_hz"] = None if cfg["lowpass_hz"] is None else int(cfg["lowpass_hz"])
    if not (0 < cfg["frequency"] <= MAX_SAMPLERATE):
        raise ValueError(f"frequency must be in 1..{MAX_SAMPLERATE}")
    if cfg["channels"] not in (1, 2):
        raise ValueError("channels must be 1 or 2")
    if cfg["lowpass_hz"] is not None and cfg["lowpass_hz"] < 0:
        raise ValueError("lowpass_hz must be >= 0")
    return cfg


def build_audio_filter(frequency: int, lowpass_hz, gain_db: float) -> str:
    """Resample first so the low-pass cutoff is always below the target Nyquist."""
    chain = [f"aresample={frequency}"]
    cutoff = int(frequency * 0.4) if lowpass_hz is None else int(lowpass_hz)
    if cutoff > 0:
        chain.append(f"lowpass=f={min(cutoff, int(frequency * 0.49))}")
    if gain_db:
        chain.append(f"volume={gain_db}dB")
    return ",".join(chain)


def check_ffmpeg() -> None:
    if shutil.which("ffmpeg") is None:
        sys.exit("Error: ffmpeg not found. Install it and make sure it's on your PATH.")


def prepare_wav(source: str, wav: str, cfg: dict) -> None:
    cmd = [
        "ffmpeg",
        "-y",
        "-i",
        source,
        "-vn",
        "-af",
        build_audio_filter(cfg["frequency"], cfg["lowpass_hz"], cfg["gain_db"]),
        "-acodec",
        "pcm_s16le",
        "-ar",
        str(cfg["frequency"]),
        "-ac",
        str(cfg["channels"]),
        wav,
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("ffmpeg stderr:\n" + result.stderr, file=sys.stderr)
        raise subprocess.CalledProcessError(result.returncode, cmd)


def ensure_qoaconv() -> Path:
    env = os.environ.get("QOACONV")
    if env and Path(env).exists():
        return Path(env)

    qoa_dir = Path(__file__).resolve().parents[2] / "libs" / "p3d-qoa"
    if qoa_dir.exists():
        made = subprocess.run(
            ["make", "qoaconv"], cwd=qoa_dir, capture_output=True, text=True
        )
        if made.returncode != 0:
            print(made.stdout + made.stderr, file=sys.stderr)
            raise subprocess.CalledProcessError(made.returncode, made.args)
        qoaconv = qoa_dir / "qoaconv"
        if qoaconv.exists():
            return qoaconv

    found = shutil.which("qoaconv")
    if found:
        return Path(found)
    raise FileNotFoundError("qoaconv not found (set QOACONV or build libs/p3d-qoa)")


def resolve_output(output_file: str, source: Path) -> Path:
    out = Path(output_file).resolve()
    if out.is_dir() or output_file.endswith(("/", "\\")):
        out = out / f"{source.stem}.qoa"
    elif out.suffix.lower() != ".qoa":
        out = out.with_name(out.name + ".qoa")  # keeps names like "track.01"
    return out


def convert(input_file: str, output_file: str, config: dict) -> None:
    check_ffmpeg()
    overrides = {k: v for k, v in (config or {}).items() if v is not None}
    cfg = normalize_config({**load_config(input_file), **overrides})

    source = Path(input_file).resolve()
    if not source.exists():
        raise FileNotFoundError(f"Input not found: {source}")
    out = resolve_output(output_file, source)
    out.parent.mkdir(parents=True, exist_ok=True)

    qoaconv = ensure_qoaconv()
    with tempfile.TemporaryDirectory() as tmp:
        wav = str(Path(tmp) / "audio.wav")
        tmp_qoa = Path(tmp) / "audio.qoa"
        prepare_wav(str(source), wav, cfg)

        result = subprocess.run(
            [str(qoaconv), wav, str(tmp_qoa)],
            cwd=str(qoaconv.parent),
            capture_output=True,
            text=True,
        )
        if result.returncode != 0:
            if result.stderr.strip():
                print(result.stderr.strip(), file=sys.stderr)
            if result.stdout.strip():
                print(result.stdout.strip())
            raise subprocess.CalledProcessError(result.returncode, result.args)

        # Validate before touching the destination.
        if (
            not tmp_qoa.exists()
            or tmp_qoa.stat().st_size < 16
            or tmp_qoa.read_bytes()[:4] != b"qoaf"
        ):
            raise ValueError("qoaconv did not produce a valid QOA file")
        shutil.move(str(tmp_qoa), str(out))

    print(f"Written: {out.name} ({cfg['frequency']} Hz, {cfg['channels']} ch)")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert audio to QOA (ffmpeg pre-filter + vendored qoaconv). "
        "Unset flags fall back to <name>.build.json, then defaults."
    )
    parser.add_argument("input", help="Input audio file (.mp3, .wav, ...)")
    parser.add_argument("output", help="Output .qoa file or directory")
    parser.add_argument(
        "--frequency", type=int, help="Sample rate in Hz (default 32000)"
    )
    parser.add_argument(
        "--channels", type=int, choices=[1, 2], help="Channels (default 1)"
    )
    parser.add_argument(
        "--lowpass-hz",
        type=int,
        help="Low-pass cutoff; default 40%% of frequency, 0 = off",
    )
    parser.add_argument(
        "--gain-db", type=float, help="Gain in dB before encoding (default -3, 0 = off)"
    )
    args = parser.parse_args()

    convert(
        args.input,
        args.output,
        {
            "frequency": args.frequency,
            "channels": args.channels,
            "lowpass_hz": args.lowpass_hz,
            "gain_db": args.gain_db,
        },
    )


if __name__ == "__main__":
    main()
