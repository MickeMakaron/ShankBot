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
#ifndef GRAPHICS_LAYER_RECT_PARSER_HPP
#define GRAPHICS_LAYER_RECT_PARSER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Draw.hpp"
namespace GraphicsLayer
{
    struct Frame;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
#include <vector>
///////////////////////////////////

namespace GraphicsLayer
{
    class RectParser
    {
        public:
            struct Rect
            {
                Rect(){}
                Rect(const RectDraw& draw) : draw(&draw){}
                const RectDraw* draw = nullptr;
            };

            struct Bar
            {
                Rect border;
                Rect fill;
                float percent;
            };

            struct Data
            {
                std::vector<std::vector<Bar>> bars;
                std::vector<Bar> hpBars;
                std::vector<Rect> textInputFields;
                Rect chatInputCursor;
            };

        public:
            explicit RectParser();

            void parse(const Frame& frame);

            const Data& getData();

        private:
            void insertBar(const Bar& b);
            static Bar mergeBarBorderAndFill(const RectDraw& border, const RectDraw& fill);

        private:
            Data mData;
            std::shared_ptr<std::vector<RectDraw>> mDraws;
    };
}


#endif // GRAPHICS_LAYER_RECT_PARSER_HPP
