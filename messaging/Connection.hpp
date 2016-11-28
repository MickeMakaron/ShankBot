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
#ifndef SB_MESSAGING_CONNECTION_HPP
#define SB_MESSAGING_CONNECTION_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Message.hpp"
#include "messaging/config.hpp"
///////////////////////////////////


///////////////////////////////////
// Windows
struct _OVERLAPPED;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <list>
#include <vector>
#include <cstring>
#include <memory>
///////////////////////////////////

namespace sb
{
namespace messaging
{
class SHANK_BOT_MESSAGING_DECLSPEC Connection
{
    public:
        enum class State : unsigned char
        {
            WRITING,
            READING,
            CONNECTING,
            DROPPED,
            IDLE,
        };

    public:
        Connection(bool connectToServer = true);
        ~Connection();

        void read();
        std::list<std::vector<char>> getMessages();
        bool sendMessages(const std::list<std::shared_ptr<Message>>& messages);
        bool sendMessage(const Message& message);

        bool isConnected() const;

        void* getEvent() const;
        State getState() const;

        void update();

        void drop();


    private:
        void clientConnect();
        void serverConnect();
        void handleRead(size_t numBytesRead);
        void handleWrite(size_t numBytesWritten);
        bool sendBufferContents(size_t size);

        void* createEvent() const;
        _OVERLAPPED* createOverlapped() const;


    protected:
        static constexpr const char* const M_PIPE_NAME = "\\\\.\\pipe\\ShankBotMessagePipe";
        static const size_t M_IN_BUFFER_SIZE = 1 << 24;
        static const size_t M_OUT_BUFFER_SIZE = 1 << 24;

    private:
        void* mEvent;
        _OVERLAPPED* mOverlapped;
        void* mPipe;
        State mState;
        char mBuffer[M_IN_BUFFER_SIZE];

        size_t mLatestWriteSize;

        std::list<std::vector<char>> mMessages;
};
}
}


#endif // SB_MESSAGING_CONNECTION_HPP
