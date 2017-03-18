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
#include "injection/SharedMemoryProtocol.hpp"
#include "utility/utility.hpp"

using namespace GraphicsLayer;
using namespace SharedMemoryProtocol;
///////////////////////////////////


unsigned char PixelData::getBytesPerPixel() const
{
    switch(format)
    {
        case PixelFormat::RGBA:
        case PixelFormat::BGRA:
            return sb::utility::BYTES_PER_PIXEL_RGBA;

        case PixelFormat::ALPHA:
            return sb::utility::BYTES_PER_PIXEL_ALPHA;

        default:
            THROW_RUNTIME_ERROR("Unimplemented pixel format.");
    }
}
