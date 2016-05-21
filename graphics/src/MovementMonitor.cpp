///////////////////////////////////
// Internal ShankBot headers
#include "MovementMonitor.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


MovementMonitor::MovementMonitor(int middleX, int middleY, unsigned int stepSize)
: M_MIDDLE_X(middleX)
, M_MIDDLE_Y(middleY)
, mLastPosX(middleX)
, mLastPosY(middleY)
, M_STEP_SIZE(stepSize)
{

}

void MovementMonitor::updateAxis(int coord, int& lastCoord, int& numSteps, int& delta) const
{
    numSteps = 0;

    if(coord == 0)
    {
        lastCoord = 0;
        return;
    }

    int currentDelta = coord - lastCoord;
    lastCoord = coord;


    if(delta == 0)
    {
        if(currentDelta < 0)
            numSteps -= 1;
        else if(currentDelta > 0)
            numSteps += 1;
    }
    else
    {
        int newDelta = delta + currentDelta;
        if(delta < 0)
        {
            if(newDelta > 0)
                numSteps += 2;
            else if(newDelta == 0)
                numSteps += 1;
        }
        else
        {
            if(newDelta < 0)
                numSteps -= 2;
            else if(newDelta == 0)
                numSteps -= 1;
        }

    }

    delta += currentDelta;
    if(delta <= -M_STEP_SIZE || delta >= M_STEP_SIZE)
    {
        numSteps += delta / M_STEP_SIZE;

        int remainder = delta % M_STEP_SIZE;
        if(remainder == 0)
        {
            if(delta < 0)
                numSteps += 1;
            else
                numSteps -= 1;
        }

        delta = remainder;
    }

}

void MovementMonitor::update(int x, int y)
{
    if(mNumStepsX != 0)
        mLastStepX = mNumStepsX;

    if(mNumStepsY != 0)
        mLastStepY = mNumStepsY;

    updateAxis(x, mLastPosX, mNumStepsX, mDeltaX);
    updateAxis(y, mLastPosY, mNumStepsY, mDeltaY);
}

void MovementMonitor::reset()
{
    mLastPosX = M_MIDDLE_X;
    mLastPosY = M_MIDDLE_Y;
    mDeltaX = 0;
    mDeltaY = 0;
    mNumStepsX = 0;
    mNumStepsY = 0;
}

int MovementMonitor::getNumStepsX() const
{
    return mNumStepsX;
}

int MovementMonitor::getNumStepsY() const
{
    return mNumStepsY;
}

int MovementMonitor::getLastStepX() const
{
    return mLastStepX;
}

int MovementMonitor::getLastStepY() const
{
    return mLastStepY;
}
