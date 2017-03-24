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
#include "monitor/Scene.hpp"
#include "utility/utility.hpp"
#include "tibiaassets/Object.hpp"
#include "monitor/Frame.hpp"
#include "monitor/TibiaContext.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>
///////////////////////////////////

Scene::Scene(const TibiaContext& context)
: mContext(context)
{
}

void Scene::update(const Frame& frame)
{
    mData.reset(new Data());
    if(!frame.spriteDraws->empty())
    {
        mCurrentDraws = frame.spriteDraws;
        mSceneWidth = frame.viewWidth;
        mSceneHeight = frame.viewHeight;
        mSceneX = frame.viewX;
        mSceneY = frame.viewY;
        mIsCurrentFrameParsed = false;
    }

    if(frame.hasMiniMapMoved)
    {
        updateMovement(frame.miniMapX, frame.miniMapY);
    }

    parseCurrentFrame();
}

const Scene::Tile& Scene::getTile(char tileX, char tileY) const
{
    if(tileX < 0 || tileY < 0 || tileX >= Constants::NUM_TILES_VIEW_X || tileY >= Constants::NUM_TILES_VIEW_Y)
        THROW_RUNTIME_ERROR("Parameters out of range.");

    return mData->tiles[tileX + VISIBILITY_OFFSET_LOW][tileY + VISIBILITY_OFFSET_LOW];
}

float Scene::getTileWidth() const
{
    assert(mSceneWidth > 0.1f);
    return mSceneWidth / ((float)Constants::NUM_TILES_VIEW_X);
}


float Scene::getTileHeight() const
{
    assert(mSceneHeight > 0.1f);
    return mSceneHeight / ((float)Constants::NUM_TILES_VIEW_Y);
}


void Scene::parseCurrentFrame()
{
//    if(mIsCurrentFrameParsed)
//        return;

    for(size_t x = 0; x < MAX_VISIBLE_TILES_X; x++)
        for(size_t y = 0; y < MAX_VISIBLE_TILES_Y; y++)
        {
            Tile& tile = mData->tiles[x][y];
            tile = Tile();
            tile.tileX = x - VISIBILITY_OFFSET_LOW;
            tile.tileY = y - VISIBILITY_OFFSET_LOW;
        }

    const float VIEW_TEX_TO_SCREEN_RATIO_X = mSceneWidth / ((float)Constants::VIEW_PIXEL_WIDTH);
    const float VIEW_TEX_TO_SCREEN_RATIO_Y = mSceneHeight / ((float)Constants::VIEW_PIXEL_HEIGHT);
    const float TILE_WIDTH = getTileWidth();
    const float TILE_HEIGHT = getTileHeight();
    for(const SpriteDraw& draw : *mCurrentDraws)
    {
        if(!draw.pairings.empty())
        {
            const SpriteInfo::Info& spriteInfo = mContext.getSpriteInfo().get(draw.pairings.front().spriteId);

            const sb::tibiaassets::Object& firstObject = mContext.getObjects()[draw.pairings.front().objects.front()];

            short x = draw.topLeft.x + firstObject.itemInfo.offsetX;
            short y = draw.topLeft.y + firstObject.itemInfo.offsetY;

            short remainderX;
            remainderX = x % Constants::TILE_PIXEL_WIDTH;
            if(remainderX > 0)
                x += Constants::TILE_PIXEL_WIDTH - remainderX;
            else
                x -= remainderX;

            char remainderY = y % Constants::TILE_PIXEL_HEIGHT;
            if(remainderY > 0)
                y += Constants::TILE_PIXEL_HEIGHT - remainderY;
            else
                y -= remainderY;

            char tileX = x / Constants::TILE_PIXEL_WIDTH + spriteInfo.tileWidth - 1;
            char tileY = y / Constants::TILE_PIXEL_HEIGHT + spriteInfo.tileHeight - 1;

            float screenX = mSceneX + VIEW_TEX_TO_SCREEN_RATIO_X * draw.topLeft.x + TILE_WIDTH * float(spriteInfo.tileWidth - 1);
            float screenY = mSceneY + VIEW_TEX_TO_SCREEN_RATIO_Y * draw.topLeft.y + TILE_HEIGHT * float(spriteInfo.tileHeight - 1);

            assert(tileX >= -VISIBILITY_OFFSET_LOW && tileX < MAX_VISIBLE_TILES_X - VISIBILITY_OFFSET_LOW);
            assert(tileY >= -VISIBILITY_OFFSET_LOW && tileY < MAX_VISIBLE_TILES_Y - VISIBILITY_OFFSET_LOW);

            Tile& tile = mData->tiles[tileX + VISIBILITY_OFFSET_LOW][tileY + VISIBILITY_OFFSET_LOW];
            for(const SpriteDraw::SpriteObjectPairing& pair : draw.pairings)
            {
                assert(!pair.objects.empty());
                for(size_t obj : pair.objects)
                {
//                    if(obj->itemInfo.isGround)
//                    {
//                        tile = Tile();
//                        tile.tileX = tileX;
//                        tile.tileY = tileY;
//                    }

                    Object o;
                    o.tileX = tileX;
                    o.tileY = tileY;
                    o.screenX = screenX;
                    o.screenY = screenY;
                    o.layer = tile.numLayers;
                    o.object = obj;

                    tile.knownLayerObjects.push_back(o);
                }
            }
            tile.numLayers++;
        }
    }

    for(size_t x = 0; x < MAX_VISIBLE_TILES_X; x++)
        for(size_t y = 0; y < MAX_VISIBLE_TILES_Y; y++)
        {
            Tile& tile = mData->tiles[x][y];
            for(Object& object : tile.knownLayerObjects)
                object.layer = (tile.numLayers - 1) - object.layer;
        }

    mIsCurrentFrameParsed = true;
}

