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

    for(size_t i = 0; i < mDraws->size();)
    {
        try
        {
            for(; i < mDraws->size(); i++)
            {
                const TextBuilder& b = *mBuilders[i];
                using T = Text::Type;
                switch(b.getTextType())
                {
                    case T::NAME:
                        mData.names.insert(mData.names.end(), b.getText().begin(), b.getText().end());
                        break;

                    case T::CLICKABLE_NPC_TEXT:
                        mData.clickableNpcText = b.getText();
                        break;

                    case T::CHAT_TAB:
                        do
                        {
                            mData.chatTabs.insert(mData.chatTabs.end(), b.getText().begin(), b.getText().end());
                            i++;
                        } while(i < mBuilders.size() && mBuilders[i]->getTextType() == T::CHAT_TAB);
                        handleDefaultSideBarText(i);
                        break;

                    case T::GUI:
                        handleGuiText(i);
                        break;

                    default:
        //                std::cout << "Unhandled text (type=" << (int)b.getTextType() << "): " << std::endl;
        //                for(const Text& t : b.getText())
        //                {
        //                    std::cout << "\t" << t.string << std::endl;
        //                }
                        break;
                }
            }
        }
        catch(const std::runtime_error& e)
        {
            std::cout << "Failed to handle text: " << e.what() << std::endl;
            i++;
        }
    }

//    for(const Text& name : mData.battle.names)
//    {
//        std::cout << "BATTLE LIST NAME: " << name.string << std::endl;
//    }
//    for(const Text& name : mData.prey.names)
//    {
//        std::cout << "PREY LIST NAME: " << name.string << std::endl;
//    }
//
//    std::cout << "centre: " << mData.centre.string << std::endl;
//    std::cout << "stop: " << mData.stop.string << std::endl;
//    std::cout << "inv min: " << mData.isInventoryMinimized << std::endl;
//    std::cout << "soul: " << mData.soul << std::endl;
//    std::cout << "cap: " << mData.cap << std::endl;
//    std::cout << "hp: " << mData.hp << std::endl;
//    std::cout << "mana: " << mData.mana << std::endl;
}

void TextParser::handleDefaultSideBarText(size_t& i)
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
    while(i < mBuilders.size() && parsedTypes.size() < NUM_TYPES)
    {
        const std::vector<Text>& text = mBuilders[i]->getText();
        auto it = text.begin();
        if(it->string == "soul:" || it->string == "CaP:")
        {
            SB_EXPECT(text.size(), ==, 2);
            i += 2;
            SB_EXPECT(i, <, mBuilders.size());
            mData.isInventoryMinimized = it->string == "CaP:";
            if(mData.isInventoryMinimized)
            {
                SB_EXPECT(text[1].string, ==, "soul:");
                readSoulAndCap(*mBuilders[i], *mBuilders[i - 1]);
            }
            else
            {
                SB_EXPECT(text[1].string, ==, "CaP:");
                readSoulAndCap(*mBuilders[i - 1], *mBuilders[i]);
            }

            parsedTypes.insert(Type::SOUL_CAP);
            it = text.end();
        }
        else if(isNumeric(it->string))
        {
            if(it->type == Text::Type::ITEM_STACK_COUNT)
            {
                mData.inventoryStackCounts.insert(mData.inventoryStackCounts.end(), text.begin(), text.end());
                it = text.end();
            }
            else
            {
                SB_EXPECT(text.size(), >=, 2);

                SB_EXPECT_TRUE(isNumeric(it->string));
                mData.hp = strToInt(it->string);

                SB_EXPECT(++it, !=, text.end());
                SB_EXPECT_TRUE(isNumeric(it->string));
                mData.mana = strToInt(it->string);

                it++;
                parsedTypes.insert(HP_MANA);
            }
        }

        while(it != text.end())
        {
            if(it->string == "Centre")
            {
                mData.centre = *it;
                parsedTypes.insert(CENTRE);
            }
            else if(it->string == "Stop")
            {
                mData.stop = *it;
                parsedTypes.insert(STOP);
            }
            else if(it->string == "Premium Features")
            {
                parsedTypes.insert(PREMIUM_FEATURES);
                mData.isPremiumFeaturesMinimized = (std::distance(it, text.end()) == 1);
                if(!mData.isPremiumFeaturesMinimized)
                {
                    break;
                }
            }
            else
            {
                SB_THROW("Unexpected text. Got \"" + it->string + "\".");
            }
            it++;
        }
        i++;
    }
    i--;

    SB_EXPECT(parsedTypes.size(), ==, NUM_TYPES);
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
        size_t numWindowsLeft = mGuiData->game.sideBarWindows.windows.size() - mData.windowOrder.size();
        if(numWindowsLeft > 0)
        {
            handleContainerText(i);
        }
        else
        {
//            std::cout << "Unhandled GUI text: " << std::endl;
//            for(const Text& t : text)
//            {
//                std::cout << "\t" << t.string << std::endl;
//            }
        }

        return;
    }

    foundIt->second(i);
}

