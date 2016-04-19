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


        enum class Type : unsigned char
        {
            PIXEL_DATA,
            DRAW_CALL
        };

        struct Header
        {
            unsigned int packetCount;
            Type type;
        };

        struct PixelData
        {
            unsigned short texX;
            unsigned short texY;
            unsigned char textureId;
            unsigned char pixels[32 * 32 * 4];

        };

        struct DrawCall
        {
            unsigned char textureId;
            unsigned short screenX;
            unsigned short screenY;
            unsigned short texX;
            unsigned short texY;
            unsigned short width;
            unsigned short height;
        };

        const size_t MAX_NUM_PIXEL_DATA = 500;
        const size_t MAX_NUM_DRAW_CALL = 3000;

        struct SharedMemorySegment
        {
            bool hasPendingChanges = false;
            unsigned int numPixelData = 0;
            unsigned int numDrawCall = 0;
            PixelData pixelData[MAX_NUM_PIXEL_DATA];
            DrawCall drawCall[MAX_NUM_DRAW_CALL];
        };

        const size_t NUM_BYTES = sizeof(SharedMemorySegment);

    }
}


#endif // GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL
