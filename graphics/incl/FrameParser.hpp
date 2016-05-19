#ifndef GRAPHICS_LAYER_FRAME_PARSER
#define GRAPHICS_LAYER_FRAME_PARSER

///////////////////////////////////
// Internal ShankBot headers
#include "TileBufferCache.hpp"
#include "ObjectParser.hpp"
#include "SharedMemoryProtocol.hpp"
#include "TibiaContext.hpp"
///////////////////////////////////



namespace GraphicsLayer
{
    class FrameParser
    {
        private:
            struct SpriteObjectPairing
            {
                size_t spriteId;
                std::set<const TibiaDat::Object*> objects;
            };

        public:
            explicit FrameParser(const TibiaContext& context);

            void updateTileBuffers(const SharedMemoryProtocol::PixelData* texxes, size_t numTexxes);
            void parse(const SharedMemoryProtocol::DrawCall* drawCalls, size_t numDrawCalls);

        private:
            static const size_t NUM_TILES_X = 16;
            static const size_t NUM_TILES_Y = 16;
            static const size_t NUM_TILES = NUM_TILES_X * NUM_TILES_Y;
            size_t mTex3TileIndexToTex1TileIndexMap[NUM_TILES];
            const TibiaContext& mContext;
            TileBufferCache<std::list<SpriteObjectPairing>> mSpriteCache;
            ObjectParser mObjectParser;
            TileBufferCache<size_t> mGuiCache;
    };

}


#endif // GRAPHICS_LAYER_FRAME_PARSER
