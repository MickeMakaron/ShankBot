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
///////////////////////////////////
// Internal ShankBot headers
#include "graphics/Gui.hpp"
#include "graphics/TextBuilder.hpp"
#include "utility/utility.hpp"
#include "graphics/Equipment.hpp"
#include "utility/file.hpp"
using namespace sb::tibiaassets;
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
#include <iostream>
#include <cassert>
///////////////////////////////////


void Gui::update(const Frame& frame)
{
    mCurrentFrame = frame;
    mCurrentState = frame.viewWidth > 0 ? State::GAME : State::MAIN_MENU;
    mIsCurrentFrameParsed = false;
}

std::vector<std::pair<size_t, Gui::Rect>> Gui::Container::getVisibleItems() const
{
    std::vector<std::pair<size_t, Gui::Rect>> visibleItems;

    static const int MIN_Y = 0;
    const int MAX_Y = window->clientArea.size.y - 1.f + 0.5f;

    const int START_Y = MIN_Y - int(scroll + 0.5f) + Constants::CONTAINER_FIRST_SLOT_LOCAL_Y;

    static const int DELTA_Y = Constants::TILE_PIXEL_HEIGHT + Constants::CONTAINER_SEPARATOR_PIXEL_WIDTH;
    size_t minVisibleRowIndex = 0;
    int y = START_Y + Constants::TILE_PIXEL_HEIGHT - 1;
    while(y < MIN_Y)
    {
        minVisibleRowIndex++;
        y += DELTA_Y;
    }

    y += Constants::CONTAINER_SEPARATOR_PIXEL_WIDTH + 1;
    size_t maxVisibleRowIndex = minVisibleRowIndex;
    while(y <= MAX_Y)
    {
        maxVisibleRowIndex++;
        y += DELTA_Y;
    }

    const int START_SCREEN_Y = int(window->clientArea.pos.y - scroll + 0.5f) + Constants::CONTAINER_FIRST_SLOT_LOCAL_Y;
    const int MIN_Y_SCREEN = int(window->clientArea.pos.y + 0.5f);
    const int MAX_Y_SCREEN = int(window->clientArea.pos.y + window->clientArea.size.y - 1.f + 0.5f);
    const int START_SCREEN_X = int(window->clientArea.pos.x + 0.5f) + Constants::CONTAINER_FIRST_SLOT_LOCAL_X;
    static const int DELTA_X = Constants::TILE_PIXEL_WIDTH + Constants::CONTAINER_SEPARATOR_PIXEL_WIDTH;
    for(size_t i = minVisibleRowIndex; i <= maxVisibleRowIndex; i++)
    {
        size_t itemsIndex = i * Constants::CONTAINER_WIDTH;
        for(size_t j = 0; j < 4; j++)
        {
            if(itemsIndex + j >= items.size())
                return visibleItems;

            visibleItems.emplace_back();
            auto& pair = visibleItems.back();
            pair.first = itemsIndex + j;

            pair.second.pos.x = START_SCREEN_X + j * DELTA_X;
            pair.second.size.x = Constants::TILE_PIXEL_WIDTH;

            int top = std::max(START_SCREEN_Y + int(i) * DELTA_Y, MIN_Y_SCREEN);
            int height = std::min(int(Constants::TILE_PIXEL_HEIGHT), MAX_Y_SCREEN - top + 1);
            assert(height >= 0);
            pair.second.pos.y = top;
            pair.second.size.y = height;
        }
    }

    return visibleItems;
}



void Gui::updateButtons(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws)
{
    auto it = guiDraws.find("button-9grid-idle.png");
    if(it != guiDraws.end())
    {
        for(const GuiDraw* guiDraw : it->second)
        {
            Button b;
            b.isDown = false;
            float x;
            float y;
            sb::utility::worldToScreenCoords
            (
                guiDraw->topLeft.x, guiDraw->topLeft.y,
                *guiDraw->transform,
                mHalfFrameWidth, mHalfFrameHeight,
                x, y
            );
            b.x = x;
            b.y = y;
            b.width = guiDraw->botRight.x - guiDraw->topLeft.x;
            b.height = guiDraw->botRight.y - guiDraw->topLeft.y;
            mButtons.push_back(b);
        }
    }

    it = guiDraws.find("button-9grid-pressed.png");
    if(it != guiDraws.end())
    {
        for(const GuiDraw* guiDraw : it->second)
        {
            Button b;
            b.isDown = true;
            float x;
            float y;
            sb::utility::worldToScreenCoords
            (
                guiDraw->topLeft.x, guiDraw->topLeft.y,
                *guiDraw->transform,
                mHalfFrameWidth, mHalfFrameHeight,
                x, y
            );
            b.x = x;
            b.y = y;
            b.width = guiDraw->botRight.x - guiDraw->topLeft.x;
            b.height = guiDraw->botRight.y - guiDraw->topLeft.y;
            mButtons.push_back(b);
        }
    }
}
//
//void Gui::updateManaAndHpLevels(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws)
//{
//    auto hpIt = guiDraws.find("hitpoints-bar-filled.png");
//    if(hpIt == guiDraws.end())
//        return;
//    assert(hpIt->second.size() == 1);
//
//    auto manaIt = guiDraws.find("mana-bar-filled.png");
//    if(manaIt == guiDraws.end())
//    {
//        std::cout << "Warning: manaIt == guiDraws.end(), " << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
//        return;
//    }
//    assert(manaIt->second.size() == 1);
//
//
//    auto bordersIt = guiDraws.find("hitpoints-manapoints-bar-border.png");
//    assert(bordersIt != guiDraws.end());
//    assert(bordersIt->second.size() == 2);
//
//    const GuiDraw& hpBar = *hpIt->second.front();
//    const GuiDraw& hpBorder = *bordersIt->second.front();
//
//    const GuiDraw& manaBar = *manaIt->second.front();
//    const GuiDraw& manaBorder = *bordersIt->second.back();
//
//    mHpLevel = (hpBar.botRight.x - hpBar.topLeft.x) / (hpBorder.botRight.x - hpBorder.topLeft.x);
//    mManaLevel = (manaBar.botRight.x - manaBar.topLeft.x) / (manaBorder.botRight.x - manaBorder.topLeft.x);
//}

const Gui::Button* Gui::setButtonText(const Text& text)
{
    for(Button& b : mButtons)
    {
        if(b.text == "")
        {
            if(b.x <= text.x && b.y <= text.y)
            {
                if(text.x - b.x < b.width && text.y - b.y < b.height)
                {
                    b.text = text.string;
                    return &b;
                }
            }
        }
    }

    return nullptr;
}
//
//void Gui::updateItems(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, const std::list<Text>& stackCounts)
//{
//    auto containerSlotsIt = guiDraws.find("containerslot.png");
//    if(containerSlotsIt == guiDraws.end())
//        return;
//
//    std::list<Vertex> containerSlots;
//    for(const GuiDraw* d : containerSlotsIt->second)
//    {
//        float width = d->botRight.x - d->topLeft.x;
//        float height = d->botRight.y - d->topLeft.y;
//        if(width > 30 && height > 30)
//        {
//            Vertex v;
//            d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, v.x, v.y);
//            containerSlots.push_back(v);
//        }
//    }
//
//    std::vector<Vertex> eqSlots;
//    if(containerSlots.size() >= (size_t)Equipment::SlotId::NUM_SLOTS)
//    {
//        eqSlots.resize((size_t)Equipment::SlotId::NUM_SLOTS);
//        auto containerIt = containerSlots.end();
//        containerIt--;
//        for(auto eqIt = eqSlots.rbegin(); eqIt != eqSlots.rend(); eqIt++)
//        {
//            *eqIt = *containerIt;
//            containerIt--;
//        }
//
//        containerIt++;
//        containerSlots.erase(containerIt, containerSlots.end());
//    }
//
//    Equipment eq;
//    struct CarriedItem
//    {
//        Vertex v;
//        const Object* item;
//    };
//    std::list<CarriedItem> carriedItems;
//    for(auto it = mCurrentFrame.guiSpriteDraws->begin(); it != mCurrentFrame.guiSpriteDraws->end(); it++)
//    {
//        const SpriteDraw& d = *it;
//        const Object* item = nullptr;
//        for(const auto& pair : d.pairings)
//            for(const Object* object : pair.objects)
//                if(object->itemInfo.isPickupable)
//                {
//                    item = object;
//                    break;
//                }
//
//        assert(item != nullptr);
//
//
//        Vertex screen;
//        d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, screen.x, screen.y);
//
//        bool isEq = false;
//        bool isCarried = false;
//        static const float SLOT_DISTANCE_EPSILON = 1.1f;
//        for(size_t i = 0; i < eqSlots.size(); i++)
//        {
//            if(std::fabs(screen.x - eqSlots[i].x) < SLOT_DISTANCE_EPSILON && std::fabs(screen.y - eqSlots[i].y) < SLOT_DISTANCE_EPSILON)
//            {
//                eq.setSlot((Equipment::SlotId)i, item);
//                isEq = true;
//                break;
//            }
//
//        }
//        if(!isEq)
//        {
//            for(const Vertex& v : containerSlots)
//            {
//                if(std::fabs(screen.x - v.x) < SLOT_DISTANCE_EPSILON && std::fabs(screen.y - v.y) < SLOT_DISTANCE_EPSILON)
//                {
//                    CarriedItem ci;
//                    ci.v = v;
//                    ci.item = item;
//                    carriedItems.push_back(ci);
//                    isCarried = true;
//                    break;
//                }
//            }
//
//            if(!isCarried)
//            {
//
//            }
//        }
//
//
//
//
////
////        std::cout << item->id;
////        if(item->itemInfo.hasMarketInfo)
////            std::cout << " " << item->itemInfo.marketInfo.name;
////
////
////
////        std::cout << " -> " << screen.x << "x" << screen.y << std::endl;
////        std::cout << std::endl;
//
//
//    }
//
//
//
//    for(const CarriedItem& item : carriedItems)
//    {
//
//        std::cout << "CI: " << item.v.x << "x" << item.v.y << " " << item.item->id;
//
//        if(item.item->itemInfo.hasMarketInfo)
//            std::cout << " \"" << item.item->itemInfo.marketInfo.name << "\"";
//
//        std::cout << std::endl;
//    }
//}


