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
#ifndef GRAPHICS_LAYER_TEXT_BUILDER_HPP
#define GRAPHICS_LAYER_TEXT_BUILDER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Constants.hpp"
#include "Text.hpp"
namespace GraphicsLayer
{
    struct TextDraw;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <list>
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    class TextBuilder
    {
        private:
            struct Line
            {
                std::list<unsigned char> characters;
                short topLeftX = 0;
                short topLeftY = 0;
                short botRightX = 0;
                short botRightY = 0;
                short maxAllowedDistanceX = 0;
                short maxAllowedDistanceY = 0;
            };

        public:
            explicit TextBuilder(const TextDraw& text, unsigned short frameWidth, unsigned short frameHeight);
            Text::Type getTextType() const;
            void insert(unsigned char character, short topLeftX, short topLeftY, short botRightX, short botRightY);
            const std::list<Text>& getText();

        private:
            void setTextType();
            void appendCharacter(unsigned char character, short topLeftX, short topLeftY, short botRightX, short botRightY);
            void appendLine(unsigned char character, short topLeftX, short topLeftY, short botRightX, short botRightY);
            void build();
            void updateTextType();


        private:
            const TextDraw& mTextDraw;
            const float M_HALF_FRAME_WIDTH;
            const float M_HALF_FRAME_HEIGHT;
            std::list<Text> mText;
            std::list<Line> mLines;
            Text::Type mTextType;
            bool mIsDisputedContextMenuOrChatTab = false;
            bool mIsBuilt = false;
    };

}


#endif // GRAPHICS_LAYER_TEXT_BUILDER_HPP
