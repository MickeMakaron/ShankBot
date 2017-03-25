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
#include "monitor/GlyphsFile.hpp"
#include "utility/utility.hpp"
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
#include <cassert>
///////////////////////////////////

namespace GraphicsLayer
{
namespace GlyphsFile
{

bool write(const std::vector<FontSample::Glyph>& glyphs, const std::string& filePath)
{
    std::ofstream file(filePath, std::ios::binary);
    if(!file.good())
    {
        return false;
    }


    std::vector<char> buffer;

    unsigned short numGlyphs = glyphs.size();
    writeStream(numGlyphs, buffer);
    for(const FontSample::Glyph& g : glyphs)
    {
        writeStream(g.width, buffer);
        writeStream(g.height, buffer);
        writeStream(g.character, buffer);
        writeStream(g.style, buffer);
        writeStream(g.pointSize, buffer);
        writeStream(g.sum, buffer);
        assert(g.width * g.height * g.bytesPerPixel == g.data.size());
        writeStream(*g.data.data(), buffer, g.data.size());
    }


    writeStream(*buffer.data(), file, buffer.size());

    return !file.fail();
}

bool read(std::vector<FontSample::Glyph>& glyphs, const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if(!file.good())
    {
        return false;
    }


    file.seekg(0, file.end);
    size_t fileSize = file.tellg();
    file.seekg(0);

    std::vector<char> buffer(fileSize);
    readStream(*buffer.data(), file, buffer.size());
    file.close();

    const char* stream = buffer.data();
    unsigned short numGlyphs;
    readStream(numGlyphs, stream);
    glyphs.resize(numGlyphs);
    for(FontSample::Glyph& g : glyphs)
    {
        readStream(g.width, stream);
        readStream(g.height, stream);
        readStream(g.character, stream);
        readStream(g.style, stream);
        readStream(g.pointSize, stream);
        readStream(g.sum, stream);
        g.data.resize(g.width * g.height * g.bytesPerPixel);
        readStream(*g.data.data(), stream, g.data.size());
    }

    return true;
}

}
}
