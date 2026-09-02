import os
import re
import argparse
from . import obj2model


def convert(input_file, output_file, config):
    """
    Full model build pipeline:

      1. Run obj2model to produce:
            <output_file>            (.bin  — geometry + animation data)
            <output_file stem>.h     (animation enum + inline texture loader)
            <output_file stem>_textures.txt   (absolute PNG paths for grit)

      2. For every PNG listed in _textures.txt, write a .grit sidecar file
         next to the PNG so the Makefile rule
             %.s %.h : %.png %.grit
         can convert it to a linked bitmap array.

      3. Delete _textures.txt — it is a transient build artefact.

    The .grit files are written into the model's ASSET directory (next to the
    PNGs), not into data/.  That way they are found by the Makefile's
    GRAPHICS scan of  assets/models/*  and compiled into the binary just like
    environment textures.
    """
    base_name = re.sub(
        r"[^a-zA-Z0-9_]", "_", os.path.splitext(os.path.basename(input_file))[0]
    )

    obj2model.convert(input_file, output_file, config)

    # _textures.txt lands next to the .bin (in data/models/)
    output_dir = os.path.dirname(os.path.abspath(output_file))
    tex_list = os.path.join(output_dir, f"{base_name}_textures.txt")

    if not os.path.exists(tex_list):
        return

    if config.get("skip_grit"):
        os.remove(tex_list)
        return

    grit_flags = config.get("grit_flags", "-gb -gB16 -gT! -p!")

    pngs = [
        line.strip()
        for line in open(tex_list)
        if line.strip() and not line.startswith("#")
    ]

    for png in pngs:
        if not os.path.exists(png):
            print(f"  [WARN] PNG not found: {png}")
            continue
        grit_file = os.path.splitext(png)[0] + ".grit"
        with open(grit_file, "w") as f:
            f.write(grit_flags)
        print(f"  Grit  {os.path.basename(grit_file)}")

    os.remove(tex_list)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NDS model build pipeline")
    parser.add_argument("input", help="Input .json model descriptor")
    parser.add_argument("output", help="Output .bin file path")
    parser.add_argument("--scale", type=float, default=None)
    parser.add_argument("--target-size", type=float, default=4.0)
    parser.add_argument("--no-center", action="store_true")
    parser.add_argument("--source-blender", action="store_true")
    parser.add_argument("--skip-grit", action="store_true")
    parser.add_argument("--grit-flags", type=str, default="-gb -gB16 -p!")
    parser.add_argument(
        "--rgba", action="store_true", help="Treat all textures as GL_RGBA"
    )
    parser.add_argument(
        "--rgba-list",
        type=str,
        default="",
        help="Comma-separated texture filename substrings to treat as GL_RGBA",
    )
    parser.add_argument(
        "--color", nargs=3, type=int, metavar=("R", "G", "B"), default=[255, 255, 255]
    )
    args = parser.parse_args()

    cli_config = {
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "skip_grit": args.skip_grit,
        "grit_flags": args.grit_flags,
        "rgba": args.rgba,
        "rgba_list": args.rgba_list,
        "color": args.color,
    }
    convert(args.input, args.output, cli_config)
