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
#ifndef GRAPHICS_LAYER_TIBIA_CONTEXT_HPP
#define GRAPHICS_LAYER_TIBIA_CONTEXT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/Object.hpp"
#include "SequenceTree.hpp"
#include "SpriteInfo.hpp"
#include "SpriteObjectBindings.hpp"
#include "FontSample.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
   class TibiaContext
    {
        private:
            template<typename T>
            using UPtr = std::unique_ptr<T>;

        public:
            explicit TibiaContext
            (
                UPtr<std::vector<sb::tibiaassets::Object>>& objects,
                UPtr<SpriteObjectBindings>& spriteObjectBindings,
                UPtr<SequenceTree>& spriteColorTree,
                UPtr<SequenceTree>& spriteTransparencyTree,
                UPtr<SpriteInfo>& spriteInfo,
                UPtr<std::vector<std::string>>& graphicsResourceNames,
                UPtr<std::list<FontSample::Glyph>>& glyphs
            );

            const std::vector<sb::tibiaassets::Object>& getObjects() const;
            const SpriteObjectBindings& getSpriteObjectBindings() const;
            const SequenceTree& getSpriteColorTree() const;
            const SequenceTree& getSpriteTransparencyTree() const;
            const SpriteInfo& getSpriteInfo() const;
            const std::vector<std::string>& getGraphicsResourceNames() const;
            const std::list<FontSample::Glyph>& getGlyphs() const;

        private:
            UPtr<std::vector<sb::tibiaassets::Object>> mObjects;
            UPtr<SpriteObjectBindings> mSpriteObjectBindings;
            UPtr<SequenceTree> mSpriteColorTree;
            UPtr<SequenceTree> mSpriteTransparencyTree;
            UPtr<SpriteInfo> mSpriteInfo;
            UPtr<std::vector<std::string>> mGraphicsResourceNames;
            UPtr<std::list<FontSample::Glyph>> mGlyphs;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_CONTEXT_HPP
