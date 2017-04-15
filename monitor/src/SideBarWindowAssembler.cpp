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
#include "utility/utility.hpp"
using namespace GraphicsLayer;
using namespace sb::utility;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <cassert>
#include <algorithm>
#include <iostream>
///////////////////////////////////


SideBarWindowAssembler::SideBarWindowAssembler(const TibiaContext& context)
: mContext(context)
{

}

bool SideBarWindowAssembler::windowContains(const GuiParser::SideBarWindow& w, const Draw& d) const
{
    Vertex topLeft;
    d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);

    unsigned short x = round(topLeft.x);
    unsigned short y = round(topLeft.y);

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

std::vector<GuiParser::SideBarWindow>::const_iterator SideBarWindowAssembler::pairWindowWithTitle(const std::vector<GuiParser::SideBarWindow>& windows, const Text& title) const
{
    unsigned short x = round(title.x);
    unsigned short y = round(title.y);

    std::vector<std::vector<GuiParser::SideBarWindow>::const_iterator> its;
    for(auto it = windows.begin(); it != windows.end(); it++)
    {
        if(it->titleBar.screen.y <= y && it->titleBar.screen.x <= x)
        {
            its.push_back(it);
        }
    }

    if(its.empty())
    {
        return windows.end();
    }

    return *std::min_element(its.begin(), its.end(), [x, y](const auto& w1, const auto& w2)
    {
        unsigned short dx1 = x - w1->titleBar.screen.x;
        unsigned short dy1 = y - w1->titleBar.screen.y;
        unsigned short dx2 = x - w2->titleBar.screen.x;
        unsigned short dy2 = y - w2->titleBar.screen.y;

        return dx1 * dx1 + dy1 * dy1 < dx2 * dx2 + dy2 * dy2;
    });
}


