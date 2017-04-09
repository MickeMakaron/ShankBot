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
#include "monitor/RectParser.hpp"
#include "monitor/Frame.hpp"
#include "utility/utility.hpp"
#include "monitor/Constants.hpp"
using namespace GraphicsLayer;
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
#include <algorithm>
///////////////////////////////////

RectParser::RectParser()
{
}

void RectParser::parse(const Frame& frame)
{
    mData = Data();
    mDraws = frame.rectDraws;

    if(mDraws == nullptr)
    {
        return;
    }

    auto printUnhandled = [this](size_t i)
    {
        const RectDraw& d = (*mDraws)[i];
//        std::cout   << "Unhandled: " << i << std::endl
//                    << "\tdraw call id: " << d.drawCallId << std::endl
//                    << "\t" << d.topLeft.x << "x" << d.topLeft.y << " (" << d.botRight.x - d.topLeft.x << "x" << d.botRight.y - d.topLeft.y << ")" << std::endl
//                    << "\t" << (int)d.color.r << " " << (int)d.color.g << " " << (int)d.color.b << " " << (int)d.color.a << std::endl;
    };
    for(size_t i = 0; i < mDraws->size(); i++)
    {
        const RectDraw& d = (*mDraws)[i];
        using C = Constants::RectColor;
        try
        {
            switch((C)d.color.packed)
            {
                case C::HP_GRAY:
                case C::HP_FULL:
                case C::HP_LIGHT_GREEN:
                case C::HP_YELLOW:
                case C::HP_LIGHT_RED:
                case C::HP_RED:
                case C::HP_DARK_RED:
                case C::SKILL_BAR_GREEN_FULL:
                {
                    short intHeight = round(d.botRight.y - d.topLeft.y);
                    if(intHeight == 3 || intHeight == 2)
                    {
                        i++;
                        SB_EXPECT(i, <, mDraws->size());

                        const RectDraw& border = (*mDraws)[i];
                        SB_EXPECT(d.drawCallId, ==, border.drawCallId);
                        SB_EXPECT(border.color.packed, ==, (int)C::HP_BACKGROUND);

                        Bar b = mergeBarBorderAndFill(border, d);
                        if(intHeight == 2)
                        {
                            mData.hpBars.push_back(b);
                        }
                        else
                        {
                            insertBar(b);
                        }
                    }
                    else
                    {
                        printUnhandled(i);
                    }
                    break;
                }

                case C::HP_BACKGROUND:
                {
                    short intHeight = round(d.botRight.y - d.topLeft.y);
                    if(intHeight == 5 || intHeight == 4)
                    {
                        Bar b;
                        b.border.draw = &d;
                        b.percent = 0.f;

                        i++;
                        if(i < mDraws->size() && (*mDraws)[i].drawCallId == d.drawCallId)
                        {
                            const RectDraw& fill = (*mDraws)[i];
                            if(round(fill.topLeft.x - d.topLeft.x) == 1 && round(fill.topLeft.y - d.topLeft.y) == 1)
                            {
                                b = mergeBarBorderAndFill(d, fill);
                            }
                            else
                            {
                                i--;
                            }
                        }
                        else
                        {
                            i--;
                        }

                        if(intHeight == 4)
                        {
                            mData.hpBars.push_back(b);
                        }
                        else
                        {
                            insertBar(b);
                        }
                    }
                    else
                    {
                        printUnhandled(i);
                    }
                    break;
                }

                case C::TEXT_INPUT_FIELD:
                    mData.textInputFields.emplace_back(d);
                    break;

                case C::TEXT_CURSOR_CHAT_LOCAL:
                case C::TEXT_CURSOR_CHAT_NO_LOCAL:
                    SB_EXPECT(round(d.botRight.x - d.topLeft.x), ==, 1);
                    SB_EXPECT(round(d.botRight.y - d.topLeft.y), ==, 13);
                    mData.chatInputCursor = Rect(d);
                    break;



                default:
                    printUnhandled(i);
                    break;
            }
        }
        catch(const std::runtime_error& e)
        {
            std::cout << "Failed to handle rect draw: " << e.what() << std::endl;
        }

    }

    for(std::vector<Bar>& bars : mData.bars)
    {
        if(bars.size() > 1)
        {
            if(bars[0].border.draw->topLeft.y > bars[1].border.draw->topLeft.y)
            {
                std::reverse(bars.begin(), bars.end());
            }
        }
        short x = round(bars[0].border.draw->topLeft.x);
        switch(x)
        {
            case 41:
                SB_EXPECT_TRUE(mData.preyBars.empty());
                mData.preyBars = bars;
                break;

            case 6:
                SB_EXPECT_TRUE(mData.skillBars.empty());
                mData.skillBars = bars;
                break;

            case 22:
                SB_EXPECT_TRUE(mData.battleBars.empty());
                mData.battleBars = bars;
                break;

            default:
                SB_THROW("Unexpected bar X offset: ", x);
        }
    }

//    for(const Bar& bar : mData.hpBars)
//    {
//        std::cout << "HP BAR: " << bar.percent * 100.f << "%" << std::endl;
//    }
//    for(const std::vector<Bar>& bars : mData.bars)
//    {
//        std::cout << "BAR GROUP: " << std::endl;
//        for(const Bar& bar : bars)
//        {
//            std::cout << "\tBAR: " << bar.percent * 100.f << "%" << std::endl;
//        }
//    }
//    for(const Rect& r : mData.textInputFields)
//    {
//        const RectDraw& d = *r.draw;
//        std::cout << "TEXT INPUT FIELD: " << d.botRight.x - d.topLeft.x << "x" << d.botRight.y - d.topLeft.y << std::endl;
//    }
}

RectParser::Bar RectParser::mergeBarBorderAndFill(const RectDraw& border, const RectDraw& fill)
{
    SB_EXPECT(round(border.botRight.y - border.topLeft.y) - round(fill.botRight.y - fill.topLeft.y), ==, 2);

    Bar b;
    b.border.draw = &border;
    b.fill.draw = &fill;

    float borderWidth = border.botRight.x - border.topLeft.x;
    float fillWidth = fill.botRight.x - fill.topLeft.x;
    b.percent = fillWidth / (borderWidth - 2.f);

    return b;
}

void RectParser::insertBar(const Bar& b)
{
    std::vector<Bar>* group;
    if(mData.bars.empty())
    {
        mData.bars.emplace_back();
        group = &mData.bars.back();
    }
    else
    {
        std::vector<Bar>& back = mData.bars.back();
        if(back.back().border.draw->drawCallId == b.border.draw->drawCallId)
        {
            group = &back;
        }
        else
        {
            mData.bars.emplace_back();
            group = &mData.bars.back();
        }
    }

    group->push_back(b);
}

const RectParser::Data& RectParser::getData()
{
    return mData;
}
