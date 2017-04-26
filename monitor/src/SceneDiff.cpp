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
#include "monitor/SceneDiff.hpp"
#include "monitor/Frame.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
#include <algorithm>
#include <cassert>
///////////////////////////////////

SceneDiff::SceneDiff()
{
    mPrevScene.layers.resize(1);
    mPrevScene.playerLayer = 0;
}

void SceneDiff::parse(const Frame& frame, const SceneParser::Data& scene)
{
    auto getGround = [](const SceneParser::Tile& t)
    {
        const sb::tibiaassets::Object* ground = nullptr;
        if(t.isObscured)
        {
            return ground;
        }

        for(const SceneParser::Object& disputes : t.objects)
        {
            const sb::tibiaassets::Object* o = disputes.disputes.front();
            if(o->type == sb::tibiaassets::Object::Type::ITEM &&
               o->itemInfo.isGround &&
               o->itemInfo.topOrder == 0 &&
               o->itemInfo.hasMinimapColor)
            {
                return o;
            }
        }

        return ground;
    };



    auto toGrounds = [&getGround](const std::array<SceneParser::Tile, SceneParser::MAX_VISIBLE_TILES_Y>& col)
    {
        std::array<const sb::tibiaassets::Object*, SceneParser::MAX_VISIBLE_TILES_Y> groundCol;
        std::transform(col.begin(), col.end(), groundCol.begin(), getGround);
        return groundCol;
    };

    const SceneParser::Layer& oldLayer = mPrevScene.layers[mPrevScene.playerLayer];
    const SceneParser::Layer& newLayer = scene.layers[scene.playerLayer];
    using Column = std::array<const sb::tibiaassets::Object*, SceneParser::MAX_VISIBLE_TILES_Y>;
    using Grid = std::array<Column, SceneParser::MAX_VISIBLE_TILES_X>;

    Grid oldGrounds;
    Grid newGrounds;
    std::transform(oldLayer.gridObjects.begin(), oldLayer.gridObjects.end(), oldGrounds.begin(), toGrounds);
    std::transform(newLayer.gridObjects.begin(), newLayer.gridObjects.end(), newGrounds.begin(), toGrounds);


    auto isObjectEqual = [](const sb::tibiaassets::Object* lhs, const sb::tibiaassets::Object* rhs)
    {
        return !lhs || !rhs || lhs == rhs;
    };

    int newOffsetY = 0;
    auto isColumnEqual = [&newOffsetY, &isObjectEqual](const Column& lhs, const Column& rhs)
    {
        assert(std::abs(newOffsetY) < rhs.size());
        if(newOffsetY < 0)
        {
            return std::equal(rhs.begin() - newOffsetY, rhs.end(), lhs.begin(), isObjectEqual);
        }
        else
        {
            return std::equal(lhs.begin() + newOffsetY, lhs.end(), rhs.begin(), isObjectEqual);
        }
    };

    int newOffsetX = 0;
    auto isGridEqual = [&newOffsetY, &newOffsetX, &isColumnEqual](const Grid& lhs, const Grid& rhs, int offsetX, int offsetY)
    {
        newOffsetX = offsetX;
        newOffsetY = offsetY;
        assert(std::abs(newOffsetX) < rhs.size());
        if(newOffsetX < 0)
        {
            return std::equal(rhs.begin() - newOffsetX, rhs.end(), lhs.begin(), isColumnEqual);
        }
        else
        {
            return std::equal(lhs.begin() + newOffsetX, lhs.end(), rhs.begin(), isColumnEqual);
        }
    };

    if(oldGrounds != newGrounds && !isGridEqual(oldGrounds, newGrounds, 0, 0))
    {
        for(size_t i = 0; i < 1; i++)
        {
//            std::cout << "-- " << i << "--" << std::endl;
            std::cout << "moving: ";

            if(isGridEqual(oldGrounds, newGrounds, 0, 1))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, 0, -1))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, 1, 0))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, -1, 0))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, 1, 1))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, 1, -1))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, -1, 1))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            else if(isGridEqual(oldGrounds, newGrounds, -1, -1))
            {
                std::cout << newOffsetX << "x" << newOffsetY;
            }
            std::cout << std::endl;
        }

    }
    for(const auto& col : newLayer.gridObjects)
    {
        for(const SceneParser::Tile& t : col)
        {
            for(const SceneParser::Object& so : t.objects)
            {
                for(const sb::tibiaassets::Object* o : so.disputes)
                {
//                    if(o->type == sb::tibiaassets::Object::Type::ITEM &&
//                       o->itemInfo.defaultAction != (unsigned short)-1)
//                    {
//                        std::cout << o->id << ": " << o->itemInfo.defaultAction << std::endl;
//                        break;
//                    }
                    if(o->type == sb::tibiaassets::Object::Type::ITEM &&
                       o->itemInfo.isRightClickable)
                    {
//                        std::cout << o->id;
//                        if(o->itemInfo.hasMarketInfo)
//                        {
//                            std::cout << ": " << o->itemInfo.marketInfo.name;
//                        }
//                        std::cout << std::endl;
                        break;
                    }
                }
            }
        }
    }

//
//    for(size_t x = 0; x < oldGrounds.size(); x++)
//    {
//        const auto& oldCol = oldGrounds[x];
//        const auto& newCol = newGrounds[x];
//        for(size_t y = 0; y < oldCol.size(); y++)
//        {
//            if(!isEqual(oldCol[y], newCol[y]))
//            {
//
//            }
//        }
//    }


//    std::cout << scene.offsetX - mPrevScene.offsetX << "x" << scene.offsetY - mPrevScene.offsetY << std::endl;
    mPrevScene = scene;
    mPrevFrame = frame;
//    int dX = map.getX() - mPrevX;
//    int dY = map.getY() - mPrevY;
//    char dLevel = map.getLevel() - mPrevLevel;
}


const SceneDiff::Data& SceneDiff::getData() const
{
    return mData;
}
