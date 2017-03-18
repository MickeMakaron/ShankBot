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
#ifndef SB_MESSAGING_OBJECT_RESPONSE_HPP
#define SB_MESSAGING_OBJECT_RESPONSE_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "messaging/Response.hpp"
#include "messaging/config.hpp"
#include "api/Object.hpp"
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC ObjectResponse : public Response
    {
        private:
            typedef unsigned char SMALL_SIZE_TYPE;
            typedef unsigned int LARGE_SIZE_TYPE;

        public:
            explicit ObjectResponse(RequestResult result = RequestResult::FAIL) : Response(result, Message::Type::OBJECT_RESPONSE){};

            void set(const std::vector<sb::tibiaassets::Object>& objects);
            const std::vector<sb::Object>& get() const;

        private:
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;

            static void objectToBinary(const sb::Object& obj, std::vector<char>& out);
            static size_t objectFromBinary(sb::Object& obj, const char* data, size_t size);


        private:
            std::vector<Object> mObjects;
    };
}
}


#endif // SB_MESSAGING_OBJECT_RESPONSE_HPP
