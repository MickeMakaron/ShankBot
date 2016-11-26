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
#ifndef SB_MESSAGING_FRAME_RESPONSE_HPP
#define SB_MESSAGING_FRAME_RESPONSE_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Message.hpp"
#include "Object.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class FrameResponse : public Message
    {
        public:
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

            struct Object
            {
                GraphicsLayer::Object::Type type;
                unsigned short id;
            };

            struct Scene
            {
                static const size_t WIDTH = 15;
                static const size_t HEIGHT = 11;
                static const size_t SIZE = WIDTH * HEIGHT;
                std::vector<Object> objects[WIDTH][HEIGHT];
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
                GraphicsLayer::Object::MiniMapColor colors[Scene::WIDTH][Scene::HEIGHT];
            };

            struct Frame
            {
                Scene scene;
                Gui gui;
                MiniMap miniMap;
            };
        private:
            typedef unsigned char SIZE_TYPE;

        public:
            explicit FrameResponse() : Message(Message::Type::FRAME_RESPONSE){};

            void set(const Frame& frame);
            const Frame& get() const;

        private:
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;
            size_t getSizeDerived() const override;


        private:
            Frame mFrame;
    };
}
}


#endif // SB_MESSAGING_FRAME_RESPONSE_HPP
