#ifndef GRAPHICS_LAYER_FRAME_PARSER
#define GRAPHICS_LAYER_FRAME_PARSER

///////////////////////////////////
// Internal ShankBot headers
#include "TileBufferCache.hpp"
#include "ObjectParser.hpp"
#include "SharedMemoryProtocol.hpp"
#include "TibiaContext.hpp"
#include "MovementMonitor.hpp"
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
            static const size_t TILE_SIZE = 32;
            static const size_t NUM_TILES_X = 16;
            static const size_t NUM_TILES_Y = 16;
            static const size_t NUM_TILES = NUM_TILES_X * NUM_TILES_Y;
            static const size_t NUM_TILES_VIEW_X = 15;
            static const size_t NUM_TILES_VIEW_Y = 11;
            static const size_t NUM_TILES_SCENE_X = NUM_TILES_VIEW_X + 1;
            static const size_t NUM_TILES_SCENE_Y = NUM_TILES_VIEW_Y + 1;
            static const size_t MIDDLE_SCREEN_INDEX = NUM_TILES_VIEW_X / 2 + (NUM_TILES_VIEW_Y / 2) * NUM_TILES_X;
            static const size_t MIDDLE_SCREEN_X = (NUM_TILES_VIEW_X / 2) * TILE_SIZE;
            static const size_t MIDDLE_SCREEN_Y = (NUM_TILES_VIEW_Y / 2) * TILE_SIZE;

            size_t mTex3TileIndexToTex1TileIndexMap[NUM_TILES];
            const TibiaContext& mContext;
            TileBufferCache<std::list<SpriteObjectPairing>> mSpriteCache;
            ObjectParser mObjectParser;
            TileBufferCache<size_t> mGuiCache;
            int mCenterTexIndex = -1;
            std::list<std::list<SpriteObjectPairing>> mPairingsMap[NUM_TILES_X][NUM_TILES_Y];
            std::list<std::set<const TibiaDat::Object*>> mCurrentScene[NUM_TILES_SCENE_X][NUM_TILES_SCENE_Y];

            MovementMonitor mMovementMonitor;
    };

}


#endif // GRAPHICS_LAYER_FRAME_PARSER
