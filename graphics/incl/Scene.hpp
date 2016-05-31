#ifndef GRAPHICS_LAYER_SCENE
#define GRAPHICS_LAYER_SCENE


///////////////////////////////////
// Internal ShankBot headers
#include "FrameParser.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <list>
///////////////////////////////////

namespace GraphicsLayer
{
    class Scene
    {
        public:
            struct Object
            {
                char tileX;
                char tileY;
                char layer;
                bool isOnStack = false;
                const TibiaDat::Object* object;

//                bool operator<(const Object& other) const
//                {
//                    return
//                    (
//                        tileX != other.tileX ||
//                        tileY != other.tileY ||
//                        layer != other.layer ||
//                        object != other.object
//                    );
//                }

                bool operator==(const Object& other) const
                {
                    return
                    (
                        tileX == other.tileX &&
                        tileY == other.tileY &&
                        layer == other.layer &&
                        object == other.object
                    );
                }
            };

            struct Tile
            {
                std::list<Object> knownLayerObjects;
                char tileX;
                char tileY;
                char numLayers;
                char height = 0;
                char stackStartLayer = -1;

                std::list<Object> unknownLayerObjects;
            };


        public:

            void update(const FrameParser::Frame& frame);

            std::list<Tile> getPickupables() const;

        private:
            void handleMovement(const FrameParser::Frame& frame);
            void clearOldTilesX(int numClears);
            void clearOldTilesY(int numClears);
            void move(int movementX, int movementY);
            void updateTex3(const FrameParser::Frame& frame);
            void updateTex1(const FrameParser::Frame& frame);
            Object parseObjectTile(const TibiaDat::Object* object, size_t spriteId, const FrameParser::DrawCallInfo& drawInfo, char tileX, char tileY, char layer) const;

        private:
            static const int NUM_TILES_VIEW_X = FrameParser::Layer::NUM_TILES_VIEW_X;
            static const int NUM_TILES_VIEW_Y = FrameParser::Layer::NUM_TILES_VIEW_Y;
            static const int NUM_TILES_SCENE_X = NUM_TILES_VIEW_X + 1;
            static const int NUM_TILES_SCENE_Y = NUM_TILES_VIEW_Y + 1;
            static const int MAX_VISIBLE_TILES_X = NUM_TILES_VIEW_X + 3;
            static const int MAX_VISIBLE_TILES_Y = NUM_TILES_VIEW_Y + 3;
            static const int MAX_STACK_TILE_HEIGHT = 3;

            std::list<FrameParser::DrawCallInfo> mTex3[NUM_TILES_SCENE_X][NUM_TILES_SCENE_Y];
            std::list<FrameParser::DrawCallInfo> mTex1[NUM_TILES_SCENE_X][NUM_TILES_SCENE_Y];

            int mLastStepX = 0;
            int mLastStepY = 0;
            int mMovementDirectionX = 0;
            int mMovementDirectionY = 0;
    };
}


#endif // GRAPHICS_LAYER_SCENE
