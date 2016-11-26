// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
///////////////////////////////////
// Internal ShankBot headers
#include "ObjectParser.hpp"
#include "exception.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
///////////////////////////////////

void ObjectParser::parse(const std::set<const Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
    for(const Object* o : objects)
    {
        switch(o->type)
        {
        case Object::Type::ITEM:
            parseItem(*o, x, y, width, height);
            break;

        case Object::Type::OUTFIT:
            parseOutfit(*o, x, y, width, height);
            break;

        case Object::Type::EFFECT:
            parseEffect(*o, x, y, width, height);
            break;

        case Object::Type::PROJECTILE:
            parseProjectile(*o, x, y, width, height);
            break;

        default:
            THROW_RUNTIME_ERROR("Failed to parse object. Invalid type.");
            break;
        }
    }
}

void ObjectParser::parseItem(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
    AppearancesReader::ItemInfo item = object.itemInfo;
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

void ObjectParser::parseOutfit(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{

}

void ObjectParser::parseEffect(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{

}

void ObjectParser::parseProjectile(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{

}

void ObjectParser::clear()
{
    mGroundTiles.clear();
    mItems.clear();
}

const std::list<ObjectParser::GroundTile>& ObjectParser::getGroundTiles() const
{
    return mGroundTiles;
}

const std::list<ObjectParser::Item>& ObjectParser::getItems() const
{
    return mItems;
}
