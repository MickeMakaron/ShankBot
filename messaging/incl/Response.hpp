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
#ifndef SB_MESSAGING_RESPONSE_HPP
#define SB_MESSAGING_RESPONSE_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Message.hpp"
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class Response : public Message
    {
        public:
            enum class Result : unsigned char
            {
                SUCCESS,
                FAIL,
            };

        public:
            explicit Response(Result result = Result::FAIL) : Message(Message::Type::RESPONSE), mResult(result){};

            void set(Result result);
            Result get() const;

        private:
            size_t getSizeDerived() const override;
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;

        private:
            Result mResult;
    };
}
}


#endif // SB_MESSAGING_RESPONSE_HPP
