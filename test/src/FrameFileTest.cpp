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
#include "monitor/FrameFile.hpp"
#include "monitor/Frame.hpp"
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
// FrameFileTest
////////////////////////////////////////
class FrameFileTest : public ::testing::Test
{
public:
    FrameFileTest()
    {
        srand(0);
        frame.hasMiniMapMoved = rand();
        frame.miniMapX = rand();
        frame.miniMapY = rand();
        frame.miniMapScreenX = rand();
        frame.miniMapScreenY = rand();
        frame.miniMapScreenWidth = rand();
        frame.miniMapScreenHeight = rand();
        frame.hasViewUpdated = rand();
        frame.viewX = rand();
        frame.viewY = rand();
        frame.viewWidth = rand();
        frame.viewHeight = rand();
        frame.width = rand();
        frame.height = rand();
        frame.spriteDraws = std::make_shared<std::vector<SpriteDraw>>(10);
        frame.guiDraws = std::make_shared<std::vector<GuiDraw>>(10);
        frame.guiSpriteDraws = std::make_shared<std::vector<SpriteDraw>>(10);
        frame.textDraws = std::make_shared<std::vector<TextDraw>>(10);
        frame.rectDraws = std::make_shared<std::vector<RectDraw>>(10);
        frame.fileIo = std::make_shared<std::vector<FileIo>>(10);
        frame.miniMapDraws = std::make_shared<std::vector<MiniMapDraw>>(10);

        unsigned short screenPixelsWidth = rand() % 500;
        unsigned short screenPixelsHeight = rand() % 500;
        sb::utility::PixelFormat screenPixelsFormat = sb::utility::PixelFormat::RGBA;
        std::vector<unsigned char> screenPixels(screenPixelsWidth * screenPixelsHeight * sb::utility::getBytesPerPixel(screenPixelsFormat));
        std::generate(screenPixels.begin(), screenPixels.end(), rand);
        frame.screenPixels = std::make_shared<RawImage>(screenPixelsFormat, screenPixelsHeight, screenPixelsWidth, screenPixels);

        std::generate(frame.spriteDraws->begin(), frame.spriteDraws->end(), genSpriteDraw);
        std::generate(frame.guiDraws->begin(), frame.guiDraws->end(), genGuiDraw);
        std::generate(frame.guiSpriteDraws->begin(), frame.guiSpriteDraws->end(), genSpriteDraw);
        std::generate(frame.textDraws->begin(), frame.textDraws->end(), genTextDraw);
        std::generate(frame.rectDraws->begin(), frame.rectDraws->end(), genRectDraw);
//        std::generate(frame.fileIo->begin(), frame.fileIo->end(), genFileIo);
        std::generate(frame.miniMapDraws->begin(), frame.miniMapDraws->end(), genMiniMapDraw);

        emptyFrame.hasMiniMapMoved = rand();
        emptyFrame.miniMapX = rand();
        emptyFrame.miniMapY = rand();
        emptyFrame.miniMapScreenX = rand();
        emptyFrame.miniMapScreenY = rand();
        emptyFrame.miniMapScreenWidth = rand();
        emptyFrame.miniMapScreenHeight = rand();
        emptyFrame.hasViewUpdated = rand();
        emptyFrame.viewX = rand();
        emptyFrame.viewY = rand();
        emptyFrame.viewWidth = rand();
        emptyFrame.viewHeight = rand();
        emptyFrame.width = rand();
        emptyFrame.height = rand();
        emptyFrame.spriteDraws = std::make_shared<std::vector<SpriteDraw>>();
        emptyFrame.guiDraws = std::make_shared<std::vector<GuiDraw>>();
        emptyFrame.guiSpriteDraws = std::make_shared<std::vector<SpriteDraw>>();
        emptyFrame.textDraws = std::make_shared<std::vector<TextDraw>>();
        emptyFrame.rectDraws = std::make_shared<std::vector<RectDraw>>();
        emptyFrame.fileIo = std::make_shared<std::vector<FileIo>>();
        emptyFrame.miniMapDraws = std::make_shared<std::vector<MiniMapDraw>>();
        emptyFrame.screenPixels = std::make_shared<RawImage>(sb::utility::PixelFormat::RGBA, 1, 1, std::vector<unsigned char>({255, 255, 255, 255}));
    }

