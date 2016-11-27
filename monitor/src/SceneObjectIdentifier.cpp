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
#include "SceneObjectIdentifier.hpp"
#include "OutfitResolver.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
///////////////////////////////////


void SceneObjectIdentifier::update(const Scene& scene, const OutfitResolver& outfitResolver)
{
    detectSceneMovement(scene);
}

bool SceneObjectIdentifier::compareOffsetScene(size_t* lhs, size_t lhsWidth, size_t lhsHeight, size_t* rhs)
{
    for(size_t x = 0; x < lhsWidth; x++)
    {
        for(size_t y = 0; y < lhsHeight; y++)
        {
            if(lhs[x + y * lhsWidth] != rhs[x + y * lhsWidth])
                return false;
        }
    }

    return true;
}



void SceneObjectIdentifier::detectSceneMovement(const Scene& scene)
{
    int movementX = 0;
    int movementY = 0;
    size_t immovableIds[Constants::NUM_TILES_VIEW_X][Constants::NUM_TILES_VIEW_Y] = {{0}};
    for(size_t x = 0; x < Constants::NUM_TILES_VIEW_X; x++)
    {
        for(size_t y = 0; y < Constants::NUM_TILES_VIEW_Y; y++)
        {
            const Scene::Tile& tile = scene.getTile(x, y);
            for(auto it = tile.knownLayerObjects.rbegin(); it != tile.knownLayerObjects.rend(); it++)
                if(!it->object->itemInfo.isMovable)
                {
                    immovableIds[x][y] = it->object->id;
                    break;
                }
        }
    }

    std::cout << ":::::::::::::::::::::::::" << std::endl;
    std::cout << "Old: " << std::endl;
    for(size_t x = 0; x < Constants::NUM_TILES_VIEW_X; x++)
    {
        for(size_t y = 0; y < Constants::NUM_TILES_VIEW_Y; y++)
        {
            std::cout << mImmovableIds[x][y] << "\t";
        }

        std::cout << std::endl;
    }
    std::cout << "New: " << std::endl;
    for(size_t x = 0; x < Constants::NUM_TILES_VIEW_X; x++)
    {
        for(size_t y = 0; y < Constants::NUM_TILES_VIEW_Y; y++)
        {
            std::cout << immovableIds[x][y] << "\t";
        }

        std::cout << std::endl;
    }
    std::cout << ":::::::::::::::::::::::::" << std::endl;

    if(compareOffsetScene(*immovableIds, Constants::NUM_TILES_VIEW_X, Constants::NUM_TILES_VIEW_Y, *mImmovableIds))
    {

        std::cout << "No movement" << std::endl;
        return;
    }

    for(int x = -5; x <= 5; x++)
    {
        for(int y = -5; y <= 5; y++)
        {
            size_t lhsX = std::max(0, -x);
            size_t lhsY = std::max(0, -y);
            size_t rhsX = std::max(0, x);
            size_t rhsY = std::max(0, y);
            size_t width = Constants::NUM_TILES_VIEW_X - std::abs(x);
            size_t height = Constants::NUM_TILES_VIEW_Y - std::abs(y);

            if(compareOffsetScene(&immovableIds[lhsX][lhsY], width, height, &mImmovableIds[rhsX][rhsY]))
            {
                std::cout << "Moved " << x << "x" << y << std::endl;
                memcpy(mImmovableIds, immovableIds, Constants::NUM_TILES_VIEW_X * Constants::NUM_TILES_VIEW_Y * sizeof(size_t));
                return;
            }
        }
    }

    std::cout << "Unknown movement" << std::endl;
    memcpy(mImmovableIds, immovableIds, Constants::NUM_TILES_VIEW_X * Constants::NUM_TILES_VIEW_Y * sizeof(size_t));
}


const Scene::Object* SceneObjectIdentifier::get(unsigned short id) const
{
    if(id >= mObjects.size())
        return nullptr;

    return mObjects[id].get();
}
