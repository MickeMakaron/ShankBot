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
#ifndef SB_MESSAGING_FRAME_RESPONSE_HPP
#define SB_MESSAGING_FRAME_RESPONSE_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "messaging/Response.hpp"
#include "tibiaassets/Object.hpp"
#include "messaging/config.hpp"
#include "api/Frame.hpp"
///////////////////////////////////

namespace sb
{
namespace messaging
{
    class SHANK_BOT_MESSAGING_DECLSPEC FrameResponse : public Response
    {
        private:
            typedef unsigned char SIZE_TYPE;

        public:
            explicit FrameResponse(RequestResult result = RequestResult::FAIL) : Response(result, Message::Type::FRAME_RESPONSE){};

            void set(const Frame& frame);
            const Frame& get() const;

        private:
            size_t fromBinaryDerived(const char* data, size_t size) override;
            void toBinaryDerived(std::vector<char>& out) const override;
            size_t getSizeDerived() const override;


        private:
            Frame mFrame;
    };
}
}


#endif // SB_MESSAGING_FRAME_RESPONSE_HPP