    ~FrameFileTest()
    {
        std::remove(std::string(filePath + ".bin").c_str());
        std::remove(std::string(filePath + ".png").c_str());
    }

    static void fillDraw(Draw& d)
    {
        d.topLeft.x = rand();
        d.topLeft.y = rand();
        d.botRight.x = rand();
        d.botRight.y = rand();

        d.transform.reset(new sb::utility::Matrix<float, 4, 4>());
        float* values = *d.transform->values;
        for(size_t i = 0; i < 4 * 4; i++)
        {
            *(values + i) = rand();
        }
    }

    static SpriteDraw genSpriteDraw()
    {
        SpriteDraw d;
        fillDraw(d);

        size_t numPairings = 3 + rand() % 5;
        for(size_t i = 0; i < numPairings; i++)
        {
            SpriteDraw::SpriteObjectPairing p;
            p.spriteId = rand();
            size_t numObjects = 3 + rand() % 5;
            for(size_t j = 0; j < numObjects; j++)
            {
                p.objects.push_back(rand());
            }
            d.pairings.push_back(p);
        }

        return d;
    }

    static GuiDraw genGuiDraw()
    {
        GuiDraw d;
        fillDraw(d);

        d.name = sb::utility::randStr(5 + rand() % 10);
        return d;
    }

    static TextDraw genTextDraw()
    {
        TextDraw d;
        fillDraw(d);
        d.color.packed = rand();
        d.isOutlined = rand();

        d.glyphDraws.reset(new std::vector<GlyphDraw>(5 + rand() % 10));
        for(GlyphDraw& g : *d.glyphDraws)
        {
            fillDraw(g);
            g.character = rand();
        }

        return d;
    }

    static RectDraw genRectDraw()
    {
        RectDraw d;
        fillDraw(d);
        d.color.packed = rand();

        return d;
    }

    static MiniMapDraw genMiniMapDraw()
    {
        MiniMapDraw d;
        fillDraw(d);
        d.pixels.reset(new std::vector<unsigned char>(20 + rand() % 100));
        std::generate(d.pixels->begin(), d.pixels->end(), rand);

        return d;
    }


    void expectEqAfterWriteRead(const Frame& f) const
    {
        EXPECT_TRUE(FrameFile::write(f, filePath));
        Frame readFrame;
        EXPECT_TRUE(FrameFile::read(readFrame, filePath));

        expectEq(f, readFrame);
    }

    void expectWriteReadTrue(Frame& f) const
    {
        EXPECT_TRUE(FrameFile::write(f, filePath));
        EXPECT_TRUE(FrameFile::read(f, filePath));
    }


    Frame nullFrame = {0};
    Frame frame;
    Frame emptyFrame;
    std::string filePath = "frameDump";
};

TEST_F(FrameFileTest, WriteReadNullFrame)
{
    expectWriteReadTrue(nullFrame);
}

TEST_F(FrameFileTest, WriteReadFrame)
{
    expectWriteReadTrue(frame);
}


TEST_F(FrameFileTest, WriteReadEmptyFrame)
{
    expectWriteReadTrue(emptyFrame);
}

TEST_F(FrameFileTest, SameDataAfterReadNullFrame)
{
    expectEqAfterWriteRead(nullFrame);
}

TEST_F(FrameFileTest, SameDataAfterReadFrame)
{
    expectEqAfterWriteRead(frame);
}

TEST_F(FrameFileTest, SameDataAfterReadEmptyFrame)
{
    expectEqAfterWriteRead(emptyFrame);
}
