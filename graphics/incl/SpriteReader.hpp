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
#ifndef GRAPHICS_LAYER_SPRITE_READER_HPP
#define GRAPHICS_LAYER_SPRITE_READER_HPP

///////////////////////////////////
// Internal ShankBot headers
namespace GraphicsLayer
{
    class CatalogContent;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <string>
#include <istream>
#include <functional>
///////////////////////////////////

namespace GraphicsLayer
{
    class SpriteReader
    {
        public:
            struct Sprite
            {
                const unsigned char* pixels;
                unsigned char tileWidth;
                unsigned char tileHeight;
                unsigned int id;
                unsigned int area;
            };

        private:


        public:
            explicit SpriteReader(const CatalogContent& catalog);
            void forEachSprite(std::function<bool(const Sprite& spr)> func) const;

        private:
            bool validateTibiaLzmaHeader(std::istream& stream) const;
            std::vector<unsigned char> decompressTibiaLzma(std::string path) const;

        private:
            unsigned int mNumSprites;
            const CatalogContent& mCatalog;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_READER_HPP
