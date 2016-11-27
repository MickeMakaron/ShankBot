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
#include "messaging/Response.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
using namespace sb;
///////////////////////////////////

size_t Response::getSizeDerived() const
{
    return sizeof(mResult) + sizeof(mResponseType);
}

size_t Response::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesToRead = Response::getSizeDerived();
    if(size < numBytesToRead)
        return -1;

    readStream(mResponseType, data);
    readStream(mResult, data);

    return numBytesToRead;
}

void Response::toBinaryDerived(std::vector<char>& out) const
{
    writeStream(mResponseType, out);
    writeStream(mResult, out);
}

void Response::set(RequestResult result)
{
    mResult = result;
}

RequestResult Response::getResult() const
{
    return mResult;
}

Message::Type Response::getResponseType() const
{
    return mResponseType;
}


Message::Type Response::readResponseType(const char* data, size_t size)
{
    Response r;
    if(!r.fromBinary(data, size))
        return Message::Type::INVALID;

    return r.getResponseType();
}
