#ifndef GRAPHICS_LAYER_TILE_BUFFER
#define GRAPHICS_LAYER_TILE_BUFFER

///////////////////////////////////
// Internal ShankBot headers
#include "PipeProtocol.hpp"
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
            TileBuffer();

            void setTile(PipeProtocol::PixelData pixels);
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
