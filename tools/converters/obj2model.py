import struct
import argparse
import json
import os
import re
import subprocess
import shutil
import sys
from collections import defaultdict

FIFO_COLOR = 0x20
FIFO_TEXCOORD = 0x22
FIFO_BEGIN = 0x40
FIFO_VERTEX16 = 0x23
FIFO_NOP = 0x00
GL_TRIANGLES = 0
GL_QUADS = 1

VALID_TEX_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}


# Utility
def sanitize(name):
    return re.sub(r"[^a-zA-Z0-9_]", "_", name)


def nearest_valid_tex_size(n):
    if n is None:
        return None
    for s in sorted(VALID_TEX_SIZES):
        if s >= n:
            return s
    return 1024


def floattov16(f):
    return max(-32768, min(32767, int(f * (1 << 12)))) & 0xFFFF


def floattot16(f):
    return max(-32768, min(32767, int(f * (1 << 4)))) & 0xFFFF


def rgb_to_rgb15(r, g, b):
    return ((r >> 3) & 0x1F) | (((g >> 3) & 0x1F) << 5) | (((b >> 3) & 0x1F) << 10)


def pack_cmds(c1, c2=FIFO_NOP, c3=FIFO_NOP, c4=FIFO_NOP):
    return struct.pack("<I", (c4 << 24) | (c3 << 16) | (c2 << 8) | c1)


def to_s16(val):
    val = int(val) & 0xFFFF
    return val if val <= 32767 else val - 65536


# Texture helpers
def find_texture_size(png_path):
    """Read width/height from the PNG IHDR chunk (bytes 16-24)."""
    try:
        with open(png_path, "rb") as f:
            f.read(16)  # 8-byte sig + 8-byte chunk header
            w = struct.unpack(">I", f.read(4))[0]
            h = struct.unpack(">I", f.read(4))[0]
        return w, h
    except Exception:
        return None, None


# MTL / OBJ parsing
def parse_mtl(mtl_path):
    """Return {material_name: texture_filename} from a .mtl file."""
    mapping = {}
    if not mtl_path or not os.path.exists(mtl_path):
        return mapping
    current = None
    with open(mtl_path, "r", errors="replace") as f:
        for line in f:
            parts = line.strip().split()
            if not parts:
                continue
            if parts[0] == "newmtl":
                current = parts[1] if len(parts) > 1 else None
            elif parts[0] == "map_Kd" and current:
                mapping[current] = line.strip()[len("map_Kd") :].strip()
    return mapping


def parse_obj_with_materials(obj_path):
    """
    Parse an OBJ file.
    Returns (vertices, texcoords, groups, mtl_filename).
    groups = list of {'material': str, 'faces': list-of-faces}
    """
    vertices, texcoords = [], []
    groups = []
    current_mat = "__no_material__"
    current_faces = []
    mtl_file = None

    def flush():
        if current_faces:
            groups.append({"material": current_mat, "faces": list(current_faces)})

    with open(obj_path, "r", errors="replace") as f:
        for line in f:
            parts = line.split()
            if not parts:
                continue
            tok = parts[0]
            if tok == "mtllib":
                mtl_file = " ".join(parts[1:])
            elif tok == "v":
                vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif tok == "vt":
                texcoords.append((float(parts[1]), float(parts[2])))
            elif tok in ("o", "g"):
                flush()
                current_faces = []
            elif tok == "usemtl":
                flush()
                current_mat = parts[1] if len(parts) > 1 else "__no_material__"
                current_faces = []
            elif tok == "f":
                face = []
                for p in parts[1:]:
                    c = p.split("/")
                    vi = int(c[0]) - 1
                    vti = int(c[1]) - 1 if len(c) > 1 and c[1] else None
                    face.append((vi, vti))
                current_faces.append(face)
    flush()
    return vertices, texcoords, groups, mtl_file


