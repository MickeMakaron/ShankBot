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


void TextParser::parse(const Frame& frame)
{
    mData = Data();
    mBuilders.clear();
    mDraws = frame.textDraws;


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
    for(const TextDraw& d : *frame.textDraws)
    {
//        const std::vector<Text>& lines = b.getText();
//        using Color = Constants::TextColor;
//        Color c = (Color)d.color.packed;
//        switch(c)
//        {
//            case Color::CLICKABLE_NPC_TEXT:
//                mTextType = Text::Type::CLICKABLE_NPC_TEXT;
//                return;
//
//            case Color::GREEN_TEXT:
//                mTextType = Text::Type::GREEN_TEXT;
//                return;
//            case Color::INFO_TEXT:
//                mTextType = Text::Type::INFO_TEXT;
//                return;
//            case Color::SAY:
//                mTextType = Text::Type::SAY;
//                return;
//            case Color::PM:
//                mTextType = Text::Type::PM;
//                return;
//            case Color::SELF_PM:
//            case Color::SELF_PM_INPUT:
//                mTextType = Text::Type::SELF_PM;
//                return;
//            case Color::SOUND:
//                mTextType = Text::Type::SOUND;
//                return;
//
//            default:
//                break;
//        }
//
//        if(mTextDraw.isOutlined)
//        {
//            switch(c)
//            {
//                case Color::HP_FULL:
//                case Color::HP_LIGHT_GREEN:
//                case Color::HP_YELLOW:
//                case Color::HP_LIGHT_RED:
//                case Color::HP_RED:
//                case Color::HP_DARK_RED:
//                    mTextType = Text::Type::NAME;
//                    return;
//                case Color::HP_GRAY:
//                    mTextType = Text::Type::NAME_OBSCURED;
//                    return;
//
//                case Color::CHAT_TAB_INACTIVE:
//                case Color::CHAT_TAB_ACTIVE:
//                case Color::CHAT_TAB_UNREAD:
//                    mTextType = Text::Type::CHAT_TAB;
//                    return;
//
//                case Color::CHAT_TAB_NEW_MESSAGE: // same as CONTEXT_MENU
//                    mIsDisputedContextMenuOrChatTab = true;
//                    build();
//                    return;
//
//                case Color::ITEM_STACK_COUNT:
//                    mTextType = Text::Type::ITEM_STACK_COUNT;
//                    return;
//
//                case Color::SERVER_BROADCAST:
//                    mTextType = Text::Type::SERVER_BROADCAST;
//                    return;
//
//                case Color::BROADCAST:
//                    mTextType = Text::Type::BROADCAST;
//                    return;
//
//                default:
//                    break;
//            }
//        }
//        else
//        {
//            switch(c)
//            {
//                case Color::GUI:
//                case Color::XP_GAIN_RATE_FULL:
//                case Color::ITEM_STACK_COUNT:
//                case Color::HP_GRAY:
//                    mTextType = Text::Type::GUI;
//                    return;
//
//                case Color::ADD_NEW_HOTKEY_ENTRY:
//                    mTextType = Text::Type::ADD_NEW_HOTKEY_ENTRY;
//                    return;
//
//                case Color::TOOLTIP:
//                    mTextType = Text::Type::TOOLTIP;
//                    return;
//
//                case Color::DISABLED_LIST_ENTRY:
//                    mTextType = Text::Type::DISABLED_LIST_ENTRY;
//                    return;
//
//                case Color::VIP_OFFLINE:
//                    mTextType = Text::Type::VIP_OFFLINE;
//                    return;
//
//                case Color::VIP_ONLINE:
//                    mTextType = Text::Type::VIP_ONLINE;
//                    return;
//
//                case Color::HOTKEY_TEXT:
//                    mTextType = Text::Type::HOTKEY_TEXT;
//                    return;
//                case Color::HOTKEY_USE_ON_SELF:
//                    mTextType = Text::Type::HOTKEY_USE_ON_SELF;
//                    return;
//                case Color::HOTKEY_USE_ON_TARGET:
//                    mTextType = Text::Type::HOTKEY_USE_ON_TARGET;
//                    return;
//                case Color::HOTKEY_USE_WITH_CROSSHAIR:
//                    mTextType = Text::Type::HOTKEY_USE_WITH_CROSSHAIR;
//                    return;
//                case Color::HOTKEY_EQUIP:
//                    mTextType = Text::Type::HOTKEY_EQUIP;
//                    return;
//                case Color::STORE_NEW:
//                    mTextType = Text::Type::STORE_NEW;
//                    return;
//
//                default:
//                {
//                    build();
//                    if(!mText.empty())
//                    {
//                        const std::string str = mText.front().string;
//                        assert(!str.empty());
//                        if(str == "  XP Gain Rate")
//                        {
//                            mTextType = Text::Type::GUI;
//                            updateTextType();
//                            return;
//                        }
//                        else if(str.back() == '%' && str.size() > 1)
//                        {
//                            bool isXpGainRateValue = true;
//                            for(size_t i = 0; i < str.size() - 1; i++)
//                                if(!isNumeric(str[i]))
//                                {
//                                    isXpGainRateValue = false;
//                                    break;
//                                }
//                            if(isXpGainRateValue)
//                            {
//                                mTextType = Text::Type::GUI;
//                                updateTextType();
//                                return;
//                            }
//                        }
//
//                        switch(c)
//                        {
//                            case Color::SELECTED_TEXT:
//                                mTextType = Text::Type::SELECTED_TEXT;
//                                updateTextType();
//                                return;
//
//                            case Color::NAME_BATTLE_WINDOW_HIGHLIGHTED:
//                                mTextType = Text::Type::NAME_BATTLE_WINDOW_HIGHLIGHTED;
//                                updateTextType();
//                                return;
//
//                            default:
//                                break;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
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
        i++;
        if(i < mBuilders.size())
        {
            return;
        }
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.size(), ==, 1);

        std::vector<std::string> parts = sb::utility::split(text[0].string, ' ');
        SB_EXPECT(parts.size(), ==, 2);
        SB_EXPECT(parts[0], ==, "Open:");
        SB_EXPECT(sb::utility::isNumeric(parts[1]), ==, true);

        mData.unjustifiedPoints.open = sb::utility::strToInt(parts[1]);
    };

    handlers["Prey"] = [this](size_t& i)
    {
        i++;
        if(i < mBuilders.size())
        {
            return;
        }
        const std::vector<Text>& text = mBuilders[i]->getText();
        SB_EXPECT(text.empty(), ==, false);
        mData.prey.numInactive = 0;
        for(const Text& t : text)
        {
            if(t.string == "Inactive")
            {
                mData.prey.numInactive++;
            }
            else
            {
                mData.prey.active.push_back(t.string);
            }
        }
    };

    handlers["VIP"] = [this](size_t& i)
    {
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
        i++;
        SB_THROW("NEED GUIPARSER DATA");
//        while(i < mBuilders.size())
//        {
//            using T = Text::Type;
//            switch(mBuilders[i]->getType())
//            {
//                case T::VIP_OFFLINE:
//                    mData.vip.offline = mBuilders[i]->getText();
//                    break;
//
//                case T::VIP_ONLINE:
//                    mData.vip.online = mBuilders[i]->getText();
//                    break;
//
//                default:
//                    i--;
//                    return;
//            }
//
//            i++;
//        }
    };



    return handlers;
}


const TextParser::Data& TextParser::getData() const
{
    return mData;
}


