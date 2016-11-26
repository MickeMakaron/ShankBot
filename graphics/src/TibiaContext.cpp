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
#include "TibiaContext.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


TibiaContext::TibiaContext
(
    UPtr<std::vector<Object>>& objects,
    UPtr<SpriteObjectBindings>& spriteObjectBindings,
    UPtr<SequenceTree>& spriteColorTree,
    UPtr<SequenceTree>& spriteTransparencyTree,
    UPtr<SpriteInfo>& spriteInfo,
    UPtr<std::vector<std::string>>& graphicsResourceNames,
    UPtr<std::list<FontSample::Glyph>>& glyphs
)
{
    mObjects.reset(objects.release());
    mSpriteObjectBindings.reset(spriteObjectBindings.release());
    mSpriteColorTree.reset(spriteColorTree.release());
    mSpriteTransparencyTree.reset(spriteTransparencyTree.release());
    mSpriteInfo.reset(spriteInfo.release());
    mGraphicsResourceNames.reset(graphicsResourceNames.release());
    mGlyphs.reset(glyphs.release());
}

const std::vector<Object>& TibiaContext::getObjects() const
{
    return *mObjects;
}

const SpriteObjectBindings& TibiaContext::getSpriteObjectBindings() const
{
    return *mSpriteObjectBindings;
}

const SequenceTree& TibiaContext::getSpriteColorTree() const
{
    return *mSpriteColorTree;
}

const SequenceTree& TibiaContext::getSpriteTransparencyTree() const
{
    return *mSpriteTransparencyTree;
}

const SpriteInfo& TibiaContext::getSpriteInfo() const
{
    return *mSpriteInfo;
}

const std::vector<std::string>& TibiaContext::getGraphicsResourceNames() const
{
    return *mGraphicsResourceNames;
}

const std::list<FontSample::Glyph>& TibiaContext::getGlyphs() const
{
    return *mGlyphs;
}

