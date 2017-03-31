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
#include "monitor/TextParser.hpp"
#include "monitor/Frame.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
#include <iostream>
///////////////////////////////////

TextParser::TextParser()
: mGuiTextHandlers(initGuiTextHandlers())
{

}


void TextParser::parse(const Frame& frame, const GuiParser::Data& guiData)
{
    mData = Data();
    mBuilders.clear();
    mDraws = frame.textDraws;
    mGuiData = &guiData;


    //    mHalfFrameWidth = frame.width / 2.f;
    //    mHalfFrameHeight = frame.height / 2.f;

    if(mDraws == nullptr)
    {
        return;
    }

    mBuilders.resize(mDraws->size());
    std::transform
    (
        mDraws->begin(), mDraws->end(),
        mBuilders.begin(),
        [&frame](const TextDraw& d)
        {
            return std::make_shared<TextBuilder>(d, frame.width, frame.height);
        }
    );

    for(size_t i = 0; i < mDraws->size(); i++)
    {
        const TextBuilder& b = *mBuilders[i];
        using T = Text::Type;
        switch(b.getTextType())
        {
            case T::NAME:
                mData.names = b.getText();
                break;

            case T::CLICKABLE_NPC_TEXT:
                mData.clickableNpcText = b.getText();
                break;

            case T::CHAT_TAB:
                mData.chatTabs = b.getText();
                break;

            case T::GUI:
                handleGuiText(i);
                break;
        }

    }

    for(const Text& name : mData.battle.names)
    {
        std::cout << "BATTLE LIST NAME: " << name.string << std::endl;
    }
    for(const Text& name : mData.prey.names)
    {
        std::cout << "PREY LIST NAME: " << name.string << std::endl;
    }
}


void TextParser::handleGuiText(size_t& i)
{
    const TextBuilder& b = *mBuilders[i];
    const std::vector<Text>& text = b.getText();
    if(text.empty())
    {
        return;
    }


    const std::string& h = text[0].string;
    auto foundIt = mGuiTextHandlers.find(h);
    if(foundIt == mGuiTextHandlers.end())
    {
        std::cout << "Unhandled GUI text: " << std::endl;
        for(const Text& t : text)
        {
            std::cout << "\t" << t.string << std::endl;
        }
        return;
    }

    try
    {
        foundIt->second(i);
    }
    catch(const std::runtime_error& e)
    {
        std::cout << "Failed to handle GUI text: \"" << e.what() << "\"." << std::endl;
        return;
    }

    if(h == "Unjustified Points")
    {

    }
    else if(h == "Prey")
    {

    }
    else if(h == "Skills")
    {

    }
    else if(h == "VIP")
    {

    }
    else if(h == "Battle")
    {

    }
    else if(h == "Your Store Inbox")
    {

    }
    else if(h == "Prey")
    {

    }
}

