import os
import re
import json
import struct
import argparse
import subprocess
import shutil
import sys
from collections import defaultdict

FIFO_COLOR = 0x20
FIFO_TEXCOORD = 0x22
FIFO_BEGIN = 0x40
FIFO_END = 0x41
FIFO_VERTEX16 = 0x23
FIFO_NOP = 0x00
GL_TRIANGLES = 0
GL_QUADS = 1
VALID_TEX_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}

# NDS VRAM budget for textures.
# Banks A+B+D = 3 * 128KB = 384KB (C is reserved for sub-BG)
NDS_VRAM_BUDGET = 384 * 1024

# Marks the start/end of a single environment's generated block inside
# environmentDb.cpp, so re-running the tool updates that block in place
# instead of duplicating or clobbering other environments in the file.
BLOCK_RE = re.compile(
    r"// === ENVIRONMENT: (\w+) BEGIN ===.*?// === ENVIRONMENT: \1 END ===\n?",
    re.DOTALL,
)


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


def sanitize(name):
    return re.sub(r"[^a-zA-Z0-9_]", "_", name)


def escape_c_string(s):
    if not s:
        return ""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def to_q20_12_raw(f):
    return round(f * 4096)


def floattov16(f):
    return max(-32768, min(32767, int(f * (1 << 12)))) & 0xFFFF


def floattot16(f):
    return int(f * (1 << 4)) & 0xFFFF


def to_signed_v16(f):
    return max(-32768, min(32767, int(f * (1 << 12))))


def to_signed_t16(f):
    return int(f * (1 << 4))


def rgb_to_rgb15(r, g, b):
    return ((r >> 3) & 0x1F) | (((g >> 3) & 0x1F) << 5) | (((b >> 3) & 0x1F) << 10)


def pack_cmds(c1, c2=FIFO_NOP, c3=FIFO_NOP, c4=FIFO_NOP):
    return struct.pack("<I", (c4 << 24) | (c3 << 16) | (c2 << 8) | c1)


def parse_mtl(mtl_path, extra_mapping=None):
    mapping = {}
    if mtl_path and os.path.exists(mtl_path):
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
    if extra_mapping:
        mapping.update(extra_mapping)
    return mapping


def parse_obj(obj_path):
    vertices, texcoords, groups = [], [], []
    current_mat, current_faces = "__no_material__", []
    is_billboard = False
    group_name = None

    def flush():
        if current_faces:
            groups.append(
                {
                    "material": current_mat,
                    "faces": list(current_faces),
                    "is_billboard": is_billboard,
                    # Raw "o"/"g" name in effect when these faces were parsed.
                    # Used as BillboardData::name for debugging at runtime.
                    "name": group_name,
                }
            )

    with open(obj_path, "r", errors="replace") as f:
        for line in f:
            parts = line.split()
            if not parts:
                continue
            tok = parts[0]
            if tok == "v":
                vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif tok == "vt":
                texcoords.append((float(parts[1]), float(parts[2])))
            elif tok in ("o", "g"):
                flush()
                group_name = parts[1] if len(parts) > 1 else None
                is_billboard = parts[1].startswith("BB_") if len(parts) > 1 else False
                current_faces = []
            elif tok == "usemtl":
                flush()
                current_mat, current_faces = (
                    parts[1] if len(parts) > 1 else "__no_material__"
                ), []
            elif tok == "f":
                face = []
                for p in parts[1:]:
                    c = p.split("/")
                    face.append(
                        (int(c[0]) - 1, int(c[1]) - 1 if len(c) > 1 and c[1] else None)
                    )
                current_faces.append(face)
    flush()
    return vertices, texcoords, groups


def compute_bounds(vertices):
    if not vertices:
        return (0, 0), (0, 0), (0, 0)
    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]
    return (min(xs), max(xs)), (min(ys), max(ys)), (min(zs), max(zs))


def convert_blender_zup(vertices):
    return [(x, z, -y) for x, y, z in vertices]


