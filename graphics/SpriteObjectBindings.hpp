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
#ifndef GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS_HPP
#define GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/AppearancesReader.hpp"
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
            explicit SpriteObjectBindings(const std::vector<sb::tibiaassets::Object>& objects);
            explicit SpriteObjectBindings(const std::vector<sb::tibiaassets::Object>& objects, std::string binPath);

            std::list<const sb::tibiaassets::Object*> getObjects(size_t spriteId) const;

            void writeToBinaryFile(std::string path) const;

        private:
            void readFromBinaryFile(std::string binPath);

            void parseObjects();
            void createBindings(const sb::tibiaassets::Object& o, size_t globalId);

        private:
            const std::vector<sb::tibiaassets::Object>& mObjects;
            std::map<size_t, std::set<size_t>> mBindings;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_OBJECT_BINDINGS_HPP
