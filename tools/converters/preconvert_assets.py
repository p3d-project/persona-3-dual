#!/usr/bin/env python3
"""Manually pre-convert asset files IN PLACE, keeping their format, to save space.

Audio (.mp3 .wav .flac .ogg .m4a): resampled to `frequency` Hz / `channels`.
Video (.mp4 .m4v .mov .mkv .webm .avi): scaled down to `size`, fps lowered to `fps`
(never raised), re-encoded with a small-file codec; audio conformed to `frequency` / `channels`.
No low-pass/gain here: mp32qoa.py / video2vid.py do that at encode time.

Settings per file (lowest to highest priority): defaults, <name>.build.json
(<name>.build.json next to the file, or ../<name>.build.json), CLI flags.
Keys: frequency, channels, fps, size, crf, preset.

Example usage:
  python tools/converters/preconvert_assets.py /work/assets/video --dry-run
  python tools/converters/preconvert_assets.py /work/assets/music --dry-run

  # 1. Preview what would change (nothing is written)
  python preconvert_assets.py assets/audio assets/video --dry-run

  # 2. Convert everything in place, keeping originals as <name>.orig
  python preconvert_assets.py assets/audio assets/video --backup

  # 3. One video with explicit settings
  python preconvert_assets.py assets/video/intro.mp4 --size 256x192 --fps 20 --crf 30 --frequency 22050

  # 4. Re-encode even files that already conform (e.g. to apply a new --crf)
  python preconvert_assets.py assets/video --force --crf 32
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import mp32qoa  # noqa: E402
import video2vid  # noqa: E402

AUDIO_EXTS = {".mp3", ".wav", ".flac", ".ogg", ".m4a"}
VIDEO_EXTS = {".mp4", ".m4v", ".mov", ".mkv", ".webm", ".avi"}
MP3_RATES = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000}
TMP_PREFIX = ".preconv-"

# Same-format encoders. mp3 -q:a 2 / vorbis -q:a 5 keep generation loss low.
AUDIO_ARGS = {
    ".mp3": ["-c:a", "libmp3lame", "-q:a", "2"],
    ".wav": ["-c:a", "pcm_s16le"],
    ".flac": ["-c:a", "flac"],
    ".ogg": ["-c:a", "libvorbis", "-q:a", "5"],
    ".m4a": ["-c:a", "aac", "-b:a", "128k"],
}


def fmt(n: int) -> str:
    return f"{n / 1048576:.2f} MB" if n >= 1048576 else f"{n / 1024:.1f} KB"


def probe(path: Path) -> dict:
    r = subprocess.run(
        [
            "ffprobe",
            "-v",
            "error",
            "-show_entries",
            "stream=codec_type,width,height,avg_frame_rate,sample_rate,channels",
            "-of",
            "json",
            str(path),
        ],
        capture_output=True,
        text=True,
    )
    if r.returncode != 0:
        raise RuntimeError("ffprobe failed: " + r.stderr.strip())
    data = json.loads(r.stdout)
    info = {"v": None, "a": None}
    for s in data.get("streams", []):
        kind = s.get("codec_type")
        if kind == "video" and info["v"] is None:
            n, _, d = s.get("avg_frame_rate", "0/1").partition("/")
            try:
                den = float(d or 1)
                fps = float(n) / den if den else 0.0
            except ValueError:
                fps = 0.0
            info["v"] = {"w": int(s["width"]), "h": int(s["height"]), "fps": fps}
        elif kind == "audio" and info["a"] is None:
            info["a"] = {
                "rate": int(s.get("sample_rate", 0)),
                "ch": int(s.get("channels", 0)),
            }
    return info


def run_ffmpeg(cmd) -> None:
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        raise RuntimeError(
            "ffmpeg failed:\n" + "\n".join(r.stderr.strip().splitlines()[-5:])
        )


def finalize(path: Path, tmp: Path, cmd, backup: bool) -> str:
    old = path.stat().st_size
    try:
        run_ffmpeg(cmd)
        if backup:
            bak = path.with_name(path.name + ".orig")
            if not bak.exists():
                shutil.copy2(path, bak)
        os.replace(tmp, path)  # atomic: the asset is never left half-written
    finally:
        if tmp.exists():
            tmp.unlink()
    return f"converted {fmt(old)} -> {fmt(path.stat().st_size)}"


def video_args(ext: str, crf: int, preset: str, ch: int):
    if ext == ".webm":  # VP9 crf scale runs higher than x264's
        return (
            [
                "-c:v",
                "libvpx-vp9",
                "-b:v",
                "0",
                "-crf",
                str(crf + 6),
                "-row-mt",
                "1",
                "-pix_fmt",
                "yuv420p",
            ],
            ["-c:a", "libopus", "-b:a", f"{48 * ch}k"],
            [],
        )
    if ext == ".avi":
        return (
            ["-c:v", "mpeg4", "-q:v", "5", "-pix_fmt", "yuv420p"],
            ["-c:a", "libmp3lame", "-q:a", "4"],
            [],
        )
    extra = ["-movflags", "+faststart"] if ext in (".mp4", ".m4v", ".mov") else []
    return (
        ["-c:v", "libx264", "-preset", preset, "-crf", str(crf), "-pix_fmt", "yuv420p"],
        ["-c:a", "aac", "-b:a", f"{64 * ch}k"],
        extra,
    )


def process_audio(path: Path, ext: str, config: dict, args) -> str:
    cfg = mp32qoa.normalize_config(config)
    freq, ch = cfg["frequency"], cfg["channels"]
    tag = f"[{freq} Hz, {ch} ch]"
    if ext == ".mp3" and freq not in MP3_RATES:
        raise ValueError(
            f"{freq} Hz is not a valid MP3 sample rate ({sorted(MP3_RATES)})"
        )

    a = probe(path)["a"]
    if a is None:
        return f"{tag} skipped (no audio stream)"
    if not args.force and a["rate"] == freq and a["ch"] == ch:
        return f"{tag} skipped (already conforms)"
    if args.dry_run:
        return f"{tag} would convert {a['rate']} Hz/{a['ch']} ch -> {freq} Hz/{ch} ch"

    tmp = path.with_name(TMP_PREFIX + path.name)
    cmd = [
        "ffmpeg",
        "-y",
        "-i",
        str(path),
        "-vn",
        "-ar",
        str(freq),
        "-ac",
        str(ch),
        *AUDIO_ARGS[ext],
        str(tmp),
    ]  # -vn also drops embedded cover art
    return f"{tag} " + finalize(path, tmp, cmd, args.backup)


def process_video(path: Path, ext: str, config: dict, args) -> str:
    cfg = video2vid.normalize_config(config)
    freq, ch, fps = cfg["frequency"], cfg["channels"], cfg["fps"]
    tw, th = (int(x) for x in cfg["size"].lower().split("x"))
    crf, preset = int(cfg.get("crf", 28)), str(cfg.get("preset", "slow"))

    info = probe(path)
    v, a = info["v"], info["a"]
    tag = f"[{tw}x{th} @{fps}fps, {freq} Hz/{ch} ch]"
    if v is None:
        return f"{tag} skipped (no video stream)"

    need_scale = v["w"] > tw or v["h"] > th
    need_fps = v["fps"] > fps + 0.01
    # Opus can't do arbitrary rates (e.g. 32 kHz), so for .webm only compare channels;
    # otherwise the file would be re-encoded on every run.
    audio_ok = a is None or (a["ch"] == ch and (ext == ".webm" or a["rate"] == freq))
    if not args.force and not need_scale and not need_fps and audio_ok:
        return f"{tag} skipped (already conforms)"
    if args.force:
        need_scale = need_scale or (v["w"], v["h"]) != (tw, th)
    if (need_scale and ext != ".avi") and (tw % 2 or th % 2):
        raise ValueError(f"size {tw}x{th} must have even dimensions for {ext}")

    if args.dry_run:
        todo = []
        if need_scale:
            todo.append(f"{v['w']}x{v['h']} -> {tw}x{th}")
        if need_fps:
            todo.append(f"{v['fps']:.1f} -> {fps} fps")
        if a and not audio_ok:
            todo.append(f"audio {a['rate']} Hz/{a['ch']} ch -> {freq} Hz/{ch} ch")
        return f"{tag} would convert ({', '.join(todo) or 're-encode'})"

    vf = []
    if need_scale:
        vf.append(f"scale={tw}:{th}:flags=lanczos")  # stretch, same as video2vid.py
    if need_fps:
        vf.append(f"fps={fps}")

    vargs, aargs, extra = video_args(ext, crf, preset, ch)
    tmp = path.with_name(TMP_PREFIX + path.name)
    cmd = ["ffmpeg", "-y", "-i", str(path), "-map", "0:v:0"]
    if a:
        cmd += ["-map", "0:a:0"]
    if vf:
        cmd += ["-vf", ",".join(vf)]
    cmd += vargs
    if a:
        if ext != ".webm":  # libopus only supports 8/12/16/24/48 kHz; ffmpeg picks one
            cmd += ["-ar", str(freq)]
        cmd += ["-ac", str(ch)] + aargs
    cmd += extra + [str(tmp)]
    return f"{tag} " + finalize(path, tmp, cmd, args.backup)


def collect(paths):
    for p in map(Path, paths):
        if p.is_dir():
            files = sorted(f for f in p.rglob("*") if f.is_file())
        elif p.is_file():
            files = [p]
        else:
            print(f"WARN  not found: {p}", file=sys.stderr)
            continue
        for f in files:
            if f.name.startswith(TMP_PREFIX):
                continue
            if f.suffix.lower() in AUDIO_EXTS | VIDEO_EXTS:
                yield f


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Pre-convert assets in place (same format): resample audio, shrink/compress video.",
        epilog="Example usage:" + __doc__.split("Example usage:")[1],
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "paths", nargs="+", help="files and/or directories (searched recursively)"
    )
    parser.add_argument(
        "--dry-run", action="store_true", help="show what would change, write nothing"
    )
    parser.add_argument(
        "--backup", action="store_true", help="keep the original as <name>.orig"
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="re-encode even if the file already conforms",
    )
    parser.add_argument("--frequency", type=int, help="audio sample rate in Hz")
    parser.add_argument("--channels", type=int, choices=[1, 2], help="audio channels")
    parser.add_argument(
        "--fps", type=int, help="max video frame rate (only ever lowered)"
    )
    parser.add_argument("--size", help="video size WxH, e.g. 256x192")
    parser.add_argument(
        "--crf", type=int, help="video quality, higher = smaller (default 28)"
    )
    parser.add_argument("--preset", help="x264 preset (default slow)")
    args = parser.parse_args()

    for tool in ("ffmpeg", "ffprobe"):
        if shutil.which(tool) is None:
            sys.exit(
                f"Error: {tool} not found. Install it and make sure it's on your PATH."
            )

    overrides = {
        k: v
        for k, v in {
            "frequency": args.frequency,
            "channels": args.channels,
            "fps": args.fps,
            "size": args.size,
            "crf": args.crf,
            "preset": args.preset,
        }.items()
        if v is not None
    }

    failures = 0
    for path in collect(args.paths):
        ext = path.suffix.lower()
        try:
            config = {**mp32qoa.load_config(str(path)), **overrides}
            handler = process_audio if ext in AUDIO_EXTS else process_video
            print(f"{path}: {handler(path, ext, config, args)}")
        except Exception as exc:  # keep going; report at the end
            failures += 1
            print(f"FAIL  {path}: {exc}", file=sys.stderr)

    if failures:
        print(f"\n{failures} file(s) failed.", file=sys.stderr)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