def build_display_list(
    faces,
    vertices,
    texcoords,
    scale,
    offset,
    tex_w,
    tex_h,
    blender_source=False,
    vertex_color=None,
):
    words = []
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
            for vi, vti in face:
                if vti is not None and tex_w and tex_h:
                    u, v_orig = texcoords[vti]
                    v = (1.0 - v_orig) if blender_source else v_orig
                    u16 = floattot16(u * tex_w)
                    v16 = floattot16(v * tex_h)
                    words.append(pack_cmds(FIFO_TEXCOORD))
                    words.append(
                        struct.pack("<I", (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16))
                    )
                vx, vy, vz = vertices[vi]
                sx = (vx - ox) * scale
                sy = (vy - oy) * scale
                sz = (vz - oz) * scale
                words.append(pack_cmds(FIFO_VERTEX16))
                words.append(struct.pack("<I", (floattov16(sy) << 16) | floattov16(sx)))
                words.append(struct.pack("<I", floattov16(sz)))
        # Explicit END closes the primitive group cleanly.
        # A subsequent BEGIN would also close it, but the last DL in draw()
        # has no following BEGIN, so the GPU would leave the group open until
        # glFlush().  Belt-and-suspenders here costs nothing.
        words.append(pack_cmds(FIFO_END))
        words.append(
            struct.pack("<I", 0)
        )  # END takes no parameters; pad with a NOP word

    emit_faces(quads, GL_QUADS)
    emit_faces(triangles, GL_TRIANGLES)
    return words


def nearest_valid_tex_size(n):
    if n is None:
        return None
    for s in sorted(VALID_TEX_SIZES):
        if s >= n:
            return s
    return 1024


def find_texture_size(png_path, max_tex_size=None):
    """
    Read the PNG header to get width/height.
    If max_tex_size is set AND Pillow is available, the PNG is resized in-place
    when either dimension exceeds the cap.  The returned (w, h) always reflects
    the on-disk size after any resize.
    """
    try:
        with open(png_path, "rb") as f:
            f.read(16)
            w = struct.unpack(">I", f.read(4))[0]
            h = struct.unpack(">I", f.read(4))[0]
    except Exception:
        return None, None

    if max_tex_size and (w > max_tex_size or h > max_tex_size):
        try:
            from PIL import Image

            img = Image.open(png_path)
            ratio = min(max_tex_size / w, max_tex_size / h)
            nw = nearest_valid_tex_size(max(8, int(w * ratio)))
            nh = nearest_valid_tex_size(max(8, int(h * ratio)))
            nw = min(nw, max_tex_size)
            nh = min(nh, max_tex_size)
            if nw != w or nh != h:
                print(
                    f"  [RESIZE] {os.path.basename(png_path)}: {w}x{h} -> {nw}x{nh}  "
                    f"({w*h*2//1024}KB -> {nw*nh*2//1024}KB)"
                )
                img.resize((nw, nh), Image.LANCZOS).save(png_path)
                return nw, nh
        except ImportError:
            print(
                "  [WARN] Pillow not installed - cannot resize textures.  "
                "Run:  pip install Pillow"
            )
        except Exception as e:
            print(f"  [WARN] Could not resize {png_path}: {e}")

    return w, h


def check_vram_budget(dl_groups, tex_paths, bpp=2, budget=NDS_VRAM_BUDGET):
    """
    Print a VRAM usage table and return True if the textures fit in `budget` bytes.
    bpp=2 for GL_RGBA (16-bit).  Set bpp=1 for 8-bit palette textures.
    """
    total = 0
    print(f"\n  {'Slot':<4} {'Texture':<30} {'Size':<10} {'Bytes':>8}  {'Cumul':>8}")
    print(f"  {'-'*4} {'-'*30} {'-'*10} {'-'*8}  {'-'*8}")
    for i, (tex_key, _, tw, th) in enumerate(dl_groups):
        tw = tw or 8
        th = th or 8
        size = tw * th * bpp
        total += size
        flag = " <-- HUGE" if size >= 128 * 1024 else ""
        print(
            f"  {i:<4} {tex_key:<30} {tw}x{th:<6} {size:>8,}  {total//1024:>6}KB{flag}"
        )

    used_pct = 100.0 * total / budget
    ok = total <= budget
    status = "OK" if ok else "*** OVERFLOW ***"
    print(
        f"\n  Total:  {total:>10,} bytes  ({total//1024} KB / {budget//1024} KB)  "
        f"{used_pct:.1f}%  [{status}]"
    )
    if not ok:
        excess = total - budget
        print(f"  Excess: {excess:>10,} bytes  ({excess//1024} KB)")
        print("  Hint:   re-run with  --max-tex-size 64  (or add more VRAM banks)")
    print()
    return ok


def find_project_root(output_dir, explicit=None):
    """
    Locate the project root (the directory containing both 'source' and 'data'),
    so environmentDb.cpp can be found/written at <root>/source/data/environmentDb.cpp
    regardless of where output_dir happens to be.

    Preference order:
    1. `explicit` (config["project_dir"], if the caller knows it)
    2. Walk upward from output_dir looking for a dir with both source/ and data/
    3. Fall back to assuming the fixed convention
       <root>/data/environments/<env> == output_dir
    """
    if explicit:
        return os.path.abspath(explicit)

    cur = os.path.abspath(output_dir)
    while True:
        if os.path.isdir(os.path.join(cur, "source")) and os.path.isdir(
            os.path.join(cur, "data")
        ):
            return cur
        parent = os.path.dirname(cur)
        if parent == cur:
            break
        cur = parent

    # Fallback: <root>/data/environments/<env> -> strip 3 levels
    guess = os.path.abspath(os.path.join(output_dir, "..", "..", ".."))
    return guess


def generate_entry_block(
    base_name,
    dl_groups,
    billboards,
    world_offset_x,
    world_offset_z,
    world_width,
    world_depth,
    bin_runtime_path,
):
    """Build the self-contained '// === ENVIRONMENT: <n> ... ===' text block
    that gets inserted into environmentDb.cpp for this environment, matching:

        struct EnvironmentTexture { const char* name; int width; int height;
                                     const unsigned int* bitmap; };
        struct BillboardData { const char* name; v16 x, y, z;
                                v16 halfWidth; v16 halfHeight; int texSlot;
                                short u0, v0; short u1, v1; };
       struct EnvironmentDbEntry { const char* name; const char* binaryFile;
                             ae::q20_12_t worldOffsetX; ae::q20_12_t worldOffsetZ;
                             ae::q20_12_t worldWidth; ae::q20_12_t worldDepth;
                             int textureCount; const EnvironmentTexture* textures;
                             int billboardCount; const BillboardData* billboards; };

    `bitmap` is left NULL here; the loader/GRIT pipeline fills it in at runtime.
    """
    lines = [f"// === ENVIRONMENT: {base_name} BEGIN ==="]

    # EnvironmentTexture[]
    n_tex = len(dl_groups)
    textures_symbol = "NULL"
    if n_tex > 0:
        textures_symbol = f"{base_name}_textures"
        lines.append(f"static const EnvironmentTexture {textures_symbol}[{n_tex}] = {{")
        for tex_key, _, tw, th in dl_groups:
            if tex_key == "__no_texture__":
                lines.append(f"    {{ NULL, {tw or 0}, {th or 0}, NULL }},")
            else:
                tex_base = sanitize(os.path.splitext(tex_key)[0])
                lines.append(
                    f'    {{ "{tex_base}.img.bin", {tw}, {th}, NULL }}, // {tex_key}'
                )
        lines.append("};")
        lines.append("")

    # BillboardData[]
    n_bb = len(billboards)
    billboards_symbol = "NULL"
    if n_bb > 0:
        billboards_symbol = f"{base_name}_billboards"
        lines.append(f"static const BillboardData {billboards_symbol}[{n_bb}] = {{")
        for b in billboards:
            name = escape_c_string(b.get("name") or "")
            # x, y, z, hw, hh are Q4.12 (matches libnds v16)
            # u0, v0, u1, v1 are Q12.4 (matches libnds t16)
            lines.append(
                f'    {{ "{name}", '
                f'ae::q4_12_t::from_raw_value({to_signed_v16(b["cx"])}), '
                f'ae::q4_12_t::from_raw_value({to_signed_v16(b["cy"])}), '
                f'ae::q4_12_t::from_raw_value({to_signed_v16(b["cz"])}), '
                f'ae::q4_12_t::from_raw_value({to_signed_v16(b["hw"])}), '
                f'ae::q4_12_t::from_raw_value({to_signed_v16(b["hh"])}), '
                f'{b["slot"]}, '
                f'ae::q12_4_t::from_raw_value({b["u0_16"]}), '
                f'ae::q12_4_t::from_raw_value({b["v0_16"]}), '
                f'ae::q12_4_t::from_raw_value({b["u1_16"]}), '
                f'ae::q12_4_t::from_raw_value({b["v1_16"]}) }},'
            )
        lines.append("};")
        lines.append("")

    # EnvironmentDbEntry
    lines.append(f"const EnvironmentDbEntry {base_name}EnvironmentDbEntry = {{")
    lines.append(f'    "{base_name}",')
    lines.append(f'    "{bin_runtime_path}",')

    lines.append(
        f"ae::q20_12_t::from_raw_value({to_q20_12_raw(world_offset_x)}), "
        f"ae::q20_12_t::from_raw_value({to_q20_12_raw(world_offset_z)}), "
        f"ae::q20_12_t::from_raw_value({to_q20_12_raw(world_width)}), "
        f"ae::q20_12_t::from_raw_value({to_q20_12_raw(world_depth)}),"
    )
    lines.append(f"    {n_tex}, {textures_symbol},")
    lines.append(f"    {n_bb}, {billboards_symbol},")
    lines.append("};")
    lines.append(f"// === ENVIRONMENT: {base_name} END ===")
    return "\n".join(lines) + "\n"


