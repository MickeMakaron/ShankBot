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
#ifndef SB_API_REQUESTER_HPP
#define SB_API_REQUESTER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "api/config.hpp"
#include "api/RequestResult.hpp"
#include "api/Frame.hpp"
#include "api/Character.hpp"
#include "api/Object.hpp"
namespace sb
{
namespace messaging
{
    class Requester;
}
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <vector>
///////////////////////////////////

namespace sb
{
class SHANK_BOT_API_DECLSPEC Requester
{
    public:
        Requester();
        ~Requester();

        RequestResult frame(Frame& f);
        RequestResult go(short x, short y);
        RequestResult login(std::string accountName, std::string password, std::vector<Character>& characters);
        RequestResult objects(std::vector<Object>& objs);

    private:
        sb::messaging::Requester* mRequester = nullptr;
};
}

#endif // SB_API_REQUESTER_HPP
