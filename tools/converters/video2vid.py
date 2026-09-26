#!/usr/bin/env python3
"""Convert a video into a multiplexed .vid (video + QOA audio in ONE file).

Settings come from (lowest to highest priority): built-in defaults,
<name>.build.json (same lookup as mp32qoa.py), the config dict, CLI flags.
Supported keys: frequency (audio quality), channels, fps, size, bits, audio_lead_ms, no_audio.

Layout (little-endian):
  16-byte header:
    0  "VID\\0"   4 u16 fps   6 u8 bpp (1 = 8-bit paletted, 2 = 16-bit)
    7  u8 audio codec (0 = none, 1 = QOA)   8 u16 width   10 u16 height
    12 u16 audio sample rate   14 u8 audio channels   15 reserved
  [512-byte BGR555 palette if bpp == 1]
  per video frame:
    u32 audio_bytes | audio_bytes of whole QOA frames (may be 0) | width*height*bpp video bytes
"""

import argparse
import json
import os
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

VID_MAGIC = b"VID\0"
AUDIO_NONE = 0
AUDIO_QOA = 1

# Must match AUDIO_CHUNK_MAX in VideoController.hpp
MAX_AUDIO_CHUNK = 16384

DEFAULTS = {
    "fps": 24,
    "size": "256x192",
    "bits": 16,
    "frequency": 32000,
    "channels": 1,
    "audio_lead_ms": 100,
    "no_audio": False,
    "lowpass_hz": None,  # None = auto (40% of frequency), 0 = off
    "gain_db": -3.0,
}


# --------------------------------------------------------------------------- #
# Config (mirrors mp32qoa.py)
# --------------------------------------------------------------------------- #
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
    cfg["fps"] = int(cfg["fps"])
    cfg["bits"] = int(cfg["bits"])
    cfg["frequency"] = int(cfg["frequency"])
    cfg["channels"] = int(cfg["channels"])
    cfg["audio_lead_ms"] = int(cfg["audio_lead_ms"])
    cfg["no_audio"] = bool(cfg["no_audio"])
    cfg["size"] = str(cfg["size"])
    cfg["gain_db"] = float(cfg["gain_db"])
    cfg["lowpass_hz"] = None if cfg["lowpass_hz"] is None else int(cfg["lowpass_hz"])
    if cfg["lowpass_hz"] is not None and cfg["lowpass_hz"] < 0:
        raise ValueError("lowpass_hz must be >= 0")
    if cfg["fps"] <= 0:
        raise ValueError("fps must be > 0")
    if cfg["bits"] not in (8, 16):
        raise ValueError("bits must be 8 or 16")
    if not (0 < cfg["frequency"] <= 65535):
        raise ValueError("frequency must be in 1..65535")
    if cfg["channels"] not in (1, 2):
        raise ValueError("channels must be 1 or 2")
    if cfg["audio_lead_ms"] < 0:
        raise ValueError("audio_lead_ms must be >= 0")
    w, h = cfg["size"].lower().split("x")
    int(w), int(h)
    return cfg


