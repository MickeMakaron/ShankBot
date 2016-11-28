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
#ifndef SB_MESSAGING_LOGIN_REQUEST_HPP
#define SB_MESSAGING_LOGIN_REQUEST_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Message.hpp"
#include "messaging/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC LoginRequest : public Message
    {
            typedef unsigned char SIZE_TYPE;
            static const SIZE_TYPE MAX_STRING_LENGTH = SIZE_TYPE(-1);

        public:
            explicit LoginRequest() : Message(Message::Type::LOGIN_REQUEST){};

            void set(const std::string& accountName, const std::string& password);
            const std::string& getAccountName() const;
            const std::string& getPassword() const;

        private:
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;

        private:
            std::string mAccountName;
            std::string mPassword;
    };
}
}


#endif // SB_MESSAGING_LOGIN_REQUEST_HPP
