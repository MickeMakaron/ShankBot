#ifndef GRAPHICS_LAYER_TIBIA_CLIENT
#define GRAPHICS_LAYER_TIBIA_CLIENT


///////////////////////////////////
// Internal ShankBot headers
#include "SharedMemoryProtocol.hpp"
#include "TileBuffer.hpp"
#include "ObjectParser.hpp"
#include "ObjectCache.hpp"
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
            explicit TibiaClient(std::string clientDirectory);

            void update();

            std::list<std::list<size_t>> getDrawnSprites() const;

            SharedMemoryProtocol::SharedMemorySegment* getSharedMemory() const; // Should not be needed unless for debugging.

        private:
            int prepareSharedMemory() const;
            char** prepareEnvironment(int shmFd) const;
            void deleteEnvironment(char** environment) const;
            void launchClient(char** environment, std::string clientDirectory) const;
            void mapSharedMemory(int shmFd);

            void checkBufferOverflow() const;

            void updateTileBuffers();
            void handleDrawCalls();

        private:
            SharedMemoryProtocol::SharedMemorySegment* mShm;
            std::map<unsigned char, TileBuffer> mTileBuffers;
            std::list<std::list<size_t>> mDrawnSprites;
            ObjectParser mObjectParser;
            ObjectCache mObjectCache;
            ObjectCache mGuiObjectCache;
            size_t mGameViewWidth;
            size_t mGameViewHeight;
            Display* mXDisplay;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_CLIENT
