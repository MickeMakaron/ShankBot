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
#ifndef GRAPHICS_LAYER_GUI_PARSER_HPP
#define GRAPHICS_LAYER_GUI_PARSER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Frame.hpp"
#include "monitor/Text.hpp"
#include "utility/Rect.hpp"
namespace GraphicsLayer
{
    class TibiaContext;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <map>
#include <set>
///////////////////////////////////

namespace GraphicsLayer
{
    namespace PlayerState
    {
        constexpr uint32_t
            NONE                    = 0,
            POISONED                = 1 << 0,
            BURNING                 = 1 << 1,
            ELECTRIFIED             = 1 << 2,
            DRUNK                   = 1 << 3,
            MAGIC_SHIELD            = 1 << 4,
            SLOWED                  = 1 << 5,
            HASTE                   = 1 << 6,
            LOGOUT_BLOCK            = 1 << 7,
            DROWNING                = 1 << 8,
            FREEZING                = 1 << 9,
            DAZZLED                 = 1 << 10,
            CURSED                  = 1 << 11,
            STRENGTHENED            = 1 << 12,
            PROTECTION_ZONE_BLOCK   = 1 << 13,
            PROTECTION_ZONE         = 1 << 14,
            BLEEDING                = 1 << 15,
            HUNGRY                  = 1 << 17,
            GUILD_WAR               = 1 << 18,
            SKULL_BLACK             = 1 << 19,
            SKULL_GREEN             = 1 << 20,
            SKULL_ORANGE            = 1 << 21,
            SKULL_RED               = 1 << 22,
            SKULL_WHITE             = 1 << 23,
            SKULL_YELLOW            = 1 << 24;
    }

    namespace VipGroup
    {
        constexpr uint8_t
            NONE                = 0,
            ENEMIES             = 1 << 0,
            FRIENDS             = 1 << 1,
            TRADING_PARTNERS    = 1 << 2;
    }

    struct GuiElement
    {
        GuiElement(){}
        GuiElement(const GuiDraw& draw) : draw(&draw){}
        const GuiDraw* draw = nullptr;
    };

    struct DrawRect
    {
        IRect screen;
        IRect local;
    };

    struct Button : public GuiElement
    {
        Button(){}
        Button(const GuiDraw& d, bool isDown, bool isEnabled = true)
        : GuiElement(d), isDown(isDown), isEnabled(isEnabled){}
        bool isDown;
        bool isEnabled;
    };


    struct MinMaxButton : public Button
    {
        MinMaxButton(){}
        MinMaxButton(const GuiDraw& d, bool isMin, bool isDown, bool isEnabled = true)
        : Button(d, isDown, isEnabled), isMin(isMin){}
        bool isMin;
    };

    struct UniqueButton : public Button
    {
        enum class Type : unsigned char
        {
            MINI_MAP_UP,
            MINI_MAP_DOWN,
            MINI_MAP_ZOOM_IN,
            MINI_MAP_ZOOM_OUT,

            INVENTORY_STORE_INBOX,

            COMBAT_EXPERT_DOVE,
            COMBAT_EXPERT_RED,
            COMBAT_EXPERT_WHITE,
            COMBAT_EXPERT_YELLOW,
            COMBAT_PVP,
            COMBAT_EXPERT,
            COMBAT_FOLLOW,
            COMBAT_STAND,
            COMBAT_DEFENSIVE,
            COMBAT_BALANCED,
            COMBAT_OFFENSIVE,

            CONTROL_MIN_MAX,
            CONTROL_LOGOUT,
            CONTROL_OPTIONS,
            CONTROL_UNJUSTIFIED_POINTS,
            CONTROL_QUEST_LOG,
            CONTROL_VIP,
            CONTROL_BATTLE,
            CONTROL_SKILLS,
            CONTROL_PREY,
            DEFAULT_SIDE_BAR_STORE,

            SKILLS_XP_BOOST,

            BATTLE_MONSTER,
            BATTLE_NPC,
            BATTLE_PARTY,
            BATTLE_PLAYER,
            BATTLE_SKULL,
            BATTLE_FILTER_MIN_MAX,
            BATTLE_SORT,

            CHAT_IGNORE,
            CHAT_NEW_TAB,
            CHAT_SERVER_MESSAGES,
            CHAT_CLOSE_TAB,
            CHAT_TAB_LEFT,
            CHAT_TAB_RIGHT,
            CHAT_SAY,
            CHAT_WHISPER,
            CHAT_YELL,

            SPELL_BAR_ATTACK,
            SPELL_BAR_HEALING,
            SPELL_BAR_SUPPORT,
            SPELL_BAR_SPECIAL,

            SIDE_BAR_ADD,
            SIDE_BAR_REMOVE,

            TRANSFER_COINS,
            CLIP_BOARD,

            NUM_TYPES,
            INVALID,
        };

        UniqueButton(){}
        UniqueButton(const GuiDraw& d, Type type, bool isDown, bool isEnabled = true)
        : Button(d, isDown, isEnabled), type(type){}

        Type type = Type::INVALID;
    };

