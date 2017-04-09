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
#ifndef GRAPHICS_LAYER_SCENE_PARSER_HPP
#define GRAPHICS_LAYER_SCENE_PARSER_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Constants.hpp"
#include "monitor/Draw.hpp"
#include "utility/Rect.hpp"
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
    class SceneParser
    {
        public:

            struct Object
            {
                std::vector<const sb::tibiaassets::Object*> disputes;
            };

            struct NonGridObject : public Object
            {
                IRect screenBounds;
            };

            struct Tile
            {
                IRect screenBounds;
                std::vector<Object> objects;
            };

            static const int VISIBILITY_OFFSET_LOW = 2;
            static const int VISIBILITY_OFFSET_HIGH = 3;
            static const int MAX_VISIBLE_TILES_X = VISIBILITY_OFFSET_LOW + Constants::NUM_TILES_VIEW_X + VISIBILITY_OFFSET_HIGH;
            static const int MAX_VISIBLE_TILES_Y = VISIBILITY_OFFSET_LOW + Constants::NUM_TILES_VIEW_Y + VISIBILITY_OFFSET_HIGH;
            struct Data
            {
                std::array<std::array<Tile, MAX_VISIBLE_TILES_Y>, MAX_VISIBLE_TILES_X> gridObjects;
                std::vector<NonGridObject> characters;
                std::vector<NonGridObject> projectiles;
                short offsetX;
                short offsetY;
                IRect screenBounds;
            };



        public:
            explicit SceneParser(const TibiaContext& context);

            void parse(const Frame& frame);

            float getTileWidth() const;
            float getTileHeight() const;

            const Data& getData() const;


        private:
            const SpriteDraw* findFirstGroundTile() const;


        private:
            const TibiaContext& mContext;
            std::shared_ptr<std::vector<SpriteDraw>> mCurrentDraws = std::make_shared<std::vector<SpriteDraw>>();
            std::shared_ptr<Data> mData = std::make_shared<Data>();
    };
}

#endif // GRAPHICS_LAYER_SCENE_PARSER_HPP
