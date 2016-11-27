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
#include "graphics/Draw.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////



//        void getScreenCoords
//        (
//            float halfScreenWidth, float halfScreenHeight,
//            unsigned short& screenX, unsigned short& screenY
//        ) const
//        {
//            worldToScreenCoords(topLeft.x, topLeft.y, *transform, halfScreenWidth, halfScreenHeight, screenX, screenY);
//        }

//        void getScreenCoords
//        (
//            float halfScreenWidth, float halfScreenHeight,
//            unsigned short& screenX, unsigned short& screenY,
//            float worldX, float worldY
//        ) const
//        {
//            worldToScreenCoords(worldX, worldY, *transform, halfScreenWidth, halfScreenHeight, screenX, screenY);
//        }

void Draw::getScreenCoords
(
    float halfScreenWidth, float halfScreenHeight,
    float& screenX, float& screenY
) const
{
    sb::utility::worldToScreenCoords(topLeft.x, topLeft.y, *transform, halfScreenWidth, halfScreenHeight, screenX, screenY);
}

void Draw::getScreenCoords
(
    float halfScreenWidth, float halfScreenHeight,
    float& screenX, float& screenY,
    float worldX, float worldY
) const
{
    sb::utility::worldToScreenCoords(worldX, worldY, *transform, halfScreenWidth, halfScreenHeight, screenX, screenY);
}