    class GuiParser
    {
        public:
            struct CreatureFlag : public GuiElement
            {
                enum class Type : unsigned char
                {
                    DIRECTIONS,
                    QUEST,
                    TALK,
                    TRADE,
                    GUILD_WAR_BLUE,
                    GUILD_WAR_GREEN,
                    GUILD_WAR_RED,
                    GUILD_WHITE,
                    GUILD_YELLOW,
                    LIGHTNING_RED,
                    PARTY_GRAY,
                    PARTY_INVITEE,
                    PARTY_INVITER,
                    PARTY_LEADER,
                    PARTY_LEADER_SHARED_XP,
                    PARTY_LEADER_SHARED_XP_FAIL,
                    PARTY_MEMBER,
                    PARTY_MEMBER_SHARED_XP,
                    PARTY_MEMBER_SHARED_XP_FAIL,
                    SKULL_BLACK,
                    SKULL_GREEN,
                    SKULL_ORANGE,
                    SKULL_RED,
                    SKULL_WHITE,
                    SKULL_YELLOW,
                    SUMMON_GREEN,
                    SUMMON_RED,
                    NUM_TYPES,
                };

                Type type;
            };

            struct MiniMapMarker : public GuiElement
            {
                enum class Type : unsigned char
                {
                    CHECKMARK,
                    QUESTIONMARK,
                    EXCLAMATIONMARK,
                    STAR,
                    CROSSMARK,
                    CROSS,
                    MOUTH,
                    BRUSH,
                    SWORD,
                    FLAG,
                    LOCK,
                    BAG,
                    SKULL,
                    DOLLAR,
                    RED_UP,
                    RED_DOWN,
                    RED_RIGHT,
                    RED_LEFT,
                    GREEN_UP,
                    GREEN_DOWN,
                    NUM_TYPES,
                };
                Type type;
            };

            struct EmptyEquipmentSlot : public GuiElement
            {
                enum class Type : unsigned char
                {
                    BACK,
                    FEET,
                    FINGER,
                    HEAD,
                    HIP,
                    LEFT_HAND,
                    RIGHT_HAND,
                    LEGS,
                    NECK,
                    TORSO,
                    NUM_TYPES,
                };

                Type type;
            };

            struct DefaultSideBar
            {
                struct MiniMap
                {
                    GuiElement rose;
                    GuiElement map;
                    GuiElement crosshair;

                    std::vector<MiniMapMarker> markers;
                };

                struct StatusBar
                {
                    float hpPercent = 0.f;
                    float manaPercent = 0.f;
                };

                struct Inventory
                {
                    std::vector<EmptyEquipmentSlot> emptySlots;
                };

                struct PremiumFeatures
                {
                    bool isMinimized;
                };

                MiniMap miniMap;
                StatusBar statusBar;
                Inventory inventory;
                PremiumFeatures premiumFeatures;

                uint32_t conditions;
                bool hasAdventurersBlessing;
            };

            struct Vip
            {
                enum class Mark : unsigned char
                {
                    NONE,
                    HEART,
                    SKULL,
                    LIGHTNING,
                    CROSSHAIR,
                    STAR,
                    YINYANG,
                    TRIANGLE,
                    CROSS,
                    DOLLAR,
                    BALKENKREUZ,
                    NUM_MARKS,
                };

                std::vector<Mark> marks;
            };

            struct Prey
            {
                struct Bonus : public GuiElement
                {
                    enum class Type : unsigned char
                    {
                        DAMAGE_BOOST,
                        DAMAGE_REDUCTION,
                        IMPROVED_LOOT,
                        IMPROVED_XP,
                        NONE,
                    };

                    Type type;
                };

                std::vector<Bonus> bonuses;
            };

            struct SideBarWindow
            {
                DrawRect titleBar;
                DrawRect clientArea;

                bool isMinimized() const {return clientArea.local.height == 4;}
            };

            struct Container
            {
                std::vector<GuiElement> slots;
            };

            struct NpcTrade
            {
                // TODO
            };

            struct SideBarWindows
            {
                std::vector<Container> containers;
                std::vector<SideBarWindow> windows;
                Vip vip;
                Prey prey;
                NpcTrade npcTrade;

                bool isWindowBeingMoved = false;
            };

            struct GameWindow
            {
                std::vector<CreatureFlag> creatureFlags;
            };

            struct SpellBar
            {
                Button attack;
                Button healing;
                Button support;
                Button special;
            };

            struct GameData
            {
                std::vector<Button> textButtons;
                std::vector<Button> texturedButtons;
                std::vector<Button> tabs;
                std::vector<MinMaxButton> minMaxButtons;
                std::vector<Button> exitButtons;
                std::array<UniqueButton, (size_t)UniqueButton::Type::NUM_TYPES> uniqueButtons;

                DefaultSideBar defaultSideBar;
                SideBarWindows sideBarWindows;
                GameWindow gameWindow;
                SpellBar spellBar;

            };

            struct MiscData
            {
                Button transferCoins;
                Button clipBoard;
            };

            struct Data
            {
                GameData game;
                MiscData misc;
            };

        public:
            explicit GuiParser();

            void parse(const Frame& frame);

            const Data& getData();

        private:
            void parsePass1();
            std::map<std::string, std::function<void(size_t&)>> initGuiDrawHandlers();
            IRect getRect(Vertex topLeft, Vertex botRight);
            IRect getScreenRect(const Draw& d);
            DrawRect getDrawRect(const Draw& d);
        private:
            Data mData;
            std::vector<std::string> mBaseNames;
            std::shared_ptr<std::vector<GuiDraw>> mDraws;
            float mHalfFrameWidth = 0.f;
            float mHalfFrameHeight = 0.f;

            const std::map<std::string, std::function<void(size_t&)>> mGuiDrawHandlers;

            struct Pass1
            {
                std::vector<GuiElement> hpManaBorders;
                GuiElement hpFill;
                GuiElement manaFill;
            };

            Pass1 pass1;
    };
}


#endif // GRAPHICS_LAYER_GUI_PARSER_HPP
