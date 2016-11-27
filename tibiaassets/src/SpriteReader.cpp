// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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
#include "tibiaassets/SpriteReader.hpp"
#include "utility/utility.hpp"
#include "utility/file.hpp"
#include "tibiaassets/CatalogContent.hpp"
#include "tibiaassets/constants.hpp"
using namespace sb::tibiaassets;
using namespace sb::utility;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <cassert>
#include <sstream>
#include <iostream>
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QImage"
///////////////////////////////////


///////////////////////////////////
// Lzma
#include "lzma/lzma.hpp"
///////////////////////////////////



SpriteReader::SpriteReader(const CatalogContent& catalog)
: mCatalog(catalog)
{
}

bool SpriteReader::validateTibiaLzmaHeader(std::istream& stream) const
{
    if(stream.bad())
        return false;

    unsigned char byte;
    readStream(byte, stream);
    if(byte != 0x00)
        return false;

    if(stream.bad())
        return false;

    readStream(byte, stream);
    if(byte != 0x70)
        return false;

    if(stream.bad())
        return false;

    readStream(byte, stream);
    if(byte > 0x14)
        return false;

    return true;
}



std::vector<unsigned char> SpriteReader::decompressTibiaLzma(std::string path) const
{
    std::ifstream file(path, std::ios::binary | std::ios::in);

    if(!validateTibiaLzmaHeader(file))
    {
        std::cout << "Failed to decompress \"" << path << "\". Invalid header." << std::endl;
        return std::vector<unsigned char>();
    }

    size_t decompressedSize = sb::utility::readTibiaSizeIndicator(file);
    unsigned int compressedSize = sb::utility::readTibiaSizeIndicator(file);

    unsigned char props[5]; // LZMA property bytes
    readStream(*props, file, 5);

    readStream(compressedSize, file);
    file.seekg(4, std::ios::cur);

    unsigned char* data = new unsigned char[compressedSize];
    readStream(*data, file, compressedSize);
    file.close();

    std::vector<unsigned char> decompressedFile;
    decompressedFile.resize(decompressedSize);
    int result = sb::lzma::uncompress(decompressedFile.data(), &decompressedSize, data, &compressedSize, props, 5);
    delete[] data;
    switch(result)
    {
        case SZ_OK:

            break;
        case SZ_ERROR_DATA:
            std::cout   << "Failed to decompress \"" << path << "\" with error code " << SZ_ERROR_DATA << "." << std::endl
                        << "Data error." << std::endl;
            return std::vector<unsigned char>();
            break;
        case SZ_ERROR_MEM:
            std::cout   << "Failed to decompress \"" << path << "\" with error code " << SZ_ERROR_DATA << "." << std::endl
                        << "Memory allocation error." << std::endl;
            return std::vector<unsigned char>();
            break;
        case SZ_ERROR_UNSUPPORTED:
            std::cout   << "Failed to decompress \"" << path << "\" with error code " << SZ_ERROR_DATA << "." << std::endl
                        << "Unsupported LZMA properties." << std::endl;
            return std::vector<unsigned char>();
            break;
        case SZ_ERROR_INPUT_EOF:
            std::cout   << "Failed to decompress \"" << path << "\" with error code " << SZ_ERROR_DATA << "." << std::endl
                        << "Unexpected end of file." << std::endl;
            return std::vector<unsigned char>();
            break;

        default:
            std::cout << "Unimplemented result code from LzmaUncompress: " << result << std::endl;
            return std::vector<unsigned char>();
            break;
    }

    return decompressedFile;
}


void SpriteReader::forEachSprite(std::function<bool(const Sprite& spr)> func) const
{
    const std::list<CatalogContent::SpriteSheet>& spriteSheets = mCatalog.getSpriteSheets();
    for(const CatalogContent::SpriteSheet& spriteSheet : spriteSheets)
    {
        Sprite spr;
        CatalogContent::getSpriteTileSize(spriteSheet.spriteSize, spr.tileWidth, spr.tileHeight);
        spr.area = spriteSheet.area;
        spr.id = spriteSheet.firstSpriteId;

        std::vector<unsigned char> data = decompressTibiaLzma(spriteSheet.path + ".lzma");

        QImage decompressedSpriteSheet = QImage::fromData(data.data(), data.size());

        decompressedSpriteSheet = decompressedSpriteSheet.convertToFormat(QImage::Format_RGBA8888);
        const size_t SPRITE_SHEET_PIXEL_WIDTH = decompressedSpriteSheet.width();
        const size_t SPRITE_SHEET_PIXEL_HEIGHT = decompressedSpriteSheet.height();
        const size_t SPRITE_PIXEL_WIDTH = spr.tileWidth * constants::TILE_PIXEL_WIDTH;
        const size_t SPRITE_PIXEL_HEIGHT = spr.tileHeight * constants::TILE_PIXEL_HEIGHT;
        const size_t NUM_SPRITES = spriteSheet.lastSpriteId - spriteSheet.firstSpriteId + 1;

        for(size_t y = 0, spriteNr = 0; y < SPRITE_SHEET_PIXEL_HEIGHT; y += SPRITE_PIXEL_HEIGHT)
        {
            for(size_t x = 0; x < SPRITE_SHEET_PIXEL_WIDTH && spriteNr < NUM_SPRITES; x += SPRITE_PIXEL_WIDTH, spriteNr++)
            {
                QImage sprite = decompressedSpriteSheet.copy(x, y, SPRITE_PIXEL_WIDTH, SPRITE_PIXEL_HEIGHT);
                spr.pixels = sprite.constBits();
                if(!func(spr))
                    return;
                spr.id++;
            }
        }
    }
}

