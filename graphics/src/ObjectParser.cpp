///////////////////////////////////
// Internal ShankBot headers
#include "ObjectParser.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
///////////////////////////////////

///////////////////////////////////
// STD C

///////////////////////////////////

void ObjectParser::parse(const std::set<const TibiaDat::Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
    for(const TibiaDat::Object* o : objects)
    {
        switch(o->type)
        {
        case TibiaDat::Object::Type::ITEM:
            parseItem(*o, x, y, width, height);
            break;

        case TibiaDat::Object::Type::OUTFIT:
            parseOutfit(*o, x, y, width, height);
            break;

        case TibiaDat::Object::Type::EFFECT:
            parseEffect(*o, x, y, width, height);
            break;

        case TibiaDat::Object::Type::DISTANCE:
            parseDistance(*o, x, y, width, height);
            break;

        default:
            throw std::runtime_error("Failed to parse object. Invalid type.");
            break;
        }
    }
}

void ObjectParser::parseItem(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
    TibiaDat::ItemInfo item = *object.itemInfo;
    if(item.walkSpeed > 0 && !item.isBlocking)
    {
        auto it = std::find_if(mGroundTiles.begin(), mGroundTiles.end(), [&](const GroundTile& tile)
        {
            return tile.id == object.id;
        });

        if(it == mGroundTiles.end())
        {
            GroundTile tile;
            tile.object = &object;
            tile.id = object.id;
            tile.x = x;
            tile.y = y;
            mGroundTiles.push_back(tile);
        }

    }
    else if(item.isPickupable)
    {
        auto it = std::find_if(mItems.begin(), mItems.end(), [&](const Item& i)
        {
            return i.id == object.id;
        });

//        if(it == mItems.end())
        {
            Item i;
            i.object = &object;
            i.id = object.id;
            i.x = x;
            i.y = y;
            mItems.push_back(i);
        }
//        else
//        {
//            if(x >= it->x && y >= it->y)
//            {
//                it->x = x;
//                it->y = y;
//            }
//        }
    }

}

void ObjectParser::parseOutfit(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{

}

void ObjectParser::parseEffect(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{

}

void ObjectParser::parseDistance(const TibiaDat::Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{

}

void ObjectParser::clear()
{
    mGroundTiles.clear();
    mItems.clear();
}

const std::list<GroundTile>& ObjectParser::getGroundTiles() const
{
    return mGroundTiles;
}

const std::list<Item>& ObjectParser::getItems() const
{
    return mItems;
}
