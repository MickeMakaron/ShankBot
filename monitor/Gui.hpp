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

            struct Data
            {
                State state = State::UNDEFINED;
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


        private:
            const TibiaContext& mContext;
            std::shared_ptr<Data> mData = std::make_shared<Data>();
            Frame mCurrentFrame;
            bool mIsCurrentFrameParsed = false;
            float mHalfFrameWidth = 0.f;
            float mHalfFrameHeight = 0.f;

            bool mIsEquipmentMinimized = false;
    };
}


#endif // GRAPHICS_LAYER_GUI_HPP
