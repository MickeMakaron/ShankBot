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
#ifndef SB_UTILITY_PIXEL_FORMAT_HPP
#define SB_UTILITY_PIXEL_FORMAT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "utility/config.hpp"
///////////////////////////////////

namespace sb
{
namespace utility
{
    enum class PixelFormat : unsigned char
    {
        RGB,
        RGBA,
        BGRA,
        ALPHA
    };

    SHANK_BOT_UTILITY_DECLSPEC unsigned char getBytesPerPixel(const PixelFormat& format);
}
}


#endif // SB_UTILITY_PIXEL_FORMAT_HPP
