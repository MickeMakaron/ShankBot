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
#ifndef GRAPHICS_LAYER_COLOR_HPP
#define GRAPHICS_LAYER_COLOR_HPP

namespace GraphicsLayer
{
    union Color
    {
        explicit Color(){}
        explicit constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a){}


        explicit constexpr Color(const unsigned char (&rgba)[4])
        : packed(rgba[0] | (rgba[1] << 8) | (rgba[2] << 16) | (rgba[3] << 24)){}

        explicit constexpr Color(int packed) : r(packed & 0xff), g((packed >> 8) & 0xff), b((packed >> 16) & 0xff), a((packed >> 24) & 0xff){}

        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        int packed;
    };
}


#endif // GRAPHICS_LAYER_COLOR_HPP
