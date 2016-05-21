#ifndef GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL
#define GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL

///////////////////////////////////
// STD C++
#include <cstdlib>
///////////////////////////////////

namespace GraphicsLayer
{
    namespace SharedMemoryProtocol
    {
        const unsigned int TILE_WIDTH = 32;
        const unsigned int TILE_HEIGHT = 32;
        const unsigned int TILE_SIZE = TILE_WIDTH * TILE_HEIGHT * 4;

        struct PixelData
        {
            unsigned short texX;
            unsigned short texY;
            unsigned char targetTextureId;
            unsigned char pixels[TILE_SIZE];
        };

        struct DrawCall
        {
            unsigned char sourceTextureId;
            unsigned char targetTextureId;
            unsigned short screenX;
            unsigned short screenY;
            unsigned short width;
            unsigned short height;
            unsigned short texX;
            unsigned short texY;
            unsigned short texWidth;
            unsigned short texHeight;
        };

        const unsigned int MAX_NUM_PIXEL_DATA = 1000;
        const unsigned int MAX_NUM_DRAW_CALL = 30000;
        const unsigned int MAX_NUM_FRAMES = 1000;

        struct SharedMemorySegment
        {
            bool hasPendingChanges = false;
            unsigned int numPixelData = 0;
            unsigned int numDrawCall = 0;
            unsigned int numFrames = 0;
            PixelData pixelData[MAX_NUM_PIXEL_DATA];
            DrawCall drawCall[MAX_NUM_DRAW_CALL];
            unsigned int numDrawCallsPerFrame[MAX_NUM_FRAMES];
            unsigned int xWindowId;
        };

        const unsigned int NUM_BYTES = sizeof(SharedMemorySegment);

    }
}


#endif // GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL
