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
#ifndef GRAPHICS_LAYER_COMBAT_SQUARE_SAMPLE_HPP
#define GRAPHICS_LAYER_COMBAT_SQUARE_SAMPLE_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "utility/Color.hpp"
#include "Constants.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <cstring>
///////////////////////////////////

namespace GraphicsLayer
{
class CombatSquareSample
{
    public:
        struct CombatSquare
        {
            enum class Type : unsigned char
            {
                ATTACK,
                ATTACK_HIGHLIGHT,
                ATTACK_IN,
                FOLLOW,
                FOLLOW_HIGHLIGHT,
                ATTACK_ATTACK_IN,
                ATTACK_ATTACK_IN_HIGHLIGHT,
                FOLLOW_ATTACK_IN,
                FOLLOW_ATTACK_IN_HIGHLIGHT,
                INVALID,
            };

            explicit CombatSquare(Type type, const unsigned char* pixels);

            Type type = Type::INVALID;
            static const size_t width = Constants::TILE_PIXEL_WIDTH;
            static const size_t height = Constants::TILE_PIXEL_HEIGHT;
            std::vector<unsigned char> pixels;
        };

    private:
        struct GenerationDetails
        {
            explicit GenerationDetails(CombatSquare::Type type, Color out, Color in = Color(0, 0, 0, 0))
            : type(type)
            , out(out)
            , in(in)
            {
            }

            CombatSquare::Type type;
            Color out;
            Color in;
        };

    public:
        static std::vector<CombatSquare> generateSamples();
        static CombatSquare::Type getType(size_t index);

    private:
        static const std::vector<GenerationDetails> M_GENERATION_DETAILS;
};
}


#endif // GRAPHICS_LAYER_COMBAT_SQUARE_SAMPLE_HPP
