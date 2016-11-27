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
#include "messaging/FrameResponse.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////

size_t FrameResponse::getSizeDerived() const
{
    size_t size = 0;
    for(size_t i = 0; i < Scene::SIZE; i++)
        size += mFrame.scene.objects[0][i].size();

    size += sizeof(SIZE_TYPE) * Scene::SIZE;

    size += sizeof(SIZE_TYPE) * (mFrame.scene.npcs.size() + 1);
    size += (sizeof(Npc::x) + sizeof(Npc::y) + sizeof(Npc::id)) * mFrame.scene.npcs.size();
    for(const Npc& n : mFrame.scene.npcs)
        size += n.name.size() + sizeof(n.x) + sizeof(n.y) + sizeof(n.id);

    size += sizeof(SIZE_TYPE) * (mFrame.scene.players.size() + 1);
    size += (sizeof(Player::x) + sizeof(Player::y) + sizeof(Player::hp)) * mFrame.scene.players.size();
    for(const Player& p : mFrame.scene.players)
        size += p.name.size();

    size += sizeof(SIZE_TYPE) * (mFrame.scene.creatures.size() + 1);
    size += (sizeof(Creature::x) + sizeof(Creature::y) + sizeof(Creature::id) + sizeof(Creature::hp)) * mFrame.scene.creatures.size();
    for(const Creature& c : mFrame.scene.creatures)
        size += c.name.size();

    size += sizeof(mFrame.miniMap);

    return Response::getSizeDerived() + size;
}

void FrameResponse::set(const Frame& frame)
{
    mFrame = frame;
}

const FrameResponse::Frame& FrameResponse::get() const
{
    return mFrame;
}

size_t FrameResponse::fromBinaryDerived(const char* data, size_t size)
{
    size_t numBytesRead = Response::fromBinaryDerived(data, size);
    if(size < numBytesRead)
        return -1;
    data += numBytesRead;
    for(size_t i = 0; i < Scene::SIZE; i++)
    {
        numBytesRead += sizeof(SIZE_TYPE);
        if(size < numBytesRead)
            return -1;

        SIZE_TYPE numObjects;
        readStream(numObjects, data);

        numBytesRead += numObjects * sizeof(Object);
        if(size < numBytesRead)
            return -1;

        std::vector<Object>& objects = mFrame.scene.objects[0][i];
        objects.resize(numObjects);
        readStream(*objects.data(), data, objects.size());
    }

    numBytesRead += sizeof(SIZE_TYPE);
    if(size < numBytesRead)
        return -1;

    SIZE_TYPE numNpcs;
    readStream(numNpcs, data);

    mFrame.scene.npcs.resize(numNpcs);
    for(Npc& npc : mFrame.scene.npcs)
    {
        numBytesRead += sizeof(SIZE_TYPE);
        if(size < numBytesRead)
            return -1;

        SIZE_TYPE strLength;
        readStream(strLength, data);

        numBytesRead += strLength;
        if(size < numBytesRead)
            return -1;

        npc.name.assign(data, strLength);
        data += strLength;

        numBytesRead += sizeof(Npc::x) + sizeof(Npc::y) + sizeof(Npc::id);
        if(size < numBytesRead)
            return -1;

        readStream(npc.x, data);
        readStream(npc.y, data);
        readStream(npc.id, data);
    }

    numBytesRead += sizeof(SIZE_TYPE);
    if(size < numBytesRead)
        return -1;

    SIZE_TYPE numCreatures;
    readStream(numCreatures, data);

    mFrame.scene.creatures.resize(numCreatures);
    for(Creature& creature : mFrame.scene.creatures)
    {
        numBytesRead += sizeof(SIZE_TYPE);
        if(size < numBytesRead)
            return -1;

        SIZE_TYPE strLength;
        readStream(strLength, data);

        numBytesRead += strLength;
        if(size < numBytesRead)
            return -1;

        creature.name.assign(data, strLength);
        data += strLength;

        numBytesRead += sizeof(Creature::x) + sizeof(Creature::y) + sizeof(Creature::id) + sizeof(Creature::hp);
        if(size < numBytesRead)
            return -1;

        readStream(creature.x, data);
        readStream(creature.y, data);
        readStream(creature.hp, data);
        readStream(creature.id, data);
    }


    numBytesRead += sizeof(SIZE_TYPE);
    if(size < numBytesRead)
        return -1;

    SIZE_TYPE numPlayers;
    readStream(numPlayers, data);

    mFrame.scene.players.resize(numPlayers);
    for(Player& player : mFrame.scene.players)
    {
        numBytesRead += sizeof(SIZE_TYPE);
        if(size < numBytesRead)
            return -1;

        SIZE_TYPE strLength;
        readStream(strLength, data);

        numBytesRead += strLength;
        if(size < numBytesRead)
            return -1;

        player.name.assign(data, strLength);
        data += strLength;

        numBytesRead += sizeof(Player::x) + sizeof(Player::y) + sizeof(Player::hp);
        if(size < numBytesRead)
            return -1;

        readStream(player.x, data);
        readStream(player.y, data);
        readStream(player.hp, data);
    }

    numBytesRead += sizeof(MiniMap);
    if(size < numBytesRead)
        return -1;

    readStream(mFrame.miniMap, data);


    return numBytesRead;
}

void FrameResponse::toBinaryDerived(std::vector<char>& out) const
{
    Response::toBinaryDerived(out);
    for(size_t i = 0; i < Scene::SIZE; i++)
    {
        const std::vector<Object>& objects = mFrame.scene.objects[0][i];
        assert(objects.size() <= SIZE_TYPE(-1));

        SIZE_TYPE size = objects.size();
        writeStream(size, out);

        if(size > 0)
            writeStream(*objects.data(), out, size);
    }

    assert(mFrame.scene.npcs.size() <= SIZE_TYPE(-1));
    SIZE_TYPE numNpcs = mFrame.scene.npcs.size();
    writeStream(numNpcs, out);

    for(const Npc& npc : mFrame.scene.npcs)
    {
        assert(npc.name.size() <= SIZE_TYPE(-1));
        SIZE_TYPE strLength = npc.name.size();
        writeStream(strLength, out);

        if(strLength > 0)
            writeStream(*npc.name.c_str(), out, strLength);

        writeStream(npc.x, out);
        writeStream(npc.y, out);
        writeStream(npc.id, out);
    }

    assert(mFrame.scene.creatures.size() <= SIZE_TYPE(-1));
    SIZE_TYPE numCreatures = mFrame.scene.creatures.size();
    writeStream(numCreatures, out);

    for(const Creature& creature : mFrame.scene.creatures)
    {
        assert(creature.name.size() <= SIZE_TYPE(-1));
        SIZE_TYPE strLength = creature.name.size();
        writeStream(strLength, out);

        if(strLength > 0)
            writeStream(*creature.name.c_str(), out, strLength);

        writeStream(creature.x, out);
        writeStream(creature.y, out);
        writeStream(creature.id, out);
        writeStream(creature.hp, out);
    }

    assert(mFrame.scene.players.size() <= SIZE_TYPE(-1));
    SIZE_TYPE numPlayers = mFrame.scene.players.size();
    writeStream(numPlayers, out);

    for(const Player& player : mFrame.scene.players)
    {
        assert(player.name.size() <= SIZE_TYPE(-1));
        SIZE_TYPE strLength = player.name.size();
        writeStream(strLength, out);

        if(strLength > 0)
            writeStream(*player.name.c_str(), out, strLength);

        writeStream(player.x, out);
        writeStream(player.y, out);
        writeStream(player.hp, out);
    }

    writeStream(mFrame.miniMap, out);
}
