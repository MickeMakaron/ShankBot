#ifndef GRAPHICS_LAYER_FRAME_PARSER
#define GRAPHICS_LAYER_FRAME_PARSER

///////////////////////////////////
// Internal ShankBot headers
#include "TileBufferCache.hpp"
#include "SharedMemoryProtocol.hpp"
#include "TibiaContext.hpp"
#include "MovementMonitor.hpp"
///////////////////////////////////



namespace GraphicsLayer
{
    class FrameParser
    {
        public:
            struct SpriteObjectPairing
            {
                size_t spriteId;
                std::set<const TibiaDat::Object*> objects;
            };

            struct Layer
            {
                static const int NUM_TILES_VIEW_X = 15;
                static const int NUM_TILES_VIEW_Y = 11;
                std::list<std::list<SpriteObjectPairing>> view[NUM_TILES_VIEW_X][NUM_TILES_VIEW_Y];

                std::list<std::list<SpriteObjectPairing>> previousX[NUM_TILES_VIEW_Y];
                std::list<std::list<SpriteObjectPairing>> previousY[NUM_TILES_VIEW_X];
            };

            struct Frame
            {
                Layer tex3;
                Layer tex1;

                int numStepsX = 0;
                int numStepsY = 0;
                bool isMoving = false;
            };

        public:
            explicit FrameParser(const TibiaContext& context);

            void updateTileBuffers(const SharedMemoryProtocol::PixelData* texxes, size_t numTexxes);
            Frame parse(const SharedMemoryProtocol::DrawCall* drawCalls, size_t numDrawCalls);

        private:
            void handleMovement(const std::list<const SharedMemoryProtocol::DrawCall*>& tex3ToTex1DrawCalls, Frame& frame);
            std::list<SpriteObjectPairing> getPairings(const SharedMemoryProtocol::DrawCall& drawCall) const;
            void parseTex3(const std::list<const SharedMemoryProtocol::DrawCall*>& drawCalls, Layer& layer) const;
            void parseTex1(const std::list<const SharedMemoryProtocol::DrawCall*>& drawCalls, Layer& layer) const;
            void getTileAdjustmentData(int& snapDistanceX, int& snapDistanceY, int& edgeAdjustX, int& edgeAdjustY) const;
            void getAdjustedScreenTileCoordinates(int snapDistanceX, int snapDistanceY, int edgeAdjustX, int edgeAdjustY, const SharedMemoryProtocol::DrawCall& d, int& x, int& y) const;


        private:
            static const int TILE_SIZE = 32;
            static const int NUM_TILES_X = 16;
            static const int NUM_TILES_Y = 16;
            static const int NUM_TILES_VIEW_X = 15;
            static const int NUM_TILES_VIEW_Y = 11;
            static const int MIDDLE_SCREEN_X = (NUM_TILES_VIEW_X / 2) * TILE_SIZE;
            static const int MIDDLE_SCREEN_Y = (NUM_TILES_VIEW_Y / 2) * TILE_SIZE;

            std::pair<short, short> mTex3TileIndexToTex1TileIndexMap[NUM_TILES_X][NUM_TILES_Y];
            const TibiaContext& mContext;
            TileBufferCache<std::list<SpriteObjectPairing>> mSpriteCache;
            TileBufferCache<size_t> mGuiCache;
            int mCenterTexIndex = -1;
            MovementMonitor mMovementMonitor;
    };

}


#endif // GRAPHICS_LAYER_FRAME_PARSER
