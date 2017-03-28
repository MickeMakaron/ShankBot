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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Gui.hpp"
#include "monitor/TextBuilder.hpp"
#include "utility/utility.hpp"
#include "utility/file.hpp"
#include "monitor/TibiaContext.hpp"
using namespace sb::tibiaassets;
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
#include <iostream>
#include <cassert>
///////////////////////////////////

Gui::Gui(const TibiaContext& context)
: mContext(context)
, mGuiDrawHandlers(initGuiDrawHandlers())
{
}

void Gui::update(const Frame& frame)
{
    mCurrentFrame = frame;
    mData = Data();
    pass1 = Pass1();
    mData.state = mCurrentFrame.viewWidth > 0 ? State::GAME : State::MAIN_MENU;
    mIsCurrentFrameParsed = false;
}

Gui::State Gui::getState() const
{
    return mData.state;
}

const Gui::Data& Gui::getData()
{
    parseCurrentFrame();
    return mData;
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

void Gui::createButtons
(
    const std::map<std::string, std::list<const GuiDraw*>>& guiDraws,
    const std::vector<std::string>& names,
    bool isDown
)
{
    for(const std::string& name : names)
    {
        auto it = guiDraws.find(name);
        if(it != guiDraws.end())
        {
            for(const GuiDraw* guiDraw : it->second)
            {
                float x;
                float y;
                sb::utility::worldToScreenCoords
                (
                    guiDraw->topLeft.x, guiDraw->topLeft.y,
                    *guiDraw->transform,
                    mHalfFrameWidth, mHalfFrameHeight,
                    x, y
                );
                std::shared_ptr<Button> b(new Button());
                b->x = x;
                b->y = y;
                b->width = guiDraw->botRight.x - guiDraw->topLeft.x;
                b->height = guiDraw->botRight.y - guiDraw->topLeft.y;
                b->isDown = isDown;
                mData.buttons.push_back(b);
            }
        }
    }
}

void Gui::updateButtons(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws)
{
    static const std::vector<std::string> upButtonNames =
    {
        "button-9grid-idle.png",
        "console-tab-passive.png",
    };
    static const std::vector<std::string> downButtonNames =
    {
        "button-9grid-pressed.png",
        "console-tab-active.png",
    };

    createButtons(guiDraws, upButtonNames, false);
    createButtons(guiDraws, downButtonNames, true);
}

void Gui::updateManaAndHpLevels(const std::map<std::string, std::list<const GuiDraw*>>& guiDraws)
{
    auto hpIt = guiDraws.find("hitpoints-bar-filled.png");
    if(hpIt == guiDraws.end())
        return;
    assert(hpIt->second.size() == 1);

    auto manaIt = guiDraws.find("mana-bar-filled.png");
    if(manaIt == guiDraws.end())
    {
        std::cout << "Warning: Expected to find mana-bar-filled. " << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl;
        return;
    }
    assert(manaIt->second.size() == 1);


    auto bordersIt = guiDraws.find("hitpoints-manapoints-bar-border.png");
    assert(bordersIt != guiDraws.end());
    assert(bordersIt->second.size() == 2);

    const GuiDraw& hpBar = *hpIt->second.front();
    const GuiDraw& hpBorder = *bordersIt->second.front();

    const GuiDraw& manaBar = *manaIt->second.front();
    const GuiDraw& manaBorder = *bordersIt->second.back();

    mData.hpLevel = (hpBar.botRight.x - hpBar.topLeft.x) / (hpBorder.botRight.x - hpBorder.topLeft.x);
    mData.manaLevel = (manaBar.botRight.x - manaBar.topLeft.x) / (manaBorder.botRight.x - manaBorder.topLeft.x);
}

std::shared_ptr<Gui::Button> Gui::setButtonText(const Text& text)
{
    for(std::shared_ptr<Button>& bPtr : mData.buttons)
    {
        Button& b = *bPtr;
        if(b.text == "")
        {
            if(b.x <= text.x && b.y <= text.y)
            {
                if(text.x - b.x < b.width && text.y - b.y < b.height)
                {
                    b.text = text.string;
                    return bPtr;
                }
            }
        }
    }

    return nullptr;
}

void Gui::parseVipWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w)
{
    if(i >= mCurrentFrame.textDraws->size() || w->isMinimized)
        return;

    std::unique_ptr<TextBuilder> builder;
    builder.reset(new TextBuilder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height));
    if(builder->getTextType() != Text::Type::VIP_ONLINE && builder->getTextType() != Text::Type::VIP_OFFLINE)
        return;

    std::vector<std::string>* currentList;
    currentList = (builder->getTextType() == Text::Type::VIP_ONLINE) ? &mData.onlineVips : &mData.offlineVips;

    for(const Text& t : builder->getText())
        currentList->push_back(t.string);

    i++;

    if(i >= mCurrentFrame.textDraws->size())
        return;

    builder.reset(new TextBuilder((*mCurrentFrame.textDraws)[i], mCurrentFrame.width, mCurrentFrame.height));
    if(builder->getTextType() == Text::Type::VIP_ONLINE || builder->getTextType() == Text::Type::VIP_OFFLINE)
    {
        currentList = (builder->getTextType() == Text::Type::VIP_ONLINE) ? &mData.onlineVips : &mData.offlineVips;
        for(const Text& t : builder->getText())
            currentList->push_back(t.string);

        i++;
    }
}