//
//void Gui::parseOutfitNames(size_t& i, const std::vector<TextDraw>& textDraws)
//{
//    assert(i < textDraws.size());
//
//    std::unique_ptr<TextBuilder> builder;
//    builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    if(builder->getTextType() != Text::Type::NAME && builder->getTextType() != Text::Type::NAME_OBSCURED);
//        return;
//
//    i++;
//    assert(i < textDraws.size());
//
//    builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    if(builder->getTextType() == Text::Type::NAME || builder->getTextType() == Text::Type::NAME_OBSCURED)
//        i++;
//
//    assert(i < textDraws.size());
//
//    builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    assert(builder->getTextType() != Text::Type::NAME && builder->getTextType() != Text::Type::NAME_OBSCURED);
//}
//
//void Gui::parseChatText(size_t& i, const std::vector<TextDraw>& textDraws)
//{
//    assert(i < textDraws.size());
//
//    std::unique_ptr<TextBuilder> builder;
//    builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    while(builder->getTextType() != Text::Type::GUI)
//    {
//        i++;
//        assert(i < textDraws.size());
//        builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    }
//
//    std::list<Text> text = builder->getText();
//    assert(text.size() == 1);
//    assert(text.front().string == "Chat On");
//
//    setButtonText(text.front());
//
//    i++;
//    assert(i < textDraws.size());
//    builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//
//    while(builder->getTextType() != Text::Type::GUI)
//    {
//        i++;
//        assert(i < textDraws.size());
//        builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    }
//}
//
//void Gui::parseSideWindowTopText(size_t& i, const std::vector<TextDraw>& textDraws)
//{
//    assert(i < textDraws.size());
//
//    std::unique_ptr<TextBuilder> builder;
//
//    builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    assert(builder->getTextType() == Text::Type::GUI);
//
//    bool isCentreParsed = false;
//    bool isStopParsed = false;
//    bool isSoulAndCapParsed = false;
//    bool isHpAndManaParsed = false;
//    bool isPremiumFeaturesParsed = false;
//    while(builder->getTextType() == Text::Type::GUI)
//    {
//        std::list<Text> text = builder->getText();
//        for(auto it = text.begin(); it != text.end(); it++)
//        {
//            const Text& t = *it;
//            if(t.string == "Centre")
//            {
//                setButtonText(t);
//                isCentreParsed = true;
//            }
//            else if(t.string == "Stop")
//            {
//                setButtonText(t);
//                isStopParsed = true;
//            }
//            else if(t.string == "soul:" || t.string == "Cs")
//            {
//                bool isSoulFront = (t.string == "soul:");
//
//                i++;
//                assert(i < textDraws.size());
//
//                builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//                assert(builder->getTextType() == Text::Type::GUI);
//                text = builder->getText();
//                assert(text.size() == 1);
//                assert(isNumeric(text.front().string));
//
//                unsigned short front = strToInt(text.front().string);
//
//                i++;
//                assert(i < textDraws.size());
//
//                builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//                assert(builder->getTextType() == Text::Type::GUI);
//                text = builder->getText();
//                assert(text.size() == 1);
//                assert(isNumeric(text.front().string));
//
//                unsigned short back = strToInt(text.front().string);
//
//                if(isSoulFront)
//                {
//                    mSoul = front;
//                    mCap = back;
//                }
//                else
//                {
//                    mCap = front;
//                    mSoul = back;
//                }
//
//                isSoulAndCapParsed = true;
//
//                break;
//
//            }
//            else if(isNumeric(t.string))
//            {
//                mHp = strToInt(t.string);
//                it++;
//                assert(it != text.end());
//
//                assert(isNumeric(it->string));
//                mMana = strToInt(it->string);
//
//                it++;
//
//                isHpAndManaParsed = true;
//
//                if(it != text.end())
//                {
//                    assert(it->string == "Premium Features");
//                    isPremiumFeaturesParsed = true;
//                    it++;
//                    if(it != text.end())
//                    {
////                        assert(it->string == "Access new areas");
////                        assert(text.back().string == "skills in an arena");
//                    }
//                }
//
//                break;
//            }
//            else if(t.string == "Premium Features")
//            {
//                isPremiumFeaturesParsed = true;
//                it++;
//                if(it != text.end())
//                {
////                    assert(it->string == "Access new areas");
////                    assert(text.back().string == "skills in an arena");
//                }
//
//                break;
//            }
//            else
//            {
//                THROW_RUNTIME_ERROR("Unexpected text: " + t.string);
//            }
//        }
//
//        i++;
//        if(i >= textDraws.size())
//            return;
//
//        if(isStopParsed && isCentreParsed && isPremiumFeaturesParsed && isSoulAndCapParsed && isHpAndManaParsed)
//            return;
//
//        builder.reset(new TextBuilder(textDraws[i], mCurrentFrame.width, mCurrentFrame.height));
//    }
//}

void Gui::parseVipWindowText(size_t& i, const SideBottomWindow& w)
{
    if(i >= mCurrentFrame.textDraws->size() || w.isMinimized)
        return;

    std::unique_ptr<TextBuilder> builder;
    builder.reset(new TextBuilder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height));
    if(builder->getTextType() != Text::Type::VIP_ONLINE && builder->getTextType() != Text::Type::VIP_OFFLINE)
        return;

    std::list<std::string>* currentList;
    currentList = (builder->getTextType() == Text::Type::VIP_ONLINE) ? &mOnlineVip : &mOfflineVip;

    for(const Text& t : builder->getText())
        currentList->push_back(t.string);

    i++;

    if(i >= mCurrentFrame.textDraws->size())
        return;

    builder.reset(new TextBuilder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height));
    if(builder->getTextType() == Text::Type::VIP_ONLINE || builder->getTextType() == Text::Type::VIP_OFFLINE)
    {
        currentList = (builder->getTextType() == Text::Type::VIP_ONLINE) ? &mOnlineVip : &mOfflineVip;
        for(const Text& t : builder->getText())
            currentList->push_back(t.string);

        i++;
    }
}

void Gui::parseBattleWindowText(size_t& i, const SideBottomWindow& w)
{
    mBattleWindow.reset(new BattleWindow());

    if(i >= mCurrentFrame.textDraws->size() || w.isMinimized)
        return;

    Rect r;
    r.pos.x = w.resizer.pos.x;
    r.pos.y = w.exitButton.pos.y;
    r.size.x = w.exitButton.pos.x + w.exitButton.size.x - r.pos.x;
    r.size.y = w.resizer.pos.y + w.resizer.size.y - r.pos.y;

    while(i < mCurrentFrame.textDraws->size())
    {
        TextBuilder builder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);
        const std::list<Text>& text = builder.getText();
        if(text.empty())
            return;

        if(text.front().localX != Constants::BATTLE_WINDOW_NAME_LOCAL_X)
            return;

        for(const Text& t : text)
        {
            assert(t.localX == Constants::BATTLE_WINDOW_NAME_LOCAL_X);

            mBattleWindow->outfits.emplace_back();
            BattleWindow::Outfit& o = mBattleWindow->outfits.back();
            o.name = t.string;
            if(t.type == Text::Type::NAME_BATTLE_WINDOW_HIGHLIGHTED)
            {
                assert(text.size() == 1);
                mBattleWindow->selectedOutfit = &o;
            }
        }

        i++;
    }

//    assert(!text.empty());
//    if(text.front().x < r.pos.x || text.front().y < r.pos.y || text.front().y + text.front().height > r.pos.y + r.size.y)
//        return;
//
//    i++;


}


