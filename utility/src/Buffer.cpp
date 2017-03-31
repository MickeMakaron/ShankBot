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
#include "utility/Buffer.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// STD C++
///////////////////////////////////

size_t Buffer::tellg() const
{
    return curr;
}

Buffer& Buffer::seekg(std::streampos pos)
{
    curr = pos;
    return *this;
}

Buffer& Buffer::seekg(std::streampos off, std::ios_base::seekdir way)
{
    using W = std::ios_base;
    switch(way)
    {
        case W::beg:
            curr = off;
            break;
        case W::cur:
            curr += off;
            break;
        case W::end:
            curr = data.size() + off;
            break;

        default:
            SB_THROW("Unimplemented seekg.");
    }

    return *this;
}

