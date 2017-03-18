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
#ifndef SB_MESSAGING_CONNECTION_MANAGER_HPP
#define SB_MESSAGING_CONNECTION_MANAGER_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "messaging/Connection.hpp"
#include "messaging/config.hpp"
///////////////////////////////////


///////////////////////////////////
// STD C++
///////////////////////////////////

namespace sb
{
namespace messaging
{
class SHANK_BOT_MESSAGING_DECLSPEC ConnectionManager
{
    public:
        enum class Event : unsigned char
        {
            DROP,
            WRITE,
            READ,
            CONNECT,
            TIME_OUT,
            INVALID_STATE,
        };
        typedef unsigned int ConnectionId;

    public:
//        ConnectionManager();

        void addConnection(bool connectToServer = true);
        bool removeConnection(ConnectionId connection);

        Connection& getConnection(ConnectionId connection) const;
        ConnectionId poll(Event& event, size_t timeOut);
//        void registerEventCallback(Event event, const std::function<void(Connection connection)>& callback);

    private:
        std::vector<std::unique_ptr<Connection>> mConnections;
        std::vector<void*> mEvents;
//        std::vector<std::function<void(Connection connection)>> mEventCallbacks;
};

}
}


#endif // SB_MESSAGING_CONNECTION_MANAGER_HPP
