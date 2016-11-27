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

#include "graphics/ShankBot.hpp"
using namespace GraphicsLayer;
#include <fstream>

unsigned int fnv1a32(const unsigned char* data, size_t size)
{
    static const unsigned int FNV_PRIME_32 = 16777619;
    static const unsigned int OFFSET_BASIS_32 = 2166136261;
    unsigned int hash = OFFSET_BASIS_32;
    for(size_t i = 0; i < size; i++)
    {
        hash ^= data[i];
        hash *= FNV_PRIME_32;
    }

    return hash;
}

#include "QtGui/QImage"
void convertTileBufferDump(size_t number)
{
    using namespace sb::utility;
    std::string readPath = stringify("tibia/packages/Tibia/bin/tileBuffers/", number, ".ppm");
    std::string writePath = stringify("tibia/packages/Tibia/bin/tileBuffers/", number, ".png");
    std::ifstream tileBuffer(readPath, std::ios::binary);
    unsigned char* rgba = new unsigned char[2048 * 2048 * 4];
    tileBuffer.read((char*)rgba, 2048 * 2048 * 4);
    tileBuffer.close();
    QImage img(rgba, 2048, 2048, QImage::Format_RGBA8888);
    img.save(QString::fromStdString(writePath));
    delete[] rgba;
    return;
}

int main(int argc, char** argv)
{
    GraphicsLayer::ShankBot sb("tibia", "version-control");
    sb.run();

    return 0;
}


