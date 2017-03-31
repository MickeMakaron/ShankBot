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
#include "monitor/SpriteInfo.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
using namespace sb::tibiaassets;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
///////////////////////////////////

SpriteInfo::SpriteInfo(const std::list<CatalogContent::SpriteSheet>& spriteSheets)
{
    parseSpriteSheets(spriteSheets);
}

void SpriteInfo::parseSpriteSheets(const std::list<CatalogContent::SpriteSheet>& spriteSheets)
{
    for(const CatalogContent::SpriteSheet& s : spriteSheets)
    {
        unsigned char tileWidth;
        unsigned char tileHeight;
        CatalogContent::getSpriteTileSize(s.spriteSize, tileWidth, tileHeight);
        for(unsigned int id = s.firstSpriteId; id <= s.lastSpriteId; id++)
            mInfo.emplace(id, Info(tileWidth, tileHeight, s.area));
    }
}

const SpriteInfo::Info& SpriteInfo::get(unsigned int spriteId) const
{
    auto result = mInfo.find(spriteId);
    if(result == mInfo.end())
    {
        SB_THROW("Sprite info for sprite id '", spriteId, "' does not exist.", "\n");
    }

    return result->second;
}
