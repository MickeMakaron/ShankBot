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
#ifndef GRAPHICS_LAYER_TIBIA_IO_UTILITY_HPP
#define GRAPHICS_LAYER_TIBIA_IO_UTILITY_HPP

///////////////////////////////////
// Internal ShankBot headers
namespace GraphicsLayer
{
    class GraphicsMonitorReader;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    std::vector<std::string> getContextMenu(GraphicsMonitorReader& reader, unsigned short screenX, unsigned short screenY);
    std::string getGreenText(GraphicsMonitorReader& reader, unsigned short screenX, unsigned short screenY);
}

#endif // GRAPHICS_LAYER_TIBIA_IO_UTILITY_HPP