void Gui::parseSkillWindowText(size_t& i, const SideBottomWindow& w)
{
    if(i >= mCurrentFrame.textDraws->size() || w.isMinimized)
        return;

    TextBuilder builder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);

    std::list<Text> text = builder.getText();
    assert(!text.empty());

    assert(builder.getTextType() == Text::Type::GUI);
    assert(text.size() == 51 || text.size() == 52);



    auto it = text.begin();

    using namespace sb::utility;
    assert(it->string == "Level");
    it++;
    assert(isNumeric(it->string));
    mLevel = strToInt(it->string);
    it++;

    assert(it->string == "  Experience");
    it++;
    assert(isNumeric(it->string));
    mExperience = strToInt(it->string);
    it++;

    if(it->string == "  XP Gain Rate")
        it++;
    else
    {
        i++;
        TextBuilder b((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);
        assert(b.getTextType() == Text::Type::GUI);

        std::list<Text> xpGainRate = b.getText();
        assert(xpGainRate.size() == 1);
        assert(xpGainRate.front().string == "  XP Gain Rate");
    }

    if(it->string == "Hit Points")
    {
        i++;
        TextBuilder b((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);
        assert(b.getTextType() == Text::Type::GUI);

        std::list<Text> xpGainRate = b.getText();
        assert(xpGainRate.size() == 1);
        mXpGainRate = parsePercentageText(xpGainRate.front().string);
    }
    else
    {
        mXpGainRate = parsePercentageText(it->string);
        it++;
    }

    assert(it->string == "Hit Points");
    it++;
    assert(isNumeric(it->string));
    mHp = strToInt(it->string);
    it++;

    assert(it->string == "Mana");
    it++;
    assert(isNumeric(it->string));
    mMana = strToInt(it->string);
    it++;

    assert(it->string == "Soul Points");
    it++;
    assert(isNumeric(it->string));
    mSoul = strToInt(it->string);
    it++;

    assert(it->string == "Capacity");
    it++;
    assert(isNumeric(it->string));
    mCap = strToInt(it->string);
    it++;

    assert(it->string == "Speed");
    it++;
    if(it->string == "Food")
    {
        i++;
        TextBuilder b((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);
        assert(b.getTextType() == Text::Type::GUI);

        std::list<Text> speed = b.getText();
        assert(speed.size() == 1);
        assert(isNumeric(speed.front().string));
        mSpeed = strToInt(speed.front().string);
    }
    else
    {
        assert(isNumeric(it->string));
        mSpeed = strToInt(it->string);
        it++;
    }

    assert(it->string == "Food");
    it++;
    mFoodMinutes = parseTimeText(it->string);
    it++;

    assert(it->string == "Stamina");
    it++;
    mStaminaMinutes = parseTimeText(it->string);
    it++;

    assert(it->string == "Offline Training");
    it++;
    mOfflineTrainingMinutes = parseTimeText(it->string);
    it++;

    assert(it->string == "Magic Level");
    it++;
    assert(isNumeric(it->string));
    mMagicLevel = strToInt(it->string);
    it++;

    assert(it->string == "Fist Fighting");
    it++;
    assert(isNumeric(it->string));
    mFistFighting = strToInt(it->string);
    it++;

    assert(it->string == "Club Fighting");
    it++;
    assert(isNumeric(it->string));
    mClubFighting = strToInt(it->string);
    it++;

    assert(it->string == "Sword Fighting");
    it++;
    assert(isNumeric(it->string));
    mSwordFighting = strToInt(it->string);
    it++;

    assert(it->string == "Axe Fighting");
    it++;
    assert(isNumeric(it->string));
    mAxeFighting = strToInt(it->string);
    it++;

    assert(it->string == "Distance Fighting");
    it++;
    assert(isNumeric(it->string));
    mDistanceFighting = strToInt(it->string);
    it++;

    assert(it->string == "Shielding");
    it++;
    assert(isNumeric(it->string));
    mShielding = strToInt(it->string);
    it++;

    assert(it->string == "Fishing");
    it++;
    assert(isNumeric(it->string));
    mFishing = strToInt(it->string);
    it++;

    assert(it->string == "Critical Hit:");
    it++;

    assert(it->string == "  Chance");
    it++;
    mCritChance = parsePercentageText(it->string);
    it++;

    assert(it->string == "  Extra Damage");
    it++;
    mCritDamage = parsePercentageText(it->string);
    it++;

    assert(it->string == "Hit Points Leech:");
    it++;

    assert(it->string == "  Chance");
    it++;
    mHpLeechChance = parsePercentageText(it->string);
    it++;

    assert(it->string == "  Amount");
    it++;
    mHpLeechAmount = parsePercentageText(it->string);
    it++;

    assert(it->string == "Mana Leech:");
    it++;

    assert(it->string == "  Chance");
    it++;
    mManaLeechChance = parsePercentageText(it->string);
    it++;

    assert(it->string == "  Amount");
    it++;
    mManaLeechAmount = parsePercentageText(it->string);
    it++;

    assert(it == text.end());
    i++;
}

unsigned short Gui::parseTimeText(const std::string& str) const
{
    size_t colonPos = str.find(':');
    assert(colonPos != str.npos);

    std::string hours = str.substr(0, colonPos);
    std::string minutes = str.substr(colonPos + 1);

    using namespace sb::utility;
    assert(isNumeric(hours));
    assert(isNumeric(minutes));

    return strToInt(hours) * 60 + strToInt(minutes);
}

unsigned short Gui::parsePercentageText(const std::string& str) const
{
    assert(str.size() > 1);
    assert(str.back() == '%');

    std::string percentage = str.substr(0, str.size() - 1);
    assert(sb::utility::isNumeric(percentage));

    return sb::utility::strToInt(percentage);
}

void Gui::parseDialogFramesText(size_t& i)
{
    if(i >= mCurrentFrame.textDraws->size())
        return;

    for(; i < mCurrentFrame.textDraws->size(); i++)
    {
        TextBuilder builder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);

        if(builder.getTextType() != Text::Type::GUI)
            return;

        std::list<Text> text = builder.getText();
        const std::string& str = text.front().string;
        if(str == "Options")
        {

        }
        else if(str == "Quest Log")
        {

        }
        else
            return;
    }
}


//                EQUIPMENT_AND_COMBAT_OPTIONS,
//                MINI_MAP,
//                MANA_AND_HP,
//                PREMIUM_FEATURES,
//                CONTROL_BUTTONS,
//                CHAT,
//                SPELL_GROUPS,
//                SPELLS,
//                GAME_WINDOW,
//                SKILLS_WINDOW,
//                BATTLE_WINDOW,
//                VIP_WINDOW,
//                QUEST_LOG,
//                UNJUSTIFIED_POINTS_WINDOW,
//                OPTIONS,
//                LEAVE_GAME_WINDOW,
//                TRADE_WINDOW,
//                CONTAINER_WINDOW,

void Gui::getRects(const std::string& name, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, std::list<Rect>& rects) const
{
    auto it = guiDraws.find(name);
    if(it == guiDraws.end())
        return;

    for(const GuiDraw* d : it->second)
    {
        Rect r;
        d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, r.pos.x, r.pos.y);
        d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, r.size.x, r.size.y, d->botRight.x, d->botRight.y);

        r.size.x -= r.pos.x;
        r.size.y -= r.pos.y;

        rects.push_back(r);
    }
}
//
//Gui::SideBottomWindow::Type Gui::resolveWindowType(float halfFrameWidth, float halfFrameHeight, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, int exitButtonTopLeftY) const
//{
//    std::list<Rect> icons;
//    getRects("icon-battlelist.png", guiDraws, icons);
//    for(Rect r : icons)
//        std::cout << r.pos.x << "x" << r.pos.y << " (" << r.size.x << "x" << r.size.y << ")" << " - " << exitButtonTopLeftY << std::endl;
//    if(!icons.empty() && int(icons.front().pos.y) == exitButtonTopLeftY)
//        return SideBottomWindow::Type::BATTLE;
//    icons.clear();
//
//    getRects("icon-skills-widget.png", guiDraws, icons);
//    if(!icons.empty() && int(icons.front().pos.y) == exitButtonTopLeftY)
//        return SideBottomWindow::Type::SKILLS;
//    icons.clear();//
//    getRects("icon-trade.png", guiDraws, icons);
//    if(!icons.empty() && int(icons.front().pos.y) == exitButtonTopLeftY)
//        return SideBottomWindow::Type::NPC_TRADE;
//    icons.clear();//
//    getRects("icon-unjustified-points-widget.png", guiDraws, icons);
//    if(!icons.empty() && int(icons.front().pos.y) == exitButtonTopLeftY)
//        return SideBottomWindow::Type::UNJUSTIFIED_POINTS;
//    icons.clear();//
//    getRects("icon-vip-widget.png", guiDraws, icons);
//    if(!icons.empty() && int(icons.front().pos.y) == exitButtonTopLeftY)
//        return SideBottomWindow::Type::VIP;
//
//    return SideBottomWindow::Type::CONTAINER;
//}

