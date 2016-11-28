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
///////////////////////////////////
// Internal ShankBot headers
#include "messaging/Connection.hpp"
#include "utility/utility.hpp"
using namespace sb::messaging;
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// Windows
#include <windows.h>
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////



Connection::Connection(bool connectToServer)
: mEvent(createEvent())
, mOverlapped(createOverlapped())
{
    if(connectToServer)
        serverConnect();
    else
        clientConnect();
}

Connection::~Connection()
{
    drop();
    CloseHandle(mEvent);

    delete mOverlapped;
}

HANDLE Connection::getEvent() const
{
    return mEvent;
}

_OVERLAPPED* Connection::createOverlapped() const
{
    _OVERLAPPED* o = new OVERLAPPED({0});
    o->hEvent = mEvent;

    return o;
}

Connection::State Connection::getState() const
{
    return mState;
}

void Connection::drop()
{
    mState = State::DROPPED;
    CancelIo(mPipe);
    DisconnectNamedPipe(mPipe);
    CloseHandle(mPipe);
}

void* Connection::createEvent() const
{
    HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);

    if(event == INVALID_HANDLE_VALUE)
        THROW_RUNTIME_ERROR("Failed to create event for message connection.");

    return event;
}

void Connection::serverConnect()
{
    while(true)
    {
        WaitNamedPipe(M_PIPE_NAME, 500);
        mPipe = CreateFile
        (
            M_PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL
        );

        if(mPipe != INVALID_HANDLE_VALUE)
        {
//            mState = State::IDLE;
            mState = State::CONNECTING;
            SetEvent(mEvent);
            return;
        }
    }
}

void Connection::clientConnect()
{
    mPipe = CreateNamedPipe
    (
        M_PIPE_NAME,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        M_OUT_BUFFER_SIZE,
        M_IN_BUFFER_SIZE,
        0,
        NULL
    );

    if(mPipe == INVALID_HANDLE_VALUE)
        THROW_RUNTIME_ERROR("Failed to create pipe for message connection.");


    BOOL result = ConnectNamedPipe(mPipe, mOverlapped);

    if(result)
    {
        drop();
        return;
    }

    DWORD error = GetLastError();
    switch(error)
    {
        case ERROR_IO_PENDING:
            mState = State::CONNECTING;
            break;

        case ERROR_PIPE_CONNECTED:
//            mState = State::IDLE;
            mState = State::CONNECTING;
            SetEvent(mEvent);
            break;

        default:
            THROW_RUNTIME_ERROR(stringify("Failed to connect pipe. Error: ", error));
    }
}

bool Connection::isConnected() const
{
    return mState != State::DROPPED && mState != State::CONNECTING;
}


void Connection::read()
{
    if(mState != State::IDLE)
        return;

    BOOL success = ReadFile(mPipe, mBuffer, sizeof(mBuffer), NULL, mOverlapped);

    if(!success && GetLastError() != ERROR_IO_PENDING)
    {
        drop();
        return;
    }

    mState = State::READING;
}

std::list<std::vector<char>> Connection::getMessages()
{
    std::list<std::vector<char>> messages;
    messages.swap(mMessages);
    return messages;
}

bool Connection::sendMessages(const std::list<std::shared_ptr<Message>>& messages)
{
    if(mState != State::IDLE)
        return false;

    size_t numBytesWritten = 0;
    char* buffer = mBuffer;

    for(const auto& msg : messages)
    {
        std::vector<char> data;
        msg->toBinary(data);
        numBytesWritten += data.size();
        if(numBytesWritten >= M_OUT_BUFFER_SIZE)
            return false;

        memcpy(buffer, data.data(), data.size());
        buffer += data.size();
    }

    return sendBufferContents(numBytesWritten);
}

bool Connection::sendBufferContents(size_t size)
{
    assert(mState == State::IDLE);
    assert(size < M_OUT_BUFFER_SIZE);

    BOOL success = WriteFile(mPipe, mBuffer, size, NULL, mOverlapped);

    if(!success)
    {
        if(GetLastError() != ERROR_IO_PENDING)
            drop();

        return false;
    }

    mLatestWriteSize = size;
    mState = State::WRITING;
    return true;
}

bool Connection::sendMessage(const Message& message)
{
    if(mState != State::IDLE)
        return false;

    std::vector<char> data;
    message.toBinary(data);
    if(data.size() > M_OUT_BUFFER_SIZE)
        return false;

    memcpy(mBuffer, data.data(), data.size());

    return sendBufferContents(data.size());
}


void Connection::update()
{
    if(mState == State::DROPPED || mState == State::IDLE)
        return;

    DWORD numBytesTransferred;
    BOOL success = GetOverlappedResult(mPipe, mOverlapped, &numBytesTransferred, FALSE);

    if(!success)
    {
        drop();
        return;
    }

    switch(mState)
    {
        case State::CONNECTING:
            mState = State::IDLE;
            break;

        case State::READING:
            handleRead(numBytesTransferred);
            break;

        case State::WRITING:
            handleWrite(numBytesTransferred);
            break;


        default:
            THROW_RUNTIME_ERROR(stringify("Unimplemented message connection state: ", (int)mState));
    }
}

void Connection::handleRead(size_t numBytesRead)
{
    assert(mState == State::READING);
    if(numBytesRead == 0)
    {
        drop();
        return;
    }

    mMessages.emplace_back();
    std::vector<char>& message = mMessages.back();
    message.assign(mBuffer, mBuffer + numBytesRead);


    mState = State::IDLE;
}

void Connection::handleWrite(size_t numBytesWritten)
{
    assert(mState == State::WRITING);
    if(numBytesWritten != mLatestWriteSize)
    {
        drop();
        return;
    }



    mState = State::IDLE;
}
