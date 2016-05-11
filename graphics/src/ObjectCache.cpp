///////////////////////////////////
// Internal ShankBot headers
#include "ObjectCache.hpp"
#include "TileBuffer.hpp"
///////////////////////////////////

using namespace GraphicsLayer;

///////////////////////////////////

bool ObjectCache::get(unsigned int textureId, size_t x, size_t y, std::set<Object>& sprite) const
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mSprites.end())
        return false;

    auto s = get(indexMap->second, x, y);
    if(s == indexMap->second.end())
        return false;

    sprite = s->second;
    return true;
}

///////////////////////////////////

void ObjectCache::set(unsigned int textureId, size_t x, size_t y, std::set<Object> sprite)
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mSprites.end())
    {
        auto insertion = mSprites.insert(std::make_pair(textureId, SpriteIndexMap()));
        indexMap = insertion.first;
    }

    set(indexMap->second, x, y, sprite);
}

///////////////////////////////////

void ObjectCache::remove(unsigned int textureId, size_t x, size_t y)
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mSprites.end())
        return;

    indexMap->second.erase(TileBuffer::coordsToIndex(x, y));
}

///////////////////////////////////

ObjectCache::SpriteMap::const_iterator ObjectCache::getIndexMap(unsigned int textureId) const
{
    return mSprites.find(textureId);
}

///////////////////////////////////

ObjectCache::SpriteMap::iterator ObjectCache::getIndexMap(unsigned int textureId)
{
    return mSprites.find(textureId);
}


///////////////////////////////////

ObjectCache::SpriteIndexMap::const_iterator ObjectCache::get(const SpriteIndexMap& indexMap, size_t x, size_t y) const
{
    return indexMap.find(TileBuffer::coordsToIndex(x, y));
}

///////////////////////////////////

void ObjectCache::set(SpriteIndexMap& indexMap, size_t x, size_t y, std::set<Object> sprite)
{
    indexMap[TileBuffer::coordsToIndex(x, y)] = sprite;
}
