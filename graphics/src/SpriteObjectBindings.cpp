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
#include "graphics/SpriteObjectBindings.hpp"
using namespace sb::tibiaassets;
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
#include <fstream>
///////////////////////////////////

SpriteObjectBindings::SpriteObjectBindings(const std::vector<Object>& objects)
: mObjects(objects)
{
    mBindings.clear();
    parseObjects();
}

SpriteObjectBindings::SpriteObjectBindings(const std::vector<Object>& objects, std::string binPath)
: mObjects(objects)
{
    mBindings.clear();
    readFromBinaryFile(binPath);
}

void SpriteObjectBindings::readFromBinaryFile(std::string binPath)
{
    std::ifstream file(binPath, std::ios::binary);

    unsigned int numSprites;
    file.read((char*)&numSprites, sizeof(numSprites));
    for(size_t i = 0; i < numSprites; i++)
    {
        size_t spriteId;
        file.read((char*)&spriteId, sizeof(spriteId));

        auto it = mBindings.insert(std::make_pair(spriteId, std::set<size_t>()));

        unsigned int numBindedObjects;
        file.read((char*)&numBindedObjects, sizeof(numBindedObjects));

        for(size_t j = 0; j < numBindedObjects; j++)
        {
            size_t objectId;
            file.read((char*)&objectId, sizeof(objectId));
            it.first->second.insert(objectId);
        }
    }

    file.close();
}

void SpriteObjectBindings::writeToBinaryFile(std::string path) const
{
    std::ofstream file(path, std::ios::binary);

    unsigned int numSprites = mBindings.size();
    file.write((char*)&numSprites, sizeof(numSprites));
    for(auto pair : mBindings)
    {
        file.write((char*)&pair.first, sizeof(pair.first));

        unsigned int numBindedObjects = pair.second.size();
        file.write((char*)&numBindedObjects, sizeof(numBindedObjects));

        for(size_t object : pair.second)
            file.write((char*)&object, sizeof(object));
    }

    file.close();
}

void SpriteObjectBindings::parseObjects()
{
    for(size_t i = 0; i < mObjects.size(); i++)
    {
        createBindings(mObjects[i], i);
    }
}

void SpriteObjectBindings::createBindings(const Object& o, size_t globalId)
{
    for(const Object::SomeInfo& info : o.someInfos)
        for(size_t spriteId : info.spriteInfo.spriteIds)
        {
            if(spriteId != 0)
            {
                auto it = mBindings.insert(std::make_pair(spriteId, std::set<size_t>()));
                it.first->second.insert(globalId);
            }
        }
}

std::list<const Object*> SpriteObjectBindings::getObjects(size_t spriteId) const
{
    std::list<const Object*> objects;

    auto found = mBindings.find(spriteId);
    if(found == mBindings.end())
        return objects;

    for(size_t id : found->second)
        objects.push_back(&mObjects[id]);

    return objects;
}
