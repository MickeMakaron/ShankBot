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

    mData->tileWidth = TILE_WIDTH;
    mData->tileHeight = TILE_HEIGHT;

    const SpriteDraw* groundTile = findFirstGroundTile();
    SB_EXPECT(groundTile, !=, nullptr);
    mData->offsetX = round(groundTile->topLeft.x + 128.f) % Constants::TILE_PIXEL_WIDTH;
    mData->offsetY = round(groundTile->topLeft.y + + 128.f) % Constants::TILE_PIXEL_HEIGHT;

    char prevTileX = -120;
    char prevTileY = -120;
    bool isPrevGround = false;
    mData->layers.resize(1);
    for(size_t i = 0; i < mCurrentDraws->size(); i++)
    {
        const SpriteDraw& draw = (*mCurrentDraws)[i];
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
            o.drawOrder = i;
            o.disputes = disputes;
            float x = (draw.topLeft.x + Constants::TILE_PIXEL_WIDTH * (spriteInfo.tileWidth - 1)) * VIEW_TEX_TO_SCREEN_RATIO_X;
            float y = (draw.topLeft.y + Constants::TILE_PIXEL_HEIGHT * (spriteInfo.tileHeight - 1)) * VIEW_TEX_TO_SCREEN_RATIO_Y;
            o.screenBounds.x = round(x) + mData->screenBounds.x;
            o.screenBounds.y = round(y) + mData->screenBounds.y;
            o.screenBounds.width = TILE_WIDTH_I;
            o.screenBounds.height = TILE_HEIGHT_I;
            o.localBounds.x = round(draw.topLeft.x);
            o.localBounds.y = round(draw.topLeft.y);
            o.localBounds.width = Constants::TILE_PIXEL_WIDTH;
            o.localBounds.height = Constants::TILE_PIXEL_HEIGHT;

            Layer& layer = mData->layers.back();
            if(firstObject.type == AObject::Type::OUTFIT)
            {
                Draw d;
                d.spriteId = draw.pairings.front().spriteId;
                d.x = VISIBILITY_OFFSET_LOW * 2 * 32 + round(draw.topLeft.x);
                d.y = VISIBILITY_OFFSET_LOW * 2 * 32 + round(draw.topLeft.y);
                d.layer = mData->layers.size() - 1;
                mData->draws.push_back(d);
                layer.characters.push_back(o);
            }
            else
            {
                layer.projectiles.push_back(o);
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

            bool isGround = firstObject.itemInfo.isGround || firstObject.itemInfo.topOrder == 1;
            if(isGround)
            {
                if(!isPrevGround && tileX + tileY < prevTileX + prevTileY)
                {
                    mData->layers.emplace_back();
                }
            }
            Layer& layer = mData->layers.back();

            Tile& tile = layer.gridObjects[tileX + VISIBILITY_OFFSET_LOW][tileY + VISIBILITY_OFFSET_LOW];
            Object o;
            o.drawOrder = i;
            if(firstObject.itemInfo.isGround && firstObject.itemInfo.topOrder == 0)
            {
                tile.isObscured = false;
                tile.speed = firstObject.itemInfo.walkSpeed;
            }

            if(firstObject.itemInfo.isBlocking && !firstObject.itemInfo.isMovable)
            {
                tile.isWalkable = false;
            }

            isPrevGround = isGround;
            Draw d;
            d.spriteId = draw.pairings.front().spriteId;
            d.x = VISIBILITY_OFFSET_LOW * 2 * 32 + round(draw.topLeft.x);
            d.y = VISIBILITY_OFFSET_LOW * 2 * 32 + round(draw.topLeft.y);
            d.layer = mData->layers.size() - 1;
            mData->draws.push_back(d);
            prevTileX = tileX;
            prevTileY = tileY;
            o.disputes = disputes;
            tile.objects.push_back(o);
        }

    }
    mData->playerLayer = -1;
    auto playerLayerIt = std::find_if(mData->layers.rbegin(), mData->layers.rend(), [](const Layer& l)
    {
        for(auto it = l.characters.rbegin(); it != l.characters.rend(); it++)
        {
            int dMiddleX = it->localBounds.x + it->localBounds.width - Constants::MIDDLE_TILE_X * Constants::TILE_PIXEL_WIDTH;
            int dMiddleY = it->localBounds.y + it->localBounds.height - Constants::MIDDLE_TILE_Y * Constants::TILE_PIXEL_HEIGHT;
            if(dMiddleX == dMiddleY && dMiddleX <= -8 && dMiddleX >= -32)
            {
                return true;
            }
        }
        return false;
    });

    mData->playerLayer = std::distance(playerLayerIt, mData->layers.rend()) - 1;
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