# --------------------------------------------------------------------------- #
# ffmpeg helpers
# --------------------------------------------------------------------------- #
def check_ffmpeg():
    try:
        subprocess.run(
            ["ffmpeg", "-version"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True,
        )
    except FileNotFoundError:
        print("Error: ffmpeg not found. Install it and make sure it's on your PATH.")
        sys.exit(1)


def _ffmpeg_run(cmd, quiet=False):
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        if not quiet:
            print("ffmpeg stderr:\n" + result.stderr, file=sys.stderr)
        raise subprocess.CalledProcessError(
            result.returncode, cmd, stderr=result.stderr
        )


def has_audio_stream(input_path: str) -> bool:
    try:
        r = subprocess.run(
            [
                "ffprobe",
                "-v",
                "error",
                "-select_streams",
                "a:0",
                "-show_entries",
                "stream=codec_type",
                "-of",
                "csv=p=0",
                input_path,
            ],
            capture_output=True,
            text=True,
        )
    except FileNotFoundError:
        return True  # can't probe; let ffmpeg decide
    return r.returncode == 0 and "audio" in r.stdout


# --------------------------------------------------------------------------- #
# Video
# --------------------------------------------------------------------------- #
def run_ffmpeg_16bit(input_path: str, out_raw: str, fps: int, size: str):
    _ffmpeg_run(
        [
            "ffmpeg",
            "-y",
            "-i",
            input_path,
            "-an",
            "-vcodec",
            "rawvideo",
            "-f",
            "rawvideo",
            "-pix_fmt",
            "bgr555le",
            "-s",
            size,
            "-r",
            str(fps),
            out_raw,
        ]
    )


def patch_alpha_bits(src: str, dst: str):
    with open(src, "rb") as f:
        data = bytearray(f.read())
    for i in range(1, len(data), 2):
        data[i] |= 0x80
    with open(dst, "wb") as f:
        f.write(data)


def encode_8bit_raw(
    input_path: str, fps: int, size: str, output_raw: str, output_pal: str
):
    w, h = (int(x) for x in size.split("x"))
    with tempfile.TemporaryDirectory() as tmp_dir:
        palette_png = os.path.join(tmp_dir, "palette.png")
        frames_tmp = os.path.join(tmp_dir, "frames.tmp")

        _ffmpeg_run(
            [
                "ffmpeg",
                "-y",
                "-i",
                input_path,
                "-vf",
                f"scale={w}:{h},palettegen=max_colors=256:stats_mode=full",
                "-frames:v",
                "1",
                palette_png,
            ]
        )
        _ffmpeg_run(
            [
                "ffmpeg",
                "-y",
                "-i",
                input_path,
                "-i",
                palette_png,
                "-lavfi",
                f"scale={w}:{h} [x]; [x][1:v] paletteuse=dither=bayer",
                "-an",
                "-vcodec",
                "rawvideo",
                "-f",
                "rawvideo",
                "-pix_fmt",
                "pal8",
                "-r",
                str(fps),
                frames_tmp,
            ]
        )

        from PIL import Image

        pal_img = Image.open(palette_png).convert("RGB")
        pal_data = list(pal_img.getdata())
        bgr555 = bytearray(512)
        for i, (r8, g8, b8) in enumerate(pal_data[:256]):
            word = (r8 >> 3) | ((g8 >> 3) << 5) | ((b8 >> 3) << 10)
            struct.pack_into("<H", bgr555, i * 2, word)

        with open(output_pal, "wb") as f:
            f.write(bgr555)
        with open(frames_tmp, "rb") as src, open(output_raw, "wb") as dst:
            while True:
                frame_data = src.read(w * h)
                if not frame_data:
                    break
                dst.write(frame_data)
                src.read(1024)  # ffmpeg appends a 1024-byte palette per pal8 frame


# --------------------------------------------------------------------------- #
# Audio (WAV -> QOA via the vendored qoaconv, same tool as mp32qoa.py)
# --------------------------------------------------------------------------- #
def build_audio_filter(frequency: int, lowpass_hz, gain_db: float) -> str:
    """Resample first so the low-pass cutoff is always below the target Nyquist."""
    chain = [f"aresample={frequency}"]
    cutoff = int(frequency * 0.4) if lowpass_hz is None else int(lowpass_hz)
    if cutoff > 0:
        chain.append(f"lowpass=f={min(cutoff, int(frequency * 0.49))}")
    if gain_db:
        chain.append(f"volume={gain_db}dB")
    return ",".join(chain)


def extract_pcm_wav(
    input_path: str,
    out_wav: str,
    frequency: int,
    channels: int,
    duration: float,
    lowpass_hz=None,
    gain_db: float = -3.0,
):
    """Extract audio, low-passed and attenuated, padded/trimmed to the video duration."""
    chain = build_audio_filter(frequency, lowpass_hz, gain_db)
    base = ["ffmpeg", "-y", "-i", input_path, "-vn"]
    tail = [
        "-acodec",
        "pcm_s16le",
        "-ar",
        str(frequency),
        "-ac",
        str(channels),
        "-t",
        f"{duration:.6f}",
        out_wav,
    ]
    try:
        _ffmpeg_run(
            base + ["-af", f"{chain},apad=whole_dur={duration:.6f}"] + tail, quiet=True
        )
    except subprocess.CalledProcessError:
        _ffmpeg_run(base + ["-af", chain] + tail)  # older ffmpeg without apad whole_dur


def ensure_qoaconv() -> Path:
    env = os.environ.get("QOACONV")
    if env and Path(env).exists():
        return Path(env)

    qoa_dir = Path(__file__).resolve().parents[2] / "libs" / "p3d-qoa"
    if qoa_dir.exists():
        subprocess.run(["make", "qoaconv"], cwd=qoa_dir, check=True)
        qoaconv = qoa_dir / "qoaconv"
        if qoaconv.exists():
            return qoaconv

    found = shutil.which("qoaconv")
    if found:
        return Path(found)
    raise FileNotFoundError("qoaconv not found (set QOACONV or build libs/p3d-qoa)")


def wav_to_qoa(wav_path: str, qoa_path: str):
    qoaconv = ensure_qoaconv()
    result = subprocess.run(
        [str(qoaconv), wav_path, qoa_path], capture_output=True, text=True
    )
    if result.returncode != 0:
        if result.stderr.strip():
            print(result.stderr.strip(), file=sys.stderr)
        if result.stdout.strip():
            print(result.stdout.strip())
        raise subprocess.CalledProcessError(result.returncode, result.args)


def read_qoa_frames(qoa_path: str):
    """Split a .qoa file into self-contained frames: (frames, rate, channels)."""
    data = Path(qoa_path).read_bytes()
    if len(data) < 16 or data[:4] != b"qoaf":
        raise ValueError("qoaconv output is not a valid QOA file")

    pos = 8  # file header: magic + u32 total samples
    frames, rate, channels = [], 0, 0
    while pos + 8 <= len(data):
        ch = data[pos]
        sr = int.from_bytes(data[pos + 1 : pos + 4], "big")
        fsamples, fsize = struct.unpack(">HH", data[pos + 4 : pos + 8])
        if fsize < 8 or pos + fsize > len(data):
            raise ValueError("corrupt QOA frame")
        if not frames:
            rate, channels = sr, ch
        frames.append((fsamples, data[pos : pos + fsize]))
        pos += fsize

    if not frames:
        raise ValueError("QOA file contains no frames")
    return frames, rate, channels


# --------------------------------------------------------------------------- #
# Muxer
# --------------------------------------------------------------------------- #
def interweave(
    raw_video,
    out_vid,
    fps,
    w,
    h,
    bpp,
    pal_file=None,
    qoa_frames=None,
    audio_rate=0,
    audio_channels=0,
    lead_ms=100,
):
    frame_size = w * h * bpp
    codec = AUDIO_QOA if qoa_frames else AUDIO_NONE
    lead_samples = int(audio_rate * lead_ms / 1000)

    audio_sent = 0  # samples per channel delivered so far
    qi = 0  # next QOA frame index
    audio_bytes_total = 0

    with open(raw_video, "rb") as f_vid, open(out_vid, "wb") as f_out:
        f_out.write(
            struct.pack(
                "<4sHBBHHHBB",
                VID_MAGIC,
                int(fps),
                int(bpp),
                codec,
                w,
                h,
                int(audio_rate),
                int(audio_channels),
                0,
            )
        )

        if pal_file:
            with open(pal_file, "rb") as f_pal:
                f_out.write(f_pal.read(512))

        frame_idx = 0
        while True:
            v_data = f_vid.read(frame_size)
            if len(v_data) < frame_size:
                break  # drop trailing partial frame

            chunk = bytearray()
            if qoa_frames:
                # Keep audio delivered up to the END of this video frame + lead.
                target = int((frame_idx + 1) * audio_rate / fps) + lead_samples
                while qi < len(qoa_frames) and audio_sent < target:
                    fsamples, blob = qoa_frames[qi]
                    if len(blob) > MAX_AUDIO_CHUNK:
                        raise ValueError(
                            f"QOA frame ({len(blob)} B) exceeds {MAX_AUDIO_CHUNK} B chunk limit"
                        )
                    if chunk and len(chunk) + len(blob) > MAX_AUDIO_CHUNK:
                        break  # spill into the next frame's chunk
                    chunk += blob
                    audio_sent += fsamples
                    qi += 1

            f_out.write(struct.pack("<I", len(chunk)))
            f_out.write(chunk)
            f_out.write(v_data)
            audio_bytes_total += len(chunk)
            frame_idx += 1

    if qoa_frames and qi < len(qoa_frames):
        print(
            f"Note: {len(qoa_frames) - qi} trailing audio frame(s) dropped (audio longer than video)"
        )
    return frame_idx, audio_bytes_total


def convert(input_path, output_path, config=None):
    check_ffmpeg()

    overrides = {k: v for k, v in (config or {}).items() if v is not None}
    cfg = normalize_config({**load_config(input_path), **overrides})

    fps, size, bits = cfg["fps"], cfg["size"], cfg["bits"]
    frequency, channels = cfg["frequency"], cfg["channels"]
    w, h = (int(x) for x in size.lower().split("x"))
    size = f"{w}x{h}"
    bpp = 2 if bits == 16 else 1

    if not output_path.endswith(".vid"):
        output_path += ".vid"
    Path(output_path).resolve().parent.mkdir(parents=True, exist_ok=True)

    use_audio = (not cfg["no_audio"]) and has_audio_stream(input_path)
    print(
        f"Processing ({bits}-bit, {fps} fps, {size}, "
        + (f"QOA {frequency} Hz {channels} ch" if use_audio else "no audio")
        + ")..."
    )

    with tempfile.TemporaryDirectory() as tmp_dir:
        raw_vid = os.path.join(tmp_dir, "v.raw")
        pal_file = os.path.join(tmp_dir, "p.pal") if bits == 8 else None

        # Video first: its frame count defines the duration the audio must cover.
        if bits == 16:
            tmp_vid = os.path.join(tmp_dir, "tmp.raw")
            run_ffmpeg_16bit(input_path, tmp_vid, fps, size)
            patch_alpha_bits(tmp_vid, raw_vid)
        else:
            encode_8bit_raw(input_path, fps, size, raw_vid, pal_file)

        total_frames = os.path.getsize(raw_vid) // (w * h * bpp)
        if total_frames == 0:
            raise ValueError("no video frames were produced")

        qoa_frames = None
        a_rate = a_ch = 0
        if use_audio:
            wav = os.path.join(tmp_dir, "a.wav")
            qoa = os.path.join(tmp_dir, "a.qoa")
            extract_pcm_wav(
                input_path,
                wav,
                frequency,
                channels,
                total_frames / fps,
                cfg["lowpass_hz"],
                cfg["gain_db"],
            )
            wav_to_qoa(wav, qoa)
            qoa_frames, a_rate, a_ch = read_qoa_frames(qoa)
            if a_rate != frequency or a_ch != channels:
                print(
                    f"Warning: QOA is {a_rate} Hz/{a_ch} ch, requested {frequency} Hz/{channels} ch"
                )

        frames, audio_bytes = interweave(
            raw_vid,
            output_path,
            fps,
            w,
            h,
            bpp,
            pal_file,
            qoa_frames,
            a_rate,
            a_ch,
            cfg["audio_lead_ms"],
        )

    print(f"Written: {output_path} / {frames} frames / {audio_bytes} audio bytes")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert video to a multiplexed .vid (video + QOA audio). "
        "Unset flags fall back to <name>.build.json, then defaults."
    )
    parser.add_argument("input")
    parser.add_argument("output")
    parser.add_argument("--bits", type=int, choices=[8, 16])
    parser.add_argument("--fps", type=int)
    parser.add_argument("--size")
    parser.add_argument(
        "--frequency", type=int, help="audio sample rate / quality (default 32000)"
    )
    parser.add_argument(
        "--channels", type=int, choices=[1, 2], help="audio channels (default 1)"
    )
    parser.add_argument(
        "--audio-lead-ms",
        type=int,
        help="how far audio is muxed ahead of video (default 100)",
    )
    parser.add_argument("--no-audio", action="store_true", default=None)
    parser.add_argument(
        "--lowpass-hz",
        type=int,
        help="low-pass cutoff; default 40%% of frequency, 0 = off",
    )
    parser.add_argument(
        "--gain-db", type=float, help="gain in dB before encoding (default -3, 0 = off)"
    )
    args = parser.parse_args()

    convert(
        args.input,
        args.output,
        {
            "bits": args.bits,
            "fps": args.fps,
            "size": args.size,
            "frequency": args.frequency,
            "channels": args.channels,
            "audio_lead_ms": args.audio_lead_ms,
            "no_audio": args.no_audio,
        },
    )
