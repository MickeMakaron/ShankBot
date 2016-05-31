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

            struct DrawCallInfo
            {
                enum class Region : char
                {
                    FULL,
                    TOP_LEFT,
                    TOP_RIGHT,
                    BOTTOM_LEFT,
                    BOTTOM_RIGHT
                };

                std::list<SpriteObjectPairing> pairings;
                short x;
                short y;
                short width;
                short height;
                short texX;
                short texY;
                Region region;


                // DEBUG
                SharedMemoryProtocol::DrawCall drawCall;

            };

            struct Layer
            {
                static const int NUM_TILES_VIEW_X = 15;
                static const int NUM_TILES_VIEW_Y = 11;
                std::list<DrawCallInfo> view[NUM_TILES_VIEW_X][NUM_TILES_VIEW_Y];

                std::list<DrawCallInfo> previousX[NUM_TILES_VIEW_Y];
                std::list<DrawCallInfo> previousY[NUM_TILES_VIEW_X];
            };

            struct Frame
            {
                Layer tex3;
                Layer tex1;

                int numStepsX = 0; // Number of X steps INITIATED since last update.
                int numStepsY = 0; // Number of y steps INITIATED since last update.
                int movementDirectionX = 0; // X direction of current ONGOING movement.
                int movementDirectionY = 0; // Y direction of current ONGOING movement.
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
            void getAdjustedScreenTileCoordinates(int snapDistanceX, int snapDistanceY, int edgeAdjustX, int edgeAdjustY, const SharedMemoryProtocol::DrawCall& d, int& tileX, int& tileY, int* x = nullptr, int* y = nullptr) const;
            DrawCallInfo getDrawCallInfo(const SharedMemoryProtocol::DrawCall& d) const;

        public:
            static const int TILE_SIZE = 32;

        private:
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
