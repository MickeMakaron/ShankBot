#ifndef GRAPHICS_LAYER_TIBIA_CLIENT
#define GRAPHICS_LAYER_TIBIA_CLIENT


///////////////////////////////////
// Internal ShankBot headers
#include "SharedMemoryProtocol.hpp"
#include "TileBuffer.hpp"
namespace GraphicsLayer
{
    class ImageTree;
}
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
            TibiaClient(std::string clientDirectory, const ImageTree& imageTree);

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
            const ImageTree& mImageTree;
            std::list<std::list<size_t>> mDrawnSprites;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_CLIENT
