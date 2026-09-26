#!/usr/bin/env python3
import sys
import os
import json
import importlib
import argparse
from typing import Optional


def load_config(input_file: str) -> dict:
    """Load configuration from a sidecar JSON file next to the input file."""
    base, _ = os.path.splitext(input_file)

    # 1. Check exactly next to the file (e.g., assets/models/akihiko/akihiko.build.json)
    config_path = base + ".build.json"
    if os.path.exists(config_path):
        with open(config_path, "r") as f:
            return json.load(f)

    # 2. Check the parent directory (e.g., assets/models/akihiko.build.json)
    parent_dir = os.path.dirname(os.path.dirname(input_file))
    file_name = os.path.basename(base)
    parent_config_path = os.path.join(parent_dir, file_name + ".build.json")
    if os.path.exists(parent_config_path):
        with open(parent_config_path, "r") as f:
            return json.load(f)

    return {}


def guess_asset_type(input_file: str) -> Optional[str]:
    """Guess the asset type based on file extension."""
    ext = os.path.splitext(input_file)[1].lower()
    if ext == ".mp3":
        return "mp32qoa"
    if ext == ".mp4":
        return "video2vid"
    if ext == ".jmap":
        return "jmap2map"
    if ext == ".obj":
        return "build_environment"
    if ext == ".json":
        return "build_model"
    return None


def main() -> None:
    parser = argparse.ArgumentParser(description="P3 Dual Project Asset Compiler")
    parser.add_argument("input", help="Input file")
    parser.add_argument("output", help="Output file or directory")
    args, unknown = parser.parse_known_args()

    # Load JSON Sidecar
    config = load_config(args.input)

    # Identify asset routing
    asset_type = config.get("asset_type") or guess_asset_type(args.input)
    if not asset_type:
        print(f"Error: Cannot determine asset type for {args.input}")
        sys.exit(1)

    # Inject unknown CLI flags into the config dynamically (Overrides JSON)
    i = 0
    while i < len(unknown):
        if unknown[i].startswith("--"):
            key = unknown[i][2:].replace("-", "_")
            vals = []
            i += 1
            while i < len(unknown) and not unknown[i].startswith("--"):
                val = unknown[i]
                try:
                    val = float(val) if "." in val else int(val)
                except ValueError:
                    pass
                vals.append(val)
                i += 1

            if len(vals) == 0:
                config[key] = True
            elif len(vals) == 1:
                config[key] = vals[0]
            else:
                config[key] = vals
        else:
            i += 1

    # Dispatch
    try:
        converter = importlib.import_module(f"converters.{asset_type}")
    except ModuleNotFoundError:
        print(f"Error: Module converters.{asset_type} not found.")
        sys.exit(1)

    converter.convert(args.input, args.output, config)


if __name__ == "__main__":
    main()
