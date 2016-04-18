#ifndef GRAPHICS_LAYER_SPRITE_CACHE
#define GRAPHICS_LAYER_SPRITE_CACHE

///////////////////////////////////
// Internal ShankBot headers
#include "Sprite.hpp"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    class SpriteCache
    {
        public:
            bool getSprite(unsigned int textureId, size_t x, size_t y, Sprite& sprite) const;
            void setSprite(unsigned int textureId, size_t x, size_t y, Sprite sprite);
            void deleteSprite(unsigned int textureId, size_t x, size_t y);

        private:
            typedef unsigned int TextureId;
            typedef std::map<size_t, Sprite> SpriteIndexMap;
            typedef std::map<TextureId, SpriteIndexMap> SpriteMap;

            size_t coordsToIndex(size_t x, size_t y) const;

            SpriteMap::const_iterator getIndexMap(unsigned int textureId) const;
            SpriteMap::iterator getIndexMap(unsigned int textureId);

            SpriteIndexMap::const_iterator getSprite(const SpriteIndexMap& indexMap, size_t x, size_t y) const;

            void setSprite(SpriteIndexMap& indexMap, size_t x, size_t y, Sprite sprite);

        private:
            SpriteMap mSprites;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_CACHE
