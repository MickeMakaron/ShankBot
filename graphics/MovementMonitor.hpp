// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
#ifndef GRAPHICS_LAYER_MOVEMENT_MONITOR_HPP
#define GRAPHICS_LAYER_MOVEMENT_MONITOR_HPP

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
            explicit MovementMonitor(int middleX, int middleY, unsigned int stepSize);

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


#endif // GRAPHICS_LAYER_MOVEMENT_MONITOR_HPP
