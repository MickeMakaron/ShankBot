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

#include "monitor/ShankBot.hpp"

int main(int argc, char** argv)
{
//    int* d = (int*)size_t(-1);
//    *d = 1;

    std::string tibiaDir = "C:/Users/Vendrii/Documents/programming/projects/ShankBot/monitor/tibia";
    std::string versionControlDir = "C:/Users/Vendrii/Documents/programming/projects/ShankBot/monitor/version-control";
    GraphicsLayer::ShankBot sb(tibiaDir, versionControlDir);
    sb.run();

    return 0;
}


