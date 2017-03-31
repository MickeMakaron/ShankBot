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
#include "utility/PixelFormat.hpp"
#include "utility/utility.hpp"
///////////////////////////////////

namespace sb
{
namespace utility
{

unsigned char getBytesPerPixel(const PixelFormat& format)
{
    switch(format)
    {
        case PixelFormat::ALPHA: return BYTES_PER_PIXEL_ALPHA;
        case PixelFormat::RGB: return BYTES_PER_PIXEL_RGB;
        case PixelFormat::RGBA: return BYTES_PER_PIXEL_RGBA;
        case PixelFormat::BGRA: return BYTES_PER_PIXEL_BGRA;
        default:
            SB_THROW(stringify("Unimplemented pixel format: ", (int)format));
    }
}

}
}