void Gui::parseBattleWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w)
{
    if(i >= mCurrentFrame.textDraws->size() || w->isMinimized)
        return;

    Rect r;
    r.pos.x = w->resizer.pos.x;
    r.pos.y = w->exitButton.pos.y;
    r.size.x = w->exitButton.pos.x + w->exitButton.size.x - r.pos.x;
    r.size.y = w->resizer.pos.y + w->resizer.size.y - r.pos.y;

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

            mData.battleWindow->outfits.emplace_back();
            std::shared_ptr<BattleWindow::Outfit>& o = mData.battleWindow->outfits.back();
            o->name = t.string;
            if(t.type == Text::Type::NAME_BATTLE_WINDOW_HIGHLIGHTED)
            {
                assert(text.size() == 1);
                mData.battleWindow->selectedOutfit = o;
            }
        }

        i++;
    }
}


void Gui::parseSkillWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w)
{
    if(i >= mCurrentFrame.textDraws->size() || w->isMinimized)
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
    mData.level = strToInt(it->string);
    it++;

    assert(it->string == "  Experience");
    it++;
    assert(isNumeric(it->string));
    mData.experience = strToInt(it->string);
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
        mData.xpGainRate = parsePercentageText(xpGainRate.front().string);
    }
    else
    {
        mData.xpGainRate = parsePercentageText(it->string);
        it++;
    }

    assert(it->string == "Hit Points");
    it++;
    assert(isNumeric(it->string));
    mData.hp = strToInt(it->string);
    it++;

    assert(it->string == "Mana");
    it++;
    assert(isNumeric(it->string));
    mData.mana = strToInt(it->string);
    it++;

    assert(it->string == "Soul Points");
    it++;
    assert(isNumeric(it->string));
    mData.soul = strToInt(it->string);
    it++;

    assert(it->string == "Capacity");
    it++;
    assert(isNumeric(it->string));
    mData.cap = strToInt(it->string);
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
        mData.speed = strToInt(speed.front().string);
    }
    else
    {
        assert(isNumeric(it->string));
        mData.speed = strToInt(it->string);
        it++;
    }

    assert(it->string == "Food");
    it++;
    mData.foodMinutes = parseTimeText(it->string);
    it++;

    assert(it->string == "Stamina");
    it++;
    mData.staminaMinutes = parseTimeText(it->string);
    it++;

    assert(it->string == "Offline Training");
    it++;
    mData.offlineTrainingMinutes = parseTimeText(it->string);
    it++;

    assert(it->string == "Magic Level");
    it++;
    assert(isNumeric(it->string));
    mData.magicLevel = strToInt(it->string);
    it++;

    assert(it->string == "Fist Fighting");
    it++;
    assert(isNumeric(it->string));
    mData.fistLevel = strToInt(it->string);
    it++;

    assert(it->string == "Club Fighting");
    it++;
    assert(isNumeric(it->string));
    mData.clubLevel = strToInt(it->string);
    it++;

    assert(it->string == "Sword Fighting");
    it++;
    assert(isNumeric(it->string));
    mData.swordLevel = strToInt(it->string);
    it++;

    assert(it->string == "Axe Fighting");
    it++;
    assert(isNumeric(it->string));
    mData.axeLevel = strToInt(it->string);
    it++;

    assert(it->string == "Distance Fighting");
    it++;
    assert(isNumeric(it->string));
    mData.distanceLevel = strToInt(it->string);
    it++;

    assert(it->string == "Shielding");
    it++;
    assert(isNumeric(it->string));
    mData.shieldingLevel = strToInt(it->string);
    it++;

    assert(it->string == "Fishing");
    it++;
    assert(isNumeric(it->string));
    mData.fishingLevel = strToInt(it->string);
    it++;

    assert(it->string == "Critical Hit:");
    it++;

    assert(it->string == "  Chance");
    it++;
    mData.critChance = parsePercentageText(it->string);
    it++;

    assert(it->string == "  Extra Damage");
    it++;
    mData.critDamage = parsePercentageText(it->string);
    it++;

    assert(it->string == "Hit Points Leech:");
    it++;

    assert(it->string == "  Chance");
    it++;
    mData.hpLeechChance = parsePercentageText(it->string);
    it++;

    assert(it->string == "  Amount");
    it++;
    mData.hpLeechAmount = parsePercentageText(it->string);
    it++;

    assert(it->string == "Mana Leech:");
    it++;

    assert(it->string == "  Chance");
    it++;
    mData.manaLeechChance = parsePercentageText(it->string);
    it++;

    assert(it->string == "  Amount");
    it++;
    mData.manaLeechAmount = parsePercentageText(it->string);
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
        mData.sideBottomWindows.insert(mData.sideBottomWindows.begin(), std::make_shared<SideBottomWindow>());
        SideBottomWindow& w = *mData.sideBottomWindows.front();

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
        for(std::shared_ptr<SideBottomWindow>& wPtr : mData.sideBottomWindows)
        {
            SideBottomWindow& w = *wPtr;
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
            case T::CLICKABLE_NPC_TEXT:
            case T::SOUND:
                break;

            case T::SELF_PM:
                mData.chatInput= "";
                for(const Text& t : builder.getText())
                {
                    mData.chatInput += t.string;
                }
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

        for(const Text& t : builder.getText())
        {
            auto tab = setButtonText(t);
            assert(tab != nullptr);
        }

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

            mData.soul = strToInt(soul->string);
            mData.cap = strToInt(cap->string);

            parsedTypes.insert(SOUL_CAP);
        }
        else if(isNumeric(title.string))
        {
            assert(text.size() >= 2);

            mData.hp = strToInt(text.front().string);
            text.pop_front();
            assert(isNumeric(text.front().string));
            mData.mana = strToInt(text.front().string);
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
    auto containerIt = mData.containers.begin();
    for(std::shared_ptr<SideBottomWindow>& w : mData.sideBottomWindows)
    {
        assert(i < mCurrentFrame.textDraws->size());
        TextBuilder builder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height);

        assert(builder.getTextType() == Text::Type::GUI);

        std::list<Text> text = builder.getText();
        assert(!text.empty());
        const std::string& str = text.front().string;
        if(str == "Skills")
        {
            w->type = SideBottomWindow::Type::SKILLS;
            parseSkillWindowText(i, w);
        }
        else if(str == "Battle")
        {
            w->type = SideBottomWindow::Type::BATTLE;
            mData.battleWindow.reset(new BattleWindow());
            parseBattleWindowText(i, w);
        }
        else if(str == "VIP")
        {
            w->type = SideBottomWindow::Type::VIP;
            parseVipWindowText(i, w);
        }
        else if(str == "Unjustified Points")
        {
            w->type = SideBottomWindow::Type::UNJUSTIFIED_POINTS;
        }
        else if(str == "NPC Trade")
        {
            w->type = SideBottomWindow::Type::NPC_TRADE;
            mData.npcTradeWindow.reset(new NpcTradeWindow());
            parseNpcTradeWindowText(i, w);
        }
        else if(str == "Browse Field")
        {
            THROW_RUNTIME_ERROR("Don't forget to implement Browse Field! :-)");
        }
        else if(w->isMinimized)
        {
            w->type = SideBottomWindow::Type::CONTAINER;
            Container& c = *mData.containers.insert(containerIt, Container());
            parseContainerText(i, c, str, w);
        }
        else if(containerIt != mData.containers.end())
        {
            w->type = SideBottomWindow::Type::CONTAINER;
            parseContainerText(i, *containerIt, str, w);
            containerIt++;
        }
        else
            THROW_RUNTIME_ERROR("Unexpected text: \"" + str + "\".");
    }
}

