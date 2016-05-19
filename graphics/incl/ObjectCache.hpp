#ifndef GRAPHICS_LAYER_OBJECT_CACHE
#define GRAPHICS_LAYER_OBJECT_CACHE

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
    class ObjectCache
    {
        private:
            typedef const TibiaDat::Object* Object;

        public:
            bool get(unsigned int textureId, size_t x, size_t y, std::set<Object>& sprite) const;
            void set(unsigned int textureId, size_t x, size_t y, std::set<Object> sprite);
            void remove(unsigned int textureId, size_t x, size_t y);

        private:
            typedef unsigned int TextureId;
            typedef std::map<size_t, std::set<Object>> SpriteIndexMap;
            typedef std::map<TextureId, SpriteIndexMap> SpriteMap;

            size_t coordsToIndex(size_t x, size_t y) const;

            SpriteMap::const_iterator getIndexMap(unsigned int textureId) const;
            SpriteMap::iterator getIndexMap(unsigned int textureId);

            SpriteIndexMap::const_iterator get(const SpriteIndexMap& indexMap, size_t x, size_t y) const;

            void set(SpriteIndexMap& indexMap, size_t x, size_t y, std::set<Object> sprite);

        private:
            SpriteMap mSprites;
    };
}


#endif // GRAPHICS_LAYER_OBJECT_CACHE
