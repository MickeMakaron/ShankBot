///////////////////////////////////
// Internal ShankBot headers
#include "SpriteObjectBindings.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


std::vector<SpriteObjectBindings::Object> SpriteObjectBindings::mObjects;
size_t SpriteObjectBindings::mNumItems;
std::map<size_t, std::set<size_t>> SpriteObjectBindings::mBindings;


void SpriteObjectBindings::initialize(const TibiaDat& dat)
{
    mObjects.clear();
    mNumItems = 0;
    mBindings.clear();

    parseObjects(dat);
}

void SpriteObjectBindings::initialize(std::string binPath)
{
    mObjects.clear();
    mNumItems = 0;
    mBindings.clear();

    readFromBinaryFile(binPath);
}

void SpriteObjectBindings::readFromBinaryFile(std::string binPath)
{
    std::ifstream file(binPath, std::ios::binary);

    unsigned int numObjects;
    file.read((char*)&numObjects, sizeof(numObjects));

    mObjects.resize(numObjects);
    file.read((char*)mObjects.data(), sizeof(Object) * numObjects);

    for(size_t i = 0; i < numObjects; i++)
    {
        TibiaDat::SpritesInfo* spritesInfo = new TibiaDat::SpritesInfo();
        file.read((char*)spritesInfo, sizeof(TibiaDat::SpritesInfo));
        mObjects[i].spritesInfo.release();
        mObjects[i].spritesInfo.reset(spritesInfo);
    }

    file.read((char*)&mNumItems, sizeof(mNumItems));

    for(size_t i = 0; i < mNumItems; i++)
    {
        TibiaDat::ItemInfo* item = new TibiaDat::ItemInfo();
        file.read((char*)item, sizeof(TibiaDat::ItemInfo));
        mObjects[i].itemInfo.release();
        mObjects[i].itemInfo.reset(item);
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

void SpriteObjectBindings::writeToBinaryFile(std::string path)
{
    std::ofstream file(path, std::ios::binary);
    unsigned int numObjects = mObjects.size();
    file.write((char*)&numObjects, sizeof(numObjects));

    file.write((char*)mObjects.data(), sizeof(Object) * numObjects);

    for(size_t i = 0; i < numObjects; i++)
    {
        file.write((char*)mObjects[i].spritesInfo.get(), sizeof(TibiaDat::SpritesInfo));
    }

    file.write((char*)&mNumItems, sizeof(mNumItems));

    for(size_t i = 0; i < mNumItems; i++)
    {

        file.write((char*)mObjects[i].itemInfo.get(), sizeof(TibiaDat::ItemInfo));
    }


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

void SpriteObjectBindings::parseObjects(const TibiaDat& dat)
{
    const std::vector<TibiaDat::Object>& items = dat.getItems();
    const std::vector<TibiaDat::ItemInfo>& itemInfos = dat.getItemInfos();
    const std::vector<TibiaDat::Object>& outfits = dat.getOutfits();
    const std::vector<TibiaDat::Object>& effects = dat.getEffects();
    const std::vector<TibiaDat::Object>& distances = dat.getDistances();
    const std::vector<TibiaDat::SpritesInfo>& spritesInfos = dat.getSpritesInfos();

    mNumItems = items.size();
    mObjects.clear();
    mObjects.resize(spritesInfos.size());
    size_t globalId = 0;
    for(size_t i = 0; i < items.size(); i++)
    {
        mObjects[globalId] =
        {
            Object::Type::ITEM,
            items[i].id,
            std::unique_ptr<TibiaDat::SpritesInfo>(new TibiaDat::SpritesInfo(spritesInfos[globalId])),
            std::unique_ptr<TibiaDat::ItemInfo>(new TibiaDat::ItemInfo(itemInfos[i]))
        };

        createBindings(items[i], globalId);
        globalId++;
    }


    for(auto o : outfits)
    {
        mObjects[globalId] =
        {
            Object::Type::OUTFIT,
            o.id,
            std::unique_ptr<TibiaDat::SpritesInfo>(new TibiaDat::SpritesInfo(spritesInfos[globalId]))
        };
        createBindings(o, globalId);
        globalId++;
    }


    for(auto o : effects)
    {
        mObjects[globalId] =
        {
            Object::Type::EFFECT,
            o.id,
            std::unique_ptr<TibiaDat::SpritesInfo>(new TibiaDat::SpritesInfo(spritesInfos[globalId]))
        };
        createBindings(o, globalId);
        globalId++;
    }


    for(auto o : distances)
    {
        mObjects[globalId] =
        {
            Object::Type::DISTANCE,
            o.id,
            std::unique_ptr<TibiaDat::SpritesInfo>(new TibiaDat::SpritesInfo(spritesInfos[globalId]))
        };
        createBindings(o, globalId);
        globalId++;
    }
}

void SpriteObjectBindings::createBindings(const TibiaDat::Object& o, size_t globalId)
{
    for(size_t spriteId : o.spriteIds)
    {
        if(spriteId != 0)
        {
            auto it = mBindings.insert(std::make_pair(spriteId, std::set<size_t>()));
            it.first->second.insert(globalId);
        }
    }
}

std::list<const SpriteObjectBindings::Object*> SpriteObjectBindings::getObjects(size_t spriteId)
{
    std::list<const Object*> objects;

    auto found = mBindings.find(spriteId);
    if(found == mBindings.end())
    {
//        std::stringstream sstream;
//        sstream << "Bindings for sprite ID " << spriteId << " does not exist.";
//        throw std::runtime_error(sstream.str());
        return objects;
    }

    for(size_t id : found->second)
        objects.push_back(&mObjects[id]);

    return objects;
}
