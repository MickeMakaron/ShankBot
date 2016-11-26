// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <map>
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
                const Object* item;
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
                std::vector<std::pair<unsigned short, std::set<const Object*>>> items; // Count/item

                std::vector<std::pair<size_t, Rect>> getVisibleItems() const;

                float scroll = 0;
                const SideBottomWindow* window = nullptr;
            };

            struct NpcTradeWindow
            {
                struct Offer
                {
                    std::string name;
                    std::set<const Object*> objects;
                    unsigned int cost = 0;
                    float weight = 0;
                    bool isAffordable = false;
                };

                std::vector<Offer> visibleOffers;
                size_t selectedOfferIndex = -1;
                unsigned int amount = 0;
                unsigned int totalPrice = 0;
                unsigned int availableMoney = 0;

                const SideBottomWindow* window = nullptr;

                enum class Tab : unsigned char
                {
                    BUY,
                    SELL,
                    INVALID,
                };
                const Button* buyButton;
                const Button* sellButton;
                const Button* okButton;

                Tab currentTab = Tab::INVALID;

            };

            struct BattleWindow
            {
                struct Outfit
                {
                    std::string name;
                    float hpPercent;
                    std::set<const Object*> objects;
                };

                std::list<Outfit> outfits;
                Outfit* selectedOutfit = nullptr;
            };


            void update(const Frame& frame);

            State getState() const;
            const std::list<Button>& getButtons();


            unsigned short getCap();
            unsigned short getSoul();
            unsigned short getMana();
            unsigned short getHp();
            float getHpLevel();
            float getManaLevel();

            unsigned short getLevel();
            unsigned int getExperience();
            unsigned short getXpGainRate();
            unsigned short getSpeed();
            unsigned short getFoodMinutes();
            unsigned short getStaminaMinutes();
            unsigned short getOfflineTrainingMinutes();
            unsigned short getMagicLevel();
            unsigned short getFistFighting();
            unsigned short getClubFighting();
            unsigned short getSwordFighting();
            unsigned short getAxeFighting();
            unsigned short getDistanceFighting();
            unsigned short getShielding();
            unsigned short getFishing();
            unsigned short getCritChance();
            unsigned short getCritDamage();
            unsigned short getHpLeechChance();
            unsigned short getHpLeechAmount();
            unsigned short getManaLeechChance();
            unsigned short getManaLeechAmount();

            const std::map<EqType, const Object*>& getEquipment();
            const std::list<Container>& getContainers();

        private:
            void parseCurrentFrame();

            void updateSideBottomWindows(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);
            void updateButtons(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);
            const Button* setButtonText(const Text& text);
//            void updateManaAndHpLevels(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);
//            void updateItems(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, const std::vector<SpriteDraw>& guiSpriteDraws);
//            void updateItems(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, const std::list<Text>& stackCounts);
//            void updateEquippedItems(size_t& i, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, const std::vector<SpriteDraw>& guiSpriteDraws);
//            void updateContainerItems(size_t& i, const std::vector<SpriteDraw>& guiSpriteDraws);
//            void parseContainer(size_t& i, SideBottomWindow& w, const std::vector<SpriteDraw>& guiSpriteDraws);
//            void parseBattleWindowItems(size_t& i, SideBottomWindow& w, const std::vector<SpriteDraw>& guiSpriteDraws);
//            void parseNpcTradeWindowItems(size_t& i, SideBottomWindow& w, const std::vector<SpriteDraw>& guiSpriteDraws);
//            void updateSideBottomWindowItems(size_t& i, const std::vector<SpriteDraw>& guiSpriteDraws);

//            void parseText(const std::vector<TextDraw>& textDraws);
//            void parseOutfitNames(size_t& i, const std::vector<TextDraw>& textDraws);
//            void parseChatText(size_t& i, const std::vector<TextDraw>& textDraws);
//            void parseSideWindowText(size_t& i, const std::vector<TextDraw>& textDraws);
//            void parseSideWindowTopText(size_t& i, const std::vector<TextDraw>& textDraws);
            void parseSkillWindowText(size_t& i, const SideBottomWindow& w);
            unsigned short parseTimeText(const std::string& str) const;
            unsigned short parsePercentageText(const std::string& str) const;
            void parseBattleWindowText(size_t& i, const SideBottomWindow& w);
            void parseVipWindowText(size_t& i, const SideBottomWindow& w);
            void parseNpcTradeWindowText(size_t& i, const SideBottomWindow& w);
            void parseDialogFramesText(size_t& i);


            void getRects(const std::string& name, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, std::list<Rect>& rects) const;
//            SideBottomWindow::Type resolveWindowType(float halfFrameWidth, float halfFrameHeight, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, int exitButtonTopLeftY) const;

            void parseNames(size_t& i);
            void parseChat(size_t& i);
            void parseSideWindowTopText(size_t& i);
            void parseSideWindowBottomText(size_t& i);
            void parseContainerSlots(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws);
            void parseContainerText(size_t& i, Container& c, const std::string& str, const SideBottomWindow& w);
            static size_t containerSlotLocalCoordsToIndex(float startX, float startY, float x, float y);

            void parseEquippedItems(size_t& i);
            void parseBattleWindowSprites(size_t& i, const SideBottomWindow& w);
            void parseContainerSprites(size_t& i, Container& c);
            void parseNpcTradeWindowSprites(size_t& i);
            void parseGuiSpriteDraws();


        private:
            Frame mCurrentFrame;
            bool mIsCurrentFrameParsed = false;
            float mHalfFrameWidth = 0.f;
            float mHalfFrameHeight = 0.f;
            State mCurrentState = State::UNDEFINED;
            std::list<Button> mButtons;
//            std::map<UiSection, std::list<Rect>> mUiSections;
            std::map<EqType, const Object*> mEquipment;
            std::list<Container> mContainers;
            std::list<SideBottomWindow> mSideBottomWindows;
            std::unique_ptr<NpcTradeWindow> mNpcTradeWindow;
            std::unique_ptr<BattleWindow> mBattleWindow;
            std::list<std::string> mOnlineVip;
            std::list<std::string> mOfflineVip;
            unsigned short mCap;
            unsigned short mSoul;
            unsigned short mMana;
            unsigned short mHp;
            float mHpLevel;
            float mManaLevel;

            unsigned short mLevel = 0;
            unsigned int mExperience = 0;
            unsigned short mXpGainRate = 0;
            unsigned short mSpeed = 0;
            unsigned short mFoodMinutes = 0;
            unsigned short mStaminaMinutes = 0;
            unsigned short mOfflineTrainingMinutes = 0;
            unsigned short mMagicLevel = 0;
            unsigned short mFistFighting = 0;
            unsigned short mClubFighting = 0;
            unsigned short mSwordFighting = 0;
            unsigned short mAxeFighting = 0;
            unsigned short mDistanceFighting = 0;
            unsigned short mShielding = 0;
            unsigned short mFishing = 0;
            unsigned short mCritChance = 0;
            unsigned short mCritDamage = 0;
            unsigned short mHpLeechChance = 0;
            unsigned short mHpLeechAmount = 0;
            unsigned short mManaLeechChance = 0;
            unsigned short mManaLeechAmount = 0;

            bool mIsEquipmentMinimized = false;
    };
}


#endif // GRAPHICS_LAYER_GUI_HPP