void SideBarWindowAssembler::assemble(const Frame& frame, const GuiParser::Data& gui, const TextParser::Data& text, const RectParser::Data& rect, const GuiSpriteParser::Data& guiSprite)
{
    mData = Data();

    mHalfFrameWidth = float(frame.width) / 2.f;
    mHalfFrameHeight = float(frame.height) / 2.f;
    mGui = &gui;
    mText = &text;
    mRect = &rect;
    mGuiSprite = &guiSprite;

    size_t iGuiSprite = 0;
    size_t iSlot = 0;
    if(!text.isInventoryMinimized && !mGuiSprite->drawGroups.empty() && mGuiSprite->drawGroups[0][0]->topLeft.x > 20.f)
    {
        iGuiSprite++;
        iSlot++;
    }

    try
    {

        SB_EXPECT(mGui->game.sideBarWindows.windows.size(), ==, text.windowOrder.size());
        size_t iCount = 0;
        for(size_t iWindow = 0; iWindow < mGui->game.sideBarWindows.windows.size(); iWindow++)
        {
            const GuiParser::SideBarWindow& window = mGui->game.sideBarWindows.windows[iWindow];

            using T = SideBarWindow::Type;
            T windowType = text.windowOrder[iWindow];
            switch(windowType)
            {
                case T::BATTLE:
                    parseBattle(window, iGuiSprite);
                    break;

                case T::VIP:
                    parseVip(window);
                    break;

                case T::CONTAINER:
                    parseContainer(window, iSlot, iGuiSprite, iCount);
                    break;

                case T::PREY:
                    parsePrey(window, iGuiSprite);
                    break;

                case T::SKILLS:
                    parseSkills(window);
                    break;

                case T::UNJUSTIFIED_POINTS:
                    mData.unjustifiedPoints.reset(new UnjustifiedPointsWindow());
                    assignWindow(*mData.unjustifiedPoints, window);
                    break;

                default:
                    SB_THROW("Unimplemented window type: ", (int)windowType);
            }
        }
    }
    catch(const std::runtime_error& e)
    {
        std::cout << "Failed to assemble side bar window: " << e.what() << std::endl;
    }

//
//    std::vector<GuiParser::SideBarWindow> windows = mGui->game.sideBarWindows.windows;
//    if(text.battle.title.type != Text::Type::INVALID)
//    {
//        mData.battle.reset(new BattleWindow());
//
//        SB_EXPECT()
//
//        const std::vector<RectParser::Bar>* battleBars = nullptr;
//        const std::vector<const SpriteDraw*>* sprites = nullptr;
//        for(const std::vector<RectParser::Bar>& bars : mRect->bars)
//        {
//            const RectDraw* draw = bars[0].border.draw;
//            assert(draw != nullptr);
//
//            size_t i = pairWindowWithTitle(windows, *draw);
//            if(i < windows.size())
//            {
//                for(const std::vector<const SpriteDraw*> draws : mGuiSprite->drawGroups)
//                {
//                    if(windowContains(windows[i], *draws[0]))
//                    {
//                        sprites = &draws;
//                        break;
//                    }
//                }
//                assert(sprites != nullptr);
//                battleBars = &bars;
//                mData.battle->titleBar = windows[i].titleBar.screen;
//                mData.battle->clientArea = windows[i].clientArea.screen;
//                windows.erase(windows.begin() + i);
//                break;
//            }
//        }
//
//
//        if(battleBars != nullptr)
//        {
//            assert(battleBars->size() <= text.battle.names.size());
//            assert(battleBars->size() <= sprites->size());
//            mData.battle->characters.reserve(battleBars->size());
//            for(size_t i = 0; i < battleBars->size(); i++)
//            {
//                BattleWindow::Character c;
//                c.draw = (*sprites)[i];
//                c.name = text.battle.names[i].string;
//                c.hpPercent = (*battleBars)[i].percent;
//                mData.battle->characters.push_back(c);
//            }
//            mData.battle->selectedCharacterIndex = text.battle.selectedNameIndex;
//            assert(mData.battle->selectedCharacterIndex == -1 || mData.battle->selectedCharacterIndex < mData.battle->characters.size());
//        }
//
//        if(mGui->game.uniqueButtons[(size_t)UniqueButton::Type::BATTLE_MONSTER].type != UniqueButton::Type::INVALID)
//        {
//            uint8_t& filter = mData.battle->filter;
//            const auto& ubs = mGui->game.uniqueButtons;
//
//            filter = BattleFilter::NONE;
//            using T = UniqueButton::Type;
//            namespace F = BattleFilter;
//            if(ubs[(size_t)T::BATTLE_MONSTER].isDown)
//            {
//                filter |= F::MONSTER;
//            }
//            if(ubs[(size_t)T::BATTLE_NPC].isDown)
//            {
//                filter |= F::NPC;
//            }
//            if(ubs[(size_t)T::BATTLE_PARTY].isDown)
//            {
//                filter |= F::PARTY;
//            }
//            if(ubs[(size_t)T::BATTLE_PLAYER].isDown)
//            {
//                filter |= F::PLAYER;
//            }
//            if(ubs[(size_t)T::BATTLE_SKULL].isDown)
//            {
//                filter |= F::SKULL;
//            }
//        }
//    }
//
//    if(text.vip.title.type != Text::Type::INVALID)
//    {
//        mData.vip.reset(new VipWindow());
//        size_t i = getWindow(windows, text)
//    }
}

const SideBarWindowAssembler::Data& SideBarWindowAssembler::getData() const
{
    return mData;
}



void SideBarWindowAssembler::assignWindow(SideBarWindow& w, const GuiParser::SideBarWindow& window)
{
    w.titleBar = window.titleBar.screen;
    w.clientArea = window.clientArea.screen;
    w.isMinimized = window.isMinimized();
}

