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
#ifndef SB_MESSAGING_MESSAGE_HPP
#define SB_MESSAGING_MESSAGE_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <cstring>
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC Message
    {
        public:
            enum class Type : unsigned char
            {
                LOGIN_REQUEST,
                LOGIN_RESPONSE,
                RESPONSE,
                GO_REQUEST,
                FRAME_RESPONSE,
                FRAME_REQUEST,
                ATTACK_REQUEST,
                OBJECT_RESPONSE,
                OBJECT_REQUEST,
                INVALID,
            };

        public:
            explicit Message(Type messageType) : M_MESSAGE_TYPE(messageType){};

            size_t fromBinary(const char* data, size_t size);
            void toBinary(std::vector<char>& out) const;
            static Type readMessageType(const char* data, size_t size);


        private:
            virtual size_t fromBinaryDerived(const char* data, size_t size);
            virtual void toBinaryDerived(std::vector<char>& out) const;

        private:
            const Type M_MESSAGE_TYPE;
    };
}
}


#endif // SB_MESSAGING_MESSAGE_HPP
