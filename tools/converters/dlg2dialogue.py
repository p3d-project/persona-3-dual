import re
import sys
import os
import argparse
import subprocess
import shutil
from dataclasses import dataclass, field
from typing import Optional

MAX_BG_SLOTS = 4


@dataclass
class Selection:
    label: str
    target: str


@dataclass
class DialogueLine:
    index: int
    label: str
    character: str
    text: str
    bg: str
    bg_index: int = 0
    selections: list[Selection] = field(default_factory=list)
    prev_index: Optional[int] = None
    next_index: Optional[int] = None


@dataclass
class Interaction:
    name: str
    description: str
    lines: list[DialogueLine] = field(default_factory=list)
    label_map: dict[str, int] = field(default_factory=dict)
    bg_order: list[str] = field(default_factory=list)


class ParseError(Exception):
    def __init__(self, line_num: int, msg: str):
        super().__init__(f"Line {line_num}: {msg}")


class DialogueParser:
    def __init__(self, src: str):
        self.src = src
        self.lines_raw = src.splitlines()
        self.pos = 0
        self.interactions: list[Interaction] = []
        self.scene_bg_defaults: dict[str, str] = {}

    def _raw_line(self):
        while self.pos < len(self.lines_raw):
            n = self.pos + 1
            raw = self.lines_raw[self.pos]
            self.pos += 1
            s = raw.strip()
            if not s or s.startswith("#"):
                continue
            return n, s
        return None, None

    def _peek(self):
        saved = self.pos
        n, s = self._raw_line()
        self.pos = saved
        return n, s

    def _parse_interaction(self, interaction: Interaction):
        local_bg: dict[str, str] = {}
        pending_label: Optional[str] = None
        auto_idx = 0
        pending_jumps: list[tuple[int, str]] = []
        dialogue_ends = {}
        counter = 0

        def resolve_bg(char, override):
            if override:
                bg = override
            elif char in local_bg:
                bg = local_bg[char]
            elif char in self.scene_bg_defaults:
                bg = self.scene_bg_defaults[char]
            else:
                safe = re.sub(r"[^A-Za-z0-9_]", "", char)
                bg = f"bg{safe}"
            if bg not in interaction.bg_order:
                interaction.bg_order.append(bg)
            return bg

        def next_auto_label():
            nonlocal auto_idx
            lbl = f"line_{auto_idx}"
            auto_idx += 1
            return lbl

        def parse_commands(text):
            for m in re.finditer(r"\[([^\]]+)\]", text):
                cmd = m.group(1)
                if cmd.lower().startswith("c "):
                    color = cmd[2:].strip()
                    if color.lower() == "reset":
                        text = text.replace(m.group(0), r"\xFF\x01\xFF")
                    else:
                        code = hex(int(color))[2:].zfill(2)
                        text = text.replace(m.group(0), rf"\xFF\x01\x{code}")
            return text

        def add_line(char, text, bg):
            nonlocal pending_label
            label = pending_label or next_auto_label()
            pending_label = None
            idx = len(interaction.lines)
            if label in interaction.label_map:
                raise ParseError(
                    0, f"Duplicate label '@{label}' in '{interaction.name}'"
                )
            bg_index = interaction.bg_order.index(bg)
            text = parse_commands(text)
            dl = DialogueLine(
                index=idx,
                label=label,
                character=char,
                text=text,
                bg=bg,
                bg_index=bg_index,
            )
            interaction.lines.append(dl)
            interaction.label_map[label] = idx
            return dl

        while True:
            pn, ps = self._peek()
            if ps is None:
                break
            if re.match(r"^\[interaction\s*:", ps, re.IGNORECASE):
                break

            n, s = self._raw_line()

            if re.match(r"^==.*==$", s):
                continue
            if m := re.match(r"^@bg\s+(\S+)\s+(.+)$", s):
                local_bg[m.group(1)] = m.group(2)
                continue
            if m := re.match(r"^@(\w+)$", s):
                pending_label = m.group(1)
                continue
            if s.lower() == "[end]":
                dialogue_ends[counter - 1] = True
                continue
            if m := re.match(r"^\[jump\s+@(\w+)\]$", s, re.IGNORECASE):
                if interaction.lines:
                    pending_jumps.append((len(interaction.lines) - 1, m.group(1)))
                continue

            if s.lower() == "[choice]":
                if not interaction.lines:
                    raise ParseError(n, "[choice] before any dialogue line")
                host = interaction.lines[-1]
                while True:
                    _, ps2 = self._peek()
                    if ps2 is None:
                        break
                    if re.match(r"^\[interaction\s*:", ps2, re.IGNORECASE):
                        break
                    if cm := re.match(r"^>\s*(.+?)\s*->\s*@(\w+)$", ps2):
                        self._raw_line()
                        host.selections.append(
                            Selection(label=cm.group(1), target=cm.group(2))
                        )
                    else:
                        break
                if not host.selections:
                    raise ParseError(
                        n, "[choice] block has no '> Option -> @label' entries"
                    )
                host.next_index = None
                continue

            if m := re.match(r"^([A-Za-z][A-Za-z0-9_ ]*)(?:\(([^)]+)\))?:\s*(.+)$", s):
                counter = counter + 1
                char = m.group(1).strip()
                bg = resolve_bg(char, m.group(2))
                add_line(char, m.group(3), bg)
                continue

            raise ParseError(n, f"Unrecognised syntax: '{s}'")

        lines = interaction.lines
        for i, dl in enumerate(lines):
            if i > 0:
                dl.prev_index = i - 1
            if dialogue_ends.get(i):
                dl.next_index = None
            elif not dl.selections and dl.next_index is None:
                if i + 1 < len(lines):
                    dl.next_index = i + 1

        for fi, tgt in pending_jumps:
            if tgt not in interaction.label_map:
                raise ParseError(
                    0, f"[jump] unknown label '@{tgt}' in '{interaction.name}'"
                )
            lines[fi].next_index = interaction.label_map[tgt]

        for dl in lines:
            for sel in dl.selections:
                if sel.target not in interaction.label_map:
                    raise ParseError(
                        0,
                        f"Choice '{sel.label}' -> unknown '@{sel.target}' in '{interaction.name}'",
                    )

    def parse(self) -> list[Interaction]:
        while True:
            n, s = self._raw_line()
            if s is None:
                break
            if re.match(r"^==.*==$", s):
                continue
            if m := re.match(r"^@bg\s+(\S+)\s+(.+)$", s):
                self.scene_bg_defaults[m.group(1)] = m.group(2)
                continue
            if m := re.match(
                r"^\[interaction\s*:\s*(\w+)(?:\s*\|\s*(.+))?\]$", s, re.IGNORECASE
            ):
                ia = Interaction(
                    name=m.group(1), description=(m.group(2) or "").strip()
                )
                self._parse_interaction(ia)
                self.interactions.append(ia)
                continue
            raise ParseError(
                n, f"Expected [interaction: name] or @bg directive, got: '{s}'"
            )
        # sort interactions, and each interaction's bg_order for consistent code output
        self.interactions.sort()
        for ia in self.interactions:
            ia.bg_order.sort()
            # remap bg_index now that bg_order is in its final sorted order
            for dl in ia.lines:
                dl.bg_index = ia.bg_order.index(dl.bg)
        return self.interactions


