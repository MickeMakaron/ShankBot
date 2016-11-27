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
#ifndef GRAPHICS_LAYER_SPRITE_HPP
#define GRAPHICS_LAYER_SPRITE_HPP


///////////////////////////////////
// STD C++
#include <vector>
///////////////////////////////////

namespace GraphicsLayer
{
    struct Sprite
    {
        Sprite(unsigned int id, unsigned short width, unsigned short height, const unsigned char* pixels);
        Sprite(unsigned int id, unsigned short width, unsigned short height, const std::vector<unsigned char>& pixels);

        const unsigned int id;
        const unsigned short width;
        const unsigned short height;
        const std::vector<unsigned char> pixels;
    };
}


#endif // GRAPHICS_LAYER_SPRITE