void SideBarWindowAssembler::parseBattle(const GuiParser::SideBarWindow& window, size_t& iGuiSprite)
{
    mData.battle.reset(new BattleWindow());
    assignWindow(*mData.battle, window);

    if(mData.battle->isMinimized)
    {
        return;
    }

    if(mText->battle.names.empty())
    {
        return;
    }

    const std::vector<const SpriteDraw*> sprites = mGuiSprite->drawGroups[iGuiSprite];
    iGuiSprite++;

    const std::vector<Text>& names = mText->battle.names;
    const std::vector<RectParser::Bar>& bars = mRect->battleBars;


    SB_EXPECT(sprites.size(), ==, names.size());
    SB_EXPECT(bars.size(), ==, names.size());

    if(names.size() > 1)
    {
        SB_EXPECT(sprites[0]->topLeft.y, <, sprites[1]->topLeft.y);
        SB_EXPECT(names[0].y, <, names[1].y);
        SB_EXPECT(bars[0].border.draw->topLeft.y, <, bars[1].border.draw->topLeft.y);
    }

    mData.battle->characters.resize(bars.size());
    for(size_t i = 0; i < bars.size(); i++)
    {
        BattleWindow::Character& c = mData.battle->characters[i];
        c.draw = sprites[i];
        c.name = names[i].string;
        c.hpPercent = bars[i].percent;
    }
    mData.battle->selectedCharacterIndex = mText->battle.selectedNameIndex;
    SB_EXPECT_TRUE(mData.battle->selectedCharacterIndex == -1 || mData.battle->selectedCharacterIndex < mData.battle->characters.size());

    if(mGui->game.uniqueButtons[(size_t)UniqueButton::Type::BATTLE_MONSTER].type != UniqueButton::Type::INVALID)
    {
        uint8_t& filter = mData.battle->filter;
        const auto& ubs = mGui->game.uniqueButtons;

        filter = BattleFilter::NONE;
        using T = UniqueButton::Type;
        namespace F = BattleFilter;
        if(ubs[(size_t)T::BATTLE_MONSTER].isDown)
        {
            filter |= F::MONSTER;
        }
        if(ubs[(size_t)T::BATTLE_NPC].isDown)
        {
            filter |= F::NPC;
        }
        if(ubs[(size_t)T::BATTLE_PARTY].isDown)
        {
            filter |= F::PARTY;
        }
        if(ubs[(size_t)T::BATTLE_PLAYER].isDown)
        {
            filter |= F::PLAYER;
        }
        if(ubs[(size_t)T::BATTLE_SKULL].isDown)
        {
            filter |= F::SKULL;
        }
    }
}

void SideBarWindowAssembler::parseVip(const GuiParser::SideBarWindow& window)
{
    mData.vip.reset(new VipWindow());
    assignWindow(*mData.vip, window);

    mData.vip->online = mText->vip.online;
    mData.vip->offline = mText->vip.offline;
    if(mData.vip->isMinimized)
    {
        SB_EXPECT_TRUE(mData.vip->online.empty());
        SB_EXPECT_TRUE(mData.vip->offline.empty());
    }
}

