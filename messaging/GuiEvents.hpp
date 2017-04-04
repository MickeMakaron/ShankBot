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
#ifndef SB_MESSAGING_GUI_EVENT_HPP
#define SB_MESSAGING_GUI_EVENT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
#include "messaging/NewEvent.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
///////////////////////////////////

namespace sb
{
namespace messaging
{
    namespace layout
    {
        SB_EVENT_LAYOUT(Float32Change, float, float)
        {
            SB_EVENT_ACCESSOR(oldVal, 0);
            SB_EVENT_ACCESSOR(newVal, 1);
        };

        SB_EVENT_LAYOUT(Uint16Change, uint16_t, uint16_t)
        {
            SB_EVENT_ACCESSOR(oldVal, 0);
            SB_EVENT_ACCESSOR(newVal, 1);
        };

        SB_EVENT_LAYOUT(Uint32Change, uint32_t, uint32_t)
        {
            SB_EVENT_ACCESSOR(oldVal, 0);
            SB_EVENT_ACCESSOR(newVal, 1);
        };

        SB_EVENT_LAYOUT(Uint16PercentChange, float, float, uint16_t, uint16_t)
        {
            SB_EVENT_ACCESSOR(oldPercent, 0);
            SB_EVENT_ACCESSOR(newPercent, 1);
            SB_EVENT_ACCESSOR(oldVal, 2);
            SB_EVENT_ACCESSOR(newVal, 3);
        };
    }

    SB_EVENT_COPY_LAYOUT(layout::Uint16PercentChange, NewEventType::HP_CHANGE){};
    SB_EVENT_COPY_LAYOUT(layout::Uint16PercentChange, NewEventType::MANA_CHANGE){};
    SB_EVENT_COPY_LAYOUT(layout::Uint16Change, NewEventType::CAP_CHANGE){};
    SB_EVENT_COPY_LAYOUT(layout::Uint16Change, NewEventType::SOUL_CHANGE){};
    SB_EVENT_COPY_LAYOUT(layout::Uint32Change, NewEventType::PLAYER_CONDITION_CHANGE){};
//    template<> struct NewEvent<NewEventType::MANA_CHANGE> : public decltype(eventCopyHelper<NewEventType::MANA_CHANGE>(std::declval<NewEvent<NewEventType::HP_CHANGE>>()))
//    template<> struct EventData<EventType::MANA_CHANGE> : public EventData<EventType::HP_CHANGE>{};
//    template<> struct EventData<EventType::CAP_CHANGE> : public EventData<EventType::UINT16_CHANGE>{};
//    template<> struct EventData<EventType::SOUL_CHANGE> : public EventData<EventType::UINT16_CHANGE>{};
//    template<> struct EventData<EventType::PLAYER_CONDITION_CHANGE> : public EventData<EventType::UINT32_CHANGE>{};
}
}


#endif // SB_MESSAGING_GUI_EVENT_HPP
