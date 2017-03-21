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
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
///////////////////////////////////

namespace GraphicsLayer
{
namespace FrameFile
{

void write(const Draw& draw, std::ostream& stream)
{
    writeStream(draw.topLeft, stream);
    writeStream(draw.botRight, stream);
    writeStream(draw.transform == nullptr ? Matrix<float, 4, 4>() : *draw.transform, stream);
}

void write(const std::shared_ptr<std::vector<SpriteDraw>>& draws, std::ostream& stream)
{
    unsigned short numDraws = draws == nullptr ? 0 : draws->size();
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
    unsigned short numDraws = draws == nullptr ? 0 : draws->size();
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
    unsigned short numDraws = draws == nullptr ? 0 : draws->size();
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
    unsigned short numDraws = draws == nullptr ? 0 : draws->size();
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
    unsigned short numDraws = draws == nullptr ? 0 : draws->size();
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
    unsigned short numDraws = draws == nullptr ? 0 : draws->size();
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
    bool isNull = screenPixels == nullptr;
    writeStream(isNull, stream);
    if(isNull)
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
    readStreamSafe(draw.topLeft, stream);
    readStreamSafe(draw.botRight, stream);

    draw.transform.reset(new Matrix<float, 4, 4>());
    readStreamSafe(*draw.transform, stream);
}

void read(std::shared_ptr<std::vector<SpriteDraw>>& draws, std::istream& stream)
{
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
    bool isNull;
    readStreamSafe(isNull, stream);
    if(isNull)
    {
        screenPixels.reset();
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


bool write(const Frame& f, const std::string& filePath)
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

//    for(size_t i = 0; i < 10; i++)
    write(f.spriteDraws, file);
//    for(size_t i = 0; i < 10; i++)
    write(f.guiDraws, file);
//    for(size_t i = 0; i < 10; i++)
    write(f.guiSpriteDraws, file);
//    for(size_t i = 0; i < 10; i++)
    write(f.textDraws, file);
//    for(size_t i = 0; i < 10; i++)
    write(f.rectDraws, file);
    write(f.miniMapDraws, file);
    write(f.screenPixels, file);


    return true;
}

bool read(Frame& f, const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);

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


//        for(size_t i = 0; i < 10; i++)
        read(f.spriteDraws, file);
//        for(size_t i = 0; i < 10; i++)
        read(f.guiDraws, file);
//        for(size_t i = 0; i < 10; i++)
        read(f.guiSpriteDraws, file);
//        for(size_t i = 0; i < 10; i++)
        read(f.textDraws, file);
//        for(size_t i = 0; i < 10; i++)
        read(f.rectDraws, file);        read(f.miniMapDraws, file);
        read(f.screenPixels, file);
    }
    catch(...)
    {
        return false;
    }


    return true;
}


}
}
