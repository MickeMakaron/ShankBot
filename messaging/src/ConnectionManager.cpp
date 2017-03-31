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
///////////////////////////////////
// Internal ShankBot headers
#include "messaging/ConnectionManager.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
using namespace sb::messaging;
///////////////////////////////////

///////////////////////////////////
// Windows
#include <windows.h>
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////

///////////////////////////////////
// STD C
///////////////////////////////////


//ConnectionManager::ConnectionManager()
////: mEventCallbacks((int)Event::NUM_EVENTS, [](Connection connection){})
//{
//}

ConnectionManager::ConnectionId ConnectionManager::poll(Event& event, size_t timeOut)
{
    DWORD result = WaitForMultipleObjects(mEvents.size(), mEvents.data(), FALSE, timeOut);

    if(result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + mEvents.size())
    {
        size_t i = result - WAIT_OBJECT_0;
        assert(i < mEvents.size());
        assert(i < mConnections.size());

        Connection& connection = *mConnections[i];
        typedef Connection::State S;
        S previousState = connection.getState();
        connection.update();

        S currentState = connection.getState();

        if(currentState == S::DROPPED)
        {
            event = Event::DROP;
            return i;
        }

        switch(previousState)
        {
            case S::READING: event = Event::READ; break;
            case S::WRITING: event = Event::WRITE; break;
            case S::CONNECTING: event = Event::CONNECT; break;
            case S::DROPPED: event = Event::DROP; break;

            default:
                SB_THROW(stringify("Unexpected state transition in message connection. From ", (int)previousState, " to ", (int)currentState));
        }
        return i;
    }

    if(result == WAIT_TIMEOUT)
    {
        event = Event::TIME_OUT;
        return -1;
    }


    DWORD error = GetLastError();
    switch(error)
    {
        case ERROR_INVALID_PARAMETER: // This happens if mEvents.size() == 0
            event = Event::INVALID_STATE;
            return -1;

        default:
            SB_THROW(stringify("Unexpected result when waiting for message connection events: ", result, ". Error: ", error));
    }
}

void ConnectionManager::addConnection(bool connectToServer)
{
    mConnections.emplace_back(new Connection(connectToServer));
    mEvents.push_back(mConnections.back()->getEvent());

//    if(mConnections.back()->isConnected())
//        mEventCallbacks[(int)Event::CONNECT](mConnections.size() - 1);
}

bool ConnectionManager::removeConnection(ConnectionId connection)
{
    if(connection >= mConnections.size())
        return false;

    mConnections.erase(mConnections.begin() + connection);
    mEvents.erase(mEvents.begin() + connection);

    return true;
}

//void ConnectionManager::registerEventCallback(Event event, const std::function<void(Connection connection)>& callback)
//{
//    if(event >= Event::NUM_EVENTS)
//        SB_THROW(stringify("Invalid event: ", (int)event));
//    mEventCallbacks[(int)event] = callback;
//}

Connection& ConnectionManager::getConnection(ConnectionId connection) const
{
    if(connection >= mConnections.size())
        SB_THROW("Connection id out of range.");

    return *mConnections[connection];
}

