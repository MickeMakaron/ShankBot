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
#include "utility/file.hpp"
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QImage"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
#include <cassert>
///////////////////////////////////

namespace GraphicsLayer
{
namespace FrameFile
{

void write(const Draw& draw, std::ostream& stream)
{
    writeStream(draw.drawCallId, stream);
    writeStream(draw.topLeft, stream);
    writeStream(draw.botRight, stream);
    bool hasTransform = draw.transform != nullptr;
    writeStream(hasTransform, stream);
    if(!hasTransform)
    {
        return;
    }
    writeStream(*draw.transform, stream);
    writeStream(draw.isDepthTestEnabled, stream);
    writeStream(draw.isDepthWriteEnabled, stream);
    writeStream(draw.hasOrder, stream);
    writeStream(draw.order, stream);
}

void write(const std::shared_ptr<std::vector<SpriteDraw>>& draws, std::ostream& stream)
{
    bool hasData = draws != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }
    unsigned short numDraws = draws->size();
    writeStream(numDraws, stream);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        const SpriteDraw& d = (*draws)[i];
        write(d, stream);

        unsigned short numPairings = d.pairings.size();
        writeStream(numPairings, stream);
        for(const SpriteDraw::SpriteObjectPairing& p : d.pairings)
        {
            unsigned int spriteId = p.spriteId;
            writeStream(spriteId, stream);

            unsigned short numObjects = p.objects.size();
            writeStream(numObjects, stream);

            for(const size_t& o : p.objects)
            {
                unsigned int object = o;
                writeStream(object, stream);
            }
        }
    }
}


void write(const std::shared_ptr<std::vector<GuiDraw>>& draws, std::ostream& stream)
{
    bool hasData = draws != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }
    unsigned short numDraws = draws->size();
    writeStream(numDraws, stream);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        const GuiDraw& d = (*draws)[i];
        write(d, stream);

        unsigned short nameLength = d.name.size();
        writeStream(nameLength, stream);
        if(nameLength)
        {
            writeStream(*d.name.c_str(), stream, nameLength);
        }
    }
}


void write(const std::shared_ptr<std::vector<GlyphDraw>>& draws, std::ostream& stream)
{
    bool hasData = draws != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }
    unsigned short numDraws = draws->size();
    writeStream(numDraws, stream);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        const GlyphDraw& d = (*draws)[i];
        write(d, stream);
        writeStream(d.character, stream);
    }
}


void write(const std::shared_ptr<std::vector<TextDraw>>& draws, std::ostream& stream)
{
    bool hasData = draws != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }
    unsigned short numDraws = draws->size();
    writeStream(numDraws, stream);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        const TextDraw& d = (*draws)[i];
        write(d, stream);

        writeStream(d.color, stream);
        writeStream(d.isOutlined, stream);
        write(d.glyphDraws, stream);
    }
}


void write(const std::shared_ptr<std::vector<RectDraw>>& draws, std::ostream& stream)
{
    bool hasData = draws != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }
    unsigned short numDraws = draws->size();
    writeStream(numDraws, stream);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        const RectDraw& d = (*draws)[i];
        write(d, stream);
        writeStream(d.color, stream);
    }
}


void write(const std::shared_ptr<std::vector<MiniMapDraw>>& draws, std::ostream& stream)
{
    bool hasData = draws != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }
    unsigned short numDraws = draws->size();
    writeStream(numDraws, stream);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        const MiniMapDraw& d = (*draws)[i];
        write(d, stream);

        unsigned int numValues = d.pixels == nullptr ? 0 : d.pixels->size();
        writeStream(numValues, stream);
        if(numValues)
        {
            writeStream(*d.pixels->data(), stream, numValues);
        }
    }
}


void write(const std::shared_ptr<RawImage>& screenPixels, std::ostream& stream)
{
    bool hasData = screenPixels != nullptr;
    writeStream(hasData, stream);
    if(!hasData)
    {
        return;
    }

    writeStream(screenPixels->format, stream);
    writeStream(screenPixels->width, stream);
    writeStream(screenPixels->height, stream);

    size_t size = getBytesPerPixel(screenPixels->format) * screenPixels->width * screenPixels->height;
    if(size)
    {
        writeStream(*screenPixels->pixels.data(), stream, size);
    }
}