void Gui::updateSideBottomWindows(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws)
{

    std::list<Rect> exitButtons;
    getRects("exit-button-small-down.png", guiDraws, exitButtons);
    getRects("exit-button-small-up.png", guiDraws, exitButtons);

    if(exitButtons.empty())
        return;

    std::list<Rect> minButtons;
    getRects("min-button-small-down.png", guiDraws, minButtons);
    getRects("min-button-small-up.png", guiDraws, minButtons);

    std::list<Rect> maxButtons;
    getRects("max-button-small-down.png", guiDraws, maxButtons);
    getRects("max-button-small-up.png", guiDraws, maxButtons);

    std::list<Rect> containerUpButtons;
    getRects("container-level-up-down.png", guiDraws, containerUpButtons);
    getRects("container-level-up-up.png", guiDraws, containerUpButtons);


    std::list<Rect> resizers;
    getRects("windowresizer.png", guiDraws, resizers);

    std::list<Rect> expandButtons;
    getRects("button-expand-12x12-down.png", guiDraws, expandButtons);
    getRects("button-expand-12x12-up.png", guiDraws, expandButtons);



    for(const Rect& r : exitButtons)
    {
        mSideBottomWindows.emplace_front();
        SideBottomWindow& w = mSideBottomWindows.front();

        w.exitButton = r;

        int x = r.pos.x;
        int y = r.pos.y;

        for(auto it = maxButtons.begin(); it != maxButtons.end(); it++)
        {
            if(int(it->pos.y) == y && int(it->pos.x + it->size.x) == x)
            {
                w.isMinimized = true;
                w.minMaxButton = *it;
                maxButtons.erase(it);
                break;
            }
        }

        if(!w.isMinimized)
        {
            for(auto it = minButtons.begin(); it != minButtons.end(); it++)
            {
                if(int(it->pos.y) == y && int(it->pos.x + it->size.x) == x)
                {
                    w.isMinimized = false;
                    w.minMaxButton = *it;
                    minButtons.erase(it);
                    break;
                }
            }
        }

        x = w.minMaxButton.pos.x;
        y = w.minMaxButton.pos.y;
//        w.type = resolveWindowType(mHalfFrameWidth, mHalfFrameHeight, guiDraws, y);
//        if(w.type == SideBottomWindow::Type::BATTLE)
        {
//            bool foundExpandButton = false;
            for(auto it = expandButtons.begin(); it != expandButtons.end(); it++)
            {
                if(int(it->pos.y) == y)
                {
//                    foundExpandButton = true;
                    w.type = SideBottomWindow::Type::BATTLE;
                    w.expandButton = *it;
                    expandButtons.erase(it);
                    break;
                }
            }

//            assert(foundExpandButton);
        }
//        else if(w.type == SideBottomWindow::Type::CONTAINER)

        if(w.type == SideBottomWindow::Type::INVALID)
        {
            for(auto it = containerUpButtons.begin(); it != containerUpButtons.end(); it++)
            {
                if(int(it->pos.y) == y && int(it->pos.x + it->size.x) == x)
                {
                    w.type = SideBottomWindow::Type::CONTAINER;
                    w.containerUpButton = *it;
                    containerUpButtons.erase(it);
                    break;
                }
            }
        }
    }

    for(const Rect& r : resizers)
    {
        SideBottomWindow* closestWindow = nullptr;
        size_t minY = -1;
        for(SideBottomWindow& w : mSideBottomWindows)
        {
            if(!w.isMinimized && w.exitButton.pos.y < r.pos.y)
            {
                size_t dy = r.pos.y - w.exitButton.pos.y;
                if(dy < minY)
                {
                    minY = dy;
                    closestWindow = &w;
                }
            }
        }

        assert(closestWindow);
        closestWindow->hasResizer = true;
        closestWindow->resizer = r;

        closestWindow->clientArea.pos.x = mCurrentFrame.width - Constants::SIDE_BOTTOM_WINDOW_CLIENT_SCREEN_LEFT_OFFSET_X;
        closestWindow->clientArea.size.x = Constants::SIDE_BOTTOM_WINDOW_CLIENT_WIDTH;
        closestWindow->clientArea.pos.y = closestWindow->exitButton.pos.y + closestWindow->exitButton.size.y + 1;
        closestWindow->clientArea.size.y = r.pos.y + r.size.y - closestWindow->clientArea.pos.y;
    }
}
//
//void Gui::updateEquippedItems(size_t& i, const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, const std::vector<SpriteDraw>& guiSpriteDraws)
//{
//    mEquipment.clear();
//
//    if(i >= guiSpriteDraws.size())
//        return;
//
//    if(guiDraws.find("button-combat-pvp-small-up.png") != guiDraws.end() || guiDraws.find("button-combat-pvp-small-down.png") != guiDraws.end())
//        return;
//
//
//    assert(guiDraws.find("button-combat-pvp-up.png") != guiDraws.end() || guiDraws.find("button-combat-pvp-down.png") != guiDraws.end());
//
//    unsigned char numEquippedItems = Constants::NUM_EQ_SLOTS;
//    if(guiDraws.find("inventory-back.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-feet.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-finger.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-head.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-hip.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-left-hand.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-legs.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-neck.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-right-hand.png") != guiDraws.end()) numEquippedItems--;
//    if(guiDraws.find("inventory-torso.png") != guiDraws.end()) numEquippedItems--;
//
//    assert(numEquippedItems <= Constants::NUM_EQ_SLOTS);
//
//    if(i + numEquippedItems > guiSpriteDraws.size())
//    {
//        i = guiSpriteDraws.size();
//        return;
//    }
//
//    std::cout << "NUM EQ ITEMS: " << (int)numEquippedItems << std::endl;
//    if(numEquippedItems != 8)
//    {
//        if(guiDraws.find("inventory-back.png") != guiDraws.end()) std::cout << "BACK" << std::endl;;
//        if(guiDraws.find("inventory-feet.png") != guiDraws.end()) std::cout << "FEET" << std::endl;;
//        if(guiDraws.find("inventory-finger.png") != guiDraws.end()) std::cout << "FINGER" << std::endl;;
//        if(guiDraws.find("inventory-head.png") != guiDraws.end()) std::cout << "HEAD" << std::endl;;
//        if(guiDraws.find("inventory-hip.png") != guiDraws.end()) std::cout << "HIP" << std::endl;;
//        if(guiDraws.find("inventory-left-hand.png") != guiDraws.end()) std::cout << "LEFTHANF" << std::endl;;
//        if(guiDraws.find("inventory-legs.png") != guiDraws.end()) std::cout << "LEGS" << std::endl;;
//        if(guiDraws.find("inventory-neck.png") != guiDraws.end()) std::cout << "NECK" << std::endl;;
//        if(guiDraws.find("inventory-right-hand.png") != guiDraws.end()) std::cout << "RIGHTHAND" << std::endl;;
//        if(guiDraws.find("inventory-torso.png") != guiDraws.end()) std::cout << "TORSO" << std::endl;;
//    }
//
//    assert(numEquippedItems == 8);
//
//    for(size_t j = 0; j < numEquippedItems; j++)
//    {
//        const SpriteDraw& s = guiSpriteDraws[i + j];
//        const Object* equippable = nullptr;
//        for(const auto& pairing : s.pairings)
//        {
//            for(const Object* o : pairing.objects)
//                if(o->itemInfo.bodyRestriction != Object::BodyRestriction::NONE)
//                {
//                    equippable = o;
//                    break;
//                }
//            if(equippable)
//                break;
//        }
//
//        assert(equippable);
//
//        typedef Object::BodyRestriction B;
//        switch(equippable->itemInfo.bodyRestriction)
//        {
//            case B::TWO_HANDED:
//            case B::WEAPON:
//            case B::SHIELD:
//                if(mEquipment.find(EqType::HAND1) == mEquipment.end())
//                    mEquipment[EqType::HAND1] = equippable;
//                else
//                {
//                    assert(mEquipment.find(EqType::HAND2) == mEquipment.end());
//                    mEquipment[EqType::HAND2] = equippable;
//                }
//                break;
//
//            case B::HELMET: mEquipment[EqType::HEAD] = equippable; break;
//            case B::AMULET: mEquipment[EqType::NECK] = equippable; break;
//            case B::BACKPACK: mEquipment[EqType::BACK] = equippable; break;
//            case B::ARMOR: mEquipment[EqType::TORSO] = equippable; break;
//            case B::LEGS: mEquipment[EqType::LEGS] = equippable; break;
//            case B::BOOTS: mEquipment[EqType::FEET] = equippable; break;
//            case B::RING: mEquipment[EqType::FINGER] = equippable; break;
//            case B::BELT: mEquipment[EqType::HIP] = equippable; break;
//
//            default:
//            {
//                std::stringstream sstream;
//                sstream << "Unexpected body restriction: " << (int)equippable->itemInfo.bodyRestriction << std::endl;
//                THROW_RUNTIME_ERROR(sstream.str());
//                break;
//            }
//        }
//    }
//
//    i += numEquippedItems;
//}
//
//void Gui::updateSideBottomWindowItems(size_t& i, const std::vector<SpriteDraw>& guiSpriteDraws)
//{
//    mContainers.clear();
//    std::cout << "WINDOW ORDER: " << std::endl;
//    for(SideBottomWindow& w : mSideBottomWindows)
//    {
//        std::cout << "\t" << (int)w.type << std::endl;
//    }
//
//    for(SideBottomWindow& w : mSideBottomWindows)
//    {
//        if(i >= guiSpriteDraws.size())
//            return;
//
//        std::cout << "Parsing window items: " << (int)w.type << std::endl;
//
//        typedef SideBottomWindow::Type T;
//        switch(w.type)
//        {
//            case T::CONTAINER: parseContainer(i, w, guiSpriteDraws); break;
//            case T::BATTLE: parseBattleWindowItems(i, w, guiSpriteDraws); break;
//            case T::NPC_TRADE: parseNpcTradeWindowItems(i, w, guiSpriteDraws); break;
//
//            default: break;
//        }
//
//    }
//}
//
//void Gui::parseBattleWindowItems(size_t& i, SideBottomWindow& w, const std::vector<SpriteDraw>& guiSpriteDraws)
//{
//    assert(i < guiSpriteDraws.size());
//    assert(w.hasResizer);
//    assert(w.type == SideBottomWindow::Type::BATTLE);
//
//    for(; i < guiSpriteDraws.size(); i++)
//    {
//        const SpriteDraw& d = guiSpriteDraws[i];
//        Vertex topLeft;
//        d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);
//        if(topLeft.x < w.resizer.pos.x || topLeft.y < w.exitButton.pos.y || topLeft.y > w.resizer.pos.y + w.resizer.size.y)
//            return;
//    }
//}
//
//void Gui::parseNpcTradeWindowItems(size_t& i, SideBottomWindow& w, const std::vector<SpriteDraw>& guiSpriteDraws)
//{
//    assert(i < guiSpriteDraws.size());
//
//}
//
//void Gui::parseContainer(size_t& i, SideBottomWindow& w, const std::vector<SpriteDraw>& guiSpriteDraws)
//{
////    assert(i < guiSpriteDraws.size());
////
////    const Object* container = nullptr;
////    for(const auto& pairing : guiSpriteDraws[i].pairings)
////    {
////        for(const Object* o : pairing.objects)
////        {
////            if(o->itemInfo.isContainer)
////            {
////                container = o;
////                break;
////            }
////            std::cout << "Container ? " << o->id << std::endl;
////        }
////        if(container)
////            break;
////    }
////
////    assert(container);
////
////    int iconWidth = int(guiSpriteDraws[i].botRight.x - guiSpriteDraws[i].topLeft.x);
////    int iconHeight = int(guiSpriteDraws[i].botRight.y - guiSpriteDraws[i].topLeft.y);
////
////    assert(iconWidth == 12);
////    assert(iconHeight == 12 || iconHeight == 10);
////
////    mContainers.emplace_back();
////    Container& c = mContainers.back();
////    if(container->itemInfo.hasMarketInfo)
////        c.name = container->itemInfo.marketInfo.name;
////
////    assert(w.type == SideBottomWindow::Type::CONTAINER);
////    c.window = &w;
////
////
////    i++;
////
////    if(c.window->isMinimized)
////        return;
////
////    if(i >= guiSpriteDraws.size())
////        return;
////
////    const int START_X = guiSpriteDraws[i].topLeft.x;
////    const int START_Y = guiSpriteDraws[i].topLeft.y;
////    assert(Constants::CONTAINER_FIRST_SLOT_LOCAL_X == START_X);
////    assert(START_Y <= Constants::CONTAINER_FIRST_SLOT_LOCAL_Y);
////
////    c.scroll = Constants::CONTAINER_FIRST_SLOT_LOCAL_Y - START_Y;
////
////    int expectedX = START_X;
////    int expectedY = START_Y;
////    static const int DELTA_X = Constants::TILE_PIXEL_WIDTH + Constants::CONTAINER_SEPARATOR_PIXEL_WIDTH;
////    static const int DELTA_Y = Constants::TILE_PIXEL_HEIGHT + Constants::CONTAINER_SEPARATOR_PIXEL_WIDTH;
////    for(unsigned char iColumn = 1; i < guiSpriteDraws.size(); i++)
////    {
////        const SpriteDraw& d = guiSpriteDraws[i];
////        int x = d.topLeft.x;
////        int y = d.topLeft.y;
////
////        if(x != expectedX || y != expectedY)
////            return;
////
////        const Object* pickupable = nullptr;
////        for(const auto& pairing : d.pairings)
////        {
////            for(const Object* o : pairing.objects)
////                if(o->itemInfo.isPickupable)
////                {
////                    pickupable = o;
////                    break;
////                }
////            if(pickupable)
////                break;
////        }
////
////        assert(pickupable);
////
////        c.items.emplace_back(1, pickupable);
////
////        if(iColumn == Constants::CONTAINER_WIDTH)
////        {
////            iColumn = 1;
////            expectedX = Constants::CONTAINER_FIRST_SLOT_LOCAL_X;
////            expectedY += DELTA_Y;
////
////        }
////        else
////        {
////            expectedX += DELTA_X;
////            iColumn++;
////        }
////    }
//
//}
//
//void Gui::updateItems(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws, const std::vector<SpriteDraw>& guiSpriteDraws)
//{
//    if(mCurrentState != State::GAME)
//        return;
//
//    size_t i = 0;
//    updateEquippedItems(i, guiDraws, guiSpriteDraws);
//    updateSideBottomWindowItems(i, guiSpriteDraws);
//}
//

