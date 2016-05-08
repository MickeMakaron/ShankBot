#ifndef GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS
#define GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS


///////////////////////////////////
// Internal ShankBot headers
#include "TibiaDat.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
#include <list>
#include <vector>
#include <map>
#include <set>
///////////////////////////////////

namespace GraphicsLayer
{
    class SpriteObjectBindings
    {
        public:
            struct Object
            {
                enum class Type : char
                {
                    ITEM,
                    OUTFIT,
                    EFFECT,
                    DISTANCE
                };

                Type type;
                size_t id;
                std::unique_ptr<TibiaDat::SpritesInfo> spritesInfo;
                std::unique_ptr<TibiaDat::ItemInfo> itemInfo;
            };

        public:
            static void initialize(const TibiaDat& dat);
            static std::list<const SpriteObjectBindings::Object*> getObjects(size_t spriteId);

            static void initialize(std::string binPath);
            static void writeToBinaryFile(std::string path);

        private:
            static void readFromBinaryFile(std::string binPath);

            static void parseObjects(const TibiaDat& dat);
            static void createBindings(const TibiaDat::Object& o, size_t globalId);

        private:
            static std::vector<Object> mObjects;
            static size_t mNumItems;
            static std::map<size_t, std::set<size_t>> mBindings;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS
