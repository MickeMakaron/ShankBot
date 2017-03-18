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
#ifndef SB_MESSAGING_RESPONSE_HPP
#define SB_MESSAGING_RESPONSE_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "messaging/config.hpp"
#include "Message.hpp"
#include "api/RequestResult.hpp"
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC Response : public Message
    {
        public:
            explicit Response(RequestResult result = RequestResult::FAIL, Message::Type type = Message::Type::INVALID) : Message(Message::Type::RESPONSE), mResponseType(type), mResult(result){};

            void set(RequestResult result);
            RequestResult getResult() const;

            Message::Type getResponseType() const;

            static Message::Type readResponseType(const char* data, size_t size);

        protected:
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;

        private:
            Message::Type mResponseType;
            RequestResult mResult;
    };
}
}


#endif // SB_MESSAGING_RESPONSE_HPP
