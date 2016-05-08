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
            void clear();
            void parse(const std::set<const SpriteObjectBindings::Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

        private:
            void parseItem(const SpriteObjectBindings::Object& object);
            void parseOutfit(const SpriteObjectBindings::Object& object);
            void parseEffect(const SpriteObjectBindings::Object& object);
            void parseDistance(const SpriteObjectBindings::Object& object);

        private:
    };
}


#endif // GRAPHICS_LAYER_OBJECT_PARSER
