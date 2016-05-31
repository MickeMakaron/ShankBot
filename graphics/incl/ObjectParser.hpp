#ifndef GRAPHICS_LAYER_OBJECT_PARSER
#define GRAPHICS_LAYER_OBJECT_PARSER


///////////////////////////////////
// Internal ShankBot headers
#include "SpriteObjectBindings.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <set>
///////////////////////////////////

namespace GraphicsLayer
{


    class ObjectParser
    {
        public:

            struct Object
            {
                const TibiaDat::Object* object;
                unsigned int id;
                unsigned short x;
                unsigned short y;
            };

            struct GroundTile : public Object
            {

            };

            struct Item : public Object
            {

            };

        public:
            void clear();
            void parse(const std::set<const TibiaDat::Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

            const std::list<GroundTile>& getGroundTiles() const;
            const std::list<Item>& getItems() const;

        private:
            void parseItem(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseOutfit(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseEffect(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseDistance(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

        private:
            std::list<GroundTile> mGroundTiles;
            std::list<Item> mItems;
    };
}


#endif // GRAPHICS_LAYER_OBJECT_PARSER