def update_environment_db(db_path, base_name, new_block, struct_header):
    """Insert or replace this environment's block in environmentDb.cpp, then
    regenerate the g_environmentDb[] lookup array covering every block found."""
    os.makedirs(os.path.dirname(db_path), exist_ok=True)

    blocks = {}
    order = []

    if os.path.exists(db_path):
        with open(db_path, "r") as f:
            content = f.read()
        for m in BLOCK_RE.finditer(content):
            name = m.group(1)
            blocks[name] = m.group(0).rstrip("\n")
            order.append(name)

    if base_name not in blocks:
        order.append(base_name)
    blocks[base_name] = new_block.rstrip("\n")

    with open(db_path, "w") as f:
        f.write("// Auto-generated by obj2environment.py\n")
        f.write(
            "// DO NOT EDIT the ENVIRONMENT blocks by hand - regenerate from source .obj files.\n"
        )
        f.write(f'#include "{struct_header}"\n\n')

        for name in order:
            f.write(blocks[name])
            f.write("\n\n")

        f.write(f"const EnvironmentDbEntry* g_environmentDb[{len(order)}] = {{\n")
        for name in order:
            f.write(f"    &{name}EnvironmentDbEntry,\n")
        f.write("};\n\n")
        f.write(f"const int g_environmentDbCount = {len(order)};\n")

    _format_cpp_h_files([db_path])


