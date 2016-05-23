///////////////////////////////////
// Internal ShankBot headers
#include "TibiaPic.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <cstring>
///////////////////////////////////



TibiaPic::TibiaPic(std::string picFilePath)
{
    read(picFilePath);
}

TibiaPic::TileSheet::TileSheet(unsigned char width, unsigned char height, std::vector<std::vector<unsigned char>> sprites)
: M_WIDTH(width)
, M_HEIGHT(height)
, M_SPRITES(sprites)
{

}

unsigned char* TibiaPic::TileSheet::mergeSprites() const
{
    unsigned char* sprites = new unsigned char[M_HEIGHT * M_WIDTH * TILE_WIDTH * TILE_HEIGHT * BYTES_PER_PIXEL];
    const size_t BYTES_PER_ROW = M_WIDTH * BYTES_PER_TILE_ROW;

    size_t spriteIndex = 0;
    for(size_t y = 0; y < M_HEIGHT; y++)
    {
        for(size_t x = 0; x < M_WIDTH; x++)
        {
            const std::vector<unsigned char>& sprite = M_SPRITES[spriteIndex];

            for(size_t iRow = 0; iRow < TILE_HEIGHT; iRow++)
            {
                memcpy(&sprites[(y * TILE_WIDTH + iRow) * BYTES_PER_ROW + x * BYTES_PER_TILE_ROW], &sprite[iRow * BYTES_PER_TILE_ROW], BYTES_PER_TILE_ROW);
            }

            spriteIndex++;
        }
    }

    return sprites;
}

const std::vector<unsigned char>& TibiaPic::TileSheet::getSprite(size_t x, size_t y) const
{
    if(x >= getWidth() || y >= getHeight())
        throw std::out_of_range("Cannot get sprite. Coordinates out of range.");

    return M_SPRITES[y * getWidth() + x];
}

unsigned char TibiaPic::TileSheet::getWidth() const
{
    return M_WIDTH;
}

unsigned char TibiaPic::TileSheet::getHeight() const
{
    return M_HEIGHT;
}

const std::vector<TibiaPic::TileSheet>& TibiaPic::getTileSheets() const
{
    return mSheets;
}

void TibiaPic::read(std::string picFilePath)
{
    std::ifstream pic(picFilePath, std::ios::binary);

    readStream(mVersion, pic);

    unsigned short numSheets;
    readStream(numSheets, pic);

    for(size_t i = 0; i < numSheets; i++)
    {
        unsigned char width;
        readStream(width, pic);

        unsigned char height;
        readStream(height, pic);

        char transparentPixelColor[3];
        pic.read(transparentPixelColor, 3);


        unsigned int numSprites = width * height;

        std::vector<std::vector<unsigned char>> sprites;
        sprites.resize(numSprites);
        for(size_t j = 0; j < numSprites; j++)
        {
            unsigned int spritePosition;
            readStream(spritePosition, pic);

            size_t currentPosition = pic.tellg();
            pic.seekg(spritePosition);

            unsigned short numBytes;
            readStream(numBytes, pic);

            std::vector<unsigned char>& pixels = sprites[j];
            pixels.resize(32 * 32 * 4);

            size_t iPixel = 0;
            for(size_t k = 0; k < numBytes;)
            {
                int start = pic.tellg();

                unsigned short numTransparentPixels;
                readStream(numTransparentPixels, pic);


                for(size_t l = 0; l < numTransparentPixels; l++)
                {
                    pixels[iPixel++] = transparentPixelColor[0];
                    pixels[iPixel++] = transparentPixelColor[1];
                    pixels[iPixel++] = transparentPixelColor[2];
                    pixels[iPixel++] = 0;
                }

                unsigned short numOpaquePixels;
                readStream(numOpaquePixels, pic);

                for(size_t l = 0; l < numOpaquePixels; l++)
                {
                    unsigned char rgb[3];
                    pic.read((char*)rgb, 3);

                    pixels[iPixel++] = rgb[0];
                    pixels[iPixel++] = rgb[1];
                    pixels[iPixel++] = rgb[2];
                    pixels[iPixel++] = 255;
                }

                int end = pic.tellg();
                k += end - start;
            }

            while(iPixel < pixels.size())
            {
                pixels[iPixel++] = transparentPixelColor[0];
                pixels[iPixel++] = transparentPixelColor[1];
                pixels[iPixel++] = transparentPixelColor[2];
                pixels[iPixel++] = 0;
            }

            pic.seekg(currentPosition);

        }
        mSheets.emplace_back(width, height, sprites);
    }



    pic.close();
}

unsigned int TibiaPic::getVersion(std::string picFilePath)
{
    std::ifstream pic(picFilePath, std::ios::binary);

    unsigned int version;
    readStream(version, pic);

    pic.close();

    return version;
}