void Gui::parseNames(size_t& i)
{
//    std::cout << "Names: " << std::endl;
    while(i < mCurrentFrame.textDraws->size())
    {
        const TextDraw& d = (*mCurrentFrame.textDraws)[i];
        TextBuilder builder(d, mCurrentFrame.width, mCurrentFrame.height);
        if(builder.getTextType() != Text::Type::NAME && builder.getTextType() != Text::Type::NAME_OBSCURED)
            return;

//        for(const Text& t : builder.getText())
//            std::cout << "\t" << t.string << std::endl;

        i++;
    }
}

void Gui::parseChat(size_t& i)
{
//    std::cout << "Chat content: " << std::endl;
    typedef Text::Type T;
    while(i < mCurrentFrame.textDraws->size())
    {
        const TextDraw& d = (*mCurrentFrame.textDraws)[i];
        TextBuilder builder(d, mCurrentFrame.width, mCurrentFrame.height);

        bool doBreak = false;
        switch(builder.getTextType())
        {
            case T::SELECTED_TEXT:
            case T::SERVER_BROADCAST:
            case T::BROADCAST:
            case T::GREEN_TEXT:
            case T::INFO_TEXT:
            case T::SAY:
            case T::PM:
            case T::SELF_PM:
            case T::CLICKABLE_NPC_TEXT:
            case T::SOUND:
                break;

            default:
                doBreak = true;
                break;
        }
        if(doBreak)
            break;

//        for(const Text& t : builder.getText())
//            std::cout << "\t" << t.string << std::endl;

        i++;
    }

    std::list<Text> chatOn = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
    assert(chatOn.size() == 1);
    assert(chatOn.front().string == "Chat on");
    if(!setButtonText(chatOn.front()))
        THROW_RUNTIME_ERROR("Failed to set \"Chat on\" button.");

//    std::cout << "Chat tabs: " << std::endl;
    while(i < mCurrentFrame.textDraws->size())
    {
        const TextDraw& d = (*mCurrentFrame.textDraws)[i];
        TextBuilder builder(d, mCurrentFrame.width, mCurrentFrame.height);
        if(builder.getTextType() != T::CHAT_TAB)
            return;

//        for(const Text& t : builder.getText())
//            std::cout << "\t" << t.string << std::endl;

        i++;
    }
}

void Gui::parseSideWindowTopText(size_t& i)
{
    enum Type : unsigned char
    {
        CENTRE,
        HP_MANA,
        PREMIUM_FEATURES,
        SOUL_CAP,
        STOP,
        NUM_TYPES,
    };

    std::set<Type> parsedTypes;

    using namespace sb::utility;
    while(i < mCurrentFrame.textDraws->size() && parsedTypes.size() < NUM_TYPES)
    {
        std::list<Text> text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
        assert(!text.empty());


        Text title = text.front();
        if(title.string == "soul:" || title.string == "Ca")
        {
            assert(text.size() == 3);

            std::string soulCap;
            for(const Text& t : text)
                soulCap += t.string;

            text.clear();

            if(soulCap == "soul:CaP:")
                mIsEquipmentMinimized = false;
            else if(soulCap == "CaP:soul:")
                mIsEquipmentMinimized = true;
            else
                THROW_RUNTIME_ERROR("Expected cap/soul. Got \"" + soulCap + "\".");

            assert(i < mCurrentFrame.textDraws->size());
            std::list<Text> val1 = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
            assert(i < mCurrentFrame.textDraws->size());
            std::list<Text> val2 = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();

            assert(val1.size() == 1);
            assert(val2.size() == 1);

            const Text* soul = &val1.front();
            const Text* cap = &val2.front();
            if(mIsEquipmentMinimized)
                std::swap(soul, cap);

            assert(isNumeric(soul->string));
            assert(isNumeric(cap->string));

            mSoul = strToInt(soul->string);
            mCap = strToInt(cap->string);

            parsedTypes.insert(SOUL_CAP);
        }
        else if(isNumeric(title.string))
        {
            assert(text.size() >= 2);

            mHp = strToInt(text.front().string);
            text.pop_front();
            assert(isNumeric(text.front().string));
            mMana = strToInt(text.front().string);
            text.pop_front();

            parsedTypes.insert(HP_MANA);
        }

        for(const Text& t : text)
        {
            if(t.string == "Centre")
            {
                if(!setButtonText(t))
                    THROW_RUNTIME_ERROR("Failed to set \"Centre\" button.");
                parsedTypes.insert(CENTRE);

            }
            else if(t.string == "Stop")
            {
                if(!setButtonText(t))
                    THROW_RUNTIME_ERROR("Failed to set \"Stop\" button.");
                parsedTypes.insert(STOP);
            }
            else if(t.string == "Premium Features")
                parsedTypes.insert(PREMIUM_FEATURES);
            else
                THROW_RUNTIME_ERROR("Unexpected text. Got \"" + t.string + "\".");
        }
    }

    assert(parsedTypes.size() == NUM_TYPES);
}


void Gui::parseSideWindowBottomText(size_t& i)
{
    auto containerIt = mContainers.begin();
    for(SideBottomWindow& w : mSideBottomWindows)
    {
        assert(i < mCurrentFrame.textDraws->size());
        TextBuilder builder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height);

        assert(builder.getTextType() == Text::Type::GUI);

        std::list<Text> text = builder.getText();
        assert(!text.empty());
        const std::string& str = text.front().string;
        if(str == "Skills")
        {
            w.type = SideBottomWindow::Type::SKILLS;
            parseSkillWindowText(i, w);
        }
        else if(str == "Battle")
        {
            w.type = SideBottomWindow::Type::BATTLE;
            parseBattleWindowText(i, w);
        }
        else if(str == "VIP")
        {
            w.type = SideBottomWindow::Type::VIP;
            parseVipWindowText(i, w);
        }
        else if(str == "Unjustified Points")
        {
            w.type = SideBottomWindow::Type::UNJUSTIFIED_POINTS;
        }
        else if(str == "NPC Trade")
        {
            w.type = SideBottomWindow::Type::NPC_TRADE;
            parseNpcTradeWindowText(i, w);
        }
        else if(str == "Browse Field")
        {
            THROW_RUNTIME_ERROR("Don't forget to implement Browse Field! :-)");
        }
        else if(w.isMinimized)
        {
            w.type = SideBottomWindow::Type::CONTAINER;
            Container& c = *mContainers.insert(containerIt, Container());
            parseContainerText(i, c, str, w);
        }
        else if(containerIt != mContainers.end())
        {
            w.type = SideBottomWindow::Type::CONTAINER;
            parseContainerText(i, *containerIt, str, w);
            containerIt++;
        }
        else
            THROW_RUNTIME_ERROR("Unexpected text: \"" + str + "\".");
    }
}

void Gui::parseNpcTradeWindowText(size_t& i, const SideBottomWindow& w)
{

    mNpcTradeWindow.reset(new NpcTradeWindow());
    mNpcTradeWindow->window = &w;

    if(i >= mCurrentFrame.textDraws->size() || w.isMinimized)
        return;

     std::list<Text> text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
     assert(text.size() == 2 || text.size() == 3);

     assert(text.front().string == "Buy");
     mNpcTradeWindow->buyButton = setButtonText(text.front());
     assert(mNpcTradeWindow->buyButton);

     text.pop_front();

     assert(text.front().string == "Sell");
     mNpcTradeWindow->sellButton = setButtonText(text.front());
     assert(mNpcTradeWindow->sellButton);

     text.pop_front();

     if(!text.empty())
     {
        assert(text.front().string == "ok");
        mNpcTradeWindow->okButton = setButtonText(text.front());
        assert(mNpcTradeWindow->okButton);
     }




     assert(mNpcTradeWindow->buyButton->isDown || mNpcTradeWindow->sellButton->isDown);
     if(mNpcTradeWindow->buyButton->isDown)
        mNpcTradeWindow->currentTab = NpcTradeWindow::Tab::BUY;
     else
        mNpcTradeWindow->currentTab = NpcTradeWindow::Tab::SELL;


    assert(i < mCurrentFrame.textDraws->size());
    text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();

    assert(!text.empty());
    using namespace sb::utility;
    while(text.front().string != "Amount:")
    {
        static const size_t LINES_PER_ITEM = 2;

        assert(text.size() % LINES_PER_ITEM == 0);

        NpcTradeWindow::Offer offer;
        if(text.front().type == Text::Type::GUI)
            offer.isAffordable = true;
        else if(text.front().type == Text::Type::DISABLED_LIST_ENTRY)
            offer.isAffordable = false;
        else
            THROW_RUNTIME_ERROR(stringify("Unexpected text type: ", (int)text.front().type));

        for(auto it = text.begin(); it != text.end();)
        {
            offer.name = it->string;
            it++;

            size_t commaIndex = it->string.find(',');
            assert(commaIndex != it->string.npos);

            std::string gold = it->string.substr(0, commaIndex);
            std::string weight = it->string.substr(commaIndex + 2);
            it++;

            gold = gold.substr(0, gold.find("gold") - 1);
            weight = weight.substr(0, weight.find("oz") - 1);

            assert(isNumeric(gold));
            offer.cost = strToInt(gold);

            if(!isNumeric(weight))
            {
                assert(weight.size() > 1);
                assert(weight.back() == '-');
                size_t decimalIndex = weight.find('.');
                if(decimalIndex != weight.npos)
                {
                    weight = weight.substr(0, decimalIndex);
                    assert(isNumeric(weight));
                    offer.weight = strToFloat(weight);
                }
                else
                    offer.weight = -1.f;

            }
            else
                offer.weight = strToFloat(weight);

            mNpcTradeWindow->visibleOffers.push_back(offer);
        }

        assert(i < mCurrentFrame.textDraws->size());
        text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
        assert(!text.empty());
    }
    assert(text.size() == 6);

    auto it = text.begin();

    assert(it->string == "Amount:");
    it++;
    assert(isNumeric(it->string));
    mNpcTradeWindow->amount = strToInt(it->string);
    it++;

    assert(it->string == "Price:");
    it++;
    assert(isNumeric(it->string));
    mNpcTradeWindow->totalPrice = strToInt(it->string);
    it++;

    assert(it->string == "Money:");
    it++;
    assert(isNumeric(it->string));
    mNpcTradeWindow->availableMoney = strToInt(it->string);
    it++;

    assert(it == text.end());

    if(!mNpcTradeWindow->okButton)
    {
        assert(i < mCurrentFrame.textDraws->size());
        text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
        assert(text.size() == 1);
        assert(text.front().string == "ok");
        mNpcTradeWindow->okButton = setButtonText(text.front());
        assert(mNpcTradeWindow->okButton);
    }
}

