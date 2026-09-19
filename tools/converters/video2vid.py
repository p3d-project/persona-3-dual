import argparse
import os
import struct
import subprocess
import sys
import tempfile


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


def _ffmpeg_run(cmd):
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("ffmpeg stderr:\n" + result.stderr, file=sys.stderr)
        raise subprocess.CalledProcessError(
            result.returncode, cmd, stderr=result.stderr
        )


def run_ffmpeg_16bit(input_path: str, out_raw: str, fps: int, size: str):
    cmd = [
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
    _ffmpeg_run(cmd)


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
            r5, g5, b5 = r8 >> 3, g8 >> 3, b8 >> 3
            word = r5 | (g5 << 5) | (b5 << 10)
            struct.pack_into("<H", bgr555, i * 2, word)

        with open(output_pal, "wb") as f:
            f.write(bgr555)
        with open(frames_tmp, "rb") as src, open(output_raw, "wb") as dst:
            while True:
                frame_data = src.read(w * h)
                if not frame_data:
                    break
                dst.write(frame_data)
                src.read(1024)


def extract_pcm(
    input_path: str, out_pcm: str, frequency: int = 32000, channels: int = 2
):
    cmd = [
        "ffmpeg",
        "-y",
        "-i",
        input_path,
        "-vn",
        "-f",
        "s16le",
        "-ar",
        str(frequency),
        "-ac",
        str(channels),
        out_pcm,
    ]
    _ffmpeg_run(cmd)


def extract_pcm_wav(
    input_path: str, out_wav: str, frequency: int = 32000, channels: int = 2
):
    cmd = [
        "ffmpeg",
        "-y",
        "-i",
        input_path,
        "-vn",
        "-acodec",
        "pcm_s16le",
        "-ar",
        str(frequency),
        "-ac",
        str(channels),
        out_wav,
    ]
    _ffmpeg_run(cmd)


def ensure_qoaconv() -> str:
    qoa_dir = os.path.normpath(
        os.path.join(os.path.dirname(__file__), "..", "..", "libs", "p3d-qoa")
    )
    subprocess.run(["make", "qoaconv"], cwd=qoa_dir, check=True)
    qoaconv = os.path.join(qoa_dir, "qoaconv")
    if not os.path.exists(qoaconv):
        raise FileNotFoundError(f"Expected built converter at {qoaconv}")
    return qoaconv


def build_qoa_sidecar(
    input_path: str, output_path: str, frequency: int = 32000, channels: int = 2
):
    qoa_out = os.path.splitext(output_path)[0] + ".qoa"
    temp_wav = os.path.join(os.path.dirname(output_path), ".video_audio_tmp.wav")
    extract_pcm_wav(input_path, temp_wav, frequency=frequency, channels=channels)
    qoaconv = ensure_qoaconv()
    subprocess.run([qoaconv, temp_wav, qoa_out], check=True)
    os.remove(temp_wav)
    return qoa_out


def interweave(
    raw_video: str,
    pcm_audio: str,
    out_vid: str,
    fps: int,
    w: int,
    h: int,
    bpp: int,
    sample_rate: int = 32000,
    channels: int = 2,
    pal_file: str = None,
):
    frame_size = w * h * bpp
    bytes_per_sample = 2 * channels

    with open(raw_video, "rb") as f_vid, open(pcm_audio, "rb") as f_aud, open(
        out_vid, "wb"
    ) as f_out:

        # write 16-byte metadata header dynamically mapping configurations
        magic = b"VID\0"
        header = struct.pack("<4s H B B H H 4x", magic, int(fps), int(bpp), 0, w, h)
        f_out.write(header)

        if pal_file:
            with open(pal_file, "rb") as f_pal:
                f_out.write(f_pal.read(512))

        frame_idx = 0
        while True:
            v_data = f_vid.read(frame_size)
            if not v_data:
                break

            start_sample = int(frame_idx * sample_rate / fps)
            end_sample = int((frame_idx + 1) * sample_rate / fps)
            samples_this_frame = end_sample - start_sample
            audio_bytes = samples_this_frame * bytes_per_sample

            a_data = f_aud.read(audio_bytes)
            if len(a_data) < audio_bytes:
                a_data += b"\x00" * (audio_bytes - len(a_data))

            f_out.write(struct.pack("<I", audio_bytes))
            f_out.write(a_data)
            f_out.write(v_data)
            frame_idx += 1

    return frame_idx


def convert(input_path, output_path, config):
    check_ffmpeg()

    fps = config.get("fps", 24)
    size = config.get("size", "256x192")
    bits = config.get("bits", 16)
    frequency = int(config.get("frequency", 32000))
    channels = int(config.get("channels", 2))

    w, h = (int(x) for x in size.split("x"))
    bpp = 2 if bits == 16 else 1

    if not output_path.endswith(".vid"):
        output_path += ".vid"

    qoa_path = build_qoa_sidecar(
        input_path, output_path, frequency=frequency, channels=channels
    )

    print(f"Processing ({bits}-bit, {fps} fps, {frequency} Hz, {channels} ch)...")
    with tempfile.TemporaryDirectory() as tmp_dir:
        raw_vid = os.path.join(tmp_dir, "v.raw")
        pcm_aud = os.path.join(tmp_dir, "a.pcm")
        pal_file = os.path.join(tmp_dir, "p.pal") if bits == 8 else None

        extract_pcm(input_path, pcm_aud, frequency=frequency, channels=channels)

        if bits == 16:
            tmp_vid = os.path.join(tmp_dir, "tmp.raw")
            run_ffmpeg_16bit(input_path, tmp_vid, fps, size)
            patch_alpha_bits(tmp_vid, raw_vid)
        else:
            encode_8bit_raw(input_path, fps, size, raw_vid, pal_file)

        frames = interweave(
            raw_vid, pcm_aud, output_path, fps, w, h, bpp, frequency, channels, pal_file
        )

    print(f"Written: {output_path} / {frames} frames")
    print(f"Audio sidecar: {qoa_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    parser.add_argument("output")
    parser.add_argument("--bits", type=int, choices=[8, 16], default=16)
    parser.add_argument("--fps", type=int, default=24)
    parser.add_argument("--size", default="256x192")
    parser.add_argument("--frequency", type=int, default=32000)
    parser.add_argument("--channels", type=int, default=2)
    args = parser.parse_args()

    cli_config = {
        "bits": args.bits,
        "fps": args.fps,
        "size": args.size,
        "frequency": args.frequency,
        "channels": args.channels,
    }
    convert(args.input, args.output, cli_config)
