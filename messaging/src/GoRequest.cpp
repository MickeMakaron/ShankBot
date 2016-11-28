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
#include "messaging/GoRequest.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

void GoRequest::set(short x, short y)
{
    mX = x;
    mY = y;
}

short GoRequest::getX() const
{
    return mX;
}

short GoRequest::getY() const
{
    return mY;
}


size_t GoRequest::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = sizeof(mX) + sizeof(mY);
    if(size < numBytesRead)
        return -1;

    readStream(mX, data);
    readStream(mY, data);

    return numBytesRead;
}

void GoRequest::toBinaryDerived(std::vector<char>& out) const
{
    writeStream(mX, out);
    writeStream(mY, out);
}
