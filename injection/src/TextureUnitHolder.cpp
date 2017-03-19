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
#include "injection/TextureUnitHolder.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
///////////////////////////////////

namespace GraphicsLayer
{
    TextureUnitHolder::TextureUnitHolder()
    : mTextureUnits()
    , mActiveTextureUnit(nullptr)
    {
        TextureUnit unit0;
        unit0.id = GL_TEXTURE0;
        mTextureUnits.push_back(unit0);
        mActiveTextureUnit = &mTextureUnits[0];
    }

    void TextureUnitHolder::setTexture(GLenum target, GLuint texture)
    {
        mActiveTextureUnit->targets[target] = texture;
    }

    void TextureUnitHolder::setActiveTextureUnit(GLenum unit)
    {
        if(unit < GL_TEXTURE0)
            throw std::runtime_error("Invalid argument in TextureUnitHolder::setActiveTextureUnit");

        size_t index = unit - GL_TEXTURE0;
        if(index >= mTextureUnits.size())
        {
            size_t oldSize = mTextureUnits.size();
            mTextureUnits.resize(index + 1);
            for(size_t i = oldSize; i < mTextureUnits.size(); i++)
                mTextureUnits[i].id = i + GL_TEXTURE0;
        }
        mActiveTextureUnit = &mTextureUnits[index];
    }

    GLuint TextureUnitHolder::getTexture(GLenum target) const
    {
        return mActiveTextureUnit->targets[target];
    }

    const std::vector<TextureUnit>& TextureUnitHolder::getTextureUnits() const
    {
        return mTextureUnits;
    }
}