void Gui::parseNpcTradeWindowText(size_t& i, const std::shared_ptr<SideBottomWindow>& w)
{
    mData.npcTradeWindow->window = w;

    if(i >= mCurrentFrame.textDraws->size() || w->isMinimized)
        return;

     std::list<Text> text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
     assert(text.size() == 2 || text.size() == 3);

     assert(text.front().string == "Buy");
     mData.npcTradeWindow->buyButton = setButtonText(text.front());
     assert(mData.npcTradeWindow->buyButton);

     text.pop_front();

     assert(text.front().string == "Sell");
     mData.npcTradeWindow->sellButton = setButtonText(text.front());
     assert(mData.npcTradeWindow->sellButton);

     text.pop_front();

     if(!text.empty())
     {
        assert(text.front().string == "ok");
        mData.npcTradeWindow->okButton = setButtonText(text.front());
        assert(mData.npcTradeWindow->okButton);
     }




     assert(mData.npcTradeWindow->buyButton->isDown || mData.npcTradeWindow->sellButton->isDown);
     if(mData.npcTradeWindow->buyButton->isDown)
        mData.npcTradeWindow->currentTab = NpcTradeWindow::Tab::BUY;
     else
        mData.npcTradeWindow->currentTab = NpcTradeWindow::Tab::SELL;


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

            mData.npcTradeWindow->visibleOffers.push_back(offer);
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
    mData.npcTradeWindow->amount = strToInt(it->string);
    it++;

    assert(it->string == "Price:");
    it++;
    assert(isNumeric(it->string));
    mData.npcTradeWindow->totalPrice = strToInt(it->string);
    it++;

    assert(it->string == "Gold:");
    it++;
    assert(isNumeric(it->string));
    mData.npcTradeWindow->availableMoney = strToInt(it->string);
    it++;

    assert(it == text.end());

    if(!mData.npcTradeWindow->okButton)
    {
        assert(i < mCurrentFrame.textDraws->size());
        text = TextBuilder((*mCurrentFrame.textDraws)[i++], mCurrentFrame.width, mCurrentFrame.height).getText();
        assert(text.size() == 1);
        assert(text.front().string == "ok");
        mData.npcTradeWindow->okButton = setButtonText(text.front());
        assert(mData.npcTradeWindow->okButton);
    }
}

