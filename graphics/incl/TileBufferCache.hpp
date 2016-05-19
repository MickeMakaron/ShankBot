#ifndef GRAPHICS_LAYER_TILE_BUFFER_CACHE
#define GRAPHICS_LAYER_TILE_BUFFER_CACHE

///////////////////////////////////
// Internal ShankBot headers
#include "SpriteObjectBindings.hpp"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    template<typename ElementType>
    class TileBufferCache
    {
        public:
            bool get(unsigned int textureId, size_t x, size_t y, ElementType& element) const;
            void set(unsigned int textureId, size_t x, size_t y, const ElementType& element);
            void remove(unsigned int textureId, size_t x, size_t y);

        private:
            typedef unsigned int TextureId;
            typedef std::map<size_t, ElementType> ElementIndexMap;
            typedef std::map<TextureId, ElementIndexMap> ElementMap;

            size_t coordsToIndex(size_t x, size_t y) const;

            typename ElementMap::const_iterator getIndexMap(unsigned int textureId) const;
            typename ElementMap::iterator getIndexMap(unsigned int textureId);

            typename ElementIndexMap::const_iterator get(const ElementIndexMap& indexMap, size_t x, size_t y) const;

            void set(ElementIndexMap& indexMap, size_t x, size_t y, const ElementType& element);

        private:
            ElementMap mElements;
    };
}

#include "TileBufferCache.inl"

#endif // GRAPHICS_LAYER_TILE_BUFFER_CACHE