void read(Draw& draw, std::istream& stream)
{
    readStreamSafe(draw.drawCallId, stream);
    readStreamSafe(draw.topLeft, stream);
    readStreamSafe(draw.botRight, stream);

    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draw.transform = nullptr;
        return;
    }

    draw.transform.reset(new Matrix<float, 4, 4>());
    readStreamSafe(*draw.transform, stream);

    readStreamSafe(draw.isDepthTestEnabled, stream);
    readStreamSafe(draw.isDepthWriteEnabled, stream);
    readStreamSafe(draw.hasOrder, stream);
    readStreamSafe(draw.order, stream);
}

void read(std::shared_ptr<std::vector<SpriteDraw>>& draws, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draws = nullptr;
        return;
    }
    draws.reset(new std::vector<SpriteDraw>());

    unsigned short numDraws;
    readStreamSafe(numDraws, stream);
    draws->resize(numDraws);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        SpriteDraw& d = (*draws)[i];
        read(d, stream);

        unsigned short numPairings;
        readStreamSafe(numPairings, stream);
        for(unsigned short j = 0; j < numPairings; j++)
        {
            d.pairings.emplace_back();
            SpriteDraw::SpriteObjectPairing& p = d.pairings.back();

            unsigned int spriteId;
            readStreamSafe(spriteId, stream);
            p.spriteId = spriteId;

            unsigned short numObjects;
            readStreamSafe(numObjects, stream);

            for(unsigned short k = 0; k < numObjects; k++)
            {
                unsigned int object;
                readStreamSafe(object, stream);
                p.objects.push_back(object);
            }
        }
    }
}


void read(std::shared_ptr<std::vector<GuiDraw>>& draws, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draws = nullptr;
        return;
    }

    draws.reset(new std::vector<GuiDraw>());

    unsigned short numDraws;
    readStreamSafe(numDraws, stream);
    draws->resize(numDraws);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        GuiDraw& d = (*draws)[i];
        read(d, stream);

        unsigned short nameLength;
        readStreamSafe(nameLength, stream);
        if(nameLength)
        {
            d.name.resize(nameLength);
            readStreamSafe(*d.name.c_str(), stream, nameLength);
        }
    }
}


void read(std::shared_ptr<std::vector<GlyphDraw>>& draws, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draws = nullptr;
        return;
    }

    draws.reset(new std::vector<GlyphDraw>());

    unsigned short numDraws;
    readStreamSafe(numDraws, stream);
    draws->resize(numDraws);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        GlyphDraw& d = (*draws)[i];
        read(d, stream);
        readStreamSafe(d.character, stream);
    }
}


void read(std::shared_ptr<std::vector<TextDraw>>& draws, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draws = nullptr;
        return;
    }

    draws.reset(new std::vector<TextDraw>());

    unsigned short numDraws;
    readStreamSafe(numDraws, stream);
    draws->resize(numDraws);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        TextDraw& d = (*draws)[i];
        read(d, stream);

        readStreamSafe(d.color, stream);
        readStreamSafe(d.isOutlined, stream);
        read(d.glyphDraws, stream);
    }
}


void read(std::shared_ptr<std::vector<RectDraw>>& draws, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draws = nullptr;
        return;
    }

    draws.reset(new std::vector<RectDraw>());

    unsigned short numDraws;
    readStreamSafe(numDraws, stream);
    draws->resize(numDraws);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        RectDraw& d = (*draws)[i];
        read(d, stream);
        readStreamSafe(d.color, stream);
    }
}


void read(std::shared_ptr<std::vector<MiniMapDraw>>& draws, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        draws = nullptr;
        return;
    }

    draws.reset(new std::vector<MiniMapDraw>());

    unsigned short numDraws;
    readStreamSafe(numDraws, stream);
    draws->resize(numDraws);
    for(unsigned short i = 0; i < numDraws; i++)
    {
        MiniMapDraw& d = (*draws)[i];
        read(d, stream);

        d.pixels.reset(new std::vector<unsigned char>());

        unsigned int numValues;
        readStreamSafe(numValues, stream);
        if(numValues)
        {
            d.pixels->resize(numValues);
            readStreamSafe(*d.pixels->data(), stream, numValues);
        }
    }
}