void Gui::parseContainerText(size_t& i, Container& c, const std::string& str, const std::shared_ptr<SideBottomWindow>& w)
{
    using namespace sb::utility;
    c.name = toLower(str);
    c.window = w;

    if(i >= mCurrentFrame.textDraws->size() || w->isMinimized)
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
        mData.containers.emplace_back();
        Container& c = mData.containers.back();

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


        size_t equippable = -1;
        for(const auto& pairing : s.pairings)
        {
            for(size_t objIndex : pairing.objects)
            {
                const Object& o = mContext.getObjects()[objIndex];
                if(o.itemInfo.bodyRestriction != Object::BodyRestriction::NONE)
                {
                    equippable = objIndex;
                    break;
                }
            }
            if(equippable != -1)
                break;
        }

        assert(equippable != -1);

        typedef Object::BodyRestriction B;
        switch(mContext.getObjects()[equippable].itemInfo.bodyRestriction)
        {
            case B::TWO_HANDED:
            case B::WEAPON:
            case B::SHIELD:
                if(mData.equipment.find(EqType::HAND1) == mData.equipment.end())
                    mData.equipment[EqType::HAND1] = equippable;
                else
                {
                    assert(mData.equipment.find(EqType::HAND2) == mData.equipment.end());
                    mData.equipment[EqType::HAND2] = equippable;
                }
                break;

            case B::HELMET: mData.equipment[EqType::HEAD] = equippable; break;
            case B::AMULET: mData.equipment[EqType::NECK] = equippable; break;
            case B::BACKPACK: mData.equipment[EqType::BACK] = equippable; break;
            case B::ARMOR: mData.equipment[EqType::TORSO] = equippable; break;
            case B::LEGS: mData.equipment[EqType::LEGS] = equippable; break;
            case B::BOOTS: mData.equipment[EqType::FEET] = equippable; break;
            case B::RING: mData.equipment[EqType::FINGER] = equippable; break;
            case B::BELT: mData.equipment[EqType::HIP] = equippable; break;

            default:
                THROW_RUNTIME_ERROR(sb::utility::stringify("Unexpected body restriction: ", (int)mContext.getObjects()[equippable].itemInfo.bodyRestriction, '\n'));
        }
    }
}

