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
#include "monitor/SideBarWindowAssembler.hpp"
#include "monitor/Frame.hpp"
#include "monitor/TibiaContext.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////


SideBarWindowAssembler::SideBarWindowAssembler(const TibiaContext& context)
: mContext(context)
{

}

bool SideBarWindowAssembler::windowContains(const GuiParser::SideBarWindow& w, const Draw& d) const
{
    Vertex topLeft;
    d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);

    unsigned short x = topLeft.x + 0.5f;
    unsigned short y = topLeft.y + 0.5f;

    if(x < w.clientArea.screen.x)
    {
        return false;
    }

    if(y < w.clientArea.screen.y)
    {
        return false;
    }

    if(x > w.clientArea.screen.x + w.clientArea.screen.width)
    {
        return false;
    }

    if(y > w.clientArea.screen.y + w.clientArea.screen.height)
    {
        return false;
    }

    return true;
}

size_t SideBarWindowAssembler::getWindow(const std::vector<GuiParser::SideBarWindow>& windows, const Draw& draw) const
{
    for(size_t i = 0; i < windows.size(); i++)
    {
        if(windowContains(windows[i], draw))
        {
            return i;
        }
    }

    return -1;
}


void SideBarWindowAssembler::assemble(const Frame& frame, const GuiParser::Data& gui, const TextParser::Data& text, const RectParser::Data& rect, const GuiSpriteParser::Data& guiSprite)
{
    mData = Data();

    mHalfFrameWidth = float(frame.width) / 2.f;
    mHalfFrameHeight = float(frame.height) / 2.f;

    std::vector<GuiParser::SideBarWindow> windows = gui.game.sideBarWindows.windows;
    if(text.battle.title.type != Text::Type::INVALID)
    {
        mData.battle.reset(new BattleWindow());

        const std::vector<RectParser::Bar>* battleBars = nullptr;
        const std::vector<const SpriteDraw*>* sprites = nullptr;
        for(const std::vector<RectParser::Bar>& bars : rect.bars)
        {
            const RectDraw* draw = bars[0].border.draw;
            assert(draw != nullptr);

            size_t i = getWindow(windows, *draw);
            if(i < windows.size())
            {
                for(const std::vector<const SpriteDraw*> draws : guiSprite.drawGroups)
                {
                    if(windowContains(windows[i], *draws[0]))
                    {
                        sprites = &draws;
                        break;
                    }
                }
                assert(sprites != nullptr);
                battleBars = &bars;
                mData.battle->titleBar = windows[i].titleBar.screen;
                mData.battle->clientArea = windows[i].clientArea.screen;
                windows.erase(windows.begin() + i);
                break;
            }
        }


        if(battleBars != nullptr)
        {
            assert(battleBars->size() <= text.battle.names.size());
            assert(battleBars->size() <= sprites->size());
            mData.battle->characters.reserve(battleBars->size());
            for(size_t i = 0; i < battleBars->size(); i++)
            {
                BattleWindow::Character c;
                c.draw = (*sprites)[i];
                c.name = text.battle.names[i].string;
                c.hpPercent = (*battleBars)[i].percent;
                mData.battle->characters.push_back(c);
            }
            mData.battle->selectedCharacterIndex = text.battle.selectedNameIndex;
            assert(mData.battle->selectedCharacterIndex == -1 || mData.battle->selectedCharacterIndex < mData.battle->characters.size());
        }
    }
}

const SideBarWindowAssembler::Data& SideBarWindowAssembler::getData() const
{
    return mData;
}
