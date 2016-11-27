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
#include "messaging/LoginRequest.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

size_t LoginRequest::getSizeDerived() const
{
    return sizeof(SIZE_TYPE) * 2 + mAccountName.size() + mPassword.size();
}

size_t LoginRequest::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = sizeof(SIZE_TYPE) * 2;
    if(size < numBytesRead)
        return -1;

    SIZE_TYPE accountNameSize;
    readStream(accountNameSize, data);

    SIZE_TYPE passwordSize;
    readStream(passwordSize, data);

    numBytesRead += accountNameSize + passwordSize;
    if(size < numBytesRead)
        return -1;


    mAccountName.assign(data, accountNameSize);
    data += accountNameSize;

    mPassword.assign(data, passwordSize);

    return numBytesRead;
}

void LoginRequest::toBinaryDerived(std::vector<char>& out) const
{
    SIZE_TYPE accountNameSize = mAccountName.size();
    writeStream(accountNameSize, out);

    SIZE_TYPE passwordSize = mPassword.size();
    writeStream(passwordSize, out);

    writeStream(*mAccountName.c_str(), out, accountNameSize);
    writeStream(*mPassword.c_str(), out, passwordSize);
}

void LoginRequest::set(const std::string& accountName, const std::string& password)
{
    mAccountName.assign(accountName, 0, MAX_STRING_LENGTH);
    mPassword.assign(password, 0, MAX_STRING_LENGTH);
}

const std::string& LoginRequest::getAccountName() const
{
    return mAccountName;
}

const std::string& LoginRequest::getPassword() const
{
    return mPassword;
}