void Gui::parseBattleWindowSprites(size_t& i, const std::shared_ptr<SideBottomWindow>& w)
{
    const std::vector<SpriteDraw>& draws = *mCurrentFrame.guiSpriteDraws;
    if(i >= draws.size() || w->isMinimized)
        return;

    assert(w->hasResizer);
    assert(w->type == SideBottomWindow::Type::BATTLE);

    assert(mData.battleWindow != nullptr);
    auto outfitIt = mData.battleWindow->outfits.begin();
    while(i < draws.size())
    {
        const SpriteDraw& d = draws[i];

        if(d.topLeft.x != Constants::BATTLE_WINDOW_SPRITE_LOCAL_X)
            break;

        assert(outfitIt != mData.battleWindow->outfits.end());

        for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
            (*outfitIt)->objects.insert(pairing.objects.begin(), pairing.objects.end());


        outfitIt++;
        i++;
    }

    assert(outfitIt == mData.battleWindow->outfits.end());
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
            for(size_t o : pairing.objects)
                if(mContext.getObjects()[o].itemInfo.isContainer)
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
    std::list<std::set<size_t>> objects;
    while(i < draws.size())
    {
        const SpriteDraw& d = draws[i];
        if(int(d.topLeft.x - START_X) % DELTA_X != 0)
            break;

        if(containerIndex != containerSlotLocalCoordsToIndex(START_X, START_Y, d.topLeft.x, d.topLeft.y))
            break;

        std::set<size_t> pickupables;
        for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
            for(size_t o : pairing.objects)
                if(mContext.getObjects()[o].itemInfo.isPickupable)
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
    assert(mData.npcTradeWindow != nullptr);
    const std::vector<SpriteDraw>& draws = *mCurrentFrame.guiSpriteDraws;
    if(i >= draws.size() || mData.npcTradeWindow->window->isMinimized)
        return;

    size_t offerIndex = 0;
    while(i < draws.size())
    {
        const SpriteDraw& d = draws[i];
        if(d.topLeft.x == Constants::NPC_TRADE_LIST_SLOT_LOCAL_X)
        {
            assert(offerIndex < mData.npcTradeWindow->visibleOffers.size());
            NpcTradeWindow::Offer& offer = mData.npcTradeWindow->visibleOffers[offerIndex];
            for(const SpriteDraw::SpriteObjectPairing& pairing : d.pairings)
                offer.objects.insert(pairing.objects.begin(), pairing.objects.end());
        }
        else if(d.topLeft.x == Constants::NPC_TRADE_CURRENT_SLOT_LOCAL_X)
        {
            assert(offerIndex == mData.npcTradeWindow->visibleOffers.size());
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

    auto containerIt = mData.containers.begin();
    for(const std::shared_ptr<SideBottomWindow>& w : mData.sideBottomWindows)
    {
        typedef SideBottomWindow::Type T;
        switch(w->type)
        {
            case T::BATTLE:
                parseBattleWindowSprites(i, w);
                break;

            case T::CONTAINER:
                assert(containerIt != mData.containers.end());
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

Gui::IRect Gui::getRect(Vertex topLeft, Vertex botRight)

{
    if(botRight.x < topLeft.x || botRight.y < topLeft.y)
    {
        std::swap(topLeft, botRight);
    }

    IRect ir;
    ir.x = topLeft.x + 0.5f;
    ir.y = topLeft.y + 0.5f;
    ir.width = botRight.x - topLeft.x + 0.5f;
    ir.height = botRight.y - topLeft.y + 0.5f;

    return ir;
}

Gui::IRect Gui::getScreenRect(const Draw& d)
{
    Vertex topLeft;
    Vertex botRight;
    d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y, d.topLeft.x, d.topLeft.y);
    d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, botRight.x, botRight.y, d.botRight.x, d.botRight.y);

    return getRect(topLeft, botRight);
}

Gui::DrawRect Gui::getDrawRect(const Draw& d)

{
    DrawRect r;
    r.local = getRect(d.topLeft, d.botRight);
    r.screen = getScreenRect(d);
    return r;
}

Gui::TmpButton Gui::createButton(const Draw& d, bool isDown, bool isEnabled)
{
    TmpButton b;
    b.screenRect = getScreenRect(d);
    b.localRect = getRect(d.topLeft, d.botRight);
    b.isDown = isDown;
    b.isEnabled = isEnabled;
    return b;
}

std::map<std::string, std::function<void(const GuiDraw&)>> Gui::initGuiDrawHandlers()
{


    auto createButtonHandlers = [this]
    (
        const std::vector<std::pair<std::string, TmpButton&>>& buttons,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            TmpButton& b = pair.second;
            handlers[pair.first + up] = [&b, this](const GuiDraw& d)
            {
                b = createButton(d, false);
            };
            handlers[pair.first + down] = [&b, this](const GuiDraw& d)
            {
                b = createButton(d, true);
            };
        }
    };

    auto createButtonVecHandlers = [this]
    (
        const std::vector<std::pair<std::string, std::vector<TmpButton>&>>& buttons,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            std::vector<TmpButton>& b = pair.second;
            handlers[pair.first + up] = [&b, this](const GuiDraw& d)
            {
                b.push_back(createButton(d, false));
            };
            handlers[pair.first + down] = [&b, this](const GuiDraw& d)
            {
                b.push_back(createButton(d, true));
            };
        }
    };

    std::map<std::string, std::function<void(const GuiDraw&)>> handlers;
    createButtonHandlers
    ({
        {"buttonbutton-sidebar-add", mData.miscButtons.addSideBar},
        {"buttonbutton-sidebar-remove", mData.miscButtons.removeSideBar},

        {"button-menu", mData.sideTopButtons.menu},

        {"button-store", mData.sideTopButtons.store},
        {"button-store-inbox", mData.sideTopButtons.storeInbox},
        {"button-store-storexp", mData.miscButtons.storeExp},

        {"button-logout", mData.sideTopButtons.logout},
        {"button-options", mData.sideTopButtons.options},
        {"button-unjustpoints", mData.sideTopButtons.unjustPoints},
        {"button-questlog", mData.sideTopButtons.questLog},
        {"button-viplist", mData.sideTopButtons.vipList},
        {"button-battlelist", mData.sideTopButtons.battleList},
        {"button-skills", mData.sideTopButtons.skills},
        {"button-preywidget", mData.sideTopButtons.prey},

        {"button-combat-pvp", mData.sideTopButtons.pvp},
        {"button-combat-pvp-small", mData.sideTopButtons.pvp},
        {"button-expert", mData.sideTopButtons.expert},
        {"button-expert-small", mData.sideTopButtons.expert},
        {"button-follow", mData.sideTopButtons.follow},
        {"button-stand", mData.sideTopButtons.stand},
        {"button-combat-defensive", mData.sideTopButtons.defensive},
        {"button-combat-balanced", mData.sideTopButtons.balanced},
        {"button-combat-offensive", mData.sideTopButtons.offensive},

        {"button-combat-dovemode", mData.expertButtons.dove},
        {"button-combat-redfistmode", mData.expertButtons.red},
        {"button-combat-whitehandmode", mData.expertButtons.white},
        {"button-combat-yellowhandmode", mData.expertButtons.yellow},


        {"automap-button-movedown", mData.sideTopButtons.moveDown},
        {"automap-button-moveup", mData.sideTopButtons.moveUp},
        {"automap-button-zoomout", mData.sideTopButtons.zoomOut},
        {"automap-button-zoomin", mData.sideTopButtons.zoomIn},

        {"console-buttonignore", mData.chatButtons.ignore},
        {"console-buttonnew", mData.chatButtons.newTab},
        {"console-buttonmessages", mData.chatButtons.messages},
        {"console-buttonclose", mData.chatButtons.closeTab},
        {"console-buttonleft", mData.chatButtons.tabLeft},
        {"console-buttonright", mData.chatButtons.tabRight},
        {"console-buttonleft-flash", mData.chatButtons.tabLeft},
        {"console-buttonright-flash", mData.chatButtons.tabRight},
        {"console-buttonleft-update", mData.chatButtons.tabLeft},
        {"console-buttonright-update", mData.chatButtons.tabRight},
        {"console-buttonsay", mData.chatButtons.say},
        {"console-buttonwhisper", mData.chatButtons.whisper},
        {"console-buttonyell", mData.chatButtons.yell},



        {"battlelist-monster", mData.battleListButtons.monster},
        {"battlelist-npc", mData.battleListButtons.npc},
        {"battlelist-party", mData.battleListButtons.party},
        {"battlelist-players", mData.battleListButtons.player},
        {"battlelist-skull", mData.battleListButtons.skull},


        {"button-transfercoins", mData.miscButtons.transferCoins},
        {"button-copytoclipboard", mData.miscButtons.clipBoard},
    }, handlers, "-down.png", "-up.png");

    createButtonVecHandlers
    ({
        {"max-button-small", pass1.smallMaxButtons},
        {"max-button-small-highlight", pass1.smallMaxButtons},
        {"min-button-small", pass1.smallMinButtons},
        {"exit-button-small", pass1.smallExitButtons},
        {"button-expand-12x12", pass1.expandButtons},
        {"scrollbar-buttondown", pass1.scrollDownButtons},
        {"scrollbar-buttonup", pass1.scrollUpButtons},
        {"scrollbar-buttonright", pass1.scrollRightButtons},
        {"scrollbar-buttonleft", pass1.scrollLeftButtons},
        {"slider-buttonleft", pass1.scrollLeftButtons},
        {"slider-buttonright", pass1.scrollRightButtons},

        {"container-level-up", pass1.containerUpButtons},
        {"container-browse-left", pass1.containerLeftButtons},
        {"container-browse-right", pass1.containerRightButtons},

        {"button-blue", pass1.blueButtons},
        {"button-gold", pass1.goldButtons},
        {"button-green", pass1.greenButtons},
        {"button-grey", pass1.greyButtons},

        {"button-arrow-left", pass1.outfitLeftButtons},
        {"button-arrow-right", pass1.outfitRightButtons},
        {"button-storeicon", pass1.outfitStoreButtons},
    }, handlers, "-down.png", "-up.png");

    const std::vector<std::pair<std::string, TmpButton&>> disabledButtons =
    {
        {"button-expert-disabled.png", mData.sideTopButtons.expert},
        {"button-expert-small-disabled.png", mData.sideTopButtons.expert},
        {"button-sidebar-add-disabled.png", mData.miscButtons.addSideBar},
        {"button-sidebar-remove-disabled.png", mData.miscButtons.removeSideBar},
    };
    for(auto& pair : disabledButtons)
    {
        TmpButton& b = pair.second;
        handlers[pair.first] = [&b, this](const GuiDraw& d)
        {
            b = createButton(d, false, false);
        };
    }

    const std::vector<std::pair<std::string, std::vector<TmpButton>&>> disabledVecButtons =
    {
        {"radiobutton-checked-disabled.png", pass1.radioButtons},
    };
    for(auto& pair : disabledVecButtons)
    {
        std::vector<TmpButton>& b = pair.second;
        handlers[pair.first] = [&b, this](const GuiDraw& d)
        {
            b.push_back(createButton(d, false, false));
        };
    }


    createButtonVecHandlers
    ({
        {"checkbox", pass1.checkBoxes},
        {"radiobutton", pass1.radioButtons},
    }, handlers, "-checked.png", "-unchecked.png");

    createButtonHandlers
    ({
        {"button-contextmenu-12x12", mData.miscButtons.contextMenu},
    }, handlers, "-pressed.png", "-idle.png");

    createButtonVecHandlers
    ({
        {"button-dropdown-20x20", pass1.dropDownButtons},
    }, handlers, "-pressed.png", "-idle.png");

    createButtonVecHandlers
    ({
        {"console-tab", pass1.chatTabs},
    }, handlers, "-active.png", "-passive.png");

    handlers["button-9grid-idle.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, false);
        if(b.screenRect.width > 3 && b.screenRect.height > 3)
        {
            pass1.grid9Buttons.push_back(b);
        }
    };
    handlers["button-9grid-pressed.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, true);
        if(b.screenRect.width > 3 && b.screenRect.height > 3)
        {
            pass1.grid9Buttons.push_back(b);
        }
    };
    handlers["button-textured-up.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, false);
        if(b.screenRect.width > 10 && b.screenRect.height > 10)
        {
            pass1.texturedButtons.push_back(b);
        }
    };
    handlers["button-textured-down.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, true);
        if(b.screenRect.width > 10 && b.screenRect.height > 10)
        {
            pass1.texturedButtons.push_back(b);
        }
    };

    auto createDrawRectHandlers = [this]
    (
        const std::vector<std::pair<std::string, DrawRect&>> rects,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : rects)
        {
            DrawRect& r = pair.second;
            handlers[pair.first] = [&r, this](const GuiDraw& d)
            {
                r = getDrawRect(d);
            };
        }
    };
    auto createDrawRectVecHandlers = [this]
    (
        const std::vector<std::pair<std::string, std::vector<DrawRect>&>> rects,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : rects)
        {
            std::vector<DrawRect>& r = pair.second;
            handlers[pair.first] = [&r, this](const GuiDraw& d)
            {
                r.push_back(getDrawRect(d));
            };
        }
    };

    createDrawRectHandlers
    ({
        {"automap-rose.png", mData.miniMapRose},
        {"automap-crosshair-dark.png", mData.miniMapCrosshair},
        {"automap-crosshair-light.png", mData.miniMapCrosshair},
        {"hitpoints-bar-filled.png", pass1.hpFill},
        {"mana-bar-filled.png", pass1.manaFill},
        {"icon-adventurers-blessing.png", mData.adventurersBlessing},

     }, handlers);

    createDrawRectVecHandlers
    ({
        {"hitpoints-manapoints-bar-border.png", pass1.hpManaBorders},
        {"spellgroup-icons-attack.png", pass1.spellGroups},
        {"spellgroup-icons-healing.png", pass1.spellGroups},
        {"spellgroup-icons-support.png", pass1.spellGroups},
        {"spellgroup-icons-special.png", pass1.spellGroups},

     }, handlers);

    auto createCreatureFlagHandlers = [this]
    (
        const std::vector<std::pair<std::string, CreatureFlag::Type>>& types,
        std::vector<CreatureFlag>& flags,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            CreatureFlag::Type type = pair.second;
            handlers[pair.first] = [&flags, this, type](const GuiDraw& d)
            {
                CreatureFlag f;
                f.rect = getDrawRect(d);
                f.type = type;
                flags.push_back(f);
            };
        }
    };


    using CF = CreatureFlag::Type;
    createCreatureFlagHandlers
    ({
        {"creature-speech-flags-directions.png", CF::DIRECTIONS},
        {"creature-speech-flags-quest.png", CF::QUEST},
        {"creature-speech-flags-talk.png", CF::TALK},
        {"creature-speech-flags-trade.png", CF::TRADE},
        {"creature-state-flags-guild-war-blue", CF::GUILD_WAR_BLUE},
        {"creature-state-flags-guild-war-green", CF::GUILD_WAR_GREEN},
        {"creature-state-flags-guild-war-red", CF::GUILD_WAR_RED},
        {"creature-state-flags-guild-white", CF::GUILD_WHITE},
        {"creature-state-flags-guild-yellow", CF::GUILD_YELLOW},
        {"creature-state-flags-lightning-red", CF::LIGHTNING_RED},
        {"creature-state-flags-party-gray-unknown", CF::PARTY_GRAY},
        {"creature-state-flags-party-invitee", CF::PARTY_INVITEE},
        {"creature-state-flags-party-inviter", CF::PARTY_INVITER},
        {"creature-state-flags-party-leader", CF::PARTY_LEADER},
        {"creature-state-flags-party-leader-shared-xp", CF::PARTY_LEADER_SHARED_XP},
        {"creature-state-flags-party-leader-shared-xp-fail", CF::PARTY_LEADER_SHARED_XP_FAIL},
        {"creature-state-flags-party-member", CF::PARTY_MEMBER},
        {"creature-state-flags-party-member-shared-xp", CF::PARTY_MEMBER_SHARED_XP},
        {"creature-state-flags-party-member-shared-xp-fail", CF::PARTY_MEMBER_SHARED_XP_FAIL},
        {"creature-state-flags-skull-black", CF::SKULL_BLACK},
        {"creature-state-flags-skull-green", CF::SKULL_GREEN},
        {"creature-state-flags-skull-orange", CF::SKULL_ORANGE},
        {"creature-state-flags-skull-red", CF::SKULL_RED},
        {"creature-state-flags-skull-white", CF::SKULL_WHITE},
        {"creature-state-flags-skull-yellow", CF::SKULL_YELLOW},
        {"creature-state-flags-summon-green", CF::SUMMON_GREEN},
        {"creature-state-flags-summon-red", CF::SUMMON_RED},
    }, pass1.creatureFlags, handlers);


    auto createPlayerStateFlagHandlers = []
    (
        const std::vector<std::pair<std::string, uint32_t>>& states,
        uint32_t& flags,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : states)
        {
            uint32_t state = pair.second;
            handlers[pair.first] = [&flags, state](const GuiDraw& d)
            {
                flags |= state;
            };
        }
    };

    namespace PS = PlayerState;
    createPlayerStateFlagHandlers
    ({
        {"player-state-flags-poisoned.png", PS::POISONED},
        {"player-state-flags-burning.png", PS::BURNING},
        {"player-state-flags-electrified.png", PS::ELECTRIFIED},
        {"player-state-flags-drunk.png", PS::DRUNK},
        {"player-state-flags-magic-shield.png", PS::MAGIC_SHIELD},
        {"player-state-flags-slowed.png", PS::SLOWED},
        {"player-state-flags-haste.png", PS::HASTE},
        {"player-state-flags-logout-block.png", PS::LOGOUT_BLOCK},
        {"player-state-flags-drowning.png", PS::DROWNING},
        {"player-state-flags-freezing.png", PS::FREEZING},
        {"player-state-flags-dazzled.png", PS::DAZZLED},
        {"player-state-flags-cursed.png", PS::CURSED},
        {"player-state-flags-strengthened.png", PS::STRENGTHENED},
        {"player-state-flags-protection-zone-block.png", PS::PROTECTION_ZONE_BLOCK},
        {"player-state-flags-protection-zone.png", PS::PROTECTION_ZONE},
        {"player-state-flags-bleeding.png", PS::BLEEDING},
        {"player-state-flags-hungry.png", PS::HUNGRY},
        {"player-state-guildwar-flag.png", PS::GUILD_WAR},
        {"player-state-playerkiller-flags-black.png", PS::SKULL_BLACK},
        {"player-state-playerkiller-flags-green.png", PS::SKULL_GREEN},
        {"player-state-playerkiller-flags-orange.png", PS::SKULL_ORANGE},
        {"player-state-playerkiller-flags-red.png", PS::SKULL_RED},
        {"player-state-playerkiller-flags-white.png", PS::SKULL_WHITE},
        {"player-state-playerkiller-flags-yellow.png", PS::SKULL_YELLOW},
    }, mData.playerStates, handlers);

    auto createMiniMapMarkerHandlers = [this]
    (
        const std::vector<std::pair<std::string, MiniMapMarker::Type>>& types,
        std::vector<MiniMapMarker>& markers,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            MiniMapMarker::Type type = pair.second;
            handlers[pair.first] = [&markers, this, type](const GuiDraw& d)
            {
                MiniMapMarker f;
                f.rect = getDrawRect(d);
                f.type = type;
                markers.push_back(f);
            };
        }
    };

    using MT = MiniMapMarker::Type;
    createMiniMapMarkerHandlers
    ({
        {"markers-checkmark.png", MT::CHECKMARK},
        {"markers-questionmark.png", MT::QUESTIONMARK},
        {"markers-exclamationmark.png", MT::EXCLAMATIONMARK},
        {"markers-star.png", MT::STAR},
        {"markers-crossmark.png", MT::CROSSMARK},
        {"markers-cross.png", MT::CROSS},
        {"markers-mouth.png", MT::MOUTH},
        {"markers-brush.png", MT::BRUSH},
        {"markers-sword.png", MT::SWORD},
        {"markers-flag.png", MT::FLAG},
        {"markers-lock.png", MT::LOCK},
        {"markers-bag.png", MT::BAG},
        {"markers-skull.png", MT::SKULL},
        {"markers-dollar.png", MT::DOLLAR},
        {"markers-red-up.png", MT::RED_UP},
        {"markers-red-down.png", MT::RED_DOWN},
        {"markers-red-right.png", MT::RED_RIGHT},
        {"markers-red-left.png", MT::RED_LEFT},
        {"markers-green-up.png", MT::GREEN_UP},
        {"markers-green-down.png", MT::GREEN_DOWN},
    }, pass1.miniMapMarkers, handlers);

    auto createEmptyEquipmentSlotHandler = [this]
    (
        const std::vector<std::pair<std::string, EmptyEquipmentSlot::Type>>& types,
        std::vector<EmptyEquipmentSlot>& slots,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            EmptyEquipmentSlot::Type type = pair.second;
            handlers[pair.first] = [&slots, this, type](const GuiDraw& d)
            {
                EmptyEquipmentSlot f;
                f.rect = getDrawRect(d);
                f.type = type;
                slots.push_back(f);
            };
        }
    };

    using ET = EmptyEquipmentSlot::Type;
    createEmptyEquipmentSlotHandler
    ({
        {"inventory-back.png", ET::BACK},
        {"inventory-feet.png", ET::FEET},
        {"inventory-finger.png", ET::FINGER},
        {"inventory-head.png", ET::HEAD},
        {"inventory-hip.png", ET::HIP},
        {"inventory-left-hand.png", ET::LEFT_HAND},
        {"inventory-right-hand.png", ET::RIGHT_HAND},
        {"inventory-legs.png", ET::LEGS},
        {"inventory-neck.png", ET::NECK},
        {"inventory-torso.png", ET::TORSO},
    }, pass1.emptyEquipmentSlots, handlers);


    handlers["containerslot.png"] = [this](const GuiDraw& d)
    {
        DrawRect r = getDrawRect(d);
        if(r.screen.width > 2 && r.screen.height > 2)
        {
            pass1.containerSlots.push_back(r);
        }
    };


    return handlers;
}


