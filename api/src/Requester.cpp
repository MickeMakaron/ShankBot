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
#include "api/Requester.hpp"
#include "messaging/Requester.hpp"
#include "messaging/Response.hpp"
#include "messaging/FrameRequest.hpp"
#include "messaging/FrameResponse.hpp"
#include "messaging/GoRequest.hpp"
#include "messaging/LoginRequest.hpp"
#include "messaging/LoginResponse.hpp"
#include "messaging/ObjectResponse.hpp"
#include "utility/utility.hpp"
using namespace sb;
namespace msg = sb::messaging;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////


Requester::Requester()
: mRequester(new messaging::Requester())
{
}

Requester::~Requester()
{
    delete mRequester;
}

RequestResult Requester::frame(Frame& f)
{
    RequestResult result;
    auto response = mRequester->request(result, std::make_unique<msg::FrameRequest>());
    if(response == nullptr)
        return result;
    assert(msg::Response::readResponseType(response->data(), response->size()) == msg::Message::Type::FRAME_RESPONSE);
    msg::FrameResponse r;
    if(!r.fromBinary(response->data(), response->size()))
        SB_THROW("Failed to read frame response.");
    f = r.get();
    return result;
}

RequestResult Requester::go(short x, short y)
{
    msg::GoRequest* req = new msg::GoRequest();
    req->set(x, y);
    RequestResult result;
    auto response = mRequester->request(result, std::unique_ptr<msg::Message>(req));
    if(response == nullptr)
        return result;
    assert(msg::Response::readResponseType(response->data(), response->size()) == msg::Message::Type::INVALID);
    return result;
}

RequestResult Requester::login(std::string accountName, std::string password, std::vector<Character>& characters)
{
    msg::LoginRequest* req = new msg::LoginRequest();
    req->set(accountName, password);
    RequestResult result;
    auto response = mRequester->request(result, std::unique_ptr<msg::Message>(req));
    if(response == nullptr)
        return result;

    assert(msg::Response::readResponseType(response->data(), response->size()) == msg::Message::Type::LOGIN_RESPONSE);
    msg::LoginResponse r;
    if(!r.fromBinary(response->data(), response->size()))
        SB_THROW("Failed to read login response.");

    characters = r.getCharacters();
    return result;
}

RequestResult Requester::objects(std::vector<Object>& objs)
{
    RequestResult result;
    auto response = mRequester->request(result, std::make_unique<msg::Message>(msg::Message::Type::OBJECT_REQUEST));
    if(response == nullptr)
        return result;
    assert(msg::Response::readResponseType(response->data(), response->size()) == msg::Message::Type::OBJECT_RESPONSE);
    msg::ObjectResponse r;
    if(!r.fromBinary(response->data(), response->size()))
       SB_THROW("Failed to read object response.");

    objs = r.get();
    return result;
}