void Gui::parseContainerText(size_t& i, Container& c, const std::string& str, const SideBottomWindow& w)
{
    using namespace sb::utility;
    c.name = toLower(str);
    c.window = &w;

    if(i >= mCurrentFrame.textDraws->size() || w.isMinimized)
        return;

    TextBuilder builder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height);
    if(builder.getTextType() != Text::Type::ITEM_STACK_COUNT)
        return;

    i++;

    std::list<Text> text = builder.getText();
    assert(!text.empty());
    static const float START_X = Constants::CONTAINER_FIRST_SLOT_LOCAL_X;
    const float START_Y = float(Constants::CONTAINER_FIRST_SLOT_LOCAL_Y) - c.scroll;
    size_t lastIndex = containerSlotLocalCoordsToIndex(START_X, START_Y, text.back().localX, text.back().localY);
    assert(lastIndex < c.capacity);
    c.items.resize(lastIndex + 1);

    for(const Text& t : text)
    {
        assert(isNumeric(t.string));

        size_t index = containerSlotLocalCoordsToIndex(START_X, START_Y, t.localX, t.localY);
        c.items[index].first = strToInt(t.string);
    }
}

void Gui::parseContainerSlots(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws)
{
    static const size_t NUM_DRAWS_PER_SLOT = 9;
    static const size_t MAIN_RECT_DRAW_INDEX_OFFSET = 4;
    static const size_t NUM_EQ_SLOTS_MIN = 1;
    static const size_t NUM_EQ_SLOTS_MAX = (size_t)EqType::NUM_TYPES + 2;

    auto containerSlotsIt = guiDraws.find("containerslot.png");
    if(containerSlotsIt == guiDraws.end())
        return;

    const std::list<const GuiDraw*>& slots = containerSlotsIt->second;
    assert(slots.size() % NUM_DRAWS_PER_SLOT == 0);

    size_t numSlots = slots.size() / NUM_DRAWS_PER_SLOT;
    size_t numEqSlots = (mIsEquipmentMinimized ? NUM_EQ_SLOTS_MIN : NUM_EQ_SLOTS_MAX);
    assert(numSlots >= numEqSlots);
    size_t numContainerSlots = numSlots - numEqSlots;
    if(numContainerSlots == 0)
        return;

    std::list<Vertex> slotsTopLeft;
    auto slotsIt = slots.begin();
    for(size_t i = 0; i < MAIN_RECT_DRAW_INDEX_OFFSET; i++)
        slotsIt++;

    size_t numNpcTradeSlots = 0;
    for(size_t i = 0; i < numContainerSlots; i++)
    {
        const Vertex& v = (*slotsIt)->topLeft;
        if(v.x == Constants::NPC_TRADE_LIST_SLOT_LOCAL_X || v.x == Constants::NPC_TRADE_CURRENT_SLOT_LOCAL_X)
        {
            numNpcTradeSlots++;
        }
        else
        {
            slotsTopLeft.push_back(v);
        }

        for(size_t j = 0; j < NUM_DRAWS_PER_SLOT; j++)
            slotsIt++;
    }
    numContainerSlots -= numNpcTradeSlots;
    assert(slotsTopLeft.size() == numContainerSlots);

    auto it = slotsTopLeft.rbegin();
    for(size_t i = 0; i < numContainerSlots;)
    {
        mContainers.emplace_back();
        Container& c = mContainers.back();

        using namespace Constants;
        static const int DELTA_X = TILE_PIXEL_WIDTH + CONTAINER_SEPARATOR_PIXEL_WIDTH;
        static const int DELTA_Y = TILE_PIXEL_HEIGHT + CONTAINER_SEPARATOR_PIXEL_WIDTH;
        static const float START_X = CONTAINER_FIRST_SLOT_LOCAL_X;
        const float START_Y = it->y - DELTA_Y * 20.f;


        assert(int(it->x - START_X + 0.5f) % DELTA_X == 0);

        c.capacity = 1;

        if(i + 1 == numContainerSlots)
        {
            c.scroll = float(CONTAINER_FIRST_SLOT_LOCAL_Y) - it->y;
            break;
        }

        int previousIndex = containerSlotLocalCoordsToIndex(START_X, START_Y, it->x, it->y);
        i++;
        it++;

        int currentIndex = containerSlotLocalCoordsToIndex(START_X, START_Y, it->x, it->y);
        if(previousIndex == currentIndex)
        {
            it--;
            c.scroll = float(CONTAINER_FIRST_SLOT_LOCAL_Y) - it->y;
            it++;
        }
        else
        {
            i++;
            it++;
            c.capacity++;

            const int EXPECTED_INDEX_DELTA = currentIndex - previousIndex;
            if(EXPECTED_INDEX_DELTA == 1)
                c.scroll = float(CONTAINER_FIRST_SLOT_LOCAL_Y) - it->y;
            previousIndex = currentIndex;

            while(i < numContainerSlots)
            {
                currentIndex = containerSlotLocalCoordsToIndex(START_X, START_Y, it->x, it->y);

                if(currentIndex - previousIndex != EXPECTED_INDEX_DELTA)
                {
                    break;
                }

                previousIndex = currentIndex;
                i++;
                it++;
                c.capacity++;
            }

            if(EXPECTED_INDEX_DELTA == -1)
            {
                it--;
                c.scroll = float(CONTAINER_FIRST_SLOT_LOCAL_Y) - it->y;
                it++;
            }


        }
//
//        int expectedIndexDelta;
//
//        if(START_X != CONTAINER_FIRST_SLOT_LOCAL_X || START_Y > CONTAINER_FIRST_SLOT_LOCAL_Y)
//        {
//            start
//        }
//
//        assert(CONTAINER_FIRST_SLOT_LOCAL_X == START_X);
//        assert(START_Y <= CONTAINER_FIRST_SLOT_LOCAL_Y);
//
//
//        c.scroll = float(CONTAINER_FIRST_SLOT_LOCAL_Y) - START_Y;
//        c.capacity = 1;
//        i++;
//        it++;
//        while(i < numContainerSlots)
//        {
//
//            if(containerSlotLocalCoordsToIndex(START_X, START_Y, it->x, it->y) != c.capacity)
//                break;
//
//            c.capacity++;
//            i++;
//            it++;
//        }
    }



}

size_t Gui::containerSlotLocalCoordsToIndex(float startX, float startY, float x, float y)
{
    using namespace Constants;
    static const size_t DELTA_X = TILE_PIXEL_WIDTH + CONTAINER_SEPARATOR_PIXEL_WIDTH;
    static const size_t DELTA_Y = TILE_PIXEL_HEIGHT + CONTAINER_SEPARATOR_PIXEL_WIDTH;
    size_t slotX = size_t(x - startX + 0.5f) / DELTA_X;
    size_t slotY = size_t(y - startY + 0.5f) / DELTA_Y;
    return slotX + slotY * CONTAINER_WIDTH;
}

void Gui::parseEquippedItems(size_t& i)
{
    const std::vector<SpriteDraw>& draws = *mCurrentFrame.guiSpriteDraws;

    if(i >= draws.size())
        return;

    while(i < draws.size())
    {
        const SpriteDraw& s = draws[i];

        if(s.topLeft.x <= Constants::NPC_TRADE_CURRENT_SLOT_LOCAL_X)
            return;

        i++;


        const Object* equippable = nullptr;
        for(const auto& pairing : s.pairings)
        {
            for(const Object* o : pairing.objects)
                if(o->itemInfo.bodyRestriction != Object::BodyRestriction::NONE)
                {
                    equippable = o;
                    break;
                }
            if(equippable)
                break;
        }

        assert(equippable);

        typedef Object::BodyRestriction B;
        switch(equippable->itemInfo.bodyRestriction)
        {
            case B::TWO_HANDED:
            case B::WEAPON:
            case B::SHIELD:
                if(mEquipment.find(EqType::HAND1) == mEquipment.end())
                    mEquipment[EqType::HAND1] = equippable;
                else
                {
                    assert(mEquipment.find(EqType::HAND2) == mEquipment.end());
                    mEquipment[EqType::HAND2] = equippable;
                }
                break;

            case B::HELMET: mEquipment[EqType::HEAD] = equippable; break;
            case B::AMULET: mEquipment[EqType::NECK] = equippable; break;
            case B::BACKPACK: mEquipment[EqType::BACK] = equippable; break;
            case B::ARMOR: mEquipment[EqType::TORSO] = equippable; break;
            case B::LEGS: mEquipment[EqType::LEGS] = equippable; break;
            case B::BOOTS: mEquipment[EqType::FEET] = equippable; break;
            case B::RING: mEquipment[EqType::FINGER] = equippable; break;
            case B::BELT: mEquipment[EqType::HIP] = equippable; break;

            default:
                THROW_RUNTIME_ERROR(sb::utility::stringify("Unexpected body restriction: ", (int)equippable->itemInfo.bodyRestriction, '\n'));
        }
    }
}

void Gui::parseBattleWindowSprites(size_t& i, const SideBottomWindow& w)
{
    const std::vector<SpriteDraw>& draws = *mCurrentFrame.guiSpriteDraws;
    if(i >= draws.size() || w.isMinimized)
        return;

    assert(w.hasResizer);
    assert(w.type == SideBottomWindow::Type::BATTLE);

    assert(mBattleWindow != nullptr);
    auto outfitIt = mBattleWindow->outfits.begin();
    while(i < draws.size())
    {
        const SpriteDraw& d = draws[i];
//        Vertex topLeft;
//        d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);
//        if(topLeft.x < w.resizer.pos.x || topLeft.y < w.exitButton.pos.y || topLeft.y > w.resizer.pos.y + w.resizer.size.y)
//            return;

        if(d.topLeft.x != Constants::BATTLE_WINDOW_SPRITE_LOCAL_X)
            break;

        assert(outfitIt != mBattleWindow->outfits.end());

        for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
            outfitIt->objects.insert(pairing.objects.begin(), pairing.objects.end());


        outfitIt++;
        i++;
    }

    assert(outfitIt == mBattleWindow->outfits.end());
}

