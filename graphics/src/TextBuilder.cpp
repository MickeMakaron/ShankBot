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
#include "graphics/TextBuilder.hpp"
#include "utility/utility.hpp"
#include "graphics/Draw.hpp"
using namespace GraphicsLayer;
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
#include <fstream>
#include <cassert>
#include <iostream>
///////////////////////////////////

TextBuilder::TextBuilder(const TextDraw& text, unsigned short frameWidth, unsigned short frameHeight)
: mTextDraw(text)
, M_HALF_FRAME_WIDTH(float(frameWidth) / 2.f)
, M_HALF_FRAME_HEIGHT(float(frameHeight) / 2.f)
{
    setTextType();
}

void TextBuilder::setTextType()
{
    typedef Constants::TextColor Color;
    Color c = (Color)mTextDraw.color.packed;
    switch(c)
    {
        case Color::CLICKABLE_NPC_TEXT:
            mTextType = Text::Type::CLICKABLE_NPC_TEXT;
            return;

        case Color::GREEN_TEXT:
            mTextType = Text::Type::GREEN_TEXT;
            return;
        case Color::INFO_TEXT:
            mTextType = Text::Type::INFO_TEXT;
            return;
        case Color::SAY:
            mTextType = Text::Type::SAY;
            return;
        case Color::PM:
            mTextType = Text::Type::PM;
            return;
        case Color::SELF_PM:
        case Color::SELF_PM_INPUT:
            mTextType = Text::Type::SELF_PM;
            return;
        case Color::SOUND:
            mTextType = Text::Type::SOUND;
            return;

        default:
            break;
    }

    if(mTextDraw.isOutlined)
    {
        switch(c)
        {
            case Color::HP_FULL:
            case Color::HP_LIGHT_GREEN:
            case Color::HP_YELLOW:
            case Color::HP_LIGHT_RED:
            case Color::HP_RED:
            case Color::HP_DARK_RED:
                mTextType = Text::Type::NAME;
                return;
            case Color::HP_GRAY:
                mTextType = Text::Type::NAME_OBSCURED;
                return;

            case Color::CHAT_TAB_INACTIVE:
            case Color::CHAT_TAB_ACTIVE:
            case Color::CHAT_TAB_UNREAD:
                mTextType = Text::Type::CHAT_TAB;
                return;

            case Color::CHAT_TAB_NEW_MESSAGE: // same as CONTEXT_MENU
                mIsDisputedContextMenuOrChatTab = true;
                build();
                return;

            case Color::ITEM_STACK_COUNT:
                mTextType = Text::Type::ITEM_STACK_COUNT;
                return;

            case Color::SERVER_BROADCAST:
                mTextType = Text::Type::SERVER_BROADCAST;
                return;

            case Color::BROADCAST:
                mTextType = Text::Type::BROADCAST;
                return;

            default:
                break;
        }
    }
    else
    {
        switch(c)
        {
            case Color::GUI:
            case Color::XP_GAIN_RATE_FULL:
            case Color::ITEM_STACK_COUNT:
            case Color::HP_GRAY:
                mTextType = Text::Type::GUI;
                return;

            case Color::ADD_NEW_HOTKEY_ENTRY:
                mTextType = Text::Type::ADD_NEW_HOTKEY_ENTRY;
                return;

            case Color::TOOLTIP:
                mTextType = Text::Type::TOOLTIP;
                return;

            case Color::DISABLED_LIST_ENTRY:
                mTextType = Text::Type::DISABLED_LIST_ENTRY;
                return;

            case Color::VIP_OFFLINE:
                mTextType = Text::Type::VIP_OFFLINE;
                return;

            case Color::VIP_ONLINE:
                mTextType = Text::Type::VIP_ONLINE;
                return;

            case Color::HOTKEY_TEXT:
                mTextType = Text::Type::HOTKEY_TEXT;
                return;
            case Color::HOTKEY_USE_ON_SELF:
                mTextType = Text::Type::HOTKEY_USE_ON_SELF;
                return;
            case Color::HOTKEY_USE_ON_TARGET:
                mTextType = Text::Type::HOTKEY_USE_ON_TARGET;
                return;
            case Color::HOTKEY_USE_WITH_CROSSHAIR:
                mTextType = Text::Type::HOTKEY_USE_WITH_CROSSHAIR;
                return;
            case Color::HOTKEY_EQUIP:
                mTextType = Text::Type::HOTKEY_EQUIP;
                return;
            case Color::STORE_NEW:
                mTextType = Text::Type::STORE_NEW;
                return;

            default:
            {
                build();
                if(!mText.empty())
                {
                    const std::string str = mText.front().string;
                    assert(!str.empty());
                    if(str == "  XP Gain Rate")
                    {
                        mTextType = Text::Type::GUI;
                        updateTextType();
                        return;
                    }
                    else if(str.back() == '%' && str.size() > 1)
                    {
                        bool isXpGainRateValue = true;
                        for(size_t i = 0; i < str.size() - 1; i++)
                            if(!isNumeric(str[i]))
                            {
                                isXpGainRateValue = false;
                                break;
                            }
                        if(isXpGainRateValue)
                        {
                            mTextType = Text::Type::GUI;
                            updateTextType();
                            return;
                        }
                    }

                    switch(c)
                    {
                        case Color::SELECTED_TEXT:
                            mTextType = Text::Type::SELECTED_TEXT;
                            updateTextType();
                            return;

                        case Color::NAME_BATTLE_WINDOW_HIGHLIGHTED:
                            mTextType = Text::Type::NAME_BATTLE_WINDOW_HIGHLIGHTED;
                            updateTextType();
                            return;

                        default:
                            break;
                    }
                }
                break;
            }
        }
    }


    std::stringstream sstream;
    sstream << "Unimplemented text color: " << mTextDraw.color.packed
            << " (" << (int)mTextDraw.color.r << " " << (int)mTextDraw.color.g << " " << (int)mTextDraw.color.b << " " << (int)mTextDraw.color.a << "). "
            << "Outlined: " << (mTextDraw.isOutlined ? "yes" : "no") << std::endl;
    std::cout << sstream.str();
//    THROW_RUNTIME_ERROR(sstream.str());
}

