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
#ifndef SB_MESSAGING_NEW_EVENT_HPP
#define SB_MESSAGING_NEW_EVENT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
#include "messaging/NewMessage.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
///////////////////////////////////

namespace sb
{
namespace messaging
{
    #define SB_EVENT_ACCESSOR(name, index) SB_MESSAGE_ACCESSOR(name, index + 1)
    enum class NewEventType : unsigned char
    {
        HP_CHANGE,
        MANA_CHANGE,
        SOUL_CHANGE,
        CAP_CHANGE,
        PLAYER_CONDITION_CHANGE,

        UINT16_CHANGE,
        UINT32_CHANGE,
        FLOAT32_CHANGE,
        NUM_TYPES,
        INVALID,
    };

    template<NewEventType type, typename... Fields>
    struct EventBase : public MessageBase<MessageType::EVENT, NewEventType, Fields...>
    {
        EventBase(){eventType() = type;}
        SB_MESSAGE_ACCESSOR(eventType, 0);
    };

    template<NewEventType type>
    struct NewEvent;
    #define SB_EVENT(eventType, ...) template<> struct NewEvent<eventType> : public EventBase<eventType, __VA_ARGS__>

}
}


#endif // SB_MESSAGING_NEW_EVENT_HPP