void Gui::parseCurrentFrame()
{
    if(mIsCurrentFrameParsed)
        return;



    mHalfFrameWidth = mCurrentFrame.width / 2.f;
    mHalfFrameHeight = mCurrentFrame.height / 2.f;

    for(const GuiDraw& g : *mCurrentFrame.guiDraws)
    {
        auto foundIt = mGuiDrawHandlers.find(sb::utility::file::basename(g.name));
        if(foundIt != mGuiDrawHandlers.end())
        {
            foundIt->second(g);
        }
        else
        {
            std::cout << "Unhandled: " << sb::utility::file::basename(g.name) << std::endl;
        }
    }
    std::cout << "Num small min/max buttons: " << pass1.smallMinButtons.size() + pass1.smallMaxButtons.size() << std::endl;

    std::map<std::string, std::list<const GuiDraw*>> guiDraws;
    for(const GuiDraw& g : *mCurrentFrame.guiDraws)
        guiDraws[sb::utility::file::basename(g.name)].push_back(&g);

    updateButtons(guiDraws);
    if(mData.state == State::GAME)
    {
        updateManaAndHpLevels(guiDraws);

        size_t textDrawsIndex = 0;
        parseNames(textDrawsIndex);
        parseChat(textDrawsIndex);
        parseSideWindowTopText(textDrawsIndex);
        updateSideBottomWindows(guiDraws);
        parseContainerSlots(guiDraws);
        parseSideWindowBottomText(textDrawsIndex);
        parseGuiSpriteDraws();
    }
    else if(mData.state == State::MAIN_MENU)
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
        THROW_RUNTIME_ERROR(sb::utility::stringify("Unexpected state: ", (int)mData.state, "\n"));
    }


    auto removeStart = std::remove_if
    (
        mData.buttons.begin(), mData.buttons.end(),
        [](const std::shared_ptr<Button>& b){return (b->text == "");}
    );
    mData.buttons.erase(removeStart, mData.buttons.end());


    mIsCurrentFrameParsed = true;
}
