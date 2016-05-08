///////////////////////////////////
// Internal ShankBot headers
#include "TibiaSpr.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <cassert>
///////////////////////////////////


TibiaSpr::TibiaSpr(std::string sprFilePath)
{
    readSprites(sprFilePath);
}

unsigned int TibiaSpr::getVersion(std::string sprFilePath)
{
    std::ifstream spr(sprFilePath, std::ios::binary);

    unsigned int version;
    readStream(version, spr);

    spr.close();

    return version;
}

const std::vector<std::vector<unsigned char>>& TibiaSpr::getSprites() const
{
    return mSprites;
}

const std::vector<size_t>& TibiaSpr::getSpriteIds() const
{
    return mIds;
}

void TibiaSpr::readSprites(std::string sprFilePath)
{
    std::ifstream spr(sprFilePath, std::ios::binary);

    readStream(mVersion, spr);
    readStream(mNumSprites, spr);

    for(size_t i = 0; i < mNumSprites; i++)
    {
        unsigned int spriteIndex;
        readStream(spriteIndex, spr);
        if(spriteIndex != 0)
        {
            int currentPos = spr.tellg();
            spr.seekg(spriteIndex);
            std::vector<unsigned char> sprite = readSpritePixels(spr);
            mSprites.push_back(sprite);
            mIds.push_back(i + 1);
            spr.seekg(currentPos);
        }

    }
    spr.close();
}

std::vector<unsigned char> TibiaSpr::readSpritePixels(std::istream& spr) const
{
    char transparentPixelColor[3];
    spr.read(transparentPixelColor, 3);

    std::vector<unsigned char> pixels;

    unsigned short numBytes;
    readStream(numBytes, spr);

    int endPos = (int)spr.tellg() + numBytes;
    while(spr.tellg() < endPos)
    {
        unsigned short numTransparentPixels;
        readStream(numTransparentPixels, spr);

        for(size_t i = 0; i < numTransparentPixels; i++)
        {
            pixels.push_back(0);
            pixels.push_back(0);
            pixels.push_back(0);
            pixels.push_back(0);
        }

        unsigned short numOpaquePixels;
        readStream(numOpaquePixels, spr);

        for(size_t i = 0; i < numOpaquePixels; i++)
        {
            unsigned char red;
            unsigned char green;
            unsigned char blue;

            readStream(red, spr);
            readStream(green, spr);
            readStream(blue, spr);

            pixels.push_back(red);
            pixels.push_back(green);
            pixels.push_back(blue);
            pixels.push_back(255);
        }
    }

    for(size_t i = pixels.size(); i < 32 * 32 * 4; i += 4)
    {
        pixels.push_back(0);
        pixels.push_back(0);
        pixels.push_back(0);
        pixels.push_back(0);
    }

    assert(pixels.size() == 32 * 32 * 4);

    return pixels;
}
