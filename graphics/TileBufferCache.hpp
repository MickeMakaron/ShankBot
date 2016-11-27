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
#ifndef GRAPHICS_LAYER_TILE_BUFFER_CACHE_HPP
#define GRAPHICS_LAYER_TILE_BUFFER_CACHE_HPP

///////////////////////////////////
// STD C++
#include <map>
#include <list>
#include <functional>
///////////////////////////////////

namespace GraphicsLayer
{
    template<typename ElementType>
    class TileBufferCache
    {
        public:
            typedef unsigned int TextureId;
            typedef std::map<size_t, ElementType> ElementIndexMap;
            typedef std::map<TextureId, ElementIndexMap> ElementMap;

        public:
            struct AreaElement
            {
                size_t x;
                size_t y;
                ElementType element;
            };

            explicit TileBufferCache(size_t width);
            bool get(unsigned int textureId, size_t x, size_t y, ElementType& element) const;
            bool getClosestTopLeft(unsigned int textureId, size_t x, size_t y, AreaElement& element) const;
            bool getClosestBotRight(unsigned int textureId, size_t x, size_t y, AreaElement& element) const;
            void forEach(unsigned int textureId, const std::function<bool(const ElementType&, const size_t&, const size_t&)>& func, size_t x = 0, size_t y = 0, bool traverseForward = true) const;

            std::list<AreaElement> getByArea(unsigned int textureId, size_t x, size_t y, size_t width, size_t height) const;
            void removeByArea(unsigned int textureId, size_t x, size_t y, size_t width, size_t height);

            void set(unsigned int textureId, size_t x, size_t y, const ElementType& element);
            void remove(unsigned int textureId, size_t x, size_t y);
            void clear(unsigned int textureId);

        private:

            size_t coordsToIndex(size_t x, size_t y) const;
            void indexToCoords(size_t index, size_t& x, size_t& y) const;

            typename ElementIndexMap::const_iterator get(const ElementIndexMap& indexMap, size_t x, size_t y) const;
            typename ElementIndexMap::const_iterator getClosestTopLeft(const ElementIndexMap& indexMap, size_t x, size_t y) const;

            void set(ElementIndexMap& indexMap, size_t x, size_t y, const ElementType& element);

        private:
            ElementMap mElements;
            const size_t M_WIDTH;
    };
}

#include "TileBufferCache.inl"

#endif // GRAPHICS_LAYER_TILE_BUFFER_CACHE_HPP
