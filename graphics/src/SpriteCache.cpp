///////////////////////////////////
// Internal ShankBot headers
#include "SpriteCache.hpp"
#include "TileBuffer.hpp"
///////////////////////////////////

using namespace GraphicsLayer;

///////////////////////////////////

bool SpriteCache::getSprite(unsigned int textureId, size_t x, size_t y, Sprite& sprite) const
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mSprites.end())
        return false;

    auto s = getSprite(indexMap->second, x, y);
    if(s == indexMap->second.end())
        return false;

    sprite = s->second;
    return true;
}

///////////////////////////////////

void SpriteCache::setSprite(unsigned int textureId, size_t x, size_t y, Sprite sprite)
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mSprites.end())
    {
        auto insertion = mSprites.insert(std::make_pair(textureId, SpriteIndexMap()));
        indexMap = insertion.first;
    }

    setSprite(indexMap->second, x, y, sprite);
}

///////////////////////////////////

void SpriteCache::deleteSprite(unsigned int textureId, size_t x, size_t y)
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mSprites.end())
        return;

    indexMap->second.erase(TileBuffer::coordsToIndex(x, y));
}

///////////////////////////////////

SpriteCache::SpriteMap::const_iterator SpriteCache::getIndexMap(unsigned int textureId) const
{
    return mSprites.find(textureId);
}

///////////////////////////////////

SpriteCache::SpriteMap::iterator SpriteCache::getIndexMap(unsigned int textureId)
{
    return mSprites.find(textureId);
}


///////////////////////////////////

SpriteCache::SpriteIndexMap::const_iterator SpriteCache::getSprite(const SpriteIndexMap& indexMap, size_t x, size_t y) const
{
    return indexMap.find(TileBuffer::coordsToIndex(x, y));
}

///////////////////////////////////

void SpriteCache::setSprite(SpriteIndexMap& indexMap, size_t x, size_t y, Sprite sprite)
{
    indexMap[TileBuffer::coordsToIndex(x, y)] = sprite;
}