void read(std::shared_ptr<RawImage>& screenPixels, std::istream& stream)
{
    bool hasData;
    readStreamSafe(hasData, stream);
    if(!hasData)
    {
        screenPixels = nullptr;
        return;
    }

    PixelFormat format;
    unsigned short width;
    unsigned short height;
    readStreamSafe(format, stream);
    readStreamSafe(width, stream);
    readStreamSafe(height, stream);

    std::vector<unsigned char> pixels;
    size_t size = getBytesPerPixel(format) * width * height;
    if(size)
    {
        pixels.resize(size);
        readStreamSafe(*pixels.data(), stream, size);
    }

    screenPixels.reset(new RawImage(format, width, height, pixels));
}

bool writeBin(const Frame& f, const std::string& filePath)
{
    std::ofstream file(filePath + ".bin", std::ios::binary);
    if(!file.good())
    {
        return false;
    }

    writeStream(f.hasMiniMapMoved, file);
    writeStream(f.miniMapX, file);
    writeStream(f.miniMapY, file);
    writeStream(f.miniMapScreenX, file);
    writeStream(f.miniMapScreenY, file);
    writeStream(f.miniMapScreenWidth, file);
    writeStream(f.miniMapScreenHeight, file);
    writeStream(f.hasViewUpdated, file);
    writeStream(f.viewX, file);
    writeStream(f.viewY, file);
    writeStream(f.viewWidth, file);
    writeStream(f.viewHeight, file);
    writeStream(f.width, file);
    writeStream(f.height, file);

    write(f.spriteDraws, file);
    write(f.guiDraws, file);
    write(f.guiSpriteDraws, file);
    write(f.textDraws, file);
    write(f.rectDraws, file);
    write(f.miniMapDraws, file);

    return !file.fail();
}

bool writeImages(const Frame& f, const std::string& filePath)
{
    if(f.screenPixels == nullptr)
    {
        return true;
    }

    const RawImage& i = *f.screenPixels;
    assert(i.format == sb::utility::PixelFormat::RGBA);
    QImage img(i.pixels.data(), i.width, i.height, QImage::Format_RGBA8888);
    img = img.mirrored(false, true);
    QString imgPath = QString::fromStdString(filePath + ".png");

    return img.save(imgPath);
}

bool write(const Frame& f, const std::string& filePath)
{
    if(!writeBin(f, filePath))
    {
        return false;
    }

    if(!writeImages(f, filePath))
    {
        return false;
    }

    return true;
}


bool readBin(Frame& f, const std::string& filePath)
{
    std::ifstream file(filePath + ".bin", std::ios::binary);

    if(!file.good())
    {
        return false;
    }

    try
    {
        readStreamSafe(f.hasMiniMapMoved, file);
        readStreamSafe(f.miniMapX, file);
        readStreamSafe(f.miniMapY, file);
        readStreamSafe(f.miniMapScreenX, file);
        readStreamSafe(f.miniMapScreenY, file);
        readStreamSafe(f.miniMapScreenWidth, file);
        readStreamSafe(f.miniMapScreenHeight, file);
        readStreamSafe(f.hasViewUpdated, file);
        readStreamSafe(f.viewX, file);
        readStreamSafe(f.viewY, file);
        readStreamSafe(f.viewWidth, file);
        readStreamSafe(f.viewHeight, file);
        readStreamSafe(f.width, file);
        readStreamSafe(f.height, file);

        read(f.spriteDraws, file);
        read(f.guiDraws, file);
        read(f.guiSpriteDraws, file);
        read(f.textDraws, file);
        read(f.rectDraws, file);        read(f.miniMapDraws, file);
    }
    catch(...)
    {
        return false;
    }

    bool isEarlyEof = file.eof();
    file.peek();
    return !isEarlyEof && file.eof();
}

bool readImages(Frame& f, const std::string& filePath)
{
    f.screenPixels = nullptr;

    std::string screenPixelsPath = filePath + ".png";
    if(!sb::utility::file::fileExists(screenPixelsPath))
    {
        return true;
    }


    QImage screenPixels(QString::fromStdString(screenPixelsPath));
    screenPixels = screenPixels.convertToFormat(QImage::Format_RGBA8888);
    f.screenPixels.reset(new RawImage(sb::utility::PixelFormat::RGBA, screenPixels.width(), screenPixels.height(), screenPixels.bits()));
    return true;
}

bool read(Frame& f, const std::string& filePath)
{
    if(!readBin(f, filePath))
    {
        return false;
    }

    if(!readImages(f, filePath))
    {
        return false;
    }

    return true;
}


}
}
