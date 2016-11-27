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
#ifndef GRAPHICS_LAYER_SCENE_OBJECT_IDENTIFIER_HPP
#define GRAPHICS_LAYER_SCENE_OBJECT_IDENTIFIER_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Scene.hpp"
namespace GraphicsLayer
{
    class OutfitResolver;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
///////////////////////////////////

namespace GraphicsLayer
{
    class SceneObjectIdentifier
    {

        public:
            void update(const Scene& scene, const OutfitResolver& outfitResolver);
            const Scene::Object* get(unsigned short id) const;


        private:
            static bool compareOffsetScene(size_t* lhs, size_t lhsWidth, size_t lhsHeight, size_t* rhs);
            void detectSceneMovement(const Scene& scene);


        private:
            size_t mImmovableIds[Constants::NUM_TILES_VIEW_X][Constants::NUM_TILES_VIEW_Y] = {{0}};
            std::list<unsigned short> mFreeIds;
            std::vector<std::unique_ptr<Scene::Object>> mObjects;
    };
}

#endif // GRAPHICS_LAYER_SCENE_OBJECT_IDENTIFIER_HPP
