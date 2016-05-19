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
//            struct Object
//            {
//                enum class Type : char
//                {
//                    ITEM,
//                    OUTFIT,
//                    EFFECT,
//                    DISTANCE
//                };
//
//                Type type;
//                size_t id;
//                std::unique_ptr<TibiaDat::SpritesInfo> spritesInfo;
//                std::unique_ptr<TibiaDat::ItemInfo> itemInfo;
//            };

        public:
            explicit SpriteObjectBindings(const TibiaDat& dat);
            explicit SpriteObjectBindings(const TibiaDat& dat, std::string binPath);

            std::list<const TibiaDat::Object*> getObjects(size_t spriteId) const;

            void writeToBinaryFile(std::string path) const;

        private:
            void readFromBinaryFile(std::string binPath);

            void parseObjects();
            void createBindings(const TibiaDat::Object& o, size_t globalId);

        private:
//            static std::vector<Object> mObjects;
//            static size_t mNumItems;
            const TibiaDat& mDat;
            std::map<size_t, std::set<size_t>> mBindings;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS
