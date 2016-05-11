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
    struct Object
    {
        const SpriteObjectBindings::Object* object;
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

    class ObjectParser
    {
        public:
            void clear();
            void parse(const std::set<const SpriteObjectBindings::Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

            const std::list<GroundTile>& getGroundTiles() const;
            const std::list<Item>& getItems() const;

        private:
            void parseItem(const SpriteObjectBindings::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseOutfit(const SpriteObjectBindings::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseEffect(const SpriteObjectBindings::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseDistance(const SpriteObjectBindings::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

        private:
            std::list<GroundTile> mGroundTiles;
            std::list<Item> mItems;
    };
}


#endif // GRAPHICS_LAYER_OBJECT_PARSER
