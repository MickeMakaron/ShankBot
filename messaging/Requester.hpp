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
#ifndef SB_MESSAGING_REQUESTER_HPP
#define SB_MESSAGING_REQUESTER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "messaging/ConnectionManager.hpp"
#include "messaging/Connection.hpp"
#include "messaging/config.hpp"
#include "api/RequestResult.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
///////////////////////////////////

namespace sb
{
namespace messaging
{

class SHANK_BOT_MESSAGING_DECLSPEC Requester
{
    public:
        Requester();
        std::shared_ptr<std::vector<char>> request(RequestResult& result, const std::unique_ptr<Message>& message);

    private:
        ConnectionManager mManager;
        Connection* mConnection = nullptr;

        static const size_t M_WAIT_TIME_MS = 10000;
};
}
}

#endif // SB_MESSAGING_REQUESTER_HPP
