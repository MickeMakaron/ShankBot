#ifndef GRAPHICS_LAYER_TIBIA_CLIENT
#define GRAPHICS_LAYER_TIBIA_CLIENT


///////////////////////////////////
// Internal ShankBot headers
#include "SharedMemoryProtocol.hpp"
#include "TileBuffer.hpp"
#include "ObjectParser.hpp"
#include "TileBufferCache.hpp"
#include "TibiaContext.hpp"
#include "FrameParser.hpp"
///////////////////////////////////

///////////////////////////////////
// Xlib
typedef struct _XDisplay Display;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <list>
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    class TibiaClient
    {
        public:
            explicit TibiaClient(std::string clientDirectory, const TibiaContext& context);

            void update();

            SharedMemoryProtocol::SharedMemorySegment* getSharedMemory() const; // Should not be needed unless for debugging.

        private:
            int prepareSharedMemory() const;
            char** prepareEnvironment(int shmFd) const;
            void deleteEnvironment(char** environment) const;
            void launchClient(char** environment, std::string clientDirectory) const;
            void mapSharedMemory(int shmFd);

            void checkBufferOverflow() const;

        private:
            const TibiaContext& mContext;
            SharedMemoryProtocol::SharedMemorySegment* mShm;
            FrameParser mFrameParser;
            Display* mXDisplay;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_CLIENT