class CodeGenerator:
    def __init__(self, interactions: list[Interaction], scene_name: str):
        self.interactions = interactions
        self.scene = scene_name

    def _ptr(self, ia: Interaction, idx: Optional[int]) -> str:
        if idx is None:
            return "NULL"
        return f"&{self.scene}_{ia.name}_lines[{idx}]"

    def _esc(self, s: str) -> str:
        return s.replace("\\", "\\\\").replace('"', '\\"')

    def _vp(self, ia: Interaction) -> str:
        return f"{self.scene}_{ia.name}"

    def generate_h(self) -> str:
        out = []
        s = self.scene
        out += [
            "#pragma once",
            "// Auto-generated by dlg2dialogue.py",
            f"// Scene: {s}",
            "",
            '#include "core/structs.h"',
            "",
            f"extern int {s}_dialogue_bg_slot;",
        ]
        out.append("")

        for ia in self.interactions:
            vp = self._vp(ia)
            n = len(ia.lines)
            nb = len(ia.bg_order)
            desc = f" — {ia.description}" if ia.description else ""

            out += [
                f"// ── interaction: {ia.name}{desc}",
                f"extern const char* {vp}_bg_names[{nb}];",
                f"extern void (*{vp}_bg_loaders[{nb}])();",
                f"void {vp}_load_bg(int bgIndex);",
                f"extern Dialogue {vp}_lines[{n}];",
                f"void {vp}_init();",
                f"void {vp}_load();",
                f"inline Dialogue* {vp}_first()",
                "{",
                f"    return &{vp}_lines[0];",
                "}",
            ]
            for dl in ia.lines:
                if not dl.label.startswith("line_"):
                    out += [
                        f"inline Dialogue* {vp}_{dl.label}()",
                        "{",
                        f"    return &{vp}_lines[{dl.index}];",
                        "}",
                    ]
            out.append("")

        out += [f"inline void {s}_init_all()", "{"]
        for ia in self.interactions:
            out.append(f"    {self._vp(ia)}_init();")
        out += ["}", ""]

        return "\n".join(out)

    def generate_cpp(self) -> str:
        out = []
        s = self.scene
        out += [
            f'#include "{s}_dialogue.h"',
            '#include "core/globals.h"',
            "#include <nds.h>",
            "",
            f"int {s}_dialogue_bg_slot = -1;",
            "",
        ]

        for ia in self.interactions:
            vp = self._vp(ia)
            n = len(ia.lines)
            nb = len(ia.bg_order)

            bg_name_entries = ", ".join(f'"{bg}"' for bg in ia.bg_order)
            out.append(f"const char* {vp}_bg_names[{nb}] = {{ {bg_name_entries} }};")
            out.append(f"void (*{vp}_bg_loaders[{nb}])() = {{")
            for i, bg in enumerate(ia.bg_order):
                out.append("    nullptr,")
            out.append("};")
            out.append("")

            out += [
                f"void {vp}_load_bg(int bgIndex)",
                "{",
                f"    if (bgIndex >= 0 && bgIndex < {nb} && {vp}_bg_loaders[bgIndex])",
                "    {",
                f"        {vp}_bg_loaders[bgIndex]();",
                "    }",
                "}",
                "",
                f"void {vp}_load()",
                "{",
            ]

            for i, bg in enumerate(ia.bg_order):
                nm = ia.bg_order[i] if ia.bg_order else "myBg"
                out += [
                    f"    {vp}_bg_loaders[{i}] = []()",
                    "    {",
                    f'        GraphicAsset bg = genericGraphics->loadGraphic("graphics/dialogue/backgrounds/{nm}/{nm}");',
                    "        if (bg.tiles)",
                    "        {",
                    f"            dmaCopy(bg.tiles, bgGetGfxPtr({s}_dialogue_bg_slot), bg.tilesLen);",
                    f"            dmaCopy(bg.map, bgGetMapPtr({s}_dialogue_bg_slot), bg.mapLen);",
                    "            vramSetBankH(VRAM_H_LCD);",
                    "            dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);",
                    "            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);",
                    f"            RenderManager::GetInstance().showBg({s}_dialogue_bg_slot);",
                    "            genericGraphics->unloadGraphic(bg);",
                    "        }",
                    "    };",
                ]
            out += [
                f"    {vp}_init();",
                "}",
                "",
                f"Dialogue {vp}_lines[{n}];",
                "",
                f"void {vp}_init()",
                "{",
            ]

            sel_var_map: dict[tuple[int, int], str] = {}
            any_sel = False
            for dl in ia.lines:
                for si, sel in enumerate(dl.selections):
                    var = f"{vp}_sel_{dl.index}_{si}"
                    sel_var_map[(dl.index, si)] = var
                    target_idx = ia.label_map[sel.target]
                    out.append(
                        f'    DialogueSelection {var} = {{ "{self._esc(sel.label)}", false, {self._ptr(ia, target_idx)} }};'
                    )
                    any_sel = True
            if any_sel:
                out.append("")

            for dl in ia.lines:
                prev_ptr = self._ptr(ia, dl.prev_index)
                next_ptr = self._ptr(ia, dl.next_index)
                sel_block = (
                    f"{{ {', '.join(sel_var_map[(dl.index, si)] for si in range(len(dl.selections)))} }}"
                    if dl.selections
                    else "{}"
                )

                out.append(
                    f'    {vp}_lines[{dl.index}] = {{ "{self._esc(dl.character)}", "{self._esc(dl.text)}", '
                    f"{dl.bg_index}, {prev_ptr}, {next_ptr}, {sel_block} }};"
                )

            out += ["}", ""]
        return "\n".join(out)


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


