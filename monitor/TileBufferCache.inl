// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
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
namespace GraphicsLayer
{
///////////////////////////////////

template<typename ElementType>
TileBufferCache<ElementType>::TileBufferCache(size_t width)
: M_WIDTH(width)
{

}

///////////////////////////////////

template<typename ElementType>
bool TileBufferCache<ElementType>::get(unsigned int textureId, size_t x, size_t y, ElementType& element) const
{
    auto indexMap = mElements.find(textureId);
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
bool TileBufferCache<ElementType>::getClosestTopLeft(unsigned int textureId, size_t x, size_t y, AreaElement& element) const
{
    auto indexMap = mElements.find(textureId);
    if(indexMap == mElements.end())
        return false;

    auto s = getClosestTopLeft(indexMap->second, x, y);
    if(s == indexMap->second.end())
        return false;

    size_t minDiff = -1;
    auto minElement = s;
    s++;
    while(s != indexMap->second.begin())
    {
        s--;

        size_t elementX;
        size_t elementY;
        indexToCoords(s->first, elementX, elementY);

        if(elementX <= x && elementY <= y)
        {
            size_t diff = x - elementX + y - elementY;
            if(diff < minDiff)
            {
                minDiff = diff;
                minElement = s;
            }
        }
    }

    if(minDiff == -1)
        return false;

    size_t elementX;
    size_t elementY;
    indexToCoords(minElement->first, elementX, elementY);
    element.x = elementX;
    element.y = elementY;
    element.element = minElement->second;
    return true;
}


///////////////////////////////////

template<typename ElementType>
bool TileBufferCache<ElementType>::getClosestBotRight(unsigned int textureId, size_t x, size_t y, AreaElement& element) const
{
    auto indexMap = mElements.find(textureId);
    if(indexMap == mElements.end())
        return false;

    size_t index = coordsToIndex(x, y);
    auto s = indexMap->second.lower_bound(index);
    if(s == indexMap->second.end())
        return false;

    size_t minDiff = -1;
    auto minElement = s;
    while(s != indexMap->second.end())
    {
        size_t elementX;
        size_t elementY;
        indexToCoords(s->first, elementX, elementY);
        if(elementX >= x && elementY >= y)
        {
            size_t diff = elementX - x + elementY - y;
            if(diff < minDiff)
            {
                minDiff = diff;
                minElement = s;
            }
        }
        s++;
    }

    size_t elementX;
    size_t elementY;
    indexToCoords(minElement->first, elementX, elementY);
    element.x = elementX;
    element.y = elementY;
    element.element = minElement->second;
    return true;
}


///////////////////////////////////

template<typename ElementType>
std::list<typename TileBufferCache<ElementType>::AreaElement> TileBufferCache<ElementType>::getByArea(unsigned int textureId, size_t x, size_t y, size_t width, size_t height) const
{
    std::list<AreaElement> elements;

    auto indexMap = mElements.find(textureId);
    if(indexMap == mElements.end())
        return elements;

    const size_t MIN_INDEX = coordsToIndex(x, y);
    const size_t MAX_INDEX = coordsToIndex(x + width - 1, y + width - 1);
    const size_t MIN_X = x;
    const size_t MAX_X = x + width - 1;
    for(const auto& pair : indexMap->second)
    {
        const size_t& INDEX = pair.first;
        if(INDEX >= MIN_INDEX)
        {
            if(INDEX > MAX_INDEX)
                return elements;

            size_t elementX;
            size_t elementY;
            indexToCoords(INDEX, elementX, elementY);
            if(elementX >= MIN_X && elementX <= MAX_X)
            {
                AreaElement areaElement;
                areaElement.x = elementX;
                areaElement.y = elementY;
                areaElement.element = pair.second;
                elements.push_back(std::move(areaElement));
            }
        }
    }

    return elements;
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::set(unsigned int textureId, size_t x, size_t y, const ElementType& element)
{
    auto indexMap = mElements.find(textureId);
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
    auto indexMap = mElements.find(textureId);
    if(indexMap == mElements.end())
        return;

    indexMap->second.erase(coordsToIndex(x, y));
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::removeByArea(unsigned int textureId, size_t x, size_t y, size_t width, size_t height)
{
    auto indexMap = mElements.find(textureId);
    if(indexMap == mElements.end())
        return;

    const size_t MIN_INDEX = coordsToIndex(x, y);
    const size_t MAX_INDEX = coordsToIndex(x + width - 1, y + height - 1);
    const size_t MIN_X = x;
    const size_t MAX_X = x + width - 1;
    for(auto it = indexMap->second.begin(); it != indexMap->second.end();)
    {
        const size_t& INDEX = it->first;
        if(INDEX >= MIN_INDEX)
        {
            if(INDEX > MAX_INDEX)
                return;

            size_t elementX;
            size_t elementY;
            indexToCoords(INDEX, elementX, elementY);
            if(elementX >= MIN_X && elementX <= MAX_X)
                it = indexMap->second.erase(it);
            else
                it++;
        }
        else
            it++;
    }
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::clear(unsigned int textureId)
{
    auto indexMap = mElements.find(textureId);
    if(indexMap == mElements.end())
        return;

    indexMap->second.clear();
}

///////////////////////////////////

template<typename ElementType>
typename TileBufferCache<ElementType>::ElementIndexMap::const_iterator TileBufferCache<ElementType>::get(const ElementIndexMap& indexMap, size_t x, size_t y) const
{
    return indexMap.find(coordsToIndex(x, y));
}

///////////////////////////////////

template<typename ElementType>
typename TileBufferCache<ElementType>::ElementIndexMap::const_iterator TileBufferCache<ElementType>::getClosestTopLeft(const ElementIndexMap& indexMap, size_t x, size_t y) const
{
    auto it = indexMap.lower_bound(coordsToIndex(x, y));
    if(it != indexMap.begin())
    {
        it--;
        return it;
    }

    return indexMap.end();
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::forEach(unsigned int textureId, const std::function<bool(const ElementType&, const size_t&, const size_t&)>& func, size_t x, size_t y, bool traverseForward) const
{
    auto indexMapIt = mElements.find(textureId);
    if(indexMapIt == mElements.end())
        return;

    const ElementIndexMap& indexMap = indexMapIt->second;
    if(indexMap.empty())
        return;

    auto it = indexMap.begin();
    if(x != 0 || y != 0)
    {
        it = indexMap.lower_bound(coordsToIndex(x, y));
        if(it == indexMap.end())
            it--;
    }

    if(traverseForward)
    {
        while(it != indexMap.end())
        {
            size_t currX;
            size_t currY;
            indexToCoords(it->first, currX, currY);
            if(!func(it->second, currX, currY))
                return;

            it++;
        }
    }
    else
    {   it++;
        while(it != indexMap.begin())
        {
            it--;

            size_t currX;
            size_t currY;
            indexToCoords(it->first, currX, currY);
            if(!func(it->second, currX, currY))
                return;
        }
    }
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::set(ElementIndexMap& indexMap, size_t x, size_t y, const ElementType& element)
{
    indexMap[coordsToIndex(x, y)] = element;
}

///////////////////////////////////

template<typename ElementType>
size_t TileBufferCache<ElementType>::coordsToIndex(size_t x, size_t y) const
{
    return x + y * M_WIDTH;
}

///////////////////////////////////

template<typename ElementType>
void TileBufferCache<ElementType>::indexToCoords(size_t index, size_t& x, size_t& y) const
{
    x = index % M_WIDTH;
    y = index / M_WIDTH;
}

///////////////////////////////////
}
