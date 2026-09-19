#!/usr/bin/env python3
"""Simplified MP3 -> QOA conversion via the vendored qoaconv tool."""

import argparse
import json
import subprocess
from pathlib import Path

DEFAULT_FREQUENCY = 32000
DEFAULT_CHANNELS = 1


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
    cfg = {"frequency": DEFAULT_FREQUENCY, "channels": DEFAULT_CHANNELS}
    cfg.update(config or {})
    cfg["frequency"] = int(cfg.get("frequency", DEFAULT_FREQUENCY))
    cfg["channels"] = int(cfg.get("channels", DEFAULT_CHANNELS))
    if cfg["frequency"] <= 0:
        raise ValueError("frequency must be > 0")
    if cfg["channels"] not in (1, 2):
        raise ValueError("channels must be 1 or 2")
    return cfg


def ensure_qoaconv() -> Path:
    qoa_dir = Path(__file__).resolve().parents[2] / "libs" / "p3d-qoa"
    subprocess.run(["make", "qoaconv"], cwd=qoa_dir, check=True)
    qoaconv = qoa_dir / "qoaconv"
    if not qoaconv.exists():
        raise FileNotFoundError(f"Expected built converter at {qoaconv}")
    return qoaconv


def convert(input_file: str, output_file: str, config: dict) -> None:
    cfg = normalize_config(load_config(input_file) | config)
    source = Path(input_file).resolve()
    out = Path(output_file).resolve()

    if out.suffix.lower() != ".qoa":
        out = out.with_suffix(".qoa")
    out.parent.mkdir(parents=True, exist_ok=True)

    qoaconv = ensure_qoaconv()
    result = subprocess.run(
        [str(qoaconv), str(source), str(out)],
        cwd=str(qoaconv.parent),
        capture_output=True,
        text=True,
    )

    if result.returncode != 0:
        if result.stderr.strip():
            print(result.stderr.strip(), file=None)
        if result.stdout.strip():
            print(result.stdout.strip())
        raise subprocess.CalledProcessError(result.returncode, result.args)

    print(f"Written: {out.name} ({cfg['frequency']} Hz, {cfg['channels']} ch)")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert MP3 music to QOA using the vendored qoaconv tool"
    )
    parser.add_argument("input", help="Input .mp3 file")
    parser.add_argument("output", help="Output .qoa file or directory")
    parser.add_argument(
        "--frequency",
        type=int,
        default=DEFAULT_FREQUENCY,
        help="Output sample frequency (default: 32000)",
    )
    parser.add_argument(
        "--channels",
        type=int,
        default=DEFAULT_CHANNELS,
        help="Output channels (default: 1)",
    )
    args = parser.parse_args()

    convert(
        args.input,
        args.output,
        {"frequency": args.frequency, "channels": args.channels},
    )


if __name__ == "__main__":
    main()
