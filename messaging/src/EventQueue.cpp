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
#include "messaging/EventQueue.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

EventQueue::EventQueue()
: Message(Message::Type::EVENT)
{
}

size_t EventQueue::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = sizeof(SIZE_TYPE);
    if(size < numBytesRead)
        return -1;

    SIZE_TYPE bufferSize;
    readStream(bufferSize, data);

    numBytesRead += bufferSize;
    if(size < numBytesRead)
        return -1;

    mBuffer.resize(bufferSize);
    if(bufferSize == 0)
    {
        return numBytesRead;
    }

    readStream(*mBuffer.data(), data, bufferSize);
    return numBytesRead;
}

void EventQueue::toBinaryDerived(std::vector<char>& out) const
{
    writeStream(SIZE_TYPE(mBuffer.size()), out);
    if(mBuffer.empty())
    {
        return;
    }
    writeStream(*mBuffer.data(), out, mBuffer.size());
}

EventType EventQueue::readType()
{
    EventType type = readEventType(mBuffer.data() + mIndex, mBuffer.size() - mIndex);
    mIndex = std::min(mBuffer.size(), mIndex + sizeof(type));
    return type;
}
