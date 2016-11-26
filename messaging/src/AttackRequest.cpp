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
#include "AttackRequest.hpp"
#include "utility.hpp"
using namespace sb::messaging;
using namespace sb::utility;
///////////////////////////////////

size_t AttackRequest::getSizeDerived() const
{
    return sizeof(mX) + sizeof(mY);
}

void AttackRequest::set(unsigned short x, unsigned short y)
{
    mX = x;
    mY = y;
}

unsigned short AttackRequest::getX() const
{
    return mX;
}

unsigned short AttackRequest::getY() const
{
    return mY;
}


size_t AttackRequest::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = getSizeDerived();
    if(size < numBytesRead)
        return -1;

    readStream(mX, data);
    readStream(mY, data);

    return numBytesRead;
}

void AttackRequest::toBinaryDerived(std::vector<char>& out) const
{
    writeStream(mX, out);
    writeStream(mY, out);
}