def convert(input_file, output_base, config):
    try:
        with open(input_file, "r", encoding="utf-8") as f:
            src = f.read()
    except FileNotFoundError:
        print(f"ERROR: File not found: {input_file}", file=sys.stderr)
        sys.exit(1)

    base = output_base or os.path.splitext(os.path.basename(input_file))[0]
    scene_name = re.sub(r"[^A-Za-z0-9_]", "_", base).lower()

    try:
        p = DialogueParser(src)
        interactions = p.parse()
    except ParseError as e:
        print(f"PARSE ERROR: {e}", file=sys.stderr)
        sys.exit(1)

    if not interactions:
        print("WARNING: No [interaction] blocks found.", file=sys.stderr)
        return

    gen = CodeGenerator(interactions, scene_name)
    cpp = gen.generate_cpp()
    h = gen.generate_h()

    if config.get("stdout"):
        print("// ===== .h =====")
        print(h)
        print("// ===== .cpp =====")
        print(cpp)
    else:
        h_path = f"{base}_dialogue.h"
        cpp_path = f"{base}_dialogue.cpp"
        with open(h_path, "w", encoding="utf-8") as f:
            f.write(h)
        with open(cpp_path, "w", encoding="utf-8") as f:
            f.write(cpp.replace("\\\\", "\\"))
        _format_cpp_h_files([h_path, cpp_path])
        print(f"Written: {h_path} / {cpp_path}")


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("input")
    ap.add_argument("-o", "--output", default=None)
    ap.add_argument("--stdout", action="store_true")
    args = ap.parse_args()

    cli_config = {"stdout": args.stdout}
    convert(args.input, args.output, cli_config)
