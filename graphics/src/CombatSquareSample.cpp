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
///////////////////////////////////
// Internal ShankBot headers
#include "CombatSquareSample.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QGuiApplication"
#include "QtGui/QPainter"
#include "QtGui/QPainterPath"
#include "QtGui/QImage"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////


const std::vector<CombatSquareSample::GenerationDetails> CombatSquareSample::M_GENERATION_DETAILS =
{
    GenerationDetails(CombatSquare::Type::ATTACK, Color(255, 0, 0)),
    GenerationDetails(CombatSquare::Type::ATTACK_HIGHLIGHT, Color(255, 128, 128)),
    GenerationDetails(CombatSquare::Type::ATTACK_IN, Color(0, 0, 0)),
    GenerationDetails(CombatSquare::Type::FOLLOW, Color(0, 255, 0)),
    GenerationDetails(CombatSquare::Type::FOLLOW_HIGHLIGHT, Color(128, 255, 128)),
    GenerationDetails(CombatSquare::Type::ATTACK_ATTACK_IN, Color(255, 0, 0), Color(0, 0, 0)),
    GenerationDetails(CombatSquare::Type::ATTACK_ATTACK_IN_HIGHLIGHT, Color(255, 128, 128), Color(0, 0, 0)),
    GenerationDetails(CombatSquare::Type::FOLLOW_ATTACK_IN, Color(0, 255, 0), Color(0, 0, 0)),
    GenerationDetails(CombatSquare::Type::FOLLOW_ATTACK_IN_HIGHLIGHT, Color(128, 255, 128), Color(0, 0, 0))
};

CombatSquareSample::CombatSquare::CombatSquare(Type type, const unsigned char* pixels)
: type(type)
, pixels(pixels, pixels + width * height * sb::utility::BYTES_PER_PIXEL_RGBA)
{
}

std::vector<CombatSquareSample::CombatSquare> CombatSquareSample::generateSamples()
{
    int argc = 0;
    QGuiApplication app(argc, nullptr);
    QImage img(CombatSquare::width, CombatSquare::height, QImage::Format_RGBA8888);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QPainterPath outPath;
    outPath.addRect(0, 0, img.width() - 1, img.height() - 1);
    QPainterPath inPath;
    inPath.addRect(2, 2, img.width() - 5, img.height() - 5);

    std::vector<CombatSquare> combatSquares;
    combatSquares.reserve(M_GENERATION_DETAILS.size());
    for(const GenerationDetails& d : M_GENERATION_DETAILS)
    {
        painter.strokePath(inPath, QPen(QColor(d.in.r, d.in.g, d.in.b, d.in.a), 3));
        painter.strokePath(outPath, QPen(QColor(d.out.r, d.out.g, d.out.b, d.out.a), 3));
        combatSquares.emplace_back(d.type, img.constBits());
    }

    return combatSquares;
}

CombatSquareSample::CombatSquare::Type CombatSquareSample::getType(size_t index)
{
    assert(index < M_GENERATION_DETAILS.size());
    return M_GENERATION_DETAILS[index].type;
}