void SideBarWindowAssembler::parseContainer(const GuiParser::SideBarWindow& window, size_t& iSlot, size_t& iGuiSprite, size_t& iCount)
{
    mData.containers.emplace_back();
    ContainerWindow& c = mData.containers.back();

    assignWindow(c, window);

    if(iGuiSprite < mGuiSprite->drawGroups.size())
    {
        Vertex iconTopLeft;
        mGuiSprite->drawGroups[iGuiSprite][0]->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, iconTopLeft.x, iconTopLeft.y);
        int dIconLeft = round(iconTopLeft.x) - c.titleBar.x;
        int dIconTop = round(iconTopLeft.y) - c.titleBar.y;
        if(mGuiSprite->drawGroups[iGuiSprite].size() == 1 && dIconLeft > 0 && dIconLeft < 6 && dIconTop > 0 && dIconTop < 6)
        {
            iGuiSprite++;
        }
    }

    if(c.isMinimized)
    {
        return;
    }

    SB_EXPECT(iSlot, <, mGui->game.sideBarWindows.containers.size());
    const std::vector<GuiElement>& slots = mGui->game.sideBarWindows.containers[iSlot].slots;
    iSlot++;
    c.capacity = slots.size();

    if(iGuiSprite >= mGuiSprite->drawGroups.size())
    {
        return;
    }

    const std::vector<const SpriteDraw*>& sprites = mGuiSprite->drawGroups[iGuiSprite];
    c.items.resize(sprites.size());
    const std::vector<Text>* counts = nullptr;
    size_t iCountText = 0;
    size_t iCountSize = 0;
    if(iCount < mText->containers.size())
    {
        counts = &mText->containers[iCount].counts;
        iCountSize = counts->size();
    }

    for(size_t i = 0; i < sprites.size(); i++)
    {
        const SpriteDraw& sprite = *sprites[i];
        Vertex topLeft = slots[i].draw->topLeft;
        Vertex botRight = slots[i].draw->botRight;
        topLeft.x -= 0.0001f;
        topLeft.y -= 0.0001f;
        botRight.x += 0.0001f;
        botRight.y += 0.0001f;
        if(sprite.topLeft.x > topLeft.x &&
           sprite.topLeft.y > topLeft.y &&
           sprite.botRight.x < botRight.x &&
           sprite.botRight.y < botRight.y)
        {
            c.items[i].sprite = &sprite;
        }
        else
        {
            c.items.clear();
            return;
        }

        if(iCountText < iCountSize)
        {
            const Text& text = (*counts)[iCountText];
            if(text.localX > topLeft.x &&
               text.localY > topLeft.y &&
               text.localX < botRight.x &&
               text.localY < botRight.y)
            {
                const std::string& count = text.string;
                SB_EXPECT_TRUE(sb::utility::isNumeric(count));
                c.items[i].count = sb::utility::strToInt(count);

                iCountText++;
            }
        }
    }

    if(counts != nullptr && iCountText >= iCountSize)
    {
        iCount++;
    }
    iGuiSprite++;


}

void SideBarWindowAssembler::parsePrey(const GuiParser::SideBarWindow& window, size_t& iGuiSprite)
{
    mData.prey.reset(new PreyWindow());
    assignWindow(*mData.prey, window);

    if(mData.prey->isMinimized)
    {
        return;
    }


    const std::vector<Text>& names = mText->prey.names;
    const std::vector<RectParser::Bar>& bars = mRect->preyBars;
    const std::vector<GuiParser::Prey::Bonus>& bonuses = mGui->game.sideBarWindows.prey.bonuses;

    SB_EXPECT_FALSE(names.empty());
    SB_EXPECT(bars.size(), ==, names.size());
    SB_EXPECT(bars.size(), ==, bonuses.size());

    std::vector<size_t> validIndices;
    for(size_t i = 0; i < names.size(); i++)
    {
        if(names[i].string != "Inactive")
        {
            validIndices.push_back(i);
        }
    }

    if(!validIndices.empty())
    {
        SB_EXPECT(iGuiSprite, <, mGuiSprite->drawGroups.size());
        SB_EXPECT(validIndices.size(), ==, mGuiSprite->drawGroups[iGuiSprite].size());
        iGuiSprite++;
    }

    mData.prey->bonuses.resize(names.size());
    for(size_t i = 0; i < names.size(); i++)
    {
        PreyWindow::Bonus& b = mData.prey->bonuses[i];
        b.name = names[i];
        b.percent = bars[i].percent;
    }

}

void SideBarWindowAssembler::parseSkills(const GuiParser::SideBarWindow& window)
{
    mData.skills.reset(new SkillsWindow());
    assignWindow(*mData.skills, window);

    if(mData.skills->isMinimized)
    {
        return;
    }

    mData.skills->values = mText->skills.values;
    mData.skills->percentages.fill(-1.f);

    if(mRect->skillBars.empty())
    {
        return;
    }

    const std::vector<RectParser::Bar>& bars = mRect->skillBars;
    const auto& y = mText->skills.yCoords;
    SB_EXPECT(bars.front().border.draw->topLeft.y, >, y.front());
    SB_EXPECT(bars.back().border.draw->topLeft.y, <, y.back());

    for(size_t i = 0, iBar = 0; i + 1 < y.size() && iBar < bars.size(); i++)
    {
        short barY = round(bars[iBar].border.draw->topLeft.y);
        if(barY > y[i] && barY < y[i + 1])
        {
            mData.skills->percentages[i] = bars[iBar].percent;
            iBar++;
        }
    }
}
