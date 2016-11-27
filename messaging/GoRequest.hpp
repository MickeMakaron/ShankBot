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
#ifndef SB_MESSAGING_GO_REQUEST_HPP
#define SB_MESSAGING_GO_REQUEST_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Message.hpp"
#include "messaging/config.hpp"
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC GoRequest : public Message
    {
        public:
            explicit GoRequest() : Message(Message::Type::GO_REQUEST){};

            void set(short x, short y);
            short getX() const;
            short getY() const;

        private:
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;
            size_t getSizeDerived() const override;

        private:
            short mX;
            short mY;
    };
}
}


#endif // SB_MESSAGING_GO_REQUEST_HPP
