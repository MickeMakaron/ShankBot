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
#include "monitor/SideBarWindow.hpp"
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
    class SideBarWindowAssembler
    {
        public:

            struct Data
            {
                std::shared_ptr<BattleWindow> battle;
                std::shared_ptr<VipWindow> vip;
                std::shared_ptr<PreyWindow> prey;
                std::shared_ptr<SkillsWindow> skills;
                std::shared_ptr<UnjustifiedPointsWindow> unjustifiedPoints;
                std::vector<ContainerWindow> containers;
            };

        public:
            SideBarWindowAssembler(const TibiaContext& context);

            void assemble(const Frame& frame, const GuiParser::Data& gui, const TextParser::Data& text, const RectParser::Data& rect, const GuiSpriteParser::Data& guiSprite);
            const Data& getData() const;

        private:
            bool windowContains(const GuiParser::SideBarWindow& w, const Draw& d) const;
            std::vector<GuiParser::SideBarWindow>::const_iterator pairWindowWithTitle(const std::vector<GuiParser::SideBarWindow>& windows, const Text& text) const;

            void assignWindow(SideBarWindow& w, const GuiParser::SideBarWindow& window);
            void parseBattle(const GuiParser::SideBarWindow& window, std::vector<std::vector<const SpriteDraw*>>& guiSprites);
            void parseVip(const GuiParser::SideBarWindow& window);
            void parseContainer(const GuiParser::SideBarWindow& window,
                                std::vector<GuiParser::Container>& containerSlots,
                                std::vector<std::vector<const SpriteDraw*>>& guiSprites,
                                std::vector<TextParser::Container>& containerCounts);
            void parsePrey(const GuiParser::SideBarWindow& window, std::vector<std::vector<const SpriteDraw*>>& guiSprites);
            void parseSkills(const GuiParser::SideBarWindow& window);

        private:
            const TibiaContext& mContext;
            Data mData;
            const GuiParser::Data* mGui;
            const TextParser::Data* mText;
            const RectParser::Data* mRect;
            const GuiSpriteParser::Data* mGuiSprite;
            float mHalfFrameWidth;
            float mHalfFrameHeight;
    };
}

#endif // GRAPHICS_LAYER_SIDE_BAR_WINDOW_ASSEMBLER_HPP
