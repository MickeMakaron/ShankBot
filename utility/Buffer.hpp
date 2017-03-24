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
#ifndef SB_UTILITY_BUFFER_HPP
#define SB_UTILITY_BUFFER_HPP



///////////////////////////////////
// Internal ShankBot headers
#include "utility/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
#include <vector>
///////////////////////////////////

namespace sb
{
namespace utility
{
    struct SHANK_BOT_UTILITY_DECLSPEC Buffer
    {
        static const std::ios_base::seekdir cur = std::ios_base::cur;
        static const std::ios_base::seekdir beg = std::ios_base::beg;
        static const std::ios_base::seekdir end = std::ios_base::end;

        std::vector<char> data;
        size_t curr = 0;

        size_t tellg() const;
        Buffer& seekg(std::streampos pos);
        Buffer& seekg(std::streampos off, std::ios_base::seekdir way);
    };
}
}

#endif // SB_UTILITY_BUFFER_HPP