std::list<Scene::Object> Scene::get(const std::function<bool(const Object& object)>& func) const
{
//    parseCurrentFrame();

    std::list<Object> objects;
    forEach([&objects, &func](const Object& object)
    {
        if(func(object))
            objects.push_back(object);
    });
//    for(size_t x = 0; x < MAX_VISIBLE_TILES_X; x++)
//        for(size_t y = 0; y < MAX_VISIBLE_TILES_Y; y++)
//        {
//            const Tile& tile = mData->tiles[x][y];
//            for(const Object& obj : tile.knownLayerObjects)
//            {
//                if(func(obj))
//                    objects.push_back(obj);
//            }
//        }

    return objects;
}

void Scene::forEach(const std::function<void(const Object& object)>& func) const
{
//    parseCurrentFrame();

    for(size_t x = 0; x < MAX_VISIBLE_TILES_X; x++)
        for(size_t y = 0; y < MAX_VISIBLE_TILES_Y; y++)
        {
            const Tile& tile = mData->tiles[x][y];
            for(const Object& obj : tile.knownLayerObjects)
            {
                func(obj);
            }
        }
}

void Scene::forEach(const std::function<void(const Tile& tile)>& func) const
{
//    parseCurrentFrame();

    for(size_t x = 0; x < MAX_VISIBLE_TILES_X; x++)
        for(size_t y = 0; y < MAX_VISIBLE_TILES_Y; y++)
        {
            func(mData->tiles[x][y]);
        }
}


void Scene::updateMovement(short x, short y)
{
    short movementX = mPreviousMiniMapX - x;
    if(movementX)
    {
        if(movementX > Constants::MINI_MAP_PIXEL_WIDTH - MAX_MOVEMENT)
            movementX -= Constants::MINI_MAP_PIXEL_WIDTH;
        else if(movementX < MAX_MOVEMENT - Constants::MINI_MAP_PIXEL_WIDTH)
            movementX += Constants::MINI_MAP_PIXEL_WIDTH;
    }

    short movementY = mPreviousMiniMapY - y;
    if(movementY)
    {
        if(movementY > Constants::MINI_MAP_PIXEL_HEIGHT - MAX_MOVEMENT)
            movementY -= Constants::MINI_MAP_PIXEL_HEIGHT;
        else if(movementY < MAX_MOVEMENT - Constants::MINI_MAP_PIXEL_HEIGHT)
            movementY += Constants::MINI_MAP_PIXEL_HEIGHT;
    }

    mMovementX += movementX;
    mMovementY += movementY;

    mPreviousMiniMapX = x;
    mPreviousMiniMapY = y;
}

char Scene::resetMovementX()
{
    char movementX = mMovementX;
    mMovementX = 0;
    return movementX;
}

char Scene::resetMovementY()
{
    char movementY = mMovementY;
    mMovementY = 0;
    return movementY;
}


const Scene::Data& Scene::getData() const
{
    return *mData;
}
