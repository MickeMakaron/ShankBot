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
#ifndef GRAPHICS_LAYER_FRAME_HPP
#define GRAPHICS_LAYER_FRAME_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Draw.hpp"
#include "FileIo.hpp"
#include "monitor/RawImage.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    struct Frame
    {
        bool hasMiniMapMoved = false;
        short miniMapX = 0;
        short miniMapY = 0;

        float miniMapScreenX = 0;
        float miniMapScreenY = 0;
        float miniMapScreenWidth = 0;
        float miniMapScreenHeight = 0;

        bool hasViewUpdated = false;
        float viewX = 0;
        float viewY = 0;
        float viewWidth = 0;
        float viewHeight = 0;

        unsigned short width = 0;
        unsigned short height = 0;

        std::shared_ptr<std::vector<SpriteDraw>> spriteDraws;
        std::shared_ptr<std::vector<GuiDraw>> guiDraws;
        std::shared_ptr<std::vector<SpriteDraw>> guiSpriteDraws;
        std::shared_ptr<std::vector<TextDraw>> textDraws;
        std::shared_ptr<std::vector<RectDraw>> rectDraws;
        std::shared_ptr<std::vector<FileIo>> fileIo; // Unused
        std::shared_ptr<std::vector<MiniMapDraw>> miniMapDraws;
        std::shared_ptr<RawImage> screenPixels;
    };
}

#endif // GRAPHICS_LAYER_FRAME_HPP
