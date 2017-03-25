// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/GlyphsFile.hpp"
#include "utility/utility.hpp"
#include "test/utility.hpp"
using namespace GraphicsLayer;
using namespace sb::test;
///////////////////////////////////

////////////////////////////////////////
// Google Test
#include "gtest/gtest.h"
////////////////////////////////////////

////////////////////////////////////////
// GlyphsFileTest
////////////////////////////////////////
class GlyphsFileTest : public ::testing::Test
{
public:
    GlyphsFileTest()
    {
        srand(0);

        unsigned short numGlyphs = 50 + rand() % 100;
        glyphs.resize(numGlyphs);

        enum Style : unsigned char
            {
                NORMAL = 1,
                BOLD = 1 << 1,
                ITALIC = 1 << 2,
            };

        using S = FontSample::Style;
        std::vector<S> styles =
        {
            S::NORMAL,
            S::BOLD,
            S::ITALIC,
        };

        for(FontSample::Glyph& g : glyphs)
        {
            g.character = rand();
            g.width = 10 + rand() % 10;
            g.height = 10 + rand() % 10;
            g.style = styles[rand() % styles.size()];
            g.sum = rand();
            g.pointSize = rand();
            g.data.resize(g.width * g.height * g.bytesPerPixel);
            std::generate(g.data.begin(), g.data.end(), rand);
        }

    }

    ~GlyphsFileTest()
    {
        std::remove(std::string(filePath).c_str());
    }


    void expectEqAfterWriteRead(const std::vector<FontSample::Glyph>& f) const
    {
        EXPECT_TRUE(GlyphsFile::write(f, filePath));
        std::vector<FontSample::Glyph> readGlyphs;
        EXPECT_TRUE(GlyphsFile::read(readGlyphs, filePath));

        ::expectEq(f, readGlyphs);
    }

    void expectWriteReadTrue(std::vector<FontSample::Glyph>& f) const
    {
        EXPECT_TRUE(GlyphsFile::write(f, filePath));
        EXPECT_TRUE(GlyphsFile::read(f, filePath));
    }


    std::vector<FontSample::Glyph> glyphs;
    std::vector<FontSample::Glyph> emptyGlyphs;
    std::string filePath = "frameDump";
};

TEST_F(GlyphsFileTest, WriteRead)
{
    expectWriteReadTrue(glyphs);
}


TEST_F(GlyphsFileTest, WriteReadEmpty)
{
    expectWriteReadTrue(emptyGlyphs);
}


TEST_F(GlyphsFileTest, SameDataAfterRead)
{
    expectEqAfterWriteRead(glyphs);
}

TEST_F(GlyphsFileTest, SameDataAfterReadEmpty)
{
    expectEqAfterWriteRead(emptyGlyphs);
}
