///////////////////////////////////
// Internal ShankBot headers
#include "TibiaClient.hpp"
#include "PngImage.hpp"
#include "ImageTree.hpp"
using namespace GraphicsLayer;
using namespace SharedMemoryProtocol;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
#include <vector>
#include <iostream>
#include <cstring>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
///////////////////////////////////


TibiaClient::TibiaClient(std::string clientDirectory, const ImageTree& imageTree)
: mImageTree(imageTree)
{
    int shmFd = prepareSharedMemory();
    char** tibiaEnv = prepareEnvironment(shmFd);

    launchClient(tibiaEnv, clientDirectory);
    deleteEnvironment(tibiaEnv);

    mapSharedMemory(shmFd);
}

void TibiaClient::checkBufferOverflow() const
{
    if(mShm->numPixelData > MAX_NUM_PIXEL_DATA)
    {
        std::stringstream sstream;
        sstream << "PIXEL DATA BUFFER OVERFLOW. (" << mShm->numPixelData << " of max " << MAX_NUM_PIXEL_DATA << ")";
        throw std::runtime_error(sstream.str());
    }

    if(mShm->numDrawCall > MAX_NUM_DRAW_CALL)
    {
        std::stringstream sstream;
        sstream << "DRAW CALL BUFFER OVERFLOW. (" << mShm->numDrawCall << " of max " << MAX_NUM_DRAW_CALL << ")";
        throw std::runtime_error(sstream.str());
    }
}

void TibiaClient::updateTileBuffers()
{
    for(size_t i = 0; i < mShm->numPixelData; i++)
    {
        const PixelData& data = mShm->pixelData[i];

        auto it = mTileBuffers.find(data.targetTextureId);
        if(it == mTileBuffers.end())
            it = mTileBuffers.insert(std::make_pair(data.targetTextureId, TileBuffer())).first;

        it->second.setTile(data);
    }
}

void TibiaClient::handleDrawCalls()
{
    for(size_t i = 0; i < mShm->numDrawCall; i++)
    {
        const DrawCall& d = mShm->drawCall[i];

        auto tileBufferIt = mTileBuffers.find(d.sourceTextureId);
        if(tileBufferIt != mTileBuffers.end())
        {
            unsigned char* pixels = tileBufferIt->second.getPixels(d.texX - d.texX % 32, d.texY - d.texY % 32, 32, 32);
//
//            if(d.targetTextureId == 0)
//            {
//                if(d.sourceTextureId >= 5 && d.sourceTextureId <= 19) // UI
//                {
//                    std::stringstream sstream;
//                    sstream << "out/ui/" << (int)d.sourceTextureId << "-" << d.texX << "x" << d.texY;
//                    writePng(sstream.str(), pixels, 32, 32);
//                }
//            }
//            else
//            {
//                if(d.targetTextureId == 3) // non-animated
//                {
//                    std::stringstream sstream;
//                    sstream << "out/nonanimated/" << (int)d.sourceTextureId << "-" << d.texX << "x" << d.texY;
//                    writePng(sstream.str(), pixels, 32, 32);
//                }
//                else if(d.targetTextureId == 1 && d.sourceTextureId >= 5 && d.sourceTextureId <= 19) // animated
//                {
//                    std::stringstream sstream;
//                    sstream << "out/animated/" << (int)d.sourceTextureId << "-" << d.texX << "x" << d.texY;
//                    writePng(sstream.str(), pixels, 32, 32);
//                }
//            }


            std::vector<unsigned char> opaquePixels;
            for(size_t i = 0; i < 32 * 32 * 4; i += 4)
            {
                if(pixels[i + 3] != 0)
                {
                    opaquePixels.push_back(pixels[i]);
                    opaquePixels.push_back(pixels[i + 1]);
                    opaquePixels.push_back(pixels[i + 2]);
                }
            }

            if(d.targetTextureId != 0 && d.targetTextureId != 2) // Do not capture UI stuff. Only capture things from the Tibia.spr.
            {
                std::list<size_t> ids;
                if(!mImageTree.find(opaquePixels, ids))
                {
//                    std::stringstream sstream;
//                    sstream << "out/notFound/" << (int)d.sourceTextureId << "-" << d.texX << "x" << d.texY;
//                    writePng(sstream.str(), pixels, 32, 32);
                }
                else
                {
//                    std::stringstream sstream;
//                    sstream << "out/found/" << (int)d.sourceTextureId << "-" << d.texX << "x" << d.texY;
//
//                    sstream << "(";
//                    for(size_t id : ids)
//                        sstream << id << ",";
//                    sstream << ")";
//
//                    writePng(sstream.str(), pixels, 32, 32);


                    mDrawnSprites.push_back(ids);
                }
            }


            delete[] pixels;
        }
    }
}

