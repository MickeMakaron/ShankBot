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
#ifndef GRAPHICS_LAYER_SIDE_BAR_WINDOW_HPP
#define GRAPHICS_LAYER_SIDE_BAR_WINDOW_HPP

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
    enum class SkillField : unsigned char
    {
        LEVEL,
        EXPERIENCE,
        XP_GAIN_RATE,
        HP,
        MANA,
        SOUL,
        CAP,
        SPEED,
        FOOD_MINUTES,
        STAMINA_MINUTES,
        OFFLINE_TRAINING_MINUTES,
        MAGIC_LEVEL,
        FIST_LEVEL,
        CLUB_LEVEL,
        SWORD_LEVEL,
        AXE_LEVEL,
        DISTANCE_LEVEL,
        SHIELDING_LEVEL,
        FISHING_LEVEL,
        CRIT_CHANCE,
        CRIT_DAMAGE,
        HP_LEECH_CHANCE,
        HP_LEECH_AMOUNT,
        MANA_LEECH_CHANCE,
        MANA_LEECH_AMOUNT,
        NUM_FIELDS,
        INVALID,
    };



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
    }

    struct SideBarWindow
    {
        enum class Type : unsigned char
        {
            UNJUSTIFIED_POINTS,
            PREY,
            SKILLS,
            BATTLE,
            VIP,
            NPC_TRADE,
            CONTAINER,
            BROWSE_FIELD,

            NUM_TYPES,
            INVALID,
        };

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
            unsigned short count = 1;
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

    struct SkillsWindow : public SideBarWindow
    {
        std::array<unsigned int, (size_t)SkillField::NUM_FIELDS> values;
        std::array<float, (size_t)SkillField::NUM_FIELDS> percentages;
    };

    struct UnjustifiedPointsWindow : public SideBarWindow
    {

    };
}


#endif // GRAPHICS_LAYER_SIDE_BAR_WINDOW_HPP
