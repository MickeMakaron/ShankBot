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
#ifndef GRAPHICS_LAYER_GRAPHICS_MONITOR_READER_HPP
#define GRAPHICS_LAYER_GRAPHICS_MONITOR_READER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "FrameParser.hpp"
namespace GraphicsLayer
{
    class TibiaClient;
    class TibiaContext;
}
///////////////////////////////////



namespace GraphicsLayer
{
    class GraphicsMonitorReader
    {
        public:
            explicit GraphicsMonitorReader(const TibiaClient& client, const TibiaContext& context, const SharedMemoryProtocol::SharedMemorySegment* shm);

            Frame getNewFrame();
            const TibiaClient& getClient() const;

        private:
            void waitForFrame() const;
            void postFrameRequest() const;

        private:
            const TibiaClient& mClient;
            const SharedMemoryProtocol::SharedMemorySegment* const mShm;
            FrameParser mFrameParser;
            Frame mLatestFrame;
    };
}

#endif // GRAPHICS_LAYER_GRAPHICS_MONITOR_READER_HPP
