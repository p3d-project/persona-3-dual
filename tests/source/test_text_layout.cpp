/**
 * @file test_text_layout.cpp
 * @brief Unit tests for the TextLayout utilities.
 *
 * @author Oles Gedz (olesgedz)
 */

#include "systems/TextLayout.hpp"

#include <gtest/gtest.h>

class TextLayoutTest : public testing::Test
{
  protected:
    Font font{};
    Text text{};

    void SetUp() override
    {
        font.lineHeight = 10;
        font.boldLoaded = true;
        for (int c = 0; c < 256; ++c)
        {
            font.glyphs[c].width = 4;
            font.glyphs[c].height = 8;
            font.boldGlyphs[c].width = 6;
            font.boldGlyphs[c].height = 8;
        }
        font.glyphs['-'].width = 2;
        text.font = &font;
        text.maxX = 20;
        text.letterSpacing = 1;
        text.spaceWidth = 3;
        text.hyphenate = true;
    }
};

TEST_F(TextLayoutTest, ExactFitDoesNotReserveHyphenSpace)
{
    text.content = "abcd";
    text.maxX = 19;
    for (text.cursorPos = 0; text.cursorPos < 4; ++text.cursorPos)
    {
        EXPECT_FALSE(TextLayout::prepareGlyph(text, font.glyphs['a']));
        EXPECT_EQ(text.cursorY, 0);
        text.cursorX += 5;
    }
}

TEST_F(TextLayoutTest, WrapIncludesSpaceButNotTrailingLetterSpacing)
{
    text.content = " ab";
    text.cursorX = 8;
    EXPECT_FALSE(TextLayout::shouldWrapWord(text)); // 8 + 3 + 9 = 20
    text.cursorX = 9;
    EXPECT_TRUE(TextLayout::shouldWrapWord(text));
}

TEST_F(TextLayoutTest, MeasurementTracksStylesAndIgnoresColourInstructions)
{
    text.content = "a\xFF\x02\x01"
                   "b\xFF\x01\xF3"
                   "c\xFF\x02\xFF"
                   "d";
    EXPECT_EQ(TextLayout::wordWidth(text, 0), 23);
    EXPECT_FALSE(text.bold);
}

TEST_F(TextLayoutTest, MissingBoldFontUsesRegularWidths)
{
    font.boldLoaded = false;
    text.content = "\xFF\x02\x01"
                   "ab";
    EXPECT_EQ(TextLayout::wordWidth(text, 0), 9);
}

TEST_F(TextLayoutTest, FormattingArgumentCanContainWhitespaceByte)
{
    text.content = "a\xFF\x01\x20"
                   "b";
    EXPECT_EQ(TextLayout::wordWidth(text, 0), 9);
}

TEST_F(TextLayoutTest, IncompleteInstructionIsNotMeasuredAsGlyphs)
{
    text.content = "ab\xFF\x02";
    EXPECT_EQ(TextLayout::wordWidth(text, 0), 9);
}

TEST_F(TextLayoutTest, HyphenFitsBeforeForcedBreak)
{
    text.content = "abcdefgh";
    text.cursorPos = 3;
    text.cursorX = 15;
    const int hyphenX = text.cursorX;
    EXPECT_TRUE(TextLayout::prepareGlyph(text, font.glyphs['d']));
    EXPECT_LE(hyphenX + font.glyphs['-'].width, text.maxX);
    EXPECT_EQ(text.cursorX, 0);
    EXPECT_EQ(text.cursorY, 10);
    EXPECT_EQ(text.cursorPos, 3);
}

TEST_F(TextLayoutTest, NoHyphenWhenRestOfWordFits)
{
    text.content = "abcd";
    text.cursorPos = 3;
    text.cursorX = 15;
    text.maxX = 19;
    EXPECT_FALSE(TextLayout::prepareGlyph(text, font.glyphs['d']));
    EXPECT_EQ(text.cursorY, 0);
}

TEST_F(TextLayoutTest, NonDialogueTextDoesNotForceBreakOrHyphenate)
{
    text.hyphenate = false;
    text.content = "abcdefgh";
    text.cursorPos = 3;
    text.cursorX = 18;
    EXPECT_FALSE(TextLayout::prepareGlyph(text, font.glyphs['d']));
    EXPECT_EQ(text.cursorX, 18);
    EXPECT_EQ(text.cursorY, 0);
}

TEST_F(TextLayoutTest, MissingHyphenStillBreaksOverlongWord)
{
    font.glyphs['-'].width = 0;
    text.content = "abcdefgh";
    text.cursorPos = 4;
    text.cursorX = 20;
    EXPECT_FALSE(TextLayout::prepareGlyph(text, font.glyphs['e']));
    EXPECT_EQ(text.cursorX, 0);
    EXPECT_EQ(text.cursorY, 10);
}

TEST_F(TextLayoutTest, MissingHyphenDoesNotReserveExtraSpacing)
{
    font.glyphs['-'].width = 0;
    text.content = "abcdefgh";
    text.cursorPos = 3;
    text.cursorX = 15;
    text.maxX = 19;
    EXPECT_FALSE(TextLayout::prepareGlyph(text, font.glyphs['d']));
    EXPECT_EQ(text.cursorY, 0);
}

TEST_F(TextLayoutTest, SkippingAcrossPagesPreservesEveryCharacter)
{
    text.content = "abcdefgh";
    text.maxY = 10;
    std::string rendered;
    int pages = 1;
    int hyphens = 0;
    int steps = 0;
    while (text.cursorPos < static_cast<int>(text.content.size()) && ++steps < 50)
    {
        const auto c = static_cast<unsigned char>(text.content[text.cursorPos]);
        hyphens += TextLayout::prepareGlyph(text, font.glyphs[c]);
        if (text.pageBreak)
        {
            EXPECT_EQ(text.cursorPos, static_cast<int>(rendered.size()));
            text.cursorX = text.startX;
            text.cursorY = text.startY;
            text.pageBreak = false;
            ++pages;
            continue;
        }
        ASSERT_LE(text.cursorX + font.glyphs[c].width, text.maxX);
        ASSERT_LE(text.cursorY + font.lineHeight, text.maxY);
        rendered += c;
        text.cursorX += font.glyphs[c].width + text.letterSpacing;
        ++text.cursorPos;
    }
    EXPECT_EQ(rendered, text.content);
    EXPECT_EQ(pages, 3);
    EXPECT_EQ(hyphens, 2);
}

TEST_F(TextLayoutTest, ExplicitNewlinePausesBeforeNextGlyph)
{
    text.content = "a\nb";
    text.maxY = 10;
    text.cursorPos = 2;
    TextLayout::newLine(text);
    EXPECT_FALSE(TextLayout::prepareGlyph(text, font.glyphs['b']));
    EXPECT_TRUE(text.pageBreak);
    EXPECT_EQ(text.cursorPos, 2);
}
