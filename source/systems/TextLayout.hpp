/**
 * @file TextLayout.hpp
 * @brief Provides text layout utilities for measuring and positioning text.
 *
 * Contains functions for applying text styles, measuring word widths, handling line breaks,
 * and determining when pages are full or words should wrap.
 *
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include "types/TextTypes.hpp"

namespace TextLayout
{
inline void applyStyle(Font& font, unsigned char flags, bool& bold, bool& italic, bool& underline)
{
    bold = flags != TextInstruction::Reset && (flags & TextInstruction::StyleBold) && font.boldLoaded;
    italic = flags != TextInstruction::Reset && (flags & TextInstruction::StyleItalic);
    underline = flags != TextInstruction::Reset && (flags & TextInstruction::StyleUnderline);
}

// Measure visible pixels, not trailing letter spacing or embedded instruction bytes.
inline int wordWidth(const Text& text, std::size_t position)
{
    bool bold = text.bold;
    bool italic = text.italic;
    bool underline = text.underline;
    int advance = 0;
    int width = 0;
    while (position < text.content.size())
    {
        const auto c = static_cast<unsigned char>(text.content[position++]);
        if (c == ' ' || c == '\n')
        {
            break;
        }
        if (c == TextInstructionMarker)
        {
            if (position + 1 >= text.content.size())
            {
                break;
            }
            const auto instruction = static_cast<unsigned char>(text.content[position++]);
            const auto argument = static_cast<unsigned char>(text.content[position++]);
            if (instruction == TextInstruction::StyleChange)
            {
                applyStyle(*text.font, argument, bold, italic, underline);
            }
            continue;
        }
        const Glyph& glyph = bold ? text.font->boldGlyphs[c] : text.font->glyphs[c];
        if (glyph.width == 0)
        {
            advance += text.spaceWidth;
            width = advance;
        }
        else
        {
            const int slant = italic ? ((glyph.height - 1) * 64) >> 8 : 0;
            width = advance + glyph.width + slant;
            advance += glyph.width + text.letterSpacing;
        }
    }
    return width;
}

inline void newLine(Text& text)
{
    text.cursorX = text.startX;
    text.cursorY += text.font->lineHeight + text.lineSpacing;
}

inline bool pageFull(Text& text)
{
    text.pageBreak = text.maxY > 0 && text.cursorY + text.font->lineHeight > text.maxY;
    return text.pageBreak;
}

inline bool shouldWrapWord(const Text& text)
{
    return text.cursorX + text.spaceWidth + wordWidth(text, text.cursorPos + 1) > text.maxX;
}

// Called before drawing a glyph. For dialogue, move the current glyph to a new
// line when the rest of its word will not fit. This is a visual character-based
// break, not linguistic hyphenation; the hyphen is drawn separately by the caller.
// Returns true when the caller should draw that hyphen at the old cursor.
inline bool prepareGlyph(Text& text, const Glyph& glyph)
{
    // Leave ordinary text wrapping and pagination to their existing paths.
    if (!text.hyphenate || pageFull(text))
    {
        return false;
    }

    const Glyph& hyphen = text.bold ? text.font->boldGlyphs['-'] : text.font->glyphs['-'];
    const int slant = text.italic ? ((hyphen.height - 1) * 64) >> 8 : 0;
    const int hyphenWidth = hyphen.width > 0 ? hyphen.width + slant : 0;
    const int remaining = wordWidth(text, text.cursorPos);
    const bool wordOverflows = text.cursorX + remaining > text.maxX;
    const int glyphSlant = text.italic ? ((glyph.height - 1) * 64) >> 8 : 0;
    const bool glyphOverflows = text.cursorX + glyph.width + glyphSlant > text.maxX;
    // Reserve room for a hyphen only when this break actually splits an overlong word.
    const bool needsHyphenSpace =
        hyphen.width > 0 && wordOverflows && text.cursorX + glyph.width + text.letterSpacing + hyphenWidth > text.maxX;
    if (text.cursorX > text.startX && (glyphOverflows || needsHyphenSpace))
    {
        const bool drawHyphen = hyphen.width > 0 && text.cursorX + hyphenWidth <= text.maxX;
        // Keep cursorPos unchanged so the caller retries this glyph on the new line/page.
        newLine(text);
        pageFull(text);
        return drawHyphen;
    }
    return false;
}
} // namespace TextLayout
