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
#ifndef SB_MESSAGING_EVENT_QUEUE_HPP
#define SB_MESSAGING_EVENT_QUEUE_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
#include "messaging/Message.hpp"
#include "messaging/Event.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC EventQueue : public Message
    {
        public:
            explicit EventQueue();

            template<EventType type>
            void push(const Event<type>& e)
            {
                e.toBinary(mBuffer);
            }

            template<EventType type>
            void push(const EventData<type>& data)
            {
                Event<type>::toBinary(data, mBuffer);
            }

            EventType readType();

            template<EventType typeT>
            EventData<typeT>& read()
            {
                EventData<typeT>* data = (EventData<typeT>*)(mBuffer.data() + mIndex);
                mIndex = std::min(mBuffer.size(), mIndex + sizeof(EventData<typeT>));
                return *data;
            }


        private:
            virtual size_t fromBinaryDerived(const char* data, size_t size);
            virtual void toBinaryDerived(std::vector<char>& out) const;


        private:
            std::vector<char> mBuffer;
            size_t mIndex = 0;


            using SIZE_TYPE = unsigned short;
    };
}
}


#endif // SB_MESSAGING_EVENT_QUEUE_HPP
