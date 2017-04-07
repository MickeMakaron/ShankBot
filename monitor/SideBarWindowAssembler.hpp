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
#ifndef GRAPHICS_LAYER_SIDE_BAR_WINDOW_ASSEMBLER_HPP
#define GRAPHICS_LAYER_SIDE_BAR_WINDOW_ASSEMBLER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/GuiParser.hpp"
#include "monitor/GuiSpriteParser.hpp"
#include "monitor/TextParser.hpp"
#include "monitor/RectParser.hpp"
namespace GraphicsLayer
{
    class TibiaContext;
    struct Frame;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <memory>
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    namespace BattleFilter
    {
        static constexpr uint8_t
            NONE    = 0,
            PLAYER  = 1 << 0,
            NPC     = 1 << 1,
            MONSTER = 1 << 2,
            SKULL   = 1 << 3,
            PARTY   = 1 << 4,
            UNKNOWN = 1 << 5,
            ALL     = PLAYER | NPC | MONSTER | SKULL | PARTY;
    };

    struct SideBarWindow
    {
        IRect titleBar;
        IRect clientArea;

        bool isMinimized;
    };

    struct BattleWindow : public SideBarWindow
    {
        struct Character
        {
            std::string name;
            float hpPercent;
            const SpriteDraw* draw = nullptr;
        };

        uint8_t filter = BattleFilter::UNKNOWN;
        std::vector<Character> characters;
        size_t selectedCharacterIndex = -1;
    };

    struct VipWindow : public SideBarWindow
    {
        std::vector<Text> online;
        std::vector<Text> offline;
    };

    struct ContainerWindow : public SideBarWindow
    {
        struct Item
        {
            unsigned short count = 0;
            const SpriteDraw* sprite = nullptr;
        };

        std::vector<Item> items;
        unsigned short capacity = 0;
    };

    struct PreyWindow : public SideBarWindow
    {
        struct Bonus
        {
            Text name;
            float percent;
        };

        std::vector<Bonus> bonuses;
    };

    class SideBarWindowAssembler
    {
        public:

            struct Data
            {
                std::shared_ptr<BattleWindow> battle;
                std::shared_ptr<VipWindow> vip;
                std::shared_ptr<PreyWindow> prey;
                std::vector<ContainerWindow> containers;
            };

        public:
            SideBarWindowAssembler(const TibiaContext& context);

            void assemble(const Frame& frame, const GuiParser::Data& gui, const TextParser::Data& text, const RectParser::Data& rect, const GuiSpriteParser::Data& guiSprite);
            const Data& getData() const;

        private:
            bool windowContains(const GuiParser::SideBarWindow& w, const Draw& d) const;
            std::vector<GuiParser::SideBarWindow>::const_iterator pairWindowWithTitle(const std::vector<GuiParser::SideBarWindow>& windows, const Text& text) const;


        private:
            const TibiaContext& mContext;
            Data mData;
            float mHalfFrameWidth;
            float mHalfFrameHeight;
    };
}

#endif // GRAPHICS_LAYER_SIDE_BAR_WINDOW_ASSEMBLER_HPP
