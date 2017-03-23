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
using namespace GraphicsLayer;
///////////////////////////////////
#include "QtGui/QImage"
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

    void expectEq(const Vertex& v1, const Vertex& v2) const
    {
        EXPECT_FLOAT_EQ(v1.x, v2.x);
        EXPECT_FLOAT_EQ(v1.y, v2.y);
    }

    template<size_t N, size_t M>
    void expectEq(const sb::utility::Matrix<float, N, M>& m1, const sb::utility::Matrix<float, N, M>& m2) const
    {
        for(size_t i = 0; i < N * M; i++)
        {
            EXPECT_FLOAT_EQ(*(*m1.values + i), *(*m2.values + i));
        }
    }

    void expectEq(const Draw& d1, const Draw& d2) const
    {
        expectEq(d1.topLeft, d2.topLeft);
        expectEq(d1.botRight, d2.botRight);

        EXPECT_EQ(d1.transform == nullptr, d2.transform == nullptr);
        if(d1.transform != nullptr)
        {
            expectEq(*d1.transform, *d2.transform);
        }
    }

    void expectEq(const SpriteDraw::SpriteObjectPairing& p1, const SpriteDraw::SpriteObjectPairing& p2) const
    {
        EXPECT_EQ(p1.spriteId, p2.spriteId);
        EXPECT_EQ(p1.objects, p2.objects);
    }

    void expectEq(const SpriteDraw& d1, const SpriteDraw& d2) const
    {
        expectEq((const Draw&)d1, (const Draw&)d2);
        expectEq(d1.pairings, d2.pairings);
    }

    void expectEq(const GuiDraw& d1, const GuiDraw& d2) const
    {
        expectEq((const Draw&)d1, (const Draw&)d2);
        EXPECT_EQ(d1.name, d2.name);
    }

    void expectEq(const GlyphDraw& d1, const GlyphDraw& d2) const
    {
        expectEq((const Draw&)d1, (const Draw&)d2);
        EXPECT_EQ(d1.character, d2.character);
    }

    void expectEq(const TextDraw& d1, const TextDraw& d2) const
    {
        expectEq((const Draw&)d1, (const Draw&)d2);
        EXPECT_EQ(d1.color.packed, d2.color.packed);
        EXPECT_EQ(d1.isOutlined, d2.isOutlined);
        expectEq(d1.glyphDraws, d2.glyphDraws);
    }

    void expectEq(const RectDraw& d1, const RectDraw& d2) const
    {
        expectEq((const Draw&)d1, (const Draw&)d2);
        EXPECT_EQ(d1.color.packed, d2.color.packed);
    }

    void expectEq(const MiniMapDraw& d1, const MiniMapDraw& d2) const
    {
        expectEq((const Draw&)d1, (const Draw&)d2);
        EXPECT_EQ(d1.pixels == nullptr, d2.pixels == nullptr);
        if(d1.pixels != nullptr)
        {
            EXPECT_EQ(*d1.pixels, *d2.pixels);
        }
    }


    void expectEq(const RawImage& d1, const RawImage& d2) const
    {
        EXPECT_EQ(d1.format, d2.format);
        EXPECT_EQ(d1.width, d2.width);
        EXPECT_EQ(d1.height, d2.height);
        EXPECT_EQ(d1.pixels, d2.pixels);
    }

    template<typename T>
    void expectEq(const std::shared_ptr<T>& p1, const std::shared_ptr<T>& p2) const
    {
        EXPECT_EQ(p1 == nullptr, p2 == nullptr);
        if(p1 != nullptr)
        {
            expectEq(*p1, *p2);
        }
    }

    template<typename T>
    void expectEq(const std::list<T>& p1, const std::list<T>& p2) const
    {
        EXPECT_EQ(p1.size(), p2.size());
        auto it1 = p1.begin();
        auto it2 = p2.begin();
        while(it1 != p1.end())
        {
            expectEq(*it1, *it2);

            it1++;
            it2++;
        }
    }

    template<typename T>
    void expectEq(const std::vector<T>& p1, const std::vector<T>& p2) const
    {
        EXPECT_EQ(p1.size(), p2.size());
        for(size_t i = 0; i < p1.size(); i++)
        {
            expectEq(p1[i], p2[i]);
        }
    }

    void expectEq(const Frame& f1, const Frame& f2) const
    {
        EXPECT_EQ(f1.hasMiniMapMoved, f2.hasMiniMapMoved);
        EXPECT_EQ(f1.miniMapX, f2.miniMapX);
        EXPECT_EQ(f1.miniMapY, f2.miniMapY);
        EXPECT_EQ(f1.miniMapScreenX, f2.miniMapScreenX);
        EXPECT_EQ(f1.miniMapScreenY, f2.miniMapScreenY);
        EXPECT_EQ(f1.miniMapScreenWidth, f2.miniMapScreenWidth);
        EXPECT_EQ(f1.miniMapScreenHeight, f2.miniMapScreenHeight);
        EXPECT_EQ(f1.hasViewUpdated, f2.hasViewUpdated);
        EXPECT_EQ(f1.viewX, f2.viewX);
        EXPECT_EQ(f1.viewY, f2.viewY);
        EXPECT_EQ(f1.viewWidth, f2.viewWidth);
        EXPECT_EQ(f1.viewHeight, f2.viewHeight);
        EXPECT_EQ(f1.width, f2.width);
        EXPECT_EQ(f1.height, f2.height);

        expectEq(f1.spriteDraws, f2.spriteDraws);
        expectEq(f1.guiDraws, f2.guiDraws);
        expectEq(f1.guiSpriteDraws, f2.guiSpriteDraws);
        expectEq(f1.textDraws, f2.textDraws);
        expectEq(f1.rectDraws, f2.rectDraws);
//        expectEq(f1.fileIo, f2.fileIo);
        expectEq(f1.miniMapDraws, f2.miniMapDraws);

        std::shared_ptr<RawImage> flippedScreenPixels;
        if(f1.screenPixels != nullptr)
        {
            QImage flippedScreen(f1.screenPixels->pixels.data(), f1.screenPixels->width, f1.screenPixels->height, QImage::Format_RGBA8888);
            flippedScreen = flippedScreen.mirrored(false, true);
            flippedScreenPixels.reset(new RawImage(sb::utility::PixelFormat::RGBA, flippedScreen.width(), flippedScreen.height(), flippedScreen.bits()));
            EXPECT_EQ(flippedScreen.byteCount(), f1.screenPixels->pixels.size());
        }
        expectEq(flippedScreenPixels, f2.screenPixels);
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
