///////////////////////////////////
// Internal ShankBot headers
#include "SpriteObjectBindings.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
///////////////////////////////////

SpriteObjectBindings::SpriteObjectBindings(const TibiaDat& dat)
: mDat(dat)
{
    mBindings.clear();
    parseObjects();
}

SpriteObjectBindings::SpriteObjectBindings(const TibiaDat& dat, std::string binPath)
: mDat(dat)
{
    mBindings.clear();
    readFromBinaryFile(binPath);
}

void SpriteObjectBindings::readFromBinaryFile(std::string binPath)
{
    std::ifstream file(binPath, std::ios::binary);

    unsigned int datVersion;
    file.read((char*)&datVersion, sizeof(datVersion));
    if(datVersion != mDat.getVersion())
    {
        std::stringstream sstream;
        sstream << "SpriteObjectBindings file and Tibia.dat file version do not match. "
                << "SpriteObjectBindings file has " << datVersion << " while Tibia.dat has " << mDat.getVersion() << "." << std::endl
                << "Path to SpriteObjectBindings file: " << binPath << "." << std::endl
                << "Path to Tibia.dat file: " << mDat.getPath() << "." << std::endl;

        throw std::runtime_error(sstream.str());
    }

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

    unsigned int datVersion = mDat.getVersion();
    file.write((char*)&datVersion, sizeof(datVersion));

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
    const std::vector<TibiaDat::Object>& objects = mDat.getObjects();
    for(size_t i = 0; i < objects.size(); i++)
    {
        createBindings(objects[i], i);
    }
}

void SpriteObjectBindings::createBindings(const TibiaDat::Object& o, size_t globalId)
{
    for(const TibiaDat::SpritesInfo& info : o.spritesInfos)
        for(size_t spriteId : info.spriteIds)
        {
            if(spriteId != 0)
            {
                auto it = mBindings.insert(std::make_pair(spriteId, std::set<size_t>()));
                it.first->second.insert(globalId);
            }
        }
}

std::list<const TibiaDat::Object*> SpriteObjectBindings::getObjects(size_t spriteId) const
{
    std::list<const TibiaDat::Object*> objects;

    auto found = mBindings.find(spriteId);
    if(found == mBindings.end())
        return objects;

    for(size_t id : found->second)
        objects.push_back(&mDat.getObjects()[id]);

    return objects;
}
