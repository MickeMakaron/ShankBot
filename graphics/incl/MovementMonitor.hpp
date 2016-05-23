#ifndef GRAPHICS_LAYER_MOVEMENT_MONITOR
#define GRAPHICS_LAYER_MOVEMENT_MONITOR

///////////////////////////////////
// Internal ShankBot headers
#include "TileBufferCache.hpp"
#include "ObjectParser.hpp"
#include "SharedMemoryProtocol.hpp"
#include "TibiaContext.hpp"
///////////////////////////////////



namespace GraphicsLayer
{
    class MovementMonitor
    {
        public:
            MovementMonitor(int middleX, int middleY, unsigned int stepSize);

            void update(int x, int y);
            void reset();

            int getNumStepsX() const;
            int getNumStepsY() const;

            int getLastStepX() const;
            int getLastStepY() const;

            int getDeltaX() const;
            int getDeltaY() const;

        private:
            void updateAxis(int coord, int& lastCoord, int& numSteps, int& delta) const;

        private:
            const int M_MIDDLE_X;
            const int M_MIDDLE_Y;
            int mLastPosX;
            int mLastPosY;
            const int M_STEP_SIZE;

            int mDeltaX = 0;
            int mDeltaY = 0;

            int mNumStepsX = 0;
            int mNumStepsY = 0;

            int mLastStepX = 0;
            int mLastStepY = 0;
    };
}


#endif // GRAPHICS_LAYER_MOVEMENT_MONITOR
