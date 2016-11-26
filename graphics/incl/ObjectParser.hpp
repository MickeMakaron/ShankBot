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
#ifndef GRAPHICS_LAYER_OBJECT_PARSER_HPP
#define GRAPHICS_LAYER_OBJECT_PARSER_HPP


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
                const Object* object;
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
            void parse(const std::set<const Object*>& objects, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

            const std::list<GroundTile>& getGroundTiles() const;
            const std::list<Item>& getItems() const;

        private:
            void parseItem(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseOutfit(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseEffect(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            void parseProjectile(const Object& object, unsigned short x, unsigned short y, unsigned short width, unsigned short height);

        private:
            std::list<GroundTile> mGroundTiles;
            std::list<Item> mItems;
    };
}


#endif // GRAPHICS_LAYER_OBJECT_PARSER_HPP