void TibiaClient::update()
{
    mDrawnSprites.clear();

    if(!mShm->hasPendingChanges)
        return;

    checkBufferOverflow();

    std::cout << "Num draw calls this frame: " << mShm->numDrawCall << std::endl;

    updateTileBuffers();
    handleDrawCalls();

    mShm->numPixelData = 0;
    mShm->numDrawCall = 0;
    mShm->hasPendingChanges = false;
}

int TibiaClient::prepareSharedMemory() const
{
    int shmFd = shm_open("shared_memory", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    int flags = fcntl(shmFd, F_GETFD);
    flags &= ~FD_CLOEXEC;
    fcntl(shmFd, F_SETFD, flags);

    if(shmFd == -1)
        throw std::runtime_error("Could not open shared memory.");

    if(ftruncate(shmFd, NUM_BYTES) == -1)
        throw std::runtime_error("Could not set shared memory size.");

    return shmFd;
}

void TibiaClient::mapSharedMemory(int shmFd)
{
    mShm = (SharedMemorySegment*)mmap(nullptr, NUM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if(mShm == MAP_FAILED)
        throw std::runtime_error("Could not map shared memory.");
}

void TibiaClient::launchClient(char** environment, std::string clientDirectory) const
{
    char* argv[] = {(char*)"./Tibia", 0};

    pid_t pid = fork();
    if(pid == 0)
    {
        chdir(clientDirectory.c_str());
        execve(argv[0], &argv[0], environment);
    }
    else if(pid < 0)
        throw std::runtime_error("Failed to fork.");
}

char** TibiaClient::prepareEnvironment(int shmFd) const
{
    size_t envSize = 0;
    while(environ[envSize] != nullptr)
        envSize++;

    size_t tibiaEnvSize = envSize + 2;
    char** tibiaEnv = new char*[tibiaEnvSize + 1];
    tibiaEnv[tibiaEnvSize] = nullptr;

    for(size_t i = 0; i < envSize; i++)
    {
        if(strncmp(environ[i], "HOME=", 5) == 0)
            tibiaEnv[i] = strdup("HOME=./");
        else
            tibiaEnv[i] = strdup(environ[i]);
    }

    tibiaEnv[envSize] = strdup("LD_PRELOAD=../bin/Debug/libGraphicsMonitor.so");
//    tibiaEnv[envSize] = strdup("LD_PRELOAD=../bin/Release/libGraphicsMonitor.so");

    std::stringstream sstream;
    sstream << "SHANKBOT_SHARED_MEMORY_FD=" << shmFd;
    tibiaEnv[envSize + 1] = strdup(sstream.str().c_str());

    return tibiaEnv;
}

void TibiaClient::deleteEnvironment(char** environment) const
{
    for(size_t i = 0; environment[i] != nullptr; i++)
        delete environment[i];

    delete[] environment;
}

std::list<std::list<size_t>> TibiaClient::getDrawnSprites() const
{
    return mDrawnSprites;
}


SharedMemorySegment* TibiaClient::getSharedMemory() const
{
    return mShm;
}
