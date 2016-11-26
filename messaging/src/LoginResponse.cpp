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
#include "LoginResponse.hpp"
#include "utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

size_t LoginResponse::getSizeDerived() const
{
    size_t size = 0;
    for(const Character& c : mCharacters)
        size += c.name.size() + c.world.size();

    return size + sizeof(SIZE_TYPE) * (mCharacters.size() * 2 + 1);
}

size_t LoginResponse::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = sizeof(SIZE_TYPE);
    if(size < numBytesRead)
        return -1;


    SIZE_TYPE numCharacters;
    readStream(numCharacters, data);
    mCharacters.resize(numCharacters);

    for(Character& c : mCharacters)
    {
        numBytesRead += sizeof(SIZE_TYPE) * 2;
        if(size < numBytesRead)
            return -1;

        SIZE_TYPE nameSize;
        readStream(nameSize, data);
        SIZE_TYPE worldSize;
        readStream(worldSize, data);

        numBytesRead += nameSize + worldSize;
        if(size < numBytesRead)
            return -1;

        c.name.assign(data, nameSize);
        data += nameSize;

        c.world.assign(data, worldSize);
        data += worldSize;
    }

    return numBytesRead;
}

void LoginResponse::toBinaryDerived(std::vector<char>& out) const
{
    SIZE_TYPE numCharacters = mCharacters.size();
    writeStream(numCharacters, out);

    for(const Character& c : mCharacters)
    {
        SIZE_TYPE nameSize = c.name.size();
        writeStream(nameSize, out);

        SIZE_TYPE worldSize = c.world.size();
        writeStream(worldSize, out);


        writeStream(*c.name.c_str(), out, nameSize);
        writeStream(*c.world.c_str(), out, worldSize);
    }
}

const std::vector<LoginResponse::Character>& LoginResponse::getCharacters() const
{
    return mCharacters;
}

void LoginResponse::addCharacter(const std::string& character, const std::string& world)
{
    mCharacters.emplace_back();
    Character& c = mCharacters.back();
    c.name.assign(character, 0, MAX_STRING_LENGTH);
    c.world.assign(world, 0, MAX_STRING_LENGTH);
}