void TextBuilder::updateTextType()
{
    for(Text& t : mText)
        t.type = mTextType;
}


void TextBuilder::appendLine(unsigned char character, short topLeftX, short topLeftY, short botRightX, short botRightY)
{
    mLines.push_back(Line());
    Line& backLine = mLines.back();
    backLine.characters.push_back(character);

    backLine.topLeftX = topLeftX;
    backLine.botRightX = botRightX;
    backLine.topLeftY = topLeftY;
    backLine.botRightY = botRightY;
    backLine.maxAllowedDistanceY = botRightY - topLeftY;
    backLine.maxAllowedDistanceX = botRightX - topLeftX;
}

void TextBuilder::appendCharacter(unsigned char character, short topLeftX, short topLeftY, short botRightX, short botRightY)
{
    Line& backLine = mLines.back();


    backLine.characters.push_back(character);

    if(topLeftX < backLine.topLeftX)
        backLine.topLeftX = topLeftX;
    if(botRightX > backLine.botRightX)
        backLine.botRightX = botRightX;

    if(topLeftY < backLine.topLeftY)
        backLine.topLeftY = topLeftY;
    if(botRightY > backLine.botRightY)
        backLine.botRightY = botRightY;

    backLine.maxAllowedDistanceY = std::max(short(backLine.botRightY - backLine.topLeftY), backLine.maxAllowedDistanceY);
    backLine.maxAllowedDistanceX = std::max(short(botRightX - topLeftX), backLine.maxAllowedDistanceX);
}

Text::Type TextBuilder::getTextType() const
{
    return mTextType;
}


void TextBuilder::insert(unsigned char character, short topLeftX, short topLeftY, short botRightX, short botRightY)
{
    if(mLines.empty())
    {
        appendLine(character, topLeftX, topLeftY, botRightX, botRightY);
        return;
    }

    Line& lastLine = mLines.back();
    if(topLeftX > lastLine.botRightX)
    {
        unsigned short dX = topLeftX - lastLine.botRightX;
        if(dX > lastLine.maxAllowedDistanceX)
        {
            appendLine(character, topLeftX, topLeftY, botRightX, botRightY);
            return;
        }
    }
    else if(topLeftX <= (lastLine.botRightX - lastLine.maxAllowedDistanceX))
    {
        appendLine(character, topLeftX, topLeftY, botRightX, botRightY);
        return;
    }
    else if(botRightY < lastLine.topLeftY || topLeftY > lastLine.botRightY)
    {
        appendLine(character, topLeftX, topLeftY, botRightX, botRightY);
        return;
    }

    if(isUppercase(character) || isNumeric(character))
    {
        unsigned char latestCharacter = mLines.back().characters.back();
        if(!isUppercase(latestCharacter) && !isNumeric(latestCharacter))
        {
            switch(latestCharacter)
            {
                case ' ':
                case '(':
                case '+':
                case '-':
                case ':':
                case ',':
                case '[':
                case '.':
                    break;

                default:
                    appendLine(character, topLeftX, topLeftY, botRightX, botRightY);
                    return;

            }
        }
    }
    appendCharacter(character, topLeftX, topLeftY, botRightX, botRightY);

    return;
}



void TextBuilder::build()
{
    if(mIsBuilt)
        return;

    for(const GlyphDraw& glyph : *mTextDraw.glyphDraws)
        insert(glyph.character, glyph.topLeft.x, glyph.topLeft.y, glyph.botRight.x, glyph.botRight.y);

    if(mIsDisputedContextMenuOrChatTab)
    {
        if(mLines.size() == 1)
        {
            std::string str;
            for(const unsigned char& c : mLines.front().characters)
                str.push_back(c);

            if(str == "Create Mark")
                mTextType = Text::Type::CONTEXT_MENU;
            else
                mTextType = Text::Type::CHAT_TAB;
        }
        else if(mLines.size() > 1)
        {
            auto itFirst = mLines.begin();
            auto itSecond = itFirst;
            itSecond++;

            if(itSecond->topLeftY >= itFirst->botRightY)
                mTextType = Text::Type::CONTEXT_MENU;
            else if(!itSecond->characters.empty() && itSecond->characters.front() == '(')
                mTextType = Text::Type::CONTEXT_MENU;
            else
                mTextType = Text::Type::CHAT_TAB;
        }
    }

    std::list<Text> lines;
    for(const Line& line : mLines)
    {
        Text t;

        mTextDraw.getScreenCoords(M_HALF_FRAME_WIDTH, M_HALF_FRAME_HEIGHT, t.x, t.y, line.topLeftX, line.topLeftY);
        t.localX = line.topLeftX;
        t.localY = line.topLeftY;
//        t.topLeftX = line.topLeftX;
//        t.topLeftY = line.topLeftY;
        t.width = line.botRightX - line.topLeftX;
        t.height = line.botRightY - line.topLeftY;
        t.string.reserve(line.characters.size());
        for(const unsigned char& c : line.characters)
            t.string.push_back(c);

        t.type = mTextType;

        lines.push_back(t);
    }

    mIsBuilt = true;
    mText = lines;
}

const std::list<Text>& TextBuilder::getText()
{
    build();
    return mText;
}