std::map<std::string, std::function<void(size_t&)>> TextParser::initGuiTextHandlers()
{
    std::map<std::string, std::function<void(size_t&)>> handlers;


    handlers["Unjustified Points"] = [this](size_t& i)
    {
        const std::vector<Text>& title = mBuilders[i]->getText();
        SB_EXPECT(title.size(), ==, 1);
        mData.unjustifiedPoints.title = title[0];
        i++;
        if(i >= mBuilders.size())
        {
            return;
        }
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 1);

        std::vector<std::string> parts = sb::utility::split(text[0].string, ' ');
        SB_EXPECT(parts.size(), ==, 2);
        SB_EXPECT(parts[0], ==, "Open:");
        SB_EXPECT_TRUE(sb::utility::isNumeric(parts[1]));

        mData.unjustifiedPoints.open = sb::utility::strToInt(parts[1]);
    };

    handlers["Prey"] = [this](size_t& i)
    {
        const std::vector<Text>& title = mBuilders[i]->getText();
        SB_EXPECT(title.size(), ==, 1);
        mData.prey.title = title[0];

        if(mGuiData->game.sideBarWindows.prey.bonuses.empty())
        {
            return;
        }
        i++;

        SB_EXPECT(i, <, mBuilders.size());
        SB_EXPECT(mBuilders[i]->getTextType(), ==, Text::Type::GUI);
        mData.prey.names = mBuilders[i]->getText();
    };

    handlers["VIP"] = [this](size_t& i)
    {
        const std::vector<Text>& title = mBuilders[i]->getText();
        SB_EXPECT(title.size(), ==, 1);
        mData.vip.title = title[0];
        i++;
        while(i < mBuilders.size())
        {
            using T = Text::Type;
            switch(mBuilders[i]->getTextType())
            {
                case T::VIP_OFFLINE:
                    mData.vip.offline = mBuilders[i]->getText();
                    break;

                case T::VIP_ONLINE:
                    mData.vip.online = mBuilders[i]->getText();
                    break;

                default:
                    i--;
                    return;
            }

            i++;
        }
    };

    handlers["Battle"] = [this](size_t& i)
    {
        const std::vector<Text>& title = mBuilders[i]->getText();
        SB_EXPECT(title.size(), ==, 1);
        mData.battle.title = title[0];
        i++;
        std::map<std::string, unsigned short> names;
        for(const Text& t : mData.names)
        {
            auto foundIt = names.find(t.string);
            if(foundIt == names.end())
            {
                names[t.string] = 1;
            }
            else
            {
                foundIt->second++;
            }
        }

        while(i < mBuilders.size())
        {
            const std::vector<Text>& text = mBuilders[i]->getText();
            using T = Text::Type;
            switch(mBuilders[i]->getTextType())
            {
                case T::GUI:
                    for(const Text& t : text)
                    {
                        auto foundIt = names.find(t.string);

                        if(foundIt == names.end())
                        {
                            i--;
                            return;
                        }

                        SB_EXPECT(foundIt->second, >, 0);
                        foundIt->second--;
                    }
                    mData.battle.names.insert(mData.battle.names.end(), text.begin(), text.end());
                    break;

                case T::NAME_BATTLE_WINDOW_HIGHLIGHTED:
                    SB_EXPECT(text.size(), ==, 1);
                    SB_EXPECT(mData.battle.selectedNameIndex, ==, -1);
                    mData.battle.selectedNameIndex = mData.battle.names.size();
                    mData.battle.names.push_back(text[0]);
                    break;

                default:
                    i--;
                    return;
            }
            i++;
        }
    };

    handlers["Skills"] = [this](size_t& i)
    {
        const std::vector<Text>& title = mBuilders[i]->getText();
        SB_EXPECT(title.size(), ==, 1);
        mData.skills.title = title[0];
        i++;
        if(i >= mBuilders.size())
        {
            return;
        }
        const std::vector<Text>& text = mBuilders[i]->getText();
        auto it = text.begin();
        if(it->string != "Level")
        {
            i--;
            return;
        }
        using namespace sb::utility;
        Skills& s = mData.skills;

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT_TRUE(isNumeric(it->string));
        s.level = strToInt(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Experience");
        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT_TRUE(isNumeric(it->string));
        s.experience = strToInt(it->string);

        SB_EXPECT(++it, !=, text.end());
        if(it->string == "  XP Gain Rate")
        {
            SB_EXPECT(++it, !=, text.end());
        }
        else
        {
            SB_EXPECT(++i, <, mBuilders.size());
            SB_EXPECT(mBuilders[i]->getTextType(), ==, Text::Type::GUI);
            SB_EXPECT(mBuilders[i]->getText().size(), ==, 1);
            SB_EXPECT(mBuilders[i]->getText().front().string, ==, "  XP Gain Rate");
        }

        if(it->string == "Hit Points")
        {
            it--;
            SB_EXPECT(++i, <, mBuilders.size());
            SB_EXPECT(mBuilders[i]->getTextType(), ==, Text::Type::GUI);
            const std::vector<Text>& xpGainRate = mBuilders[i]->getText();

            SB_EXPECT(xpGainRate.size(), ==, 1);
            s.xpGainRate = parsePercentageText(xpGainRate.front().string);
        }
        else
        {
            s.xpGainRate = parsePercentageText(it->string);
        }

        auto readNumeric = [](std::vector<Text>::const_iterator& it, const std::vector<Text>& text, const std::string& field)
        {
            SB_EXPECT(++it, !=, text.end());
            SB_EXPECT(it->string, ==, field);
            SB_EXPECT(++it, !=, text.end());
            SB_EXPECT_TRUE(isNumeric(it->string));
            return strToInt(it->string);
        };

        s.hp = readNumeric(it, text, "Hit Points");
        s.mana = readNumeric(it, text, "Mana");
        s.soul = readNumeric(it, text, "Soul Points");
        s.cap = readNumeric(it, text, "Capacity");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Speed");

        SB_EXPECT(++it, !=, text.end());
        if(it->string == "Food")
        {
            SB_EXPECT(++i, <, mBuilders.size());
            SB_EXPECT(mBuilders[i]->getTextType(), ==, Text::Type::GUI);
            const std::vector<Text>& speed = mBuilders[i]->getText();

            SB_EXPECT(speed.size(), ==, 1);
            SB_EXPECT_TRUE(isNumeric(speed.front().string));
            s.speed = strToInt(speed.front().string);
        }
        else
        {
            SB_EXPECT_TRUE(isNumeric(it->string));
            s.speed = strToInt(it->string);
        }

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Food");
        SB_EXPECT(++it, !=, text.end());
        s.foodMinutes = parseTimeText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Stamina");
        SB_EXPECT(++it, !=, text.end());
        s.staminaMinutes = parseTimeText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string.find("Offline Training"), ==, 0);
        std::string offlineTrainingMinutes = it->string.substr(std::string("Offline Training").size());
        s.staminaMinutes = parseTimeText(offlineTrainingMinutes);

        s.magicLevel = readNumeric(it, text, "Magic Level");
        s.fistLevel = readNumeric(it, text, "Fist Fighting");
        s.clubLevel = readNumeric(it, text, "Club Fighting");
        s.swordLevel = readNumeric(it, text, "Sword Fighting");
        s.axeLevel = readNumeric(it, text, "Axe Fighting");
        s.distanceLevel = readNumeric(it, text, "Distance Fighting");
        s.shieldingLevel = readNumeric(it, text, "Shielding");
        s.fishingLevel = readNumeric(it, text, "Fishing");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Critical Hit:");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Chance");
        SB_EXPECT(++it, !=, text.end());
        s.critChance = parsePercentageText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Extra Damage");
        SB_EXPECT(++it, !=, text.end());
        s.critDamage = parsePercentageText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Hit Points Leech:");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Chance");
        SB_EXPECT(++it, !=, text.end());
        s.hpLeechChance = parsePercentageText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Amount");
        SB_EXPECT(++it, !=, text.end());
        s.hpLeechAmount = parsePercentageText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Mana Leech:");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Chance");
        SB_EXPECT(++it, !=, text.end());
        s.manaLeechChance = parsePercentageText(it->string);

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "  Amount");
        SB_EXPECT(++it, !=, text.end());
        s.manaLeechAmount = parsePercentageText(it->string);

        SB_EXPECT(++it, ==, text.end());
    };

    return handlers;
}


const TextParser::Data& TextParser::getData() const
{
    return mData;
}



unsigned short TextParser::parseTimeText(const std::string& str)
{
    size_t colonPos = str.find(':');
    SB_EXPECT(colonPos, !=, str.npos);

    std::string hours = str.substr(0, colonPos);
    std::string minutes = str.substr(colonPos + 1);

    using namespace sb::utility;
    SB_EXPECT_TRUE(isNumeric(hours));
    SB_EXPECT_TRUE(isNumeric(minutes));
    return strToInt(hours) * 60 + strToInt(minutes);
}

unsigned short TextParser::parsePercentageText(const std::string& str)
{
    SB_EXPECT(str.size(), >, 1);
    SB_EXPECT(str.back(), ==, '%');

    std::string percentage = str.substr(0, str.size() - 1);

    using namespace sb::utility;
    SB_EXPECT_TRUE(isNumeric(percentage));
    return strToInt(percentage);
}
