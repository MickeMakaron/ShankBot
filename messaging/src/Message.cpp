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
#include "messaging/Message.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

size_t Message::fromBinary(const char* data, size_t size)
{
    if(size < sizeof(M_MESSAGE_TYPE))
        return 0;

    Type type;
    readStream(type, data);
    if(type != M_MESSAGE_TYPE)
        return 0;
    size -= sizeof(type);

    size_t readBytes = fromBinaryDerived(data, size);
    if(readBytes == -1)
        return 0;

    return sizeof(type) + readBytes;
}

void Message::toBinary(std::vector<char>& out) const
{
    writeStream(M_MESSAGE_TYPE, out);
    toBinaryDerived(out);
}

Message::Type Message::readMessageType(const char* data, size_t size)
{
    if(size < sizeof(Type))
        return Type::INVALID;

    return *(Type*)data;
}

size_t Message::fromBinaryDerived(const char* data, size_t size)
{
    return 0;
}

void Message::toBinaryDerived(std::vector<char>& out) const
{
    return;
}
