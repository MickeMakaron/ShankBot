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
#ifndef GRAPHICS_LAYER_SPRITE_INFO_HPP
#define GRAPHICS_LAYER_SPRITE_INFO_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/CatalogContent.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <unordered_map>
///////////////////////////////////

namespace GraphicsLayer
{
    class SpriteInfo
    {
        public:
            struct Info
            {
                Info(unsigned char tileWidth, unsigned char tileHeight, unsigned int area)
                : tileWidth(tileWidth), tileHeight(tileHeight), area(area){};
                unsigned char tileWidth;
                unsigned char tileHeight;
                unsigned int area;
            };

        public:
            explicit SpriteInfo(const std::list<sb::tibiaassets::CatalogContent::SpriteSheet>& spriteSheets);
            const Info& get(unsigned int spriteId) const;

        private:
            void parseSpriteSheets(const std::list<sb::tibiaassets::CatalogContent::SpriteSheet>& spriteSheets);

        private:
            std::unordered_map<unsigned int, Info> mInfo;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_INFO_HPP
