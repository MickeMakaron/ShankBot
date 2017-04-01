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
                unsigned short level = 0;
                unsigned int experience = 0;
                unsigned short xpGainRate = 0;

                unsigned short hp = 0;
                unsigned short mana = 0;
                unsigned short soul = 0;
                unsigned short cap = 0;
                unsigned short speed = 0;
                unsigned short foodMinutes = 0;
                unsigned short staminaMinutes = 0;
                unsigned short offlineTrainingMinutes = 0;
                unsigned short magicLevel = 0;


                unsigned short fistLevel = 0;
                unsigned short clubLevel = 0;
                unsigned short swordLevel = 0;
                unsigned short axeLevel = 0;
                unsigned short distanceLevel = 0;
                unsigned short shieldingLevel = 0;
                unsigned short fishingLevel = 0;

                unsigned short critChance = 0;
                unsigned short critDamage = 0;

                unsigned short hpLeechChance = 0;
                unsigned short hpLeechAmount = 0;

                unsigned short manaLeechChance = 0;
                unsigned short manaLeechAmount = 0;
            };

            struct Container : public SideBarWindow
            {
                std::vector<Text> counts;
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
                std::vector<Text> clickableNpcText;
                std::vector<Container> containers;

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
