///////////////////////////////////
// Internal ShankBot headers
#include "TileBuffer.hpp"
///////////////////////////////////

namespace GraphicsLayer
{

///////////////////////////////////

template<typename ElementType>
bool TileBufferCache<ElementType>::get(unsigned int textureId, size_t x, size_t y, ElementType& element) const
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mElements.end())
        return false;

    auto s = get(indexMap->second, x, y);
    if(s == indexMap->second.end())
        return false;

    element = s->second;
    return true;
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::set(unsigned int textureId, size_t x, size_t y, const ElementType& element)
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mElements.end())
    {
        auto insertion = mElements.insert(std::make_pair(textureId, ElementIndexMap()));
        indexMap = insertion.first;
    }

    set(indexMap->second, x, y, element);
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::remove(unsigned int textureId, size_t x, size_t y)
{
    auto indexMap = getIndexMap(textureId);
    if(indexMap == mElements.end())
        return;

    indexMap->second.erase(TileBuffer::coordsToIndex(x, y));
}

///////////////////////////////////

template<typename ElementType>
typename TileBufferCache<ElementType>::ElementMap::const_iterator TileBufferCache<ElementType>::getIndexMap(unsigned int textureId) const
{
    return mElements.find(textureId);
}

///////////////////////////////////

template<typename ElementType>
typename TileBufferCache<ElementType>::ElementMap::iterator TileBufferCache<ElementType>::getIndexMap(unsigned int textureId)
{
    return mElements.find(textureId);
}


///////////////////////////////////

template<typename ElementType>
typename TileBufferCache<ElementType>::ElementIndexMap::const_iterator TileBufferCache<ElementType>::get(const ElementIndexMap& indexMap, size_t x, size_t y) const
{
    return indexMap.find(TileBuffer::coordsToIndex(x, y));
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::set(ElementIndexMap& indexMap, size_t x, size_t y, const ElementType& element)
{
    indexMap[TileBuffer::coordsToIndex(x, y)] = element;
}

///////////////////////////////////
}
