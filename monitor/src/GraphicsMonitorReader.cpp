// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/GraphicsMonitorReader.hpp"
#include "monitor/TibiaClient.hpp"

using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////

GraphicsMonitorReader::GraphicsMonitorReader(const TibiaClient& client, const TibiaContext& context, const SharedMemoryProtocol::SharedMemorySegment* shm)
: mClient(client)
, mShm(shm)
, mFrameParser(context)
{


}


void GraphicsMonitorReader::waitForFrame() const
{
    switch(WaitForSingleObject(mShm->semRead, 500))
    {
        case WAIT_ABANDONED:
            SB_THROW("Wait abandoned.");

        case WAIT_OBJECT_0:
            return;

        case WAIT_TIMEOUT:
            if(!mClient.isAlive())
                SB_THROW("Tibia client unexpectedly terminated.");

            mClient.getInput().sendPaintMessage();
            waitForFrame();
            return;

        case WAIT_FAILED:
        {
            SB_THROW("Failed wait. Error code: ", GetLastError(), "\n");
        }

        default:
            SB_THROW("Unimplemented wait signal.");
    }
}

void GraphicsMonitorReader::postFrameRequest() const
{
    if(ReleaseSemaphore(mShm->semWrite, 1, NULL) == 0)
    {
        SB_THROW("Could not release shared memory write semaphore. Error code: ", GetLastError(), "\n");
    }
}

Frame GraphicsMonitorReader::getNewFrame()
{
    postFrameRequest();
    waitForFrame();
    std::list<Frame> frames = mFrameParser.parse(mShm);
    assert(frames.size() == 1);
    mLatestFrame = frames.front();
    return frames.front();
}

const TibiaClient& GraphicsMonitorReader::getClient() const
{
    return mClient;
}

