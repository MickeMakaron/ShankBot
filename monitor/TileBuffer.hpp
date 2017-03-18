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
#ifndef GRAPHICS_LAYER_TILE_BUFFER
#define GRAPHICS_LAYER_TILE_BUFFER

///////////////////////////////////
// Internal ShankBot headers
#include "injection/SharedMemoryProtocol.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstdlib>
///////////////////////////////////

namespace GraphicsLayer
{
    class TileBuffer
    {
        public:
            static const size_t WIDTH = 512;
            static const size_t HEIGHT = 512;
            static const size_t BYTES_PER_PIXEL = 4;
            static const size_t NUM_BYTES = WIDTH * HEIGHT * BYTES_PER_PIXEL;

        public:
            explicit TileBuffer();

            void setTile(SharedMemoryProtocol::PixelData pixels);
            unsigned char* getTile(size_t x, size_t y) const;
            unsigned char* getPixels(size_t x, size_t y, size_t width, size_t height) const;

            const unsigned char* getPixels() const;


            static size_t coordsToIndex(size_t x, size_t y);

        private:


        private:
            unsigned char mPixels[NUM_BYTES];
    };
}


#endif // GRAPHICS_LAYER_TILE_BUFFER
