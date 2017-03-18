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
#ifndef SB_TIBIAASSETS_CATALOG_CONTENT_HPP
#define SB_TIBIAASSETS_CATALOG_CONTENT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <list>
#include <string>
///////////////////////////////////

namespace sb
{
namespace tibiaassets
{
    class SHANK_BOT_TIBIAASSETS_DECLSPEC CatalogContent
    {
        public:
            explicit CatalogContent(std::string filePath);

            struct Appearances
            {
                std::string path;
                unsigned int version;
            };

            enum class SpriteSize : unsigned char
            {
                _1x1 = 0,
                _1x2 = 1,
                _2x1 = 2,
                _2x2 = 3
            };

            struct SpriteSheet
            {
                std::string path;
                SpriteSize spriteSize;
                int firstSpriteId;
                int lastSpriteId;
                int area;
            };

            static void getSpriteTileSize(SpriteSize size, unsigned char& width, unsigned char& height);

            const std::list<Appearances>& getAppearances() const;
            const std::list<SpriteSheet>& getSpriteSheets() const;

        private:
            void read(std::string filePath);

        private:
            std::list<Appearances> mAppearances;
            std::list<SpriteSheet> mSpriteSheets;
    };
}
}


#endif // SB_TIBIAASSETS_CATALOG_CONTENT_HPP
