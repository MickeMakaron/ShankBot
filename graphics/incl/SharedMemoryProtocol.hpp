#ifndef GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL_HPP
#define GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL_HPP

///////////////////////////////////
// Internal ShankBot headers
namespace GraphicsLayer
{
    struct Vertex;
}
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
            enum class PixelFormat : unsigned char
            {
                RGBA,
                BGRA,
                ALPHA
            };

            unsigned short texX;
            unsigned short texY;
            unsigned short width;
            unsigned short height;
            unsigned char targetTextureId;
            PixelFormat format;
        };

        struct TextureData
        {
            unsigned short width;
            unsigned short height;
            unsigned char id;
        };

        struct VertexBufferWrite
        {
            unsigned short bufferId = 0;
            unsigned int numBytes = 0;
        };

        struct VertexBufferInfo
        {
            enum class VertexType : unsigned char
            {
                TEXTURED,
                COLORED,
            };

            typedef float Unknown;
            typedef unsigned short Index;


            unsigned int numVertices = 0;
            unsigned int numUnknown = 0;
            unsigned int numIndices = 0;
            static const unsigned int VERTICES_OFFSET = 0;
            unsigned int unknownOffset = 0;
            unsigned int indicesOffset = 0;

            unsigned short bufferId = 0;

            VertexType vertexType = VertexType::TEXTURED;
        };

        struct DrawCall
        {
            enum class PrimitiveType : unsigned char
            {
                TRIANGLE,
                TRIANGLE_STRIP
            };

            unsigned short bufferId = 0;
            unsigned char sourceTextureId = 0;
            unsigned char targetTextureId = 0;
            PrimitiveType type = PrimitiveType::TRIANGLE;
        };

        struct Frame
        {
            unsigned short numPixelData = 0;
            unsigned short numVertexBufferWrites = 0;
            unsigned short numVertexBufferInfos = 0;
            unsigned short numDrawCalls = 0;
            unsigned int numPixelBytes = 0;
            unsigned int numVertexBytes = 0;
            unsigned short numTextureData = 0;
        };

        const unsigned int MAX_NUM_PIXEL_DATA = 1000 * 3;
        const unsigned int MAX_NUM_VERTEX_BUFFERS = 1000;
        const unsigned int MAX_NUM_VERTEX_BUFFER_INFOS = 1000 * 2;
        const unsigned int MAX_NUM_DRAW_CALL = 1000 * 2;
        const unsigned int MAX_NUM_FRAMES = 1000;
        const unsigned int MAX_NUM_TEXTURE_DATA = 50;
        const unsigned int VERTEX_BUFFER_SIZE = 1000 * 1000 * 3;
        const unsigned int PIXEL_BUFFER_SIZE = 1000 * 1000 * 5;


        struct SharedMemorySegment
        {
            bool hasPendingChanges = false;

            PixelData pixelData[MAX_NUM_PIXEL_DATA];
            unsigned char pixelBuffer[PIXEL_BUFFER_SIZE];

            VertexBufferWrite vertexBufferWrites[MAX_NUM_VERTEX_BUFFERS];
            char vertexBuffer[VERTEX_BUFFER_SIZE];

            VertexBufferInfo vertexBufferInfos[MAX_NUM_VERTEX_BUFFER_INFOS];

            DrawCall drawCalls[MAX_NUM_DRAW_CALL];

            TextureData textureData[MAX_NUM_TEXTURE_DATA];

            Frame frames[MAX_NUM_FRAMES];
            unsigned int numFrames = 0;

        };

        const unsigned int NUM_BYTES = sizeof(SharedMemorySegment);
    }
}



#endif // GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL_HPP
