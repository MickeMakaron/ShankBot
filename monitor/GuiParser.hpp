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

    struct TmpButton
    {
        IRect screenRect;
        IRect localRect;
        bool isDown;
        bool isEnabled;
    };

    struct TextButton : public TmpButton
    {
        TextButton(){};
        TextButton(const TmpButton& b) : TmpButton(b){}
        TextButton& operator=(const TmpButton& b){TmpButton::operator=(b);}
        std::string text;
    };

    class GuiParser
    {
        public:
            struct CreatureFlag
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

                DrawRect rect;
                Type type;
            };

            struct MiniMapMarker
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

                DrawRect rect;
                Type type;
            };

            struct EmptyEquipmentSlot
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

                DrawRect rect;
                Type type;
            };

            struct DefaultSideBar
            {
                struct MiniMap
                {
                    TmpButton up;
                    TmpButton down;
                    TmpButton zoomIn;
                    TmpButton zoomOut;
                    DrawRect rose;
                    DrawRect map;
                    DrawRect crosshair;

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
                    TmpButton storeInbox;
                    std::vector<EmptyEquipmentSlot> emptySlots;
                };

                struct CombatControls
                {
                    struct ExpertControls
                    {
                        TmpButton dove;
                        TmpButton red;
                        TmpButton white;
                        TmpButton yellow;
                    };

                    TmpButton pvp;
                    TmpButton expert;
                    TmpButton follow;
                    TmpButton stand;
                    TmpButton defensive;
                    TmpButton balanced;
                    TmpButton offensive;

                    ExpertControls expertControls;
                };

                struct ControlButtons
                {
                    TmpButton menu;
                    TmpButton logout;
                    TmpButton options;
                    TmpButton unjustPoints;
                    TmpButton questLog;
                    TmpButton vipList;
                    TmpButton battleList;
                    TmpButton skills;
                    TmpButton prey;
                };

                struct PremiumFeatures
                {
                    TmpButton minMax;
                    bool isMinimized;
                };

                MiniMap miniMap;
                StatusBar statusBar;
                Inventory inventory;
                CombatControls combatControls;
                ControlButtons controlButtons;
                PremiumFeatures premiumFeatures;

                TmpButton store;
                TmpButton inventoryMinMax;
                TmpButton controlButtonsMinMax;
                uint32_t conditions;
                bool hasAdventurersBlessing;
                bool isInventoryMinimized;
            };


            struct Skills
            {
                TmpButton xpBoost;
            };

            struct BattleList
            {
                struct Filter
                {
                    TmpButton monster;
                    TmpButton npc;
                    TmpButton party;
                    TmpButton player;
                    TmpButton skull;
                };

                TmpButton filterMinMax;
                Filter filter;
                TmpButton sort;
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

            struct ScrollBar
            {
                bool isHorizontal;
                TmpButton min;
                TmpButton max;
                DrawRect handle;
            };

            struct SideBarWindows
            {
                std::vector<TmpButton> minButtons;
                std::vector<TmpButton> maxButtons;
                std::vector<TmpButton> exitButtons;
                std::vector<DrawRect> resizers;
                std::vector<DrawRect> widgets;

                Skills skills;
                BattleList battleList;
                Vip vip;
                UnjustifiedPoints unjustifiedPoints;
                Prey prey;
            };

            struct GameWindow
            {
                std::vector<CreatureFlag> creatureFlags;
            };

            struct ChatWindow
            {
                std::vector<TmpButton> tabs;
                TextButton toggleChat;
                TmpButton ignore;
                TmpButton newTab;

                TmpButton messages;
                TmpButton closeTab;
                TmpButton tabLeft;
                TmpButton tabRight;

                enum class Volume : unsigned char
                {
                    SAY,
                    WHISPER,
                    YELL,
                    NUM_VOLUMES,
                };

                Volume currentVolume;
                TmpButton volume;
            };

            struct SpellBar
            {
                TmpButton attack;
                TmpButton healing;
                TmpButton support;
                TmpButton special;

            };

            struct GameData
            {
                std::vector<TextButton> textButtons;
                std::vector<ScrollBar> scrollBars;

                DefaultSideBar defaultSideBar;
                SideBarWindows sideBarWindows;
                GameWindow gameWindow;
                ChatWindow chatWindow;
                SpellBar spellBar;

                TmpButton addSideBar;
                TmpButton removeSideBar;
            };

            struct MiscData
            {
                TmpButton transferCoins;
                TmpButton clipBoard;
            };

            struct Data
            {
                GameData game;
                MiscData misc;
            };

        public:
            explicit GuiParser(const TibiaContext& context);

            void update(const Frame& frame);

            const Data& getData();

        private:
            void parseCurrentFrame();
            void runPass1();
            void assembleScrollBars)=:
            DrawRect merge(const DrawRect& r1, const DrawRect& r2) const;
            IRect merge(const IRect& r1, const IRect& r2) const;
            std::map<std::string, std::function<void(const GuiDraw&)>> initGuiDrawHandlers();
            IRect getRect(Vertex topLeft, Vertex botRight);
            IRect getScreenRect(const Draw& d);
            DrawRect getDrawRect(const Draw& d);
            TmpButton createButton(const Draw& d, bool isDown, bool isEnabled = true);

        private:
            const TibiaContext& mContext;
            Data mData;
            Frame mCurrentFrame;
            bool mIsCurrentFrameParsed = false;
            float mHalfFrameWidth = 0.f;
            float mHalfFrameHeight = 0.f;

            const std::map<std::string, std::function<void(const GuiDraw&)>> mGuiDrawHandlers;

            struct Pass1
            {
                std::vector<TmpButton> smallMinButtons;
                std::vector<TmpButton> smallMaxButtons;
                std::vector<TmpButton> smallExitButtons;
                std::vector<TmpButton> scrollDownButtons;
                std::vector<TmpButton> scrollUpButtons;
                std::vector<TmpButton> scrollRightButtons;
                std::vector<TmpButton> scrollLeftButtons;
                std::vector<DrawRect> scrollVerticalHandles;
                std::vector<DrawRect> scrollHorizontalHandles;
                std::vector<TmpButton> containerUpButtons;
                std::vector<TmpButton> containerLeftButtons;
                std::vector<TmpButton> containerRightButtons;

                std::vector<TextButton> grid9Buttons;
                std::vector<TmpButton> texturedButtons;
                std::vector<TmpButton> blueButtons;
                std::vector<TmpButton> goldButtons;
                std::vector<TmpButton> greenButtons;
                std::vector<TmpButton> greyButtons;

                std::vector<TmpButton> radioButtons;
                std::vector<TmpButton> checkBoxes;

                std::vector<TmpButton> dropDownButtons;

                std::vector<TmpButton> outfitLeftButtons;
                std::vector<TmpButton> outfitRightButtons;
                std::vector<TmpButton> outfitStoreButtons;

                std::vector<DrawRect> containerSlots;

                std::vector<DrawRect> hpManaBorders;
                DrawRect hpFill;
                DrawRect manaFill;

                std::vector<DrawRect> spellGroups;
            };

            Pass1 pass1;
    };
}


#endif // GRAPHICS_LAYER_GUI_PARSER_HPP