def convert(obj_path, output_dir, config):
    scale = config.get("scale", None)
    target_size = config.get("target_size", 4.0)
    center = config.get("center", True)
    if config.get("no_center"):
        center = False
    blender_source = config.get("source_blender", False)
    max_tex_size = config.get("max_tex_size", None)

    vertex_color = config.get("color", [255, 255, 255])
    if isinstance(vertex_color, list) and len(vertex_color) == 3:
        vertex_color = tuple(vertex_color)
    elif vertex_color is None:
        vertex_color = (255, 255, 255)

    extra_mapping = None
    if config.get("mapping"):
        try:
            with open(config["mapping"]) as f:
                extra_mapping = json.load(f)
        except Exception:
            pass

    obj_path = os.path.abspath(obj_path)
    output_dir = os.path.abspath(output_dir)
    os.makedirs(output_dir, exist_ok=True)

    project_root = find_project_root(output_dir, config.get("project_dir"))
    db_path = os.path.abspath(
        config.get("db_path")
        or os.path.join(project_root, "source", "data", "environmentDb.cpp")
    )
    struct_header = config.get("struct_header", "data/environmentDb.hpp")
    bin_runtime_prefix = config.get("bin_runtime_prefix", "") or ""

    obj_dir = os.path.dirname(obj_path)
    base_name = sanitize(os.path.splitext(os.path.basename(obj_path))[0])

    mtl_path = None
    with open(obj_path, "r", errors="replace") as f:
        for line in f:
            parts = line.strip().split()
            if parts and parts[0] == "mtllib":
                mtl_path = os.path.join(obj_dir, " ".join(parts[1:]))
                break
    mat_to_tex = parse_mtl(mtl_path, extra_mapping)

    vertices, texcoords, groups = parse_obj(obj_path)
    if blender_source:
        vertices = convert_blender_zup(vertices)

    (xmin, xmax), (ymin, ymax), (zmin, zmax) = compute_bounds(vertices)
    max_dim = max(xmax - xmin, ymax - ymin, zmax - zmin)
    if scale is None:
        scale = (target_size / max_dim) if max_dim > 0 else 1.0

    if center:
        ox = (xmin + xmax) / 2.0
        oy = ymin
        oz = (zmin + zmax) / 2.0
    else:
        ox = oy = oz = 0.0
    offset = (ox, oy, oz)

    trans_min_x = (xmin - ox) * scale
    trans_max_x = (xmax - ox) * scale
    trans_min_z = (zmin - oz) * scale
    trans_max_z = (zmax - oz) * scale
    world_offset_x = -trans_min_x
    world_offset_z = -trans_min_z
    world_width = trans_max_x - trans_min_x
    world_depth = trans_max_z - trans_min_z

    merged, tex_paths = defaultdict(list), {}
    billboards = []

    for g in groups:
        mat, faces = g["material"], g["faces"]
        if not faces:
            continue
        tex_rel = mat_to_tex.get(mat)
        if tex_rel is None:
            tex_key = "__no_texture__"
        else:
            tex_abs = os.path.join(obj_dir, tex_rel)
            tex_key = os.path.basename(tex_rel)
            tex_paths[tex_key] = tex_abs

        if g.get("is_billboard"):
            v_idx, vt_idx = set(), set()
            for face in faces:
                for vi, vti in face:
                    v_idx.add(vi)
                    if vti is not None:
                        vt_idx.add(vti)
            b_verts = [vertices[i] for i in v_idx]
            (b_xmin, b_xmax), (b_ymin, b_ymax), (b_zmin, b_zmax) = compute_bounds(
                b_verts
            )
            cx = ((b_xmin + b_xmax) / 2.0 - ox) * scale
            cy = ((b_ymin + b_ymax) / 2.0 - oy) * scale
            cz = ((b_zmin + b_zmax) / 2.0 - oz) * scale
            hw = (max(b_xmax - b_xmin, b_zmax - b_zmin) / 2.0) * scale
            hh = ((b_ymax - b_ymin) / 2.0) * scale
            u_min, u_max, v_min, v_max = 0.0, 1.0, 0.0, 1.0
            if vt_idx:
                b_uvs = [texcoords[i] for i in vt_idx]
                u_min, u_max = min(uv[0] for uv in b_uvs), max(uv[0] for uv in b_uvs)
                v_min_raw, v_max_raw = min(uv[1] for uv in b_uvs), max(
                    uv[1] for uv in b_uvs
                )
                if blender_source:
                    v_min, v_max = 1.0 - v_max_raw, 1.0 - v_min_raw
                else:
                    v_min, v_max = v_min_raw, v_max_raw
            billboards.append(
                {
                    "name": (g.get("name") or "").removeprefix("BB_") or tex_key,
                    "cx": cx,
                    "cy": cy,
                    "cz": cz,
                    "hw": hw,
                    "hh": hh,
                    "tex_key": tex_key,
                    "u_min": u_min,
                    "u_max": u_max,
                    "v_min": v_min,
                    "v_max": v_max,
                }
            )
            if tex_key not in merged:
                merged[tex_key] = []
        else:
            merged[tex_key].extend(faces)

    dl_groups = []
    tex_to_slot = {}

    for tex_key, faces in merged.items():
        tex_abs = tex_paths.get(tex_key)
        # find_texture_size will resize the PNG in-place if max_tex_size is set
        tw, th = find_texture_size(tex_abs, max_tex_size) if tex_abs else (None, None)
        if tw is None or th is None:
            if tex_key != "__no_texture__":
                print(f"\n[WARNING] Could not find or read texture image: {tex_abs}")
                print("-> Defaulting to 8x8. Check your file path!")
            tw, th = 8, 8
        tw, th = nearest_valid_tex_size(tw), nearest_valid_tex_size(th)
        words = build_display_list(
            faces,
            vertices,
            texcoords,
            scale,
            offset,
            tw,
            th,
            blender_source=blender_source,
            vertex_color=vertex_color,
        )
        tex_to_slot[tex_key] = len(dl_groups)
        dl_groups.append((tex_key, words, tw, th))

    # VRAM budget check
    bpp = 2  # GL_RGBA = 16bpp
    print(f"\n{'─'*60}")
    print(f"  VRAM budget check  (budget = {NDS_VRAM_BUDGET//1024} KB  =  banks A+B+D)")
    print(f"{'─'*60}")
    vram_ok = check_vram_budget(dl_groups, tex_paths, bpp=bpp, budget=NDS_VRAM_BUDGET)
    if not vram_ok:
        print(
            "  *** ACTION REQUIRED: textures will not all fit in VRAM.  "
            "Re-run with --max-tex-size 64 ***\n"
        )

    for b in billboards:
        b["slot"] = tex_to_slot[b["tex_key"]]
        tw, th = dl_groups[b["slot"]][2] or 8, dl_groups[b["slot"]][3] or 8
        b["u0_16"] = to_signed_t16(b["u_min"] * tw)
        b["v0_16"] = to_signed_t16(b["v_min"] * th)
        b["u1_16"] = to_signed_t16(b["u_max"] * tw)
        b["v1_16"] = to_signed_t16(b["v_max"] * th)

    billboards.sort(key=lambda b: b["slot"])

    n = len(dl_groups)
    bin_path = os.path.join(output_dir, f"{base_name}.bin")
    tex_list_path = os.path.join(output_dir, f"{base_name}_textures.txt")

    with open(bin_path, "wb") as f:
        f.write(b"ENV1")
        f.write(struct.pack("<I", n))
        for _, words, _, _ in dl_groups:
            f.write(struct.pack("<I", len(words)))
            for w in words:
                f.write(w)
    print(f"Written: {base_name}.bin")

    with open(tex_list_path, "w") as t:
        t.write(f"# Textures for {base_name} — run GRIT on each\n\n")
        for i, (tex_key, _, tw, th) in enumerate(dl_groups):
            abs_path = tex_paths.get(tex_key, f"<missing: {tex_key}>")
            t.write(
                f"# Slot {i}  ({tw}x{th})  var: {sanitize(os.path.splitext(tex_key)[0])}Bitmap\n{abs_path}\n\n"
            )
    print(f"Written: {base_name}_textures.txt")

    # --- environmentDb.cpp entry --------------------------------------
    bin_runtime_path = f"{bin_runtime_prefix}{base_name}.bin"
    block = generate_entry_block(
        base_name,
        dl_groups,
        billboards,
        world_offset_x,
        world_offset_z,
        world_width,
        world_depth,
        bin_runtime_path,
    )
    update_environment_db(db_path, base_name, block, struct_header)
    print(f"Updated: {db_path} (entry: {base_name})")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert a multi-texture OBJ to an NDS C header."
    )
    parser.add_argument("input")
    parser.add_argument("output_dir")
    parser.add_argument("--scale", type=float, default=None)
    parser.add_argument("--target-size", type=float, default=4.0)
    parser.add_argument("--no-center", action="store_true")
    parser.add_argument("--source-blender", action="store_true")
    parser.add_argument("--mapping", type=str, default=None)
    parser.add_argument("--rgba", action="store_true")
    parser.add_argument("--rgba-list", type=str, default="")
    parser.add_argument(
        "--color", nargs=3, type=int, metavar=("R", "G", "B"), default=[255, 255, 255]
    )
    parser.add_argument(
        "--max-tex-size",
        type=int,
        default=None,
        help="Cap texture dimensions (e.g. 64 or 128). "
        "Requires Pillow; resizes PNGs in-place.",
    )
    parser.add_argument(
        "--project-dir",
        type=str,
        default=None,
        help="Explicit project root override (auto-detected otherwise by walking "
        "up from output_dir looking for sibling source/ and data/ dirs)",
    )
    parser.add_argument(
        "--db-path",
        type=str,
        default=None,
        help="Override output path for environmentDb.cpp "
        "(default: <project_root>/source/data/environmentDb.cpp)",
    )
    parser.add_argument(
        "--struct-header",
        type=str,
        default="data/environmentDb.hpp",
        help="Include path written at the top of environmentDb.cpp "
        "(default: data/environmentDb.h)",
    )
    parser.add_argument(
        "--bin-runtime-prefix",
        type=str,
        default="",
        help="Prefix prepended to the .bin filename stored in binaryFile, "
        "e.g. 'environments/<env>/' if the loader needs a full relative path "
        "(default: none, just '<n>.bin')",
    )
    args = parser.parse_args()

    cli_config = {
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "mapping": args.mapping,
        "rgba": args.rgba,
        "rgba_list": args.rgba_list,
        "color": args.color,
        "max_tex_size": args.max_tex_size,
        "project_dir": args.project_dir,
        "db_path": args.db_path,
        "struct_header": args.struct_header,
        "bin_runtime_prefix": args.bin_runtime_prefix,
    }
    convert(args.input, args.output_dir, cli_config)
