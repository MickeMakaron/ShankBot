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
#ifndef GRAPHICS_LAYER_SCENE_HPP
#define GRAPHICS_LAYER_SCENE_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Constants.hpp"
#include "Draw.hpp"
namespace GraphicsLayer
{
    class SpriteInfo;
    struct Frame;
    class TibiaContext;
}
namespace sb
{
namespace tibiaassets
{
    struct Object;
}
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <list>
#include <functional>
///////////////////////////////////

namespace GraphicsLayer
{
    class Scene
    {
        public:

            struct Object
            {
                char tileX;
                char tileY;
                char layer;
                float screenX;
                float screenY;
                size_t object;
            };


            struct Tile
            {
                std::vector<Object> objects;
                char tileX;
                char tileY;
                char numLayers = 0;
            };

            static const int VISIBILITY_OFFSET_LOW = 2;
            static const int VISIBILITY_OFFSET_HIGH = 3;
            static const int MAX_VISIBLE_TILES_X = VISIBILITY_OFFSET_LOW + Constants::NUM_TILES_VIEW_X + VISIBILITY_OFFSET_HIGH;
            static const int MAX_VISIBLE_TILES_Y = VISIBILITY_OFFSET_LOW + Constants::NUM_TILES_VIEW_Y + VISIBILITY_OFFSET_HIGH;
            struct Data
            {
                std::array<std::array<Tile, MAX_VISIBLE_TILES_Y>, MAX_VISIBLE_TILES_X> tiles;
                bool isMovingX;
                bool isMovingY;
                short offsetX;
                short offsetY;
            };



        public:
            explicit Scene(const TibiaContext& context);

            void update(const Frame& frame);

            std::list<Object> get(const std::function<bool(const Object& object)>& func) const;
            void forEach(const std::function<void(const Object& object)>& func) const;
            void forEach(const std::function<void(const Tile& tile)>& func) const;
            const Tile& getTile(char tileX, char tileY) const;
            char resetMovementX();
            char resetMovementY();

            float getTileWidth() const;
            float getTileHeight() const;

            const Data& getData() const;


        private:
            void updateMovement(short x, short y);
            void parseCurrentFrame();
            const SpriteDraw* findFirstGroundTile() const;


        private:
            static const int MAX_MOVEMENT = 10;

            std::shared_ptr<std::vector<SpriteDraw>> mCurrentDraws = std::make_shared<std::vector<SpriteDraw>>();
            bool mIsCurrentFrameParsed = false;

            std::shared_ptr<Data> mData = std::make_shared<Data>();

            const TibiaContext& mContext;

            short mPreviousMiniMapX = 0;
            short mPreviousMiniMapY = 0;
            char mMovementX = 0;
            char mMovementY = 0;

            float mSceneX = 0;
            float mSceneY = 0;
            float mSceneWidth = 0;
            float mSceneHeight = 0;
    };
}

#endif // GRAPHICS_LAYER_SCENE_HPP
