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
#ifndef GRAPHICS_LAYER_GUI_HPP
#define GRAPHICS_LAYER_GUI_HPP

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
    namespace PlayerStateMERP
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

    namespace VipGroupMERP
    {
        constexpr uint8_t
            NONE                = 0,
            ENEMIES             = 1 << 0,
            FRIENDS             = 1 << 1,
            TRADING_PARTNERS    = 1 << 2;
    }

    class Gui
    {
        public:
            enum class State : unsigned char
            {
                MAIN_MENU,
                GAME,
                UNDEFINED,
            };

            struct Item
            {
                const sb::tibiaassets::Object* item;
                unsigned short stackSize;
            };

            struct Button
            {
                bool isDown = false;
                std::string text = "";
                unsigned short x;
                unsigned short y;
                unsigned short width;
                unsigned short height;
            };


            enum class UiSection : unsigned char
            {
                EQUIPMENT_AND_COMBAT_OPTIONS,
                MINI_MAP,
                MANA_AND_HP,
                PREMIUM_FEATURES,
                CONTROL_BUTTONS,
                CHAT,
                SPELL_GROUPS,
                SPELLS,
                GAME_WINDOW,
                SKILLS_WINDOW,
                BATTLE_WINDOW,
                VIP_WINDOW,
                QUEST_LOG,
                UNJUSTIFIED_POINTS_WINDOW,
                OPTIONS,
                LEAVE_GAME_WINDOW,
                TRADE_WINDOW,
                CONTAINER_WINDOW,
            };


            enum class EqType : unsigned char
            {
                HAND1,
                HAND2,
                NECK,
                FINGER,
                HEAD,
                TORSO,
                LEGS,
                FEET,
                BACK,
                HIP,
                NUM_TYPES,
            };

            struct Rect
            {
                Vertex pos;
                Vertex size;
            };

            struct SideBottomWindow
            {
                enum class Type : unsigned char
                {
                    BATTLE,
                    CONTAINER,
                    NPC_TRADE,
                    UNJUSTIFIED_POINTS,
                    SKILLS,
                    VIP,
                    INVALID,
                };

                Type type = Type::INVALID;

                bool isMinimized = false;
                bool hasResizer = false;
                Rect exitButton;
                Rect minMaxButton;
                Rect resizer;

                Rect clientArea;

                union
                {
                    Rect containerUpButton; // Container
                    Rect expandButton; // Battle window
                };
            };


            struct Container
            {
                std::string name;
                unsigned char capacity = 0;
                std::vector<std::pair<unsigned short, std::set<size_t>>> items; // Count/item

                std::vector<std::pair<size_t, Rect>> getVisibleItems() const;

                float scroll = 0;
                std::shared_ptr<SideBottomWindow> window;
            };

            struct NpcTradeWindow
            {
                struct Offer
                {
                    std::string name;
                    std::set<size_t> objects;
                    unsigned int cost = 0;
                    float weight = 0;
                    bool isAffordable = false;
                };

                std::vector<Offer> visibleOffers;
                size_t selectedOfferIndex = -1;
                unsigned int amount = 0;
                unsigned int totalPrice = 0;
                unsigned int availableMoney = 0;

                std::shared_ptr<SideBottomWindow> window;

                enum class Tab : unsigned char
                {
                    BUY,
                    SELL,
                    INVALID,
                };
                std::shared_ptr<Button> buyButton;
                std::shared_ptr<Button> sellButton;
                std::shared_ptr<Button> okButton;

                Tab currentTab = Tab::INVALID;

            };

            struct BattleWindow
            {
                struct Outfit
                {
                    std::string name;
                    float hpPercent;
                    std::set<size_t> objects;
                };

                std::vector<std::shared_ptr<Outfit>> outfits;
                std::shared_ptr<Outfit> selectedOutfit;
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

            struct SideTopButtons
            {
                TmpButton premFeaturesMinMax;

                TmpButton menu;
                TmpButton store;
                TmpButton logout;
                TmpButton options;
                TmpButton unjustPoints;
                TmpButton questLog;
                TmpButton vipList;
                TmpButton battleList;
                TmpButton skills;
                TmpButton prey;

                TmpButton storeInbox;
                TmpButton pvp;
                TmpButton expert;
                TmpButton follow;
                TmpButton stand;
                TmpButton defensive;
                TmpButton balanced;
                TmpButton offensive;
                TmpButton eqMinMax;
                TmpButton stop;

                TmpButton moveDown;
                TmpButton zoomIn;
                TmpButton moveUp;
                TmpButton zoomOut;
                TmpButton rose;
                TmpButton centre;
            };

            struct ExpertButtons
            {
                TmpButton dove;
                TmpButton red;
                TmpButton white;
                TmpButton yellow;

            };

            struct ChatButtons
            {
                TextButton toggleChat;
                TmpButton ignore;
                TmpButton newTab;
                TmpButton messages;
                TmpButton closeTab;
                TmpButton tabLeft;
                TmpButton tabRight;

                TmpButton say;
                TmpButton whisper;
                TmpButton yell;


                std::vector<TextButton> tabs;
                std::vector<TextButton> clickableText;
            };

            struct BattleListButtons
            {
                TmpButton monster;
                TmpButton npc;
                TmpButton party;
                TmpButton player;
                TmpButton skull;
            };

            struct MiscButtons
            {
                TmpButton transferCoins;
                TmpButton storeExp;
                TmpButton addSideBar;
                TmpButton removeSideBar;
                TmpButton clipBoard;
                TmpButton contextMenu;
            };

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
                    TmpButton centre;
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
                    std::vector<size_t> back;
                    std::vector<size_t> feet;
                    std::vector<size_t> finger;
                    std::vector<size_t> head;
                    std::vector<size_t> hip;
                    std::vector<size_t> leftHand;
                    std::vector<size_t> rightHand;
                    std::vector<size_t> legs;
                    std::vector<size_t> neck;
                    std::vector<size_t> torso;
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
                    TmpButton stop;

                    std::shared_ptr<ExpertControls> expertControls;
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
                std::shared_ptr<Inventory> inventory;
                CombatControls combatControls;
                std::shared_ptr<ControlButtons> controlButtons;

                TmpButton store;
                TmpButton inventoryMinMax;
                TmpButton controlButtonsMinMax;
                unsigned short soul;
                unsigned short cap;
                uint32_t conditions;
                bool hasAdventurersBlessing;
            };

            struct SideBarWindow
            {
                std::string name;
                TmpButton exit;
                TmpButton minMax;
                DrawRect clientArea;
                DrawRect titleBar;
                bool isMinimized;
                bool isResizable;
            };

            struct Skills : public SideBarWindow
            {
                unsigned short level = 0;
                unsigned int experience = 0;
                unsigned short xpGainRate = 0;
                float levelPercent = 0.f;

                TmpButton xpBoost;

                unsigned short hp = 0;
                unsigned short mana = 0;
                unsigned short soul = 0;
                unsigned short cap = 0;
                unsigned short speed = 0;
                unsigned short foodMinutes = 0;
                unsigned short staminaMinutes = 0;
                unsigned short offlineTrainingMinutes = 0;
                unsigned short magicLevel = 0;
                float staminaPercent = 0.f;
                float offlineTrainingPercent = 0.f;
                float magicLevelPercent = 0.f;


                unsigned short fistLevel = 0;
                unsigned short clubLevel = 0;
                unsigned short swordLevel = 0;
                unsigned short axeLevel = 0;
                unsigned short distanceLevel = 0;
                unsigned short shieldingLevel = 0;
                unsigned short fishingLevel = 0;
                float fistLevelPercent = 0.f;
                float clubLevelPercent = 0.f;
                float swordLevelPercent = 0.f;
                float axeLevelPercent = 0.f;
                float distanceLevelPercent = 0.f;
                float shieldingLevelPercent = 0.f;
                float fishingLevelPercent = 0.f;

                unsigned short critChance = 0;
                unsigned short critDamage = 0;

                unsigned short hpLeechChance = 0;
                unsigned short hpLeechAmount = 0;

                unsigned short manaLeechChance = 0;
                unsigned short manaLeechAmount = 0;

            };

            struct BattleList : public SideBarWindow
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

                struct Character
                {
                    std::string name;
                    std::vector<size_t> objectIds;
                    float hpPercent;
                };

                std::vector<Character> characters;
            };

            struct Vip : public SideBarWindow
            {

                struct Player
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

                    std::string name;
                    bool isOnline;
                    uint8_t groups;
                    Mark mark;
                };

                std::vector<Player> players;
            };

            struct UnjustifiedPoints : public SideBarWindow
            {
                // Todo...
            };

            struct Prey : public SideBarWindow
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

            struct BattleListFilters
            {
                TmpButton monster;
                TmpButton npc;
                TmpButton party;
                TmpButton player;
                TmpButton skull;
            };


            struct SideBarWindows
            {
                std::vector<TmpButton> minMaxButtons;
                std::vector<TmpButton> exitButtons;
                std::vector<TmpButton> contextMenuButtons;
                std::vector<TmpButton> expandButtons;
                std::shared_ptr<BattleListFilters> battleListFilters;
                std::vector<ScrollBar> scrollBars;
                std::vector<DrawRect> resizers;
                std::vector<DrawRect> widgets;
            };

            struct GameWindow
            {
                std::vector<CreatureFlag> creatureFlags;
            };

            struct ChatWindow
            {
                std::vector<TextButton> tabs;
                TextButton toggleChat;
                TmpButton ignore;
                TmpButton newTab;
                std::shared_ptr<TmpButton> messages;
                std::shared_ptr<TmpButton> closeTab;
                std::shared_ptr<TmpButton> tabLeft;
                std::shared_ptr<TmpButton> tabRight;

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
                DefaultSideBar defaultSideBar;
                SideBarWindows sideBarWindows;
                GameWindow gameWindow;
                ChatWindow chatWindow;
                SpellBar spellBar;


            };



            struct Data
            {
                SideTopButtons sideTopButtons;
                ChatButtons chatButtons;
                BattleListButtons battleListButtons;
                ExpertButtons expertButtons;
                MiscButtons miscButtons;
                DrawRect miniMapRose;
                DrawRect miniMapCrosshair;
                DrawRect adventurersBlessing;
                State state = State::UNDEFINED;
                uint32_t playerStates;
                std::vector<std::shared_ptr<Button>> buttons;
                unsigned short cap = 0;
                unsigned short soul = 0;
                unsigned short mana = 0;
                unsigned short hp = 0;
                float hpLevel = 0.f;
                float manaLevel = 0.f;

                unsigned short level = 0;
                unsigned int experience = 0;
                unsigned short xpGainRate = 0;
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

                std::map<EqType, size_t> equipment;
                std::vector<Container> containers;

                std::vector<std::shared_ptr<SideBottomWindow>> sideBottomWindows;
                std::shared_ptr<NpcTradeWindow> npcTradeWindow;
                std::shared_ptr<BattleWindow> battleWindow;
                std::vector<std::string> onlineVips;
                std::vector<std::string> offlineVips;

                std::string chatInput;

            };

        public:
            explicit Gui(const TibiaContext& context);

            void update(const Frame& frame);

            const Data& getData();
            State getState() const;

        private:
            std::map<std::string, std::function<void(const GuiDraw&)>> initGuiDrawHandlers();

            void parseCurrentFrame();

            void updateSideBottomWindows(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);

            void createButtons
            (
                const std::map<std::string, std::list<const GuiDraw*>>& guiDraws,
                const std::vector<std::string>& names,
                bool isDown
            );
            void updateButtons(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);
            std::shared_ptr<Button> setButtonText(const Text& text);
            void updateManaAndHpLevels(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);

            void parseSkillWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w);
            unsigned short parseTimeText(const std::string& str) const;
            unsigned short parsePercentageText(const std::string& str) const;
            void parseBattleWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w);
            void parseVipWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w);
            void parseNpcTradeWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w);
            void parseDialogFramesText(size_t& i);


            void getRects(const std::string& name, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, std::list<Rect>& rects) const;

            void parseNames(size_t& i);
            void parseChat(size_t& i);
            void parseSideWindowTopText(size_t& i);
            void parseSideWindowBottomText(size_t& i);
            void parseContainerSlots(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);
            void parseContainerText(size_t& i, Container& c, const std::string& str, const std::shared_ptr<SideBottomWindow>& w);
            static size_t containerSlotLocalCoordsToIndex(float startX, float startY, float x, float y);

            void parseEquippedItems(size_t& i);
            void parseBattleWindowSprites(size_t& i, const std::shared_ptr<SideBottomWindow>& w);
            void parseContainerSprites(size_t& i, Container& c);
            void parseNpcTradeWindowSprites(size_t& i);
            void parseGuiSpriteDraws();

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

            bool mIsEquipmentMinimized = false;

            const std::map<std::string, std::function<void(const GuiDraw&)>> mGuiDrawHandlers;

            struct Pass1
            {
                std::vector<TmpButton> smallMinButtons;
                std::vector<TmpButton> smallMaxButtons;
                std::vector<TmpButton> smallExitButtons;
                std::vector<TmpButton> expandButtons;
                std::vector<TmpButton> scrollDownButtons;
                std::vector<TmpButton> scrollUpButtons;
                std::vector<TmpButton> scrollRightButtons;
                std::vector<TmpButton> scrollLeftButtons;
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

                std::vector<TmpButton> chatTabs;

                std::vector<DrawRect> containerSlots;

                std::vector<DrawRect> hpManaBorders;
                DrawRect hpFill;
                DrawRect manaFill;

                std::vector<MiniMapMarker> miniMapMarkers;
                std::vector<EmptyEquipmentSlot> emptyEquipmentSlots;

                std::vector<DrawRect> spellGroups;
            };

            Pass1 pass1;
    };
}


#endif // GRAPHICS_LAYER_GUI_HPP
