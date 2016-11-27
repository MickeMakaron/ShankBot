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
#include "messaging/Requester.hpp"
#include "messaging/FrameRequest.hpp"
#include "messaging/GoRequest.hpp"
#include "messaging/LoginRequest.hpp"
#include "utility/utility.hpp"
using namespace sb::messaging;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////


Requester::Requester()
{
    mManager.addConnection();
    ConnectionManager::Event e;
    ConnectionManager::ConnectionId c;
    c = mManager.poll(e, M_WAIT_TIME_MS);
    assert(e == ConnectionManager::Event::CONNECT);
    mConnection = &mManager.getConnection(c);
}

FrameResponse Requester::frame(Status& status)
{
    auto response = request(status, std::make_unique<FrameRequest>());
    if(status != Status::SUCCESS)
        return FrameResponse();
    assert(Response::readResponseType(response->data(), response->size()) == Message::Type::FRAME_RESPONSE);
    FrameResponse r;
    if(!r.fromBinary(response->data(), response->size()))
        THROW_RUNTIME_ERROR("Failed to read frame response.");
    return r;
}

Response Requester::go(Status& status, short x, short y)
{
    GoRequest* req = new GoRequest();
    req->set(x, y);
    auto response = request(status, std::unique_ptr<Message>(req));
    if(status != Status::SUCCESS)
        return Response();
    assert(Response::readResponseType(response->data(), response->size()) == Message::Type::INVALID);

    Response r;
    if(!r.fromBinary(response->data(), response->size()))
        THROW_RUNTIME_ERROR("Failed to read go response.");
    return r;
}

LoginResponse Requester::login(Status& status, std::string accountName, std::string password)
{
    LoginRequest* req = new LoginRequest();
    req->set(accountName, password);
    auto response = request(status, std::unique_ptr<Message>(req));
    if(status != Status::SUCCESS)
        return LoginResponse();
    assert(Response::readResponseType(response->data(), response->size()) == Message::Type::LOGIN_RESPONSE);
    LoginResponse r;
    if(!r.fromBinary(response->data(), response->size()))
        THROW_RUNTIME_ERROR("Failed to read login response.");
    return r;
}

std::shared_ptr<std::vector<char>> Requester::request(Status& status, const std::unique_ptr<Message>& message)
{
    typedef ConnectionManager::ConnectionId C;
    typedef ConnectionManager::Event E;
    mConnection->sendMessage(*message);
    E e;
    C c = mManager.poll(e, M_WAIT_TIME_MS);
    if(e != E::WRITE)
    {
        switch(e)
        {
            case E::DROP: status = Status::DROP; return nullptr;
            case E::TIME_OUT: status = Status::TIME_OUT; return nullptr;

            default:
                THROW_RUNTIME_ERROR(sb::utility::stringify("Unexpected event: ", (int)e));
        }
    }

    mConnection->read();
    c = mManager.poll(e, M_WAIT_TIME_MS);
    if(e != E::READ)
    {
        switch(e)
        {
            case E::DROP: status = Status::DROP; return nullptr;
            case E::TIME_OUT: status = Status::TIME_OUT; return nullptr;

            default:
                THROW_RUNTIME_ERROR(sb::utility::stringify("Unexpected event: ", (int)e));

        }
    }

    std::list<std::vector<char>> messages = mConnection->getMessages();
    status = Status::SUCCESS;
    assert(messages.size() == 1);
    assert(!messages.front().empty());

    auto response = std::make_shared<std::vector<char>>(messages.front());
    assert(Message::readMessageType(response->data(), response->size()) == Message::Type::RESPONSE);
    return response;
}
