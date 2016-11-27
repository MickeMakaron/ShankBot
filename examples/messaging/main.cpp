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

#include "messaging/Message.hpp"
#include "messaging/ConnectionManager.hpp"
#include "messaging/Requester.hpp"
#include "messaging/LoginResponse.hpp"
using namespace GraphicsLayer;
using namespace sb::messaging;

#include <string>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <sstream>

void handleLoginResponse(const LoginResponse& r)
{
    std::cout << "Login response: " << std::endl;
    for(const LoginResponse::Character& c : r.getCharacters())
        std::cout << "\t" << c.name << "\t\t\t" << c.world << std::endl;
    std::cout << std::endl;
}

void handleRequestResult(const Response& r)
{
    std::cout << "Response: " << (int)r.get() << std::endl;
}

void handleFrameResponse(const FrameResponse& r)
{
    const FrameResponse::Frame& f = r.get();

    std::cout << ":::::::::::: FRAME :::::::::::::::" << std::endl;
    std::cout << "Object count: " << std::endl;
    for(size_t y = 0; y < f.scene.HEIGHT; y++)
    {
        for(size_t x = 0; x < f.scene.WIDTH; x++)
        {
            std::cout << "\t" << f.scene.objects[x][y].size() << " ";
        }

        std::cout << std::endl;
    }
    std::cout << "---------" << std::endl;

    std::cout << "Mini map: " << std::endl;
    std::cout << "\tCoords: " << f.miniMap.x << "x" << f.miniMap.y << ", " << (int)f.miniMap.level << std::endl;
    for(size_t y = 0; y < f.scene.HEIGHT; y++)
    {
        for(size_t x = 0; x < f.scene.WIDTH; x++)
        {
            std::cout << "\t" << (int)f.miniMap.colors[x][y] << "\t";
        }

        std::cout << std::endl;
    }
    std::cout << "---------" << std::endl;


    std::cout << "Players : " << std::endl;
    for(const FrameResponse::Player& p : f.scene.players)
        std::cout << "\t" << p.name << ": " << p.x << "x" << p.y << " (" << p.hp * 100.f << "%)" << std::endl;
    std::cout << "---------" << std::endl;

    std::cout << "Creatures : " << std::endl;
    for(const FrameResponse::Creature& p : f.scene.creatures)
        std::cout << "\t" << p.name << "(" << p.id << "): " << p.x << "x" << p.y << " (" << p.hp * 100.f << "%)" << std::endl;
    std::cout << "---------" << std::endl;

    std::cout << "Npcs : " << std::endl;
    for(const FrameResponse::Npc& p : f.scene.npcs)
        std::cout << "\t" << p.name << ": " << p.x << "x" << p.y << std::endl;
    std::cout << "---------" << std::endl;

}


enum class Command : unsigned char
{
    LOGIN,
    GO,
    FRAME,
    INVALID,
};

Command resolveCommand(const std::string& str)
{
    if(str == "login") return Command::LOGIN;
    if(str.substr(0, 2) == "go") return Command::GO;
    if(str == "frame") return Command::FRAME;

    return Command::INVALID;
}

void parseGoCommand(std::string command, short& x, short& y)
{
    std::stringstream sstream(command);
    std::string go;
    sstream >> go;
    assert(go == "go");

    sstream >> x >> y;
}

int main(int argc, char** argv)
{
    Requester requester;
    while(true)
    {

        std::string message;
        getline(std::cin, message);
        Command c = resolveCommand(message);

        Requester::Status s;
        switch(c)
        {
            case Command::LOGIN:
            {
                LoginResponse r = requester.login(s, "FrixaRael", "choochoo1");
                if(s != Requester::Status::SUCCESS)
                    throw std::runtime_error("Failed to request login.");
                handleLoginResponse(r);
                break;
            }

            case Command::GO:
            {
                short x;
                short y;
                parseGoCommand(message, x, y);
                Response r = requester.go(s, x, y);
                if(s != Requester::Status::SUCCESS)
                    throw std::runtime_error("Failed to request go.");
                handleRequestResult(r);
                break;
            }

            case Command::FRAME:
            {
                FrameResponse r = requester.frame(s);
                if(s != Requester::Status::SUCCESS)
                    throw std::runtime_error("Failed to request frame.");
                handleFrameResponse(r);
                break;
            }

            default:
                std::cout << "Invalid command." << std::endl;
                break;
        }
    }

    return 0;
}

