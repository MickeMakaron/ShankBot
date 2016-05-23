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

            void update(const FrameParser::Frame& frame);

        private:
            void handleMovement(const FrameParser::Frame& frame);

        private:
            static const int NUM_TILES_VIEW_X = FrameParser::Layer::NUM_TILES_VIEW_X;
            static const int NUM_TILES_VIEW_Y = FrameParser::Layer::NUM_TILES_VIEW_Y;
            static const int NUM_TILES_SCENE_X = NUM_TILES_VIEW_X + 1;
            static const int NUM_TILES_SCENE_Y = NUM_TILES_VIEW_Y + 1;
            std::list<std::list<FrameParser::SpriteObjectPairing>> mScene[NUM_TILES_SCENE_X][NUM_TILES_SCENE_Y];

            int mLastStepX = 0;
            int mLastStepY = 0;
    };
}


#endif // GRAPHICS_LAYER_SCENE
