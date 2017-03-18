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
                bool isOnStack = false;
                float screenX;
                float screenY;
                size_t object;

//                bool operator<(const Object& other) const
//                {
//                    return
//                    (
//                        tileX != other.tileX ||
//                        tileY != other.tileY ||
//                        layer != other.layer ||
//                        object != other.object
//                    );
//                }

                bool operator==(const Object& other) const
                {
                    return
                    (
                        tileX == other.tileX &&
                        tileY == other.tileY &&
                        layer == other.layer &&
                        object == other.object
                    );
                }
            };


            struct Tile
            {
                std::list<Object> knownLayerObjects;
                char tileX;
                char tileY;
                char numLayers = 0;
                char height = 0;
                char stackStartLayer = -1;

                std::list<Object> unknownLayerObjects;
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


        private:
            void updateMovement(short x, short y);
            void parseCurrentFrame();

        private:
            static const int VISIBILITY_OFFSET_LOW = 2;
            static const int VISIBILITY_OFFSET_HIGH = 3;
            static const int MAX_VISIBLE_TILES_X = VISIBILITY_OFFSET_LOW + Constants::NUM_TILES_VIEW_X + VISIBILITY_OFFSET_HIGH;
            static const int MAX_VISIBLE_TILES_Y = VISIBILITY_OFFSET_LOW + Constants::NUM_TILES_VIEW_Y + VISIBILITY_OFFSET_HIGH;
            static const int MAX_MOVEMENT = 10;

            std::shared_ptr<std::vector<SpriteDraw>> mCurrentDraws = std::make_shared<std::vector<SpriteDraw>>();
            bool mIsCurrentFrameParsed = false;

            Tile mParsedCurrentFrame[MAX_VISIBLE_TILES_X][MAX_VISIBLE_TILES_Y];
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
