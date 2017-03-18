// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
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
#ifndef SB_API_FRAME_HPP
#define SB_API_FRAME_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/Object.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace sb
{
    struct Outfit
    {
        std::string name;
        unsigned short x;
        unsigned short y;
    };

    struct Player : public Outfit
    {
        float hp;
    };

    struct Npc : public Outfit
    {
        unsigned short id;
    };

    struct Creature : public Outfit
    {
        float hp;
        unsigned short id;
    };

    typedef unsigned int GlobalObjectId;

    struct Scene
    {
        static const size_t WIDTH = 15;
        static const size_t HEIGHT = 11;
        static const size_t SIZE = WIDTH * HEIGHT;
        std::vector<GlobalObjectId> objects[WIDTH][HEIGHT];
        std::vector<Player> players;
        std::vector<Npc> npcs;
        std::vector<Creature> creatures;
    };

    struct Gui
    {

    };

    struct MiniMap
    {
        unsigned int x;
        unsigned int y;
        unsigned char level;
        sb::tibiaassets::Object::MiniMapColor colors[Scene::WIDTH][Scene::HEIGHT];
    };

    struct Frame
    {
        Scene scene;
        Gui gui;
        MiniMap miniMap;
    };
}


#endif // SB_API_FRAME_HPP
