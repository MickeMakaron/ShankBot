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
#ifndef SB_MESSAGING_EVENT_HPP
#define SB_MESSAGING_EVENT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
///////////////////////////////////

namespace sb
{
namespace messaging
{
    enum class EventType : unsigned char
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

    template<EventType typeT>
    struct EventData
    {

    };

    template<EventType typeT>
    struct Event
    {
        using Data = EventData<typeT>;

        explicit Event(){}
        explicit Event(const Data& data) : data(data){}



        size_t fromBinary(const char* buffer, size_t size)
        {
            return fromBinary(*this, buffer, size);
        }

        static constexpr getSize()
        {
            return sizeof(type) + sizeof(data);
        }

        static size_t fromBinary(Event<typeT>& e, const char* buffer, size_t size)
        {
            if(size < e.getSize())
            {
                return -1;
            }

            memcpy(&e.type, buffer, sizeof(e.type));
            memcpy(&e.data, buffer + sizeof(e.type), sizeof(e.data));

            return e.getSize();
        }

        void toBinary(std::vector<char>& stream) const
        {
            toBinary(data, stream);
        }
        static void toBinary(const Data& d, std::vector<char>& stream)
        {
            size_t insertIndex = stream.size();
            stream.resize(insertIndex + sizeof(type) + sizeof(d));
            memcpy(&stream[insertIndex], &type, sizeof(type));
            insertIndex += sizeof(type);
            memcpy(&stream[insertIndex], &d, sizeof(d));
        }

        static constexpr EventType type = typeT;
        Data data;
        static_assert(std::is_pod<Data>::value, "Event data must be POD.");
    };
    EventType readEventType(const char* buffer, size_t size)
    {
        if(size < sizeof(EventType))
        {
            return EventType::INVALID;
        }
        return *((EventType*)buffer);
    }
}
}


#endif // SB_MESSAGING_EVENT_HPP
