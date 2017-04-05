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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/GuiSpriteParser.hpp"
#include "monitor/Frame.hpp"
#include "monitor/TibiaContext.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
///////////////////////////////////

GuiSpriteParser::GuiSpriteParser()
{
}

void GuiSpriteParser::parse(const Frame& frame)
{
    mData = Data();

    if(frame.guiSpriteDraws == nullptr || frame.guiSpriteDraws->empty())
    {
        return;
    }

    const std::vector<SpriteDraw>& draws = *frame.guiSpriteDraws;
    std::vector<size_t> splitPoints;
    for(size_t i = 0; i < draws.size();)
    {
        size_t drawCallId = draws[i].drawCallId;
        size_t j = i;
        while(j < draws.size() && draws[j].drawCallId == drawCallId)
        {
            j++;
        }

        mData.drawGroups.emplace_back();
        std::vector<const SpriteDraw*>& group = mData.drawGroups.back();
        group.reserve(j - i);
        for(size_t k = i; k < j; k++)
        {
            group.emplace_back(&draws[k]);
        }

        i = j;
    }
}

const GuiSpriteParser::Data& GuiSpriteParser::getData()
{
    return mData;
}
