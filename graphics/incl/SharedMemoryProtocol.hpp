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
        struct PixelData
        {
            unsigned short texX;
            unsigned short texY;
            unsigned char targetTextureId;
            unsigned char pixels[32 * 32 * 4];
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

        const size_t MAX_NUM_PIXEL_DATA = 1000;
        const size_t MAX_NUM_DRAW_CALL = 30000;

        struct SharedMemorySegment
        {
            bool hasPendingChanges = false;
            unsigned int numPixelData = 0;
            unsigned int numDrawCall = 0;
            PixelData pixelData[MAX_NUM_PIXEL_DATA];
            DrawCall drawCall[MAX_NUM_DRAW_CALL];
            unsigned int xWindowId;
        };

        const size_t NUM_BYTES = sizeof(SharedMemorySegment);

    }
}


#endif // GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL
