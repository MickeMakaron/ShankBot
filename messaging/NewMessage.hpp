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
#ifndef SB_MESSAGING_NEW_MESSAGE_HPP
#define SB_MESSAGING_NEW_MESSAGE_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
#include "messaging/Serializable.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <tuple>
#include <vector>
///////////////////////////////////

namespace sb
{
namespace messaging
{
    #define SB_MESSAGE_ACCESSOR(name, index) SB_ACCESSOR(name, index + 1)

    enum class MessageType : unsigned char
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
        EVENT,
        INVALID,
    };
    template<MessageType type, typename... Fields>
    struct MessageBase : public Serializable<MessageType, Fields...>
    {
        MessageBase(){messageType() = type;}
        SB_ACCESSOR(messageType, 0);
    };

    template<MessageType type>
    struct NewMessage;
    #define SB_MESSAGE(messageType, ...) template<> struct NewMessage<messageType> : public MessageBase<messageType, __VA_ARGS__>

}
}


#endif // SB_MESSAGING_NEW_MESSAGE_HPP