void Gui::parseContainerSprites(size_t& i, Container& c)
{
    const std::vector<SpriteDraw>& draws = *mCurrentFrame.guiSpriteDraws;
    if(i >= draws.size() || c.window->isMinimized)
        return;

    if(draws[i].topLeft.x == Constants::SIDE_BOTTOM_WINDOW_ICON_LOCAL_X)
    {
        bool isContainerFound = false;

        for(const SpriteDraw::SpriteObjectPairing& pairing : draws[i].pairings)
        {
            for(const Object* o : pairing.objects)
                if(o->itemInfo.isContainer)
                {
                    isContainerFound = true;
                    break;
                }

            if(isContainerFound)
                break;
        }

        assert(isContainerFound);
        i++;
    }

    static const float START_X = Constants::CONTAINER_FIRST_SLOT_LOCAL_X;
    const float START_Y = Constants::CONTAINER_FIRST_SLOT_LOCAL_Y - c.scroll;
    static const int DELTA_X = Constants::TILE_PIXEL_WIDTH + Constants::CONTAINER_SEPARATOR_PIXEL_WIDTH;

    size_t containerIndex = 0;
    std::list<std::set<const Object*>> objects;
    while(i < draws.size())
    {
        const SpriteDraw& d = draws[i];
        if(int(d.topLeft.x - START_X) % DELTA_X != 0)
            break;

        if(containerIndex != containerSlotLocalCoordsToIndex(START_X, START_Y, d.topLeft.x, d.topLeft.y))
            break;

        std::set<const Object*> pickupables;
        for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
            for(const Object* o : pairing.objects)
                if(o->itemInfo.isPickupable)
                {
                    pickupables.insert(o);
                }

        assert(!pickupables.empty());
        objects.push_back(pickupables);


        containerIndex++;
        i++;
    }

    assert(objects.size() <= c.capacity);

    if(c.items.size() < objects.size())
        c.items.resize(objects.size());

    containerIndex = 0;
    for(const auto& set : objects)
    {
        auto& pair = c.items[containerIndex++];
        pair.second = set;
        if(pair.first == 0)
            pair.first = 1;
    }
}

void Gui::parseNpcTradeWindowSprites(size_t& i)
{
    assert(mNpcTradeWindow != nullptr);
    const std::vector<SpriteDraw>& draws = *mCurrentFrame.guiSpriteDraws;
    if(i >= draws.size() || mNpcTradeWindow->window->isMinimized)
        return;

    size_t offerIndex = 0;
    while(i < draws.size())
    {
        const SpriteDraw& d = draws[i];
        if(d.topLeft.x == Constants::NPC_TRADE_LIST_SLOT_LOCAL_X)
        {
            assert(offerIndex < mNpcTradeWindow->visibleOffers.size());
            NpcTradeWindow::Offer& offer = mNpcTradeWindow->visibleOffers[offerIndex];
            for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
                offer.objects.insert(pairing.objects.begin(), pairing.objects.end());
        }
        else if(d.topLeft.x == Constants::NPC_TRADE_CURRENT_SLOT_LOCAL_X)
        {
            assert(offerIndex == mNpcTradeWindow->visibleOffers.size());
//            std::set<const Object*> objects;
//            for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
//                for(const Object* object : pairing.objects)
//                    if(object->itemInfo.hasMarketInfo)
//                    {
//                        objects.insert(object);
//                    }
//
//            std::list<size_t> matchingOfferIndices;
//            for(size_t j = 0; j < mNpcTradeWindow->visibleOffers.size(); j++)
//                if(objects.find(mNpcTradeWindow->visibleOffers[j].object) != objects.end())
//                    matchingOfferIndices.push_back(j);
//
//            assert(matchingOfferIndices.size() == 1);
//            mNpcTradeWindow->selectedOfferIndex = matchingOfferIndices.front();
        }
        else
            return;

        i++;
        offerIndex++;
    }
}

void Gui::parseGuiSpriteDraws()
{
    size_t i = 0;
    parseEquippedItems(i);

    auto containerIt = mContainers.begin();
    for(const SideBottomWindow& w : mSideBottomWindows)
    {
        typedef SideBottomWindow::Type T;
        switch(w.type)
        {
            case T::BATTLE:
                parseBattleWindowSprites(i, w);
                break;

            case T::CONTAINER:
                assert(containerIt != mContainers.end());
                parseContainerSprites(i, *containerIt);
                containerIt++;
                break;

            case T::NPC_TRADE:
                parseNpcTradeWindowSprites(i);
                break;

            default:
                break;
        }
    }
}

