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
#include "messaging/messageHelpers.hpp"
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
    template<typename... Fields>
    struct MessageBase
    {
        std::tuple<Fields...> data;

        #define SB_ACCESSOR(name, index) auto& name() {return std::get<index>(this->data);}
        #define SB_MESSAGE_ACCESSOR(name, index) SB_ACCESSOR(name, index + 1)

        void toBinary(std::vector<char>& stream) const
        {
            std::tuple<Fields...>* d = (std::tuple<Fields...>*)&data;
            forEach(*d, [&stream](auto& e){::sb::messaging::toBinary(e, stream); return true;});
        }

        size_t fromBinary(const char* buffer, size_t size)
        {
            size_t startSize = size;
            if(forEach(data, [&buffer, &size](auto& e)
            {
                return ::sb::messaging::fromBinary(e, buffer, size);
            }))
            {
                return startSize - size;
            }

            return -1;
        }

        template<typename... OtherFields>
        bool operator==(const MessageBase<OtherFields...>& other) const
        {
            return data == other.data;
        }


    };

    template<typename FunctionT, typename... Elements>
    bool forEach(MessageBase<Elements...>& e, const FunctionT& function)
    {
        return forEach(e.data, function);
    }

    template<typename T, typename... Fields>
    void toBinary(std::vector<T>& t, std::vector<char>& stream, MessageBase<Fields...>*)
    {
        unsigned short size = t.size();
        stream.insert(stream.end(), (char*)&size, ((char*)&size) + sizeof(size));
        for(T& m : t)
        {
            m.toBinary(stream);
        }
    }

    template<typename T, typename... Fields>
    bool fromBinary(std::vector<T>& t, const char*& buffer, size_t& bSize, MessageBase<Fields...>*)
    {
        unsigned short size;
        if(bSize < sizeof(size))
        {
            return false;
        }
        size = *buffer;
        buffer += sizeof(size);
        bSize -= sizeof(size);
        t.resize(size);
        for(T& m : t)
        {
            size_t bytesRead = m.fromBinary(buffer, bSize);
            if(bytesRead == -1)
            {
                return false;
            }

            buffer += bytesRead;
            bSize -= bytesRead;
        }
        return true;
    }



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
    template<typename... Fields>
    struct NewMessage : public MessageBase<MessageType, Fields...>
    {
        SB_ACCESSOR(messageType, 0);
    };
}
}


#endif // SB_MESSAGING_NEW_MESSAGE_HPP
