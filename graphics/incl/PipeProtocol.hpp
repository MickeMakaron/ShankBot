#ifndef GRAPHICS_LAYER_PIPE_PROTOCOL
#define GRAPHICS_LAYER_PIPE_PROTOCOL

namespace GraphicsLayer
{
    namespace PipeProtocol
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
    }
}


#endif // GRAPHICS_LAYER_PIPE_PROTOCOL