# Display-list builder (single texture group)
def build_display_list_for_faces(
    faces,
    vertices,
    texcoords,
    tex_width,
    tex_height,
    vertex_color,
    scale,
    offset,
    blender_source,
):
    """Build a packed NDS display list for the given face list."""
    words = []
    has_uvs = any(vt is not None for face in faces for _, vt in face)
    ox, oy, oz = offset

    if vertex_color is not None:
        r, g, b = vertex_color
        words.append(pack_cmds(FIFO_COLOR))
        words.append(struct.pack("<I", rgb_to_rgb15(r, g, b)))

    triangles, quads = [], []
    for face in faces:
        if len(face) == 3:
            triangles.append(face)
        elif len(face) == 4:
            quads.append(face)
        else:
            for i in range(1, len(face) - 1):
                triangles.append([face[0], face[i], face[i + 1]])

    def emit_faces(face_list, prim_type):
        if not face_list:
            return
        words.append(pack_cmds(FIFO_BEGIN))
        words.append(struct.pack("<I", prim_type))

        for face in face_list:
            for v_idx, vt_idx in face:
                if has_uvs and vt_idx is not None and tex_width and tex_height:
                    u, v = texcoords[vt_idx]
                    # V is always flipped: OBJ/Blender V=0 is bottom, NDS V=0 is top.
                    u16 = floattot16(u * tex_width)
                    v16 = floattot16((1.0 - v) * tex_height)
                    words.append(pack_cmds(FIFO_TEXCOORD))
                    words.append(
                        struct.pack("<I", (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16))
                    )

                vx, vy, vz = vertices[v_idx]
                sx = (vx - ox) * scale
                sy = (vy - oy) * scale
                sz = (vz - oz) * scale

                words.append(pack_cmds(FIFO_VERTEX16))
                words.append(struct.pack("<I", (floattov16(sy) << 16) | floattov16(sx)))
                words.append(struct.pack("<I", floattov16(sz)))

    emit_faces(quads, GL_QUADS)
    emit_faces(triangles, GL_TRIANGLES)
    return words


# Keyframe optimiser
def optimize_keyframes(keyframes):
    if len(keyframes) <= 2:
        return keyframes
    optimized = [keyframes[0]]
    for i in range(1, len(keyframes) - 1):
        prev, curr, nxt = optimized[-1], keyframes[i], keyframes[i + 1]
        if (
            curr.get("rot", [0, 0, 0])
            == prev.get("rot", [0, 0, 0])
            == nxt.get("rot", [0, 0, 0])
        ) and (
            curr.get("pos", [0, 0, 0])
            == prev.get("pos", [0, 0, 0])
            == nxt.get("pos", [0, 0, 0])
        ):
            continue
        optimized.append(curr)
    optimized.append(keyframes[-1])
    return optimized


# ---------------------------------------------------------------------------
# MDL2 binary format
# ---------------------------------------------------------------------------
#
# Header      : 'MDL2' | u32 nodeCount | u32 animCount | u32 texCount
# Tex table   : texCount × { char[64] name | u16 w | u16 h | u8 isRGBA | u8[3] pad }
# Nodes       : nodeCount × { s32 pid | s32 px | s32 py | s32 pz |
#                              u32 subListCount |
#                              subListCount × { s32 texSlot | u32 dlSize | u32[dlSize] } }
# Animations  : animCount × { char[32] name | u32 duration |
#                              nodeCount × { u32 kfCount | Keyframe[kfCount] } }
#
# Keyframe (16 bytes)  : s32 time | s16 rx | s16 ry | s16 rz | s16 px | s16 py | s16 pz
#
# texSlot == -1 means "no texture bound for this sub-list".

TEX_TABLE_ENTRY_SIZE = 72  # 64 + 2 + 2 + 1 + 3


# Main converter
def convert_model_json(
    input_file,
    output_file,
    vertex_color=None,
    scale=None,
    target_size=4.0,
    center=True,
    blender_source=False,
    rgba_all=False,
    rgba_list=None,
):
    if rgba_list is None:
        rgba_list = []

    base_dir = os.path.dirname(os.path.abspath(input_file))
    model_name = os.path.splitext(os.path.basename(input_file))[0].replace("-", "_")
    safe_name = sanitize(model_name)

    with open(input_file, "r") as f:
        data = json.load(f)

    bin_out = (
        output_file
        if output_file.endswith(".bin")
        else os.path.splitext(output_file)[0] + ".bin"
    )
    out_dir = os.path.dirname(bin_out)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    # Parse every node's OBJ file
    all_verts = []
    node_parsed = []  # (vertices, texcoords, groups, mat_to_tex, obj_dir) per node

    for node in data["nodes"]:
        obj_path = os.path.join(base_dir, node["obj"])
        obj_dir = os.path.dirname(obj_path)
        v, vt, groups, mtl_file = parse_obj_with_materials(obj_path)

        if blender_source:
            v = [(x, z, -y) for x, y, z in v]

        mat_to_tex = {}
        if mtl_file:
            mat_to_tex = parse_mtl(os.path.join(obj_dir, mtl_file))

        node_parsed.append((v, vt, groups, mat_to_tex, obj_dir))
        all_verts.extend(v)

    if not all_verts:
        all_verts = [(0.0, 0.0, 0.0)]

    # Compute global scale / offset
    xs = [p[0] for p in all_verts]
    ys = [p[1] for p in all_verts]
    zs = [p[2] for p in all_verts]
    max_dim = max(max(xs) - min(xs), max(ys) - min(ys), max(zs) - min(zs))
    if scale is None:
        scale = (target_size / max_dim) if max_dim > 0 else 1.0
    if center:
        offset = ((min(xs) + max(xs)) / 2.0, min(ys), (min(zs) + max(zs)) / 2.0)
    else:
        offset = (0.0, 0.0, 0.0)

    # Collect all unique textures across all nodes (ordered)
    tex_registry = {}  # tex_key (basename) -> abs_path  (insertion-ordered in 3.7+)
    for v, vt, groups, mat_to_tex, obj_dir in node_parsed:
        for g in groups:
            tex_rel = mat_to_tex.get(g["material"])
            if tex_rel is None:
                continue
            tex_abs = os.path.join(obj_dir, tex_rel)
            tex_key = os.path.basename(tex_rel)
            if tex_key not in tex_registry:
                tex_registry[tex_key] = tex_abs

    tex_info = []
    tex_slot_map = {}

    print(f"\n  Texture table for {safe_name}:")
    for tex_key in sorted(tex_registry.keys()):
        tex_abs = tex_registry[tex_key]
        slot = len(tex_info)
        tex_slot_map[tex_key] = slot

        w, h = find_texture_size(tex_abs)
        if w is None:
            print(f"  [WARNING] Could not read '{tex_abs}', defaulting to 8×8")
            w, h = 8, 8
        w = nearest_valid_tex_size(w)
        h = nearest_valid_tex_size(h)
        is_rgba = rgba_all or any(req in tex_key for req in rgba_list)
        tex_info.append((tex_key, tex_abs, w, h, is_rgba))
        print(f"    [{slot}] {tex_key}  {w}×{h}  {'GL_RGBA' if is_rgba else 'GL_RGB'}")

    if not tex_info:
        print("    (no textures found - untextured model)")

    # Write MDL2 binary
    node_count = len(data["nodes"])
    anim_count = len(data["animations"])
    tex_count = len(tex_info)

    with open(bin_out, "wb") as out:
        # Header
        out.write(b"MDL2")
        out.write(struct.pack("<I I I", node_count, anim_count, tex_count))

        # Texture table (72 bytes each)
        for tex_key, _, w, h, is_rgba in tex_info:
            name_bytes = tex_key.encode("utf-8")[:63].ljust(64, b"\0")
            out.write(
                struct.pack("<64s H H B 3x", name_bytes, w, h, 1 if is_rgba else 0)
            )

        # Nodes
        for node_idx, node in enumerate(data["nodes"]):
            pid = node["parent"]
            origin = node.get("origin", [0, 0, 0])
            if blender_source:
                origin = [origin[0], origin[2], -origin[1]]

            bb_scale = 1.0 if blender_source else 16.0
            ox_n = to_s16(floattov16(((origin[0] / bb_scale) - offset[0]) * scale))
            oy_n = to_s16(floattov16(((origin[1] / bb_scale) - offset[1]) * scale))
            oz_n = to_s16(floattov16(((origin[2] / bb_scale) - offset[2]) * scale))

            v, vt, groups, mat_to_tex, obj_dir = node_parsed[node_idx]

            # Group faces by texture slot
            slot_faces = defaultdict(list)
            for g in groups:
                tex_rel = mat_to_tex.get(g["material"])
                if tex_rel is None:
                    slot = -1
                else:
                    slot = tex_slot_map.get(os.path.basename(tex_rel), -1)
                slot_faces[slot].extend(g["faces"])

            sub_lists = []
            for slot, faces in slot_faces.items():
                tw = tex_info[slot][2] if slot >= 0 else None
                th = tex_info[slot][3] if slot >= 0 else None
                words = build_display_list_for_faces(
                    faces, v, vt, tw, th, vertex_color, scale, offset, blender_source
                )
                sub_lists.append((slot, words))

            out.write(struct.pack("<i i i i I", pid, ox_n, oy_n, oz_n, len(sub_lists)))
            for slot, words in sub_lists:
                out.write(struct.pack("<i I", slot, len(words)))
                for w in words:
                    out.write(w)

        # Animations
        for anim_name, anim_data in data["animations"].items():
            name_bytes = anim_name.encode("utf-8")[:31].ljust(32, b"\0")
            anim_duration = int(float(anim_data["duration"]))
            out.write(struct.pack("<32s I", name_bytes, anim_duration))

            for node_id in range(node_count):
                str_id = str(node_id)
                optimized_kf = []
                if str_id in anim_data["tracks"]:
                    optimized_kf = optimize_keyframes(anim_data["tracks"][str_id])

                out.write(struct.pack("<I", len(optimized_kf)))

                for kf in optimized_kf:
                    rot = kf.get("rot", [0, 0, 0])
                    pos = kf.get("pos", [0, 0, 0])
                    if blender_source:
                        rot = [rot[0], rot[2], -rot[1]]
                        pos = [pos[0], pos[2], -pos[1]]

                    rx = to_s16((rot[0] / 360.0) * 32768)
                    ry = to_s16((rot[1] / 360.0) * 32768)
                    rz = to_s16((rot[2] / 360.0) * 32768)

                    bb_scale = 1.0 if blender_source else 16.0
                    px = to_s16(floattov16((pos[0] / bb_scale) * scale))
                    py = to_s16(floattov16((pos[1] / bb_scale) * scale))
                    pz = to_s16(floattov16((pos[2] / bb_scale) * scale))

                    time_frames = int(float(kf["time"]))
                    out.write(
                        struct.pack(
                            "<i h h h h h h", time_frames, rx, ry, rz, px, py, pz
                        )
                    )

    print(f"Written: {bin_out}")

    # Generate C++ header
    header_out = os.path.splitext(bin_out)[0] + ".hpp"
    with open(header_out, "w") as h:
        h.write("#pragma once\n")
        h.write("// Auto-generated by obj2model.py - DO NOT EDIT.\n")
        h.write(f"// Source: {os.path.basename(input_file)}\n")
        h.write(f"// Scale:  {scale:.6f}   Centred: {center}\n\n")
        h.write('#include "controllers/AnimationController.hpp"\n\n')

        # Animation enum
        h.write(f"enum Model_{safe_name}\n{{\n")
        for i, anim_name in enumerate(data["animations"].keys()):
            safe_anim = anim_name.upper().replace(".", "_").replace("-", "_")
            h.write(f"    MODEL_{safe_name.upper()}_{safe_anim} = {i},\n")
        h.write("};\n\n")

        # Texture slot enum
        h.write(f"enum Model_{safe_name}_TexSlot\n{{\n")
        for i, (tex_key, _, w, h_px, is_rgba) in enumerate(tex_info):
            safe_tex = sanitize(os.path.splitext(tex_key)[0]).upper()
            h.write(f"    MODEL_{safe_name.upper()}_TEX_{safe_tex} = {i},\n")
        h.write(f"    MODEL_{safe_name.upper()}_TEX_COUNT = {tex_count}\n}};\n\n")

        # Inline texture loader
        # The function calls AnimationController::loadTextures() with the
        # sizes baked in from the build
        h.write("// Call after AnimationController::loadModel() to upload textures.\n")
        h.write(f"// Pass bitmaps in the same order as Model_{safe_name}_TexSlot.\n")
        h.write(
            f"inline void {safe_name}_loadTextures(AnimationController& ctrl, const unsigned int* bitmaps[MODEL_{safe_name.upper()}_TEX_COUNT])\n"
        )
        h.write("{\n")
        if tex_count > 0:
            widths_str = ", ".join(str(t[2]) for t in tex_info)
            heights_str = ", ".join(str(t[3]) for t in tex_info)
            rgba_str = ", ".join("true" if t[4] else "false" for t in tex_info)
            h.write(f"    static const int widths[{tex_count}] = {{ {widths_str} }};\n")
            h.write(
                f"    static const int heights[{tex_count}] = {{ {heights_str} }};\n"
            )
            h.write(f"    static const bool isRGBA[{tex_count}] = {{ {rgba_str} }};\n")
            h.write(
                f"    ctrl.loadTextures(MODEL_{safe_name.upper()}_TEX_COUNT, bitmaps, widths, heights, isRGBA);\n"
            )
        h.write("}\n")

    _format_cpp_h_files([header_out])
    print(f"Written: {header_out}")

    # Generate texture list for grit
    tex_list_out = os.path.splitext(bin_out)[0] + "_textures.txt"
    with open(tex_list_out, "w") as t:
        t.write(f"# Textures for {safe_name} - run GRIT on each file listed below.\n\n")
        for i, (tex_key, tex_abs, w, h_px, is_rgba) in enumerate(tex_info):
            safe_var = sanitize(os.path.splitext(tex_key)[0])
            fmt = "GL_RGBA" if is_rgba else "GL_RGB"
            t.write(f"# Slot {i}  ({w}×{h_px})  {fmt}  var: {safe_var}Bitmap\n")
            t.write(f"{tex_abs}\n\n")
    print(f"Written: {tex_list_out}")


# Dispatcher (single .obj path kept for non-animated models)
def _format_cpp_h_files(paths: list[str]) -> None:
    clang_format = shutil.which("clang-format")
    if clang_format is None:
        return
    inputs = [p for p in paths if os.path.exists(p)]
    if not inputs:
        return
    try:
        subprocess.run([clang_format, "--style=file", "-i", *inputs], check=True)
    except subprocess.CalledProcessError:
        print("Warning: clang-format failed on generated files.", file=sys.stderr)


def convert(input_file, output_file, config):
    vertex_color = config.get("color")
    if isinstance(vertex_color, list) and len(vertex_color) == 3:
        vertex_color = tuple(vertex_color)

    scale = config.get("scale")
    target_size = config.get("target_size", 4.0)
    center = config.get("center", True)
    if config.get("no_center"):
        center = False
    blender_source = config.get("source_blender", False)

    raw_rgba_list = config.get("rgba_list", [])
    if isinstance(raw_rgba_list, str):
        rgba_list = [x.strip() for x in raw_rgba_list.split(",") if x.strip()]
    elif isinstance(raw_rgba_list, list):
        rgba_list = [str(x).strip() for x in raw_rgba_list]
    else:
        rgba_list = []

    rgba_all = config.get("rgba", False)

    if input_file.endswith(".json"):
        convert_model_json(
            input_file,
            output_file,
            vertex_color=vertex_color,
            scale=scale,
            target_size=target_size,
            center=center,
            blender_source=blender_source,
            rgba_all=rgba_all,
            rgba_list=rgba_list,
        )
    else:
        raise NotImplementedError(
            "Single-OBJ conversion not implemented in this version. "
            "Use a JSON descriptor for multi-node / multi-texture models."
        )


# CLI
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert a JSON-described multi-node model to NDS MDL2 format"
    )
    parser.add_argument("input", help="Input .json model descriptor")
    parser.add_argument("output", help="Output .bin file path")
    parser.add_argument("--color", nargs=3, type=int, metavar=("R", "G", "B"))
    parser.add_argument("--scale", type=float, default=None)
    parser.add_argument("--target-size", type=float, default=4.0)
    parser.add_argument("--no-center", action="store_true")
    parser.add_argument("--source-blender", action="store_true")
    parser.add_argument(
        "--rgba", action="store_true", help="Treat all textures as GL_RGBA"
    )
    parser.add_argument(
        "--rgba-list",
        type=str,
        default="",
        help="Comma-separated list of texture filename substrings to treat as GL_RGBA",
    )
    args = parser.parse_args()

    cli_config = {
        "color": args.color,
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "rgba": args.rgba,
        "rgba_list": args.rgba_list,
    }
    convert(args.input, args.output, cli_config)
