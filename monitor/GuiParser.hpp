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
#include "Frame.hpp"
#include "Text.hpp"
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
    };

    namespace VipGroup
    {
        constexpr uint8_t
            NONE                = 0,
            ENEMIES             = 1 << 0,
            FRIENDS             = 1 << 1,
            TRADING_PARTNERS    = 1 << 2;
    }
    struct Rect
    {
        Vertex pos;
        Vertex size;
    };

    struct GuiElement
    {
        GuiElement(){};
        GuiElement(const GuiDraw& draw) : draw(&draw){}
        const GuiDraw* draw = nullptr;
    };


    struct IRect
    {
        unsigned short x;
        unsigned short y;
        unsigned short width;
        unsigned short height;
    };

    struct DrawRect
    {
        IRect screen;
        IRect local;
    };

    struct Button : public GuiElement
    {
        Button(){};
        Button(const GuiDraw& d, bool isDown, bool isEnabled = true)
        : GuiElement(d), isDown(isDown), isEnabled(isEnabled){}
        bool isDown;
        bool isEnabled;
    };

    struct MinMaxButton : public Button
    {
        MinMaxButton(){};
        MinMaxButton(const GuiDraw& d, bool isMin, bool isDown, bool isEnabled = true)
        : Button(d, isDown, isEnabled), isMin(isMin){}
        bool isMin;
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
                    Button up;
                    Button down;
                    Button zoomIn;
                    Button zoomOut;
                    GuiElement rose;
                    GuiElement map;
                    GuiElement crosshair;

                    std::vector<MiniMapMarker> markers;
                };

                struct StatusBar
                {
                    unsigned short hp;
                    unsigned short mana;
                    float hpPercent;
                    float manaPercent;
                };

                struct Inventory
                {
                    Button storeInbox;
                    std::vector<EmptyEquipmentSlot> emptySlots;
                };

                struct CombatControls
                {
                    struct ExpertControls
                    {
                        Button dove;
                        Button red;
                        Button white;
                        Button yellow;
                    };

                    Button pvp;
                    Button expert;
                    Button follow;
                    Button stand;
                    Button defensive;
                    Button balanced;
                    Button offensive;

                    ExpertControls expertControls;
                };

                struct ControlButtons
                {
                    Button menu;
                    Button logout;
                    Button options;
                    Button unjustPoints;
                    Button questLog;
                    Button vipList;
                    Button battleList;
                    Button skills;
                    Button prey;
                };

                struct PremiumFeatures
                {
                    bool isMinimized;
                };

                MiniMap miniMap;
                StatusBar statusBar;
                Inventory inventory;
                CombatControls combatControls;
                ControlButtons controlButtons;
                PremiumFeatures premiumFeatures;

                Button store;
                Button controlButtonsMinMax;
                uint32_t conditions;
                bool hasAdventurersBlessing;
            };


            struct Skills
            {
                Button xpBoost;
            };

            struct BattleList
            {
                struct Filter
                {
                    Button monster;
                    Button npc;
                    Button party;
                    Button player;
                    Button skull;
                };

                Button filterMinMax;
                Filter filter;
                Button sort;
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

            struct UnjustifiedPoints
            {
                // Todo...
            };

            struct Prey
            {
                // Todo...
            };

            struct SideBarWindow
            {
                DrawRect titleBar;
                DrawRect clientArea;
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
                Skills skills;
                BattleList battleList;
                Vip vip;
                UnjustifiedPoints unjustifiedPoints;
                Prey prey;
                NpcTrade npcTrade;
            };

            struct GameWindow
            {
                std::vector<CreatureFlag> creatureFlags;
            };

            struct ChatWindow
            {
                Button ignore;
                Button newTab;

                Button messages;
                Button closeTab;
                Button tabLeft;
                Button tabRight;

                enum class Volume : unsigned char
                {
                    SAY,
                    WHISPER,
                    YELL,
                    NUM_VOLUMES,
                };

                Volume currentVolume;
                Button volume;
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

                DefaultSideBar defaultSideBar;
                SideBarWindows sideBarWindows;
                GameWindow gameWindow;
                ChatWindow chatWindow;
                SpellBar spellBar;

                Button addSideBar;
                Button removeSideBar;
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
