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
#ifndef GRAPHICS_LAYER_FONT_SAMPLE_HPP
#define GRAPHICS_LAYER_FONT_SAMPLE_HPP

///////////////////////////////////
// Qt
class QPainter;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <string>
#include <vector>
///////////////////////////////////

namespace GraphicsLayer
{
    class FontSample
    {
        public:
            enum Style : unsigned char
            {
                NORMAL = 1,
                BOLD = 1 << 1,
                ITALIC = 1 << 2,
            };

            struct Glyph
            {
                unsigned short width = 0;
                unsigned short height = 0;
                static const unsigned char bytesPerPixel = 1; // grayscale
                unsigned char character = 0;
                Style style = Style::NORMAL;
                std::vector<unsigned char> data;
                float pointSize = 0.f;
                unsigned int sum = 0;
            };

        public:
            explicit FontSample(std::string family, float minPointSize, float maxPointSize, unsigned char styleFlags = Style::NORMAL);

            const std::vector<Glyph>& getGlyphs() const;

        private:
            void createGlyphSamples(std::string family, float minPointSize, float maxPointSize, unsigned char styleFlags);
            void generateGlyphs(QPainter& painter);

        private:
            std::vector<Glyph> mGlyphs;
    };
}


#endif // GRAPHICS_LAYER_FONT_SAMPLE_HPP