void TextParser::handleContainerText(size_t& i)
{
    const std::vector<Text>& title = mBuilders[i]->getText();
    SB_EXPECT(title.size(), ==, 1);

    mData.windowOrder.push_back(GraphicsLayer::SideBarWindow::Type::CONTAINER);
    mData.containers.emplace_back();
    Container& c = mData.containers.back();

    c.title = title[0];

    i++;
    if(i < mBuilders.size() && mBuilders[i]->getTextType() == Text::Type::ITEM_STACK_COUNT)
    {
        c.counts = mBuilders[i]->getText();
    }
    else
    {
        i--;
    }
}

std::map<std::string, std::function<void(size_t&)>> TextParser::initGuiTextHandlers()
{
    std::map<std::string, std::function<void(size_t&)>> handlers;


    handlers["Unjustified Points"] = [this](size_t& i)
    {
        const std::vector<Text>& title = mBuilders[i]->getText();
        SB_EXPECT(title.size(), ==, 1);
        mData.windowOrder.push_back(GraphicsLayer::SideBarWindow::Type::UNJUSTIFIED_POINTS);
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
        mData.windowOrder.push_back(GraphicsLayer::SideBarWindow::Type::PREY);
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
        mData.windowOrder.push_back(GraphicsLayer::SideBarWindow::Type::VIP);
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
        mData.windowOrder.push_back(GraphicsLayer::SideBarWindow::Type::BATTLE);
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
            std::vector<Text> text = mBuilders[i]->getText();
            using T = Text::Type;
            switch(mBuilders[i]->getTextType())
            {
                case T::GUI:
                    for(Text& t : text)
                    {
                        auto foundIt = names.find(t.string);

                        if(foundIt == names.end())
                        {
                            if(t.string.back() != '-')
                            {
                                i--;
                                return;
                            }
                            std::string prefix = t.string.substr(0, t.string.size() - 1);
                            foundIt = names.lower_bound(prefix);
                            if(foundIt == names.end() || foundIt->first.compare(0, prefix.size(), prefix) != 0)
                            {
                                i--;
                                return;
                            }

                            t.string = foundIt->first;
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
        mData.windowOrder.push_back(GraphicsLayer::SideBarWindow::Type::SKILLS);
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
        using F = SkillField;
        auto& s = mData.skills.values;
        auto& y = mData.skills.yCoords;

        auto setField = [this](const auto& valFunc, F field, const Text& text)
        {
            mData.skills.values[(size_t)field] = valFunc(text.string);
            mData.skills.yCoords[(size_t)field] = round(text.localY);
        };

        auto expectStr = [](std::vector<Text>::const_iterator& it, const std::vector<Text>& text, const std::string& fieldStr)
        {
            SB_EXPECT(++it, !=, text.end());
            SB_EXPECT(it->string, ==, fieldStr);
            SB_EXPECT(++it, !=, text.end());
        };

        auto setNumeric = [&setField](SkillField field, const Text& text)
        {
            SB_EXPECT_TRUE(isNumeric(text.string));
            setField(strToInt, field, text);
        };

        auto readNumeric = [&expectStr, &setNumeric](SkillField field, std::vector<Text>::const_iterator& it, const std::vector<Text>& text, const std::string& fieldStr)
        {
            expectStr(it, text, fieldStr);
            setNumeric(field, *it);
        };

        auto readTime = [&expectStr, &setField](SkillField field, std::vector<Text>::const_iterator& it, const std::vector<Text>& text, const std::string& fieldStr)
        {
            expectStr(it, text, fieldStr);
            setField(parseTimeText, field, *it);
        };

        auto readPercent = [&expectStr, &setField](SkillField field, std::vector<Text>::const_iterator& it, const std::vector<Text>& text, const std::string& fieldStr)
        {
            expectStr(it, text, fieldStr);
            setField(parsePercentageText, field, *it);
        };

        SB_EXPECT(++it, !=, text.end());
        setNumeric(F::LEVEL, *it);

        readNumeric(F::EXPERIENCE, it, text, "  Experience");
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
            setField(parsePercentageText, F::XP_GAIN_RATE, xpGainRate.front());
        }
        else
        {
            setField(parsePercentageText, F::XP_GAIN_RATE, *it);
        }

        readNumeric(F::HP, it, text, "Hit Points");
        readNumeric(F::MANA, it, text, "Mana");
        readNumeric(F::SOUL, it, text, "Soul Points");
        readNumeric(F::CAP, it, text, "Capacity");

        expectStr(it, text, "Speed");
        if(it->string == "Food")
        {
            SB_EXPECT(++i, <, mBuilders.size());
            SB_EXPECT(mBuilders[i]->getTextType(), ==, Text::Type::GUI);
            const std::vector<Text>& speed = mBuilders[i]->getText();

            SB_EXPECT(speed.size(), ==, 1);
            setNumeric(F::SPEED, speed.front());
        }
        else
        {
            setNumeric(F::SPEED, *it);
        }

        readTime(F::FOOD_MINUTES, it, text, "Food");
        readTime(F::STAMINA_MINUTES, it, text, "Stamina");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string.find("Offline Training"), ==, 0);
        std::string offlineTrainingMinutes = it->string.substr(std::string("Offline Training").size());
        setField([&offlineTrainingMinutes](const std::string& str){return parseTimeText(offlineTrainingMinutes);},
                 F::OFFLINE_TRAINING_MINUTES,
                 *it);


        readNumeric(F::MAGIC_LEVEL, it, text, "Magic Level");
        readNumeric(F::FIST_LEVEL, it, text, "Fist Fighting");
        readNumeric(F::CLUB_LEVEL, it, text, "Club Fighting");
        readNumeric(F::SWORD_LEVEL, it, text, "Sword Fighting");
        readNumeric(F::AXE_LEVEL, it, text, "Axe Fighting");
        readNumeric(F::DISTANCE_LEVEL, it, text, "Distance Fighting");
        readNumeric(F::SHIELDING_LEVEL, it, text, "Shielding");
        readNumeric(F::FISHING_LEVEL, it, text, "Fishing");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Critical Hit:");
        readPercent(F::CRIT_CHANCE, it, text, "  Chance");
        readPercent(F::CRIT_DAMAGE, it, text, "  Extra Damage");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Hit Points Leech:");
        readPercent(F::HP_LEECH_CHANCE, it, text, "  Chance");
        readPercent(F::HP_LEECH_AMOUNT, it, text, "  Amount");

        SB_EXPECT(++it, !=, text.end());
        SB_EXPECT(it->string, ==, "Mana Leech:");
        readPercent(F::MANA_LEECH_CHANCE, it, text, "  Chance");
        readPercent(F::MANA_LEECH_AMOUNT, it, text, "  Amount");

        SB_EXPECT(++it, ==, text.end());
    };

    handlers["Chat on"] = [this](size_t& i)
    {
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 1);
        mData.isChatOn = true;
        mData.toggleChat = text[0];
    };
    handlers["Chat off"] = [this](size_t& i)
    {
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 1);
        mData.isChatOn = false;
        mData.toggleChat = text[0];
    };

    handlers["Centre"] = [this](size_t& i)
    {
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 2);
        SB_EXPECT(text[1].string, ==, "Stop");
        mData.centre = text[0];
        mData.stop = text[1];
    };



    handlers["soul:"] = [this](size_t& i)
    {
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 2);
        SB_EXPECT(text[1].string, ==, "CaP:");

        i += 2;
        SB_EXPECT(i, <, mBuilders.size());
        readSoulAndCap(*mBuilders[i - 1], *mBuilders[i]);
    };

    handlers["CaP:"] = [this](size_t& i)
    {
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 2);
        SB_EXPECT(text[1].string, ==, "soul:");

        i += 2;
        SB_EXPECT(i, <, mBuilders.size());
        readSoulAndCap(*mBuilders[i], *mBuilders[i - 1]);
    };



    return handlers;
}

void TextParser::readSoulAndCap(const TextBuilder& soul, const TextBuilder& cap)
{
    const std::vector<Text>& soulText = soul.getText();
    const std::vector<Text>& capText = cap.getText();

    SB_EXPECT(soulText.size(), ==, 1);
    SB_EXPECT(capText.size(), ==, 1);

    SB_EXPECT_TRUE(sb::utility::isNumeric(soulText[0].string));
    SB_EXPECT_TRUE(sb::utility::isNumeric(capText[0].string));

    mData.soul = sb::utility::strToInt(soulText[0].string);
    mData.cap = sb::utility::strToInt(capText[0].string);
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
