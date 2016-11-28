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
#include "messaging/ObjectResponse.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
using namespace sb;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////
void ObjectResponse::objectToBinary(const sb::Object& obj, std::vector<char>& out)
{
    const char* data = (const char*)&obj;

    writeStream(obj.type, out);
    writeStream(obj.id, out);

    const sb::tibiaassets::Object::ItemInfo& info = obj.itemInfo;
    writeStream(info.isGround, out);
    writeStream(info.unknown1, out);
    writeStream(info.walkSpeed, out);
    writeStream(info.topOrder, out);
    writeStream(info.isContainer, out);
    writeStream(info.isStackable, out);
    writeStream(info.isUsable, out);
    writeStream(info.isReadable, out);
    writeStream(info.isWritable, out);
    writeStream(info.maxCharacters, out);
    writeStream(info.isFluid, out);
    writeStream(info.isSplash, out);
    writeStream(info.isBlocking, out);
    writeStream(info.isMovable, out);
    writeStream(info.isBlockingRangedAttack, out);
    writeStream(info.isPathBlocking, out);
    writeStream(info.isPickupable, out);
    writeStream(info.isHangable, out);
    writeStream(info.isWall, out);
    writeStream(info.isRotatable, out);
    writeStream(info.isFloorChange, out);
    writeStream(info.hasIdleAnimation, out);
    writeStream(info.lightBrightness, out);
    writeStream(info.lightColor, out);
    writeStream(info.height, out);
    writeStream(info.hasMinimapColor, out);
    writeStream(info.minimapColor, out);
    writeStream(info.isTransparent, out);
    writeStream(info.defaultAction, out);
    writeStream(info.offsetX, out);
    writeStream(info.offsetY, out);
    writeStream(info.bodyRestriction, out);
    writeStream(info.isItemDestroyer, out);
    writeStream(info.hasMarketInfo, out);

    const sb::tibiaassets::Object::MarketInfo& mi = info.marketInfo;
    writeStream(mi.id1, out);
    writeStream(mi.id2, out);
    writeStream(mi.vocationRestriction, out);
    writeStream(mi.classRestriction, out);
    writeStream(mi.levelRestriction, out);

    assert(mi.name.size() < SMALL_SIZE_TYPE(-1));
    SMALL_SIZE_TYPE numCharacters = mi.name.size();
    writeStream(numCharacters, out);
    if(numCharacters > 0)
        writeStream(*mi.name.c_str(), out, mi.name.size());
}

size_t ObjectResponse::objectFromBinary(sb::Object& obj, const char* data, size_t size)
{
    const char* start = data;
    const char* end = start + size;

    readStreamSafe(obj.type, data, end);
    readStreamSafe(obj.id, data, end);

    sb::tibiaassets::Object::ItemInfo& info = obj.itemInfo;

    readStreamSafe(info.isGround, data, end);
    readStreamSafe(info.unknown1, data, end);
    readStreamSafe(info.walkSpeed, data, end);
    readStreamSafe(info.topOrder, data, end);
    readStreamSafe(info.isContainer, data, end);
    readStreamSafe(info.isStackable, data, end);
    readStreamSafe(info.isUsable, data, end);
    readStreamSafe(info.isReadable, data, end);
    readStreamSafe(info.isWritable, data, end);
    readStreamSafe(info.maxCharacters, data, end);
    readStreamSafe(info.isFluid, data, end);
    readStreamSafe(info.isSplash, data, end);
    readStreamSafe(info.isBlocking, data, end);
    readStreamSafe(info.isMovable, data, end);
    readStreamSafe(info.isBlockingRangedAttack, data, end);
    readStreamSafe(info.isPathBlocking, data, end);
    readStreamSafe(info.isPickupable, data, end);
    readStreamSafe(info.isHangable, data, end);
    readStreamSafe(info.isWall, data, end);
    readStreamSafe(info.isRotatable, data, end);
    readStreamSafe(info.isFloorChange, data, end);
    readStreamSafe(info.hasIdleAnimation, data, end);
    readStreamSafe(info.lightBrightness, data, end);
    readStreamSafe(info.lightColor, data, end);
    readStreamSafe(info.height, data, end);
    readStreamSafe(info.hasMinimapColor, data, end);
    readStreamSafe(info.minimapColor, data, end);
    readStreamSafe(info.isTransparent, data, end);
    readStreamSafe(info.defaultAction, data, end);
    readStreamSafe(info.offsetX, data, end);
    readStreamSafe(info.offsetY, data, end);
    readStreamSafe(info.bodyRestriction, data, end);
    readStreamSafe(info.isItemDestroyer, data, end);
    readStreamSafe(info.hasMarketInfo, data, end);

    sb::tibiaassets::Object::MarketInfo& mi = obj.itemInfo.marketInfo;
    readStreamSafe(mi.id1, data, end);
    readStreamSafe(mi.id2, data, end);
    readStreamSafe(mi.vocationRestriction, data, end);
    readStreamSafe(mi.classRestriction, data, end);
    readStreamSafe(mi.levelRestriction, data, end);

    SMALL_SIZE_TYPE numCharacters;
    if(!readStreamSafe(numCharacters, data, end))
        return -1;

    size_t numBytesRead = data - start;
    numBytesRead += numCharacters;
    if(size < numBytesRead)
        return -1;

    if(numCharacters > 0)
        mi.name.assign(data, numCharacters);

    return numBytesRead;
}

void ObjectResponse::set(const std::vector<sb::tibiaassets::Object>& objects)
{
    mObjects.resize(objects.size());
    for(size_t i = 0; i < mObjects.size(); i++)
    {
        const sb::tibiaassets::Object& o = objects[i];
        Object& mo = mObjects[i];

        mo.id = o.id;
        mo.type = o.type;
        mo.itemInfo = o.itemInfo;
    }
}

const std::vector<Object>& ObjectResponse::get() const
{
    return mObjects;
}

size_t ObjectResponse::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = Response::fromBinaryDerived(data, size);
    if(size < numBytesRead)
        return -1;
    data += numBytesRead;

    LARGE_SIZE_TYPE numObjects;
    numBytesRead += sizeof(numObjects);
    if(size < numBytesRead)
        return -1;
    readStream(numObjects, data);

    mObjects.resize(numObjects);
    for(size_t i = 0; i < numObjects; i++)
    {
        size_t objSize = objectFromBinary(mObjects[i], data, size - numBytesRead);
        if(objSize == -1)
            return -1;
        data += objSize;

        numBytesRead += objSize;
        if(size < numBytesRead)
            return -1;
    }
}

void ObjectResponse::toBinaryDerived(std::vector<char>& out) const
{
    Response::toBinaryDerived(out);
    LARGE_SIZE_TYPE numObjects = mObjects.size();
    writeStream(numObjects, out);
    for(const Object& obj : mObjects)
        objectToBinary(obj, out);
}
