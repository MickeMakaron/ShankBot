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
#ifndef SB_LZMA_LZMA_HPP
#define SB_LZMA_LZMA_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "lzma/config.hpp"
///////////////////////////////////

///////////////////////////////////
// Lzma
#include "lzma/7zTypes.h"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstring>
///////////////////////////////////

namespace sb
{
namespace lzma
{
    SHANK_BOT_LZMA_DECLSPEC int uncompress
    (
        unsigned char* dest,
        size_t* destLen,
        const unsigned char* src,
        size_t* srcLen,
        const unsigned char* props,
        size_t propsSize
    );
}
}

#endif // SB_LZMA_LZMA_HPP
