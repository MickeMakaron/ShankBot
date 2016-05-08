///////////////////////////////////
// Internal ShankBot headers
#include "ObjectParser.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++

///////////////////////////////////

///////////////////////////////////
// STD C

///////////////////////////////////

void ObjectParser::parse(const std::set<const SpriteObjectBindings::Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
    for(const SpriteObjectBindings::Object* o : objects)
    {
        switch(o->type)
        {
            case SpriteObjectBindings::Object::Type::ITEM:
                parseItem(*o);
                break;

            case SpriteObjectBindings::Object::Type::OUTFIT:
                parseOutfit(*o);
                break;

            case SpriteObjectBindings::Object::Type::EFFECT:
                parseEffect(*o);
                break;

            case SpriteObjectBindings::Object::Type::DISTANCE:
                parseDistance(*o);
                break;

            default:
                throw std::runtime_error("Failed to parse object. Invalid type.");
                break;
        }
    }
}

void ObjectParser::parseItem(const SpriteObjectBindings::Object& object)
{

}

void ObjectParser::parseOutfit(const SpriteObjectBindings::Object& object)
{

}

void ObjectParser::parseEffect(const SpriteObjectBindings::Object& object)
{

}

void ObjectParser::parseDistance(const SpriteObjectBindings::Object& object)
{

}

void ObjectParser::clear()
{

}
