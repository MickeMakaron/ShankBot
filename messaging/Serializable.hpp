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
#ifndef SB_MESSAGING_SERIALIZABLE_HPP
#define SB_MESSAGING_SERIALIZABLE_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
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
    template<size_t i, typename FunctionT, typename... Elements>
    typename std::enable_if<i >= sizeof...(Elements), bool>::type
    forEachHelper(std::tuple<Elements...>& elements, const FunctionT& function)
    {
        return true;
    }

    template<size_t i, typename FunctionT, typename... Elements>
    typename std::enable_if<i < sizeof...(Elements), bool>::type
    forEachHelper(std::tuple<Elements...>& elements, const FunctionT& function)
    {
        if(!forEach(std::get<i>(elements), function))
        {
            return false;
        }
        return forEachHelper<i + 1>(elements, function);
    }

    template<typename FunctionT, typename... Elements>
    bool forEach(std::tuple<Elements...>& elements, const FunctionT& function)
    {
        return forEachHelper<0>(elements, function);
    }


    template<typename FunctionT, typename Element>
    bool forEach(Element& t, const FunctionT& function)
    {
        return function(t);
    }

    template<typename T>
    void toBinary(const T& t, std::vector<char>& stream)
    {
        stream.insert(stream.end(), (char*)&t, ((char*)&t) + sizeof(t));
    }

    template<typename T>
    void toBinary(std::vector<T>& t, std::vector<char>& stream, void*)
    {
        unsigned short size = t.size();
        stream.insert(stream.end(), (char*)&size, ((char*)&size) + sizeof(size));
        stream.insert(stream.end(), (char*)t.data(), (char*)(t.data() + size));
        std::cout << "rep count: " << size << std::endl;
        for(size_t i = 0; i < size; i++)
        {
            std::cout << "\t" << t[i] << std::endl;
        }
    }

    template<typename T>
    void toBinary(std::vector<std::vector<T>>& t, std::vector<char>& stream, std::vector<T>*)
    {
        unsigned short size = t.size();
        stream.insert(stream.end(), (char*)&size, ((char*)&size) + sizeof(size));
        for(std::vector<T>& m : t)
        {
            toBinary(m, stream, m.data());
        }
    }

    template<typename T>
    void toBinary(std::vector<T>& t, std::vector<char>& stream)
    {
        toBinary(t, stream, t.data());
    }


    template<typename T>
    bool fromBinary(T& t, const char*& buffer, size_t& size)
    {
        if(size < sizeof(t))
        {
            return false;
        }
        memcpy((char*)&t, buffer, sizeof(t));
        buffer += sizeof(t);
        size -= sizeof(t);

        return true;
    }



    template<typename T>
    bool fromBinary(std::vector<T>& t, const char*& buffer, size_t& bSize, void*)
    {
        unsigned short size;
        if(bSize < sizeof(size))
        {
            return false;
        }
        memcpy(&size, buffer, sizeof(size));
        buffer += sizeof(size);
        bSize -= sizeof(size);
        if(bSize < sizeof(T) * size)
        {
            return false;
        }
        t.resize(size);
        memcpy(t.data(), buffer, sizeof(T) * size);
        buffer += sizeof(T) * size;
        bSize -= sizeof(T) * size;

        return true;
    }


    template<typename T>
    bool fromBinary(std::vector<std::vector<T>>& t, const char*& buffer, size_t& bSize, std::vector<T>*)
    {
        unsigned short size;
        if(bSize < sizeof(size))
        {
            return false;
        }
        memcpy(&size, buffer, sizeof(size));
        buffer += sizeof(size);
        bSize -= sizeof(size);
        t.resize(size);
        for(std::vector<T>& m : t)
        {
            if(!fromBinary(m, buffer, bSize, m.data()))
            {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    bool fromBinary(std::vector<T>& t, const char*& buffer, size_t& bSize)
    {
        return fromBinary(t, buffer, bSize, t.data());
    }


    template<typename... Fields>
    struct Serializable
    {
        std::tuple<Fields...> data;

        #define SB_ACCESSOR(name, index) auto& name() {return std::get<index>(this->data);}

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
        bool operator==(const Serializable<OtherFields...>& other) const
        {
            return data == other.data;
        }
    };

    template<typename FunctionT, typename... Elements>
    bool forEach(Serializable<Elements...>& e, const FunctionT& function)
    {
        return forEach(e.data, function);
    }

    template<typename T, typename... Fields>
    void toBinary(std::vector<T>& t, std::vector<char>& stream, Serializable<Fields...>*)
    {
        unsigned short size = t.size();
        stream.insert(stream.end(), (char*)&size, ((char*)&size) + sizeof(size));
        for(T& m : t)
        {
            m.toBinary(stream);
        }
    }

    template<typename T, typename... Fields>
    bool fromBinary(std::vector<T>& t, const char*& buffer, size_t& bSize, Serializable<Fields...>*)
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
            if(bytesRead == (size_t)-1)
            {
                return false;
            }

            buffer += bytesRead;
            bSize -= bytesRead;
        }
        return true;
    }
}
}


#endif // SB_MESSAGING_SERIALIZABLE_HPP
