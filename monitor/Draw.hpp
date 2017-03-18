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
#ifndef GRAPHICS_LAYER_DRAW_HPP
#define GRAPHICS_LAYER_DRAW_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "utility/Matrix.hpp"
#include "utility/Color.hpp"
#include "utility/Vertex.hpp"
#include "tibiaassets/Object.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
#include <list>
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    struct Draw
    {
        Vertex topLeft;
        Vertex botRight;
        std::shared_ptr<sb::utility::Matrix<float, 4, 4>> transform;

//        void getScreenCoords
//        (
//            float halfScreenWidth, float halfScreenHeight,
//            unsigned short& screenX, unsigned short& screenY
//        ) const
//        {
//            worldToScreenCoords(topLeft.x, topLeft.y, *transform, halfScreenWidth, halfScreenHeight, screenX, screenY);
//        }

//        void getScreenCoords
//        (
//            float halfScreenWidth, float halfScreenHeight,
//            unsigned short& screenX, unsigned short& screenY,
//            float worldX, float worldY
//        ) const
//        {
//            worldToScreenCoords(worldX, worldY, *transform, halfScreenWidth, halfScreenHeight, screenX, screenY);
//        }

        void getScreenCoords
        (
            float halfScreenWidth, float halfScreenHeight,
            float& screenX, float& screenY
        ) const;

        void getScreenCoords
        (
            float halfScreenWidth, float halfScreenHeight,
            float& screenX, float& screenY,
            float worldX, float worldY
        ) const;

    };

    struct SpriteDraw : public Draw
    {
        struct SpriteObjectPairing
        {
            size_t spriteId;
            std::list<size_t> objects;
        };

        std::list<SpriteObjectPairing> pairings;
    };

    struct RectDraw : public Draw
    {
        Color color;
    };

    struct GuiDraw : public Draw
    {
        std::string name;
    };

    struct GlyphDraw : public Draw
    {
        unsigned char character;
    };

    struct TextDraw : public Draw
    {
        std::shared_ptr<std::vector<GlyphDraw>> glyphDraws;
        Color color;
        bool isOutlined;
    };

    struct MiniMapDraw : public Draw
    {
        std::shared_ptr<std::vector<unsigned char>> pixels;
    };
}

#endif // GRAPHICS_LAYER_DRAW_HPP
