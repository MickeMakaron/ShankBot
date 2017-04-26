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
#ifndef GRAPHICS_LAYER_SCENE_DIFF_HPP
#define GRAPHICS_LAYER_SCENE_DIFF_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "monitor/SceneParser.hpp"
#include "monitor/Frame.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
///////////////////////////////////

namespace GraphicsLayer
{
    class SceneDiff
    {
        public:
            struct Data
            {

            };

        public:
            SceneDiff();
            void parse(const Frame& frame, const SceneParser::Data& scene);
            const Data& getData() const;

        private:
            Data mData;
            SceneParser::Data mPrevScene;
            Frame mPrevFrame;
            unsigned int mPrevX = 0;
            unsigned int mPrevY = 0;
            unsigned char mPrevLevel = 0;
    };
}

#endif // GRAPHICS_LAYER_SCENE_DIFF_HPP