void Gui::parseCurrentFrame()
{
    if(mIsCurrentFrameParsed)
        return;

    mButtons.clear();
    mEquipment.clear();
    mContainers.clear();
    mSideBottomWindows.clear();
    mNpcTradeWindow.release();
    mBattleWindow.release();
    mOnlineVip.clear();
    mOfflineVip.clear();

    mHalfFrameWidth = mCurrentFrame.width / 2.f;
    mHalfFrameHeight = mCurrentFrame.height / 2.f;

    std::map<std::string, std::list<const GuiDraw*>> guiDraws;
    for(const GuiDraw& g : *mCurrentFrame.guiDraws)
        guiDraws[sb::utility::file::basename(g.name)].push_back(&g);


//    for(const GuiDraw& d : *mCurrentFrame.guiDraws)
//    {
//        Vertex topLeft;
//        d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);
//        Vertex botRight;
//        d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, botRight.x, botRight.y, d.botRight.x, d.botRight.y);
//        float width = botRight.x - topLeft.x;
//        float height = botRight.y - topLeft.y;
//        std::cout << "Gui: \"" << basename(d.name) << "\" " << d.topLeft.x << "x" << d.topLeft.y << " -> " << topLeft.x << "x" << topLeft.y << " (" << width << "x" << height << ")" << std::endl;
//    }

//
//    for(const TextDraw& d : *mCurrentFrame.textDraws)
//    {
//        TextBuilder builder(d, mCurrentFrame.width, mCurrentFrame.height);
//        std::cout << (int)builder.getTextType() << std::endl;
//        for(const Text& t : builder.getText())
//        {
//            std::cout << "\t\"" << t.string << "\"" << "\t" << t.localX << "x" << t.localY << " ->" << t.x << "x" << t.y << std::endl;
//        }
//    }

    updateButtons(guiDraws);
    if(mCurrentState == State::GAME)
    {
        size_t textDrawsIndex = 0;
        parseNames(textDrawsIndex);
        parseChat(textDrawsIndex);
        parseSideWindowTopText(textDrawsIndex);
        updateSideBottomWindows(guiDraws);
        parseContainerSlots(guiDraws);
        parseSideWindowBottomText(textDrawsIndex);
        parseGuiSpriteDraws();
    }
    else if(mCurrentState == State::MAIN_MENU)
    {
        for(const TextDraw& d : *mCurrentFrame.textDraws)
        {
            TextBuilder builder(d, mCurrentFrame.width, mCurrentFrame.height);
            if(builder.getTextType() == Text::Type::GUI)
            {
                for(const Text& t : builder.getText())
                    setButtonText(t);
            }
        }
    }
    else
    {
        THROW_RUNTIME_ERROR(sb::utility::stringify("Unexpected state: ", (int)mCurrentState, "\n"));
    }

//    std::cout << "Num side bottom windows: " << mSideBottomWindows.size() << std::endl;


//    for(auto str : mOnlineVip)
//        std::cout << "Online: " << str << std::endl;
//    for(auto str : mOfflineVip)
//        std::cout << "Offline: " << str << std::endl;

//    std::cout << "Containers: " << mContainers.size() << std::endl;
//    for(const Container& c : mContainers)
//    {
//        std::cout << "\t" << "Cap: " << (int)c.capacity << std::endl;
//        std::cout << "\t" << "Scroll: " << (int)c.scroll << std::endl;
//    }

//    std::cout << "Contents of battle window: " << std::endl;
//    for(const BattleWindow::Outfit& o : mBattleWindow->outfits)
//    {
//        std::cout << "\t" << o.name << std::endl;
//    }
//
//    if(mNpcTradeWindow != nullptr)
//    {
//        const NpcTradeWindow& w = *mNpcTradeWindow;
//        std::cout << "NPC TRADE DUMP: " << std::endl;
//        std::cout << "\tOffers: " << std::endl;
//        for(const auto& offer : w.visibleOffers)
//        {
//            std::cout   << "\t\tName: " << offer.name << std::endl
//                        << "\t\tCost: " << offer.cost << std::endl
//                        << "\t\tWeight: " << offer.weight << std::endl
//                        << "\t\tAffordable: " << (int)offer.isAffordable << std::endl
//                        << "\t\tObjects: ";
//            for(const Object* o : offer.objects)
//                std::cout << o->id << " ";
//            std::cout << std::endl;
//        }
//        std::cout   << "\tSelected: " << w.selectedOfferIndex << std::endl
//                    << "\tTotal price: " << w.totalPrice << std::endl
//                    << "\tAmount: " << w.amount << std::endl
//                    << "\tAvailable money: " << w.availableMoney << std::endl
//                    << "\tTab: " << (int)w.currentTab << std::endl;
//    }
//
//    if(mBattleWindow != nullptr)
//    {
//        const BattleWindow& w = *mBattleWindow;
//
//        std::cout << "BATTLE WINDOW DUMP: " << std::endl;
//        std::cout << "\tOutfits: " << std::endl;
//        for(const auto& outfit : w.outfits)
//        {
//            std::cout   << "\t\tName: " << outfit.name << std::endl
//                        << "\t\tHp percent: " << outfit.hpPercent << std::endl
//                        << "\t\tObjects: ";
//            for(const Object* object : outfit.objects)
//                std::cout << object->id << " ";
//            std::cout << std::endl;
//
//        }
//        if(w.selectedOutfit)
//            std::cout << "\tSelected outfit: " << w.selectedOutfit->name << std::endl;
//    }
//
//    if(!mContainers.empty())
//    {
//        std::cout << "CONTAINER DUMP (" << mContainers.size() << "):" << std::endl;
//        for(const Container& c : mContainers)
//        {
//            std::cout   << "\tName: " << c.name << std::endl
//                        << "\tCapacity: " << (int)c.capacity << std::endl
//                        << "\tScroll: " << c.scroll << std::endl
//                        << "\tNum items: " << c.items.size() << std::endl;
//
//            if(!c.items.empty())
//            {
//                std::cout << "\tItems: " << std::endl;
//                for(const auto& pair : c.items)
//                {
//                    std::cout <<  "\t\tCount: " << pair.first << ", Objects: ";
//                    for(const Object* object : pair.second)
//                        std::cout << object->id << " ";
//                    std::cout << std::endl;
//                }
//            }
//        }
//
//
//
//    }




//    if(guiDraws.find("containerslot.png") != guiDraws.end())
//        for(const GuiDraw* d : guiDraws.find("containerslot.png")->second)
//        {
//            float width = d->botRight.x - d->topLeft.x;
//            float height = d->botRight.y - d->topLeft.y;
//
//            Vertex topLeft;
//            d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);
//
//            if(width > 30 && height > 30)
//            {
//                std::cout << "ContainerSlot: " << d->topLeft.x << "x" << d->topLeft.y << " -> " << topLeft.x << "x" << topLeft.y << " (" << width << "x" << height << ")" << std::endl;
//            }
//        }


//    for(const SpriteDraw& d : *mCurrentFrame.guiSpriteDraws)
//    {
//        const Object* sprite = *d.pairings.front().objects.begin();
//        Vertex topLeft;
//        d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y);
//
//        std::cout << "Sprite: " << sprite->id << " " << d.topLeft.x << "x" << d.topLeft.y << " -> " << topLeft.x << "x" << topLeft.y;
//        if(sprite->itemInfo.hasMarketInfo)
//            std::cout << " (" << sprite->itemInfo.marketInfo.name << ")";
//        std::cout << std::endl;
//    }





//    updateSideBottomWindows(guiDraws);
//    updateButtons(guiDraws);
//    parseText(*mCurrentFrame.textDraws);
//    updateItems(guiDraws, *mCurrentFrame.guiSpriteDraws);
//    updateManaAndHpLevels(guiDraws);

////    auto backgroundsIt = guiDraws.find("background.png");
////    if(backgroundsIt != guiDraws.end())
//    for(const auto& pair : guiDraws)
//    {
////        for(const GuiDraw* d : backgroundsIt->second)
//        for(const GuiDraw* d : pair.second)
//        {
//            Vertex topLeft;
//            Vertex botRight;
//            d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y, d->topLeft.x, d->topLeft.y);
//            d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, botRight.x, botRight.y, d->botRight.x, d->botRight.y);
//
//            float width = botRight.x - topLeft.x;
//            float height = botRight.y - topLeft.y;
////            std::cout << "BG: " << topLeft.x << "x" << topLeft.y << " " << width << "x" << height << std::endl;
//            std::cout << pair.first << ": " << topLeft.x << "x" << topLeft.y << " " << width << "x" << height << std::endl;
//        }
//    }



//    for(const RectDraw& rect : *mCurrentFrame.rectDraws)
//    {
//        typedef Constants::RectColor Color;
//        float screenX;
//        float screenY;
//        rect.getScreenCoords(mCurrentFrame.width / 2, mCurrentFrame.height / 2, screenX, screenY);
//        float rectWidth = rect.botRight.x - rect.topLeft.x;
//        float rectHeight = rect.botRight.y - rect.topLeft.y;
////        worldToScreenCoords(rect.topLeft.x, rect.v.y, *rect.transform, mCurrentFrame.width / 2, mCurrentFrame.height / 2, screenX, screenY);
//        switch((Color)rect.color.packed)
//        {
//            case Color::TEXT_INPUT_FIELD:
//                std::cout << "TEXT_INPUT_FIELD at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::GUI_TEXT_UNDERLINE: // same as TEXT_CURSOR_MENUS and HP_GRAY
//                if(rectWidth > rectHeight)
//                {
//                    if(screenX > mCurrentFrame.viewX + mCurrentFrame.viewWidth)
//                        std::cout << "GUI_TEXT_UNDERLINE at " << screenX << "x" << screenY << std::endl;
//                    else
//                        std::cout << "HP_GRAY at " << screenX << "x" << screenY << std::endl;
//                }
//                else
//                    std::cout << "TEXT_CURSOR_MENUS at " << screenX << "x" << screenY << std::endl;
//                break;
//
//            case Color::TEXT_CURSOR_CHAT:
//                std::cout << "TEXT_CURSOR_CHAT at " << screenX << "x" << screenY << std::endl;
//                break;
//
//            case Color::TEXT_SELECTION_BOX:
//                std::cout << "TEXT_SELECTION_BOX at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::LIST_MENU_BACKGROUND: // same as LIST_MENU_UNSELECTED_ENTRY_BOX
//                if(rectHeight * 4 > rectWidth)
//                    std::cout << "LIST_MENU_BACKGROUND at " << screenX << "x" << screenY << std::endl;
//                else
//                    std::cout << "LIST_MENU_UNSELECTED_ENTRY_BOX at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::LIST_MENU_SELECTED_ENTRY_BOX:
//                std::cout << "LIST_MENU_SELECTED_ENTRY_BOX at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::HP_FULL: // same as SKILL_BAR_GREEN and HP_FULL_BATTLE_LIST
//                if(screenX > mCurrentFrame.viewX + mCurrentFrame.viewWidth)
//                {
//                    // TODO: BATTLE_LIST
//                    std::cout << "SKILL_BAR_GREEN at " << screenX << "x" << screenY << ", " << rectWidth << "x" << rectHeight <<  std::endl;
//                }
//                else
//                    std::cout << "HP_FULL at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::HP_LIGHT_GREEN: // same as HP_LIGHT_GREEN_BATTLE_LIST
//                // TODO: BATTLE_LIST
//                std::cout << "HP_LIGHT_GREEN at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::HP_YELLOW: // same as HP_YELLOW_BATTLE_LIST
//                // TODO: BATTLE_LIST
//                std::cout << "HP_YELLOW at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::HP_LIGHT_RED: // same asHP_LIGHT_RED _BATTLE_LIST
//                // TODO: BATTLE_LIST
//                std::cout << "HP_LIGHT_RED at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::HP_RED: // same as SKILL_BAR_RED and HP_RED_BATTLE_LIST
//                // TODO: BATTLE_LIST
//                if(screenX > mCurrentFrame.viewX + mCurrentFrame.viewWidth)
//                    std::cout << "SKILL_BAR_RED at " << screenX << "x" << screenY << std::endl;
//                else
//                    std::cout << "HP_RED at " << screenX << "x" << screenY << std::endl;
//                break;
//            case Color::HP_DARK_RED: // same as HP_DARK_RED_BATTLE_LIST
//                // TODO: BATTLE_LIST
//                std::cout << "HP_DARK_RED at " << screenX << "x" << screenY << std::endl;
//                break;
//
//            case Color::SKILL_BAR_GREEN_FULL:
//                std::cout << "SKILL_BAR_GREEN_FULL at " << screenX << "x" << screenY << ", " << rectWidth << "x" << rectHeight <<  std::endl;
//                break;
//
//            case Color::HP_BACKGROUND: // same as SCENE_CLEAR_COLOR
//                if(rectHeight * 4 < rectWidth)
//                    std::cout << "HP_BACKGROUND at " << screenX << "x" << screenY << std::endl;
//                else
//                    std::cout << "SCENE_CLEAR_COLOR at " << screenX << "x" << screenY << std::endl;
//                break;
//
//            default:
//                std::cout << "UNKNOWN: " << screenX << "x" << screenY << ", " << rectWidth << "x" << rectHeight << " (" << (int)rect.color.r << " " << (int)rect.color.g << " " << (int)rect.color.b << " " << (int)rect.color.a << ")" << std::endl;
//                break;
//        }
//    }

    auto removeStart = std::remove_if(mButtons.begin(), mButtons.end(), [](const Button& b){return (b.text == "");});
    mButtons.erase(removeStart, mButtons.end());


    mIsCurrentFrameParsed = true;
}

Gui::State Gui::getState() const
{
    return mCurrentState;
}

const std::list<Gui::Button>& Gui::getButtons()
{
    parseCurrentFrame();
    return mButtons;
}

unsigned short Gui::getCap()
{
    parseCurrentFrame();
    return mCap;
}

unsigned short Gui::getSoul()
{
    parseCurrentFrame();
    return mSoul;
}

unsigned short Gui::getHp()
{
    parseCurrentFrame();
    return mHp;
}

unsigned short Gui::getMana()
{
    parseCurrentFrame();
    return mMana;
}

float Gui::getHpLevel()
{
    parseCurrentFrame();
    return mHpLevel;
}

float Gui::getManaLevel()
{
    parseCurrentFrame();
    return mManaLevel;
}

unsigned short Gui::getLevel()
{
    return mLevel;
}

unsigned int Gui::getExperience()
{
    return mExperience;
}

unsigned short Gui::getXpGainRate()
{
    return mXpGainRate;
}

unsigned short Gui::getSpeed()
{
    return mSpeed;
}

unsigned short Gui::getFoodMinutes()
{
    return mFoodMinutes;
}

unsigned short Gui::getStaminaMinutes()
{
    return mStaminaMinutes;
}

unsigned short Gui::getOfflineTrainingMinutes()
{
    return mOfflineTrainingMinutes;
}

unsigned short Gui::getMagicLevel()
{
    return mMagicLevel;
}

unsigned short Gui::getFistFighting()
{
    return mFistFighting;
}

unsigned short Gui::getClubFighting()
{
    return mClubFighting;
}

unsigned short Gui::getSwordFighting()
{
    return mSwordFighting;
}

unsigned short Gui::getAxeFighting()
{
    return mAxeFighting;
}

unsigned short Gui::getDistanceFighting()
{
    return mDistanceFighting;
}

unsigned short Gui::getShielding()
{
    return mShielding;
}

unsigned short Gui::getFishing()
{
    return mFishing;
}

unsigned short Gui::getCritChance()
{
    return mCritChance;
}

unsigned short Gui::getCritDamage()
{
    return mCritDamage;
}

unsigned short Gui::getHpLeechChance()
{
    return mHpLeechChance;
}

unsigned short Gui::getHpLeechAmount()
{
    return mHpLeechAmount;
}

unsigned short Gui::getManaLeechChance()
{
    return mManaLeechChance;
}

unsigned short Gui::getManaLeechAmount()
{
    return mManaLeechAmount;
}

const std::map<Gui::EqType, const Object*>& Gui::getEquipment()
{
    return mEquipment;
}

const std::list<Gui::Container>& Gui::getContainers()
{
    return mContainers;
}
