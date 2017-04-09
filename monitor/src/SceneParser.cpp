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
#include "monitor/SceneParser.hpp"
#include "utility/utility.hpp"
#include "tibiaassets/Object.hpp"
#include "monitor/Frame.hpp"
#include "monitor/TibiaContext.hpp"
using namespace GraphicsLayer;
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>
///////////////////////////////////

SceneParser::SceneParser(const TibiaContext& context)
: mContext(context)
{
}

void SceneParser::parse(const Frame& frame)
{
    mData.reset(new Data());
    mCurrentDraws = frame.spriteDraws;
    if(mCurrentDraws == nullptr || mCurrentDraws->empty())
    {
        return;
    }

    mData->screenBounds.x = round(frame.viewX);
    mData->screenBounds.y = round(frame.viewY);
    mData->screenBounds.width = round(frame.viewWidth);
    mData->screenBounds.height = round(frame.viewHeight);

    const float VIEW_TEX_TO_SCREEN_RATIO_X = mData->screenBounds.width / ((float)Constants::VIEW_PIXEL_WIDTH);
    const float VIEW_TEX_TO_SCREEN_RATIO_Y = mData->screenBounds.height / ((float)Constants::VIEW_PIXEL_HEIGHT);
    const float TILE_WIDTH = getTileWidth();
    const float TILE_HEIGHT = getTileHeight();
    const int TILE_WIDTH_I = round(TILE_WIDTH);
    const int TILE_HEIGHT_I = round(TILE_HEIGHT);

    const SpriteDraw* groundTile = findFirstGroundTile();
    SB_EXPECT(groundTile, !=, nullptr);
    mData->offsetX = round(groundTile->topLeft.x + 128.f) % Constants::TILE_PIXEL_WIDTH;
    mData->offsetY = round(groundTile->topLeft.y + + 128.f) % Constants::TILE_PIXEL_HEIGHT;

    for(size_t x = 0; x < mData->gridObjects.size(); x++)
    {
        std::array<Tile, MAX_VISIBLE_TILES_Y>& col = mData->gridObjects[x];
        for(size_t y = 0; y < col.size(); y++)
        {
            Tile& t = col[y];
            t.screenBounds.x = int(x) - VISIBILITY_OFFSET_LOW;
            t.screenBounds.y = int(y) - VISIBILITY_OFFSET_LOW;
            t.screenBounds.x = round(float(t.screenBounds.x) * TILE_WIDTH) + mData->screenBounds.x + mData->offsetX;
            t.screenBounds.y = round(float(t.screenBounds.y) * TILE_HEIGHT) + mData->screenBounds.y + mData->offsetY;
            t.screenBounds.width = TILE_WIDTH_I;
            t.screenBounds.height = TILE_HEIGHT_I;
        }
    }

    for(const SpriteDraw& draw : *mCurrentDraws)
    {
        using AObject = sb::tibiaassets::Object;
        std::vector<const AObject*> disputes;
        for(const auto& pairing : draw.pairings)
        {
            for(const size_t objectId : pairing.objects)
            {
                disputes.push_back(&mContext.getObjects()[objectId]);
            }
        }
        SB_EXPECT_FALSE(disputes.empty());
        const AObject& firstObject = *disputes.front();
        const SpriteInfo::Info& spriteInfo = mContext.getSpriteInfo().get(draw.pairings.front().spriteId);

        if(firstObject.type == AObject::Type::OUTFIT || firstObject.type == AObject::Type::PROJECTILE)
        {
            NonGridObject o;
            o.disputes = disputes;
            float x = (draw.topLeft.x + Constants::TILE_PIXEL_WIDTH * (spriteInfo.tileWidth - 1)) * VIEW_TEX_TO_SCREEN_RATIO_X;
            float y = (draw.topLeft.y + Constants::TILE_PIXEL_HEIGHT * (spriteInfo.tileHeight - 1)) * VIEW_TEX_TO_SCREEN_RATIO_Y;
            o.screenBounds.x = round(x) + mData->screenBounds.x;
            o.screenBounds.y = round(y) + mData->screenBounds.y;
            o.screenBounds.width = TILE_WIDTH_I;
            o.screenBounds.height = TILE_HEIGHT_I;
            if(firstObject.type == AObject::Type::OUTFIT)
            {
                mData->characters.push_back(o);
            }
            else
            {
                mData->projectiles.push_back(o);
            }
        }
        else
        {
            short x = round(draw.topLeft.x);
            short y = round(draw.topLeft.y);
            x += short(firstObject.itemInfo.offsetX) - mData->offsetX;
            y += short(firstObject.itemInfo.offsetY) - mData->offsetY;

            short remainderX = x % Constants::TILE_PIXEL_WIDTH;
            short remainderY = y % Constants::TILE_PIXEL_HEIGHT;

            x += (remainderX < 0 ? -remainderX : Constants::TILE_PIXEL_WIDTH - remainderX);
            y += (remainderY < 0 ? -remainderY : Constants::TILE_PIXEL_HEIGHT - remainderY);

            char tileX = x / Constants::TILE_PIXEL_WIDTH + spriteInfo.tileWidth - 1;
            char tileY = y / Constants::TILE_PIXEL_HEIGHT + spriteInfo.tileHeight - 1;
            assert(tileX >= -VISIBILITY_OFFSET_LOW && tileX < MAX_VISIBLE_TILES_X - VISIBILITY_OFFSET_LOW);
            assert(tileY >= -VISIBILITY_OFFSET_LOW && tileY < MAX_VISIBLE_TILES_Y - VISIBILITY_OFFSET_LOW);

            Tile& tile = mData->gridObjects[tileX + VISIBILITY_OFFSET_LOW][tileY + VISIBILITY_OFFSET_LOW];
            Object o;
            o.disputes = disputes;
            tile.objects.push_back(o);
        }

    }
}

float SceneParser::getTileWidth() const
{
    assert(mData->screenBounds.width > 0.1f);
    return float(mData->screenBounds.width) / float(Constants::NUM_TILES_VIEW_X);
}


float SceneParser::getTileHeight() const
{
    assert(mData->screenBounds.height > 0.1f);
    return float(mData->screenBounds.height) / float(Constants::NUM_TILES_VIEW_Y);
}

const SpriteDraw* SceneParser::findFirstGroundTile() const
{
    assert(mCurrentDraws != nullptr);
    for(const auto& draw : *mCurrentDraws)
    {
        for(const auto& pairing : draw.pairings)
        {
            for(const size_t& id : pairing.objects)
            {
                const sb::tibiaassets::Object& obj = mContext.getObjects()[id];
                if(obj.type == sb::tibiaassets::Object::Type::ITEM && obj.itemInfo.isGround)
                {
                    return &draw;
                }
            }
        }
    }
    return nullptr;
}

const SceneParser::Data& SceneParser::getData() const
{
    return *mData;
}
