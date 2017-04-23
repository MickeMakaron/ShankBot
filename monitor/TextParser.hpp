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
#ifndef GRAPHICS_LAYER_TEXT_PARSER_HPP
#define GRAPHICS_LAYER_TEXT_PARSER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Text.hpp"
#include "monitor/TextBuilder.hpp"
#include "monitor/Draw.hpp"
#include "monitor/GuiParser.hpp"
#include "monitor/SideBarWindow.hpp"
namespace GraphicsLayer
{
    struct Frame;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
#include <vector>
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    class TextParser
    {
        public:
            struct SideBarWindow
            {
                Text title;
            };

            struct UnjustifiedPoints : public SideBarWindow
            {
                unsigned short open;
            };

            struct Prey : public SideBarWindow
            {
                std::vector<Text> names;
            };

            struct Vip : public SideBarWindow
            {
                std::vector<Text> online;
                std::vector<Text> offline;
            };

            struct Battle : public SideBarWindow
            {
                std::vector<Text> names;
                size_t selectedNameIndex = -1;
            };

            struct Skills : public SideBarWindow
            {
                std::array<unsigned int, (size_t)SkillField::NUM_FIELDS> values;
                std::array<short, (size_t)SkillField::NUM_FIELDS> yCoords;
            };

            struct Container : public SideBarWindow
            {
                std::vector<Text> counts;
            };

            struct WindowOrder
            {
                WindowOrder(const SideBarWindow* window, GraphicsLayer::SideBarWindow::Type type)
                : type(type), window(window){}
                GraphicsLayer::SideBarWindow::Type type;
                const SideBarWindow* window;
            };

            struct Data
            {
                std::vector<Text> names;
                std::vector<Text> chatTabs;
                Text toggleChat;
                bool isChatOn;
                Text centre;
                Text stop;
                bool isInventoryMinimized;
                unsigned short soul;
                unsigned short cap;
                unsigned short hp;
                unsigned short mana;
                bool isPremiumFeaturesMinimized;
                std::vector<Text> clickableNpcText;
                std::vector<Container> containers;
                std::vector<Text> inventoryStackCounts;
                std::vector<WindowOrder> windowOrder;

                UnjustifiedPoints unjustifiedPoints;
                Prey prey;
                Vip vip;
                Battle battle;
                Skills skills;
            };

        public:
            TextParser();
            void parse(const Frame& frame, const GuiParser::Data& guiData);
            const Data& getData() const;

        private:
            std::map<std::string, std::function<void(size_t&)>> initGuiTextHandlers();
            void handleGuiText(size_t& i);
            void handleDefaultSideBarText(size_t& i);
            void handleContainerText(size_t& i);
            static unsigned short parseTimeText(const std::string& str);
            static unsigned short parsePercentageText(const std::string& str);
            void readSoulAndCap(const TextBuilder& soul, const TextBuilder& cap);


        private:
            Data mData;
            std::shared_ptr<std::vector<TextDraw>> mDraws;
            std::vector<std::shared_ptr<TextBuilder>> mBuilders;
            const GuiParser::Data* mGuiData;
            const std::map<std::string, std::function<void(size_t&)>> mGuiTextHandlers;
    };
}


#endif // GRAPHICS_LAYER_TEXT_PARSER_HPP
