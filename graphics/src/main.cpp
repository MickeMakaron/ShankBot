#include <unistd.h>
#include <iostream>
#include <wait.h>
#include <cstring>

#include <fstream>

#include <fcntl.h>
#include <sstream>

#include <vector>
#include <map>

#include "PngImage.hpp"
#include "SpriteCache.hpp"
#include "SpriteDatabase.hpp"
#include "utility.hpp"

using namespace GraphicsLayer;

void setEnvironmentVar(std::string name, std::string value)
{
    std::string concatenated = name + std::string("=") + value;
    putenv((char*)concatenated.c_str());
}


#include "TileBuffer.hpp"
#include <ctime>

TileBuffer* tileBuffers = new TileBuffer[20];

#include <sys/mman.h>
#include "SharedMemoryProtocol.hpp"

int main(int argc, char *args[])
{
    int shmFd = shm_open("shared_memory", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    int flags = fcntl(shmFd, F_GETFD);
    flags &= ~FD_CLOEXEC;
    fcntl(shmFd, F_SETFD, flags);

    if(shmFd == -1)
        throw std::runtime_error("Could not open shared memory.");

    if(ftruncate(shmFd, SharedMemoryProtocol::NUM_BYTES) == -1)
        throw std::runtime_error("Could not set shared memory size.");

//    std::string TIBIA_DIR = "/home/vendrii/Documents/programming/projects/test/tibia";
//
//    setEnvironmentVar("HOME", TIBIA_DIR);
////
////    std::string environmentVar = "HOME=";
////    putenv((char*)(environmentVar + TIBIA_DIR).c_str());

//    PngImage png = readPng("Achad-0-0.png");
//
////    unsigned char* rgba = new unsigned char[pngWut::imgWidth * pngWut::imgHeight * 4];
//////    memcpy(rgba, *row_pointers, imgWidth * imgHeight * 4);
////    for(size_t y = 0; y < pngWut::imgHeight; y++)
////        memcpy(&rgba[y * pngWut::imgWidth * 4], pngWut::row_pointers[y], pngWut::imgWidth * 4);
////
////    std::cout << "Pixels: " << std::endl << "\t" << "'";
////    for(size_t i = 0; i < 20; i++)
////        std::cout << (int)(rgba[i]) << " ";
////    std::cout << "'" << std::endl;
//
//
//    unsigned char* rgb = rgbaToRgb(png.pixelData, png.width, png.height);
//
//
//    writePixelsToFile("achadYO.ppm", rgb, png.width, png.height);

    SpriteDatabase db("spriteDatabase");

    putenv((char*)"HOME=./");
    putenv((char*)"LD_PRELOAD=../bin/Debug/libGraphicsMonitor.so");

    for(char** env = environ; *env != 0; env++)
        std::cout << *env << std::endl;

    char* argv[] = {(char*)"./Tibia", 0};
    pid_t pid = fork();

    if(pid == 0)
    {
        chdir("./tibia");
        std::stringstream sstream;

        sstream.str(std::string());
        sstream << "SHANKBOT_SHARED_MEMORY_FD=" << shmFd;
        std::string strShmFd = sstream.str();
        putenv((char*)strShmFd.c_str());

        for(char** env = environ; *env != 0; env++)
            std::cout << *env << std::endl;

        std::cout << "Hello from Child!" << std::endl;
        execve(argv[0], &argv[0], environ);

    }
    else if(pid > 0)
    {
        using namespace GraphicsLayer::SharedMemoryProtocol;

        SharedMemorySegment* shm = (SharedMemorySegment*)mmap(nullptr, SharedMemoryProtocol::NUM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
        if(shm == MAP_FAILED)
            throw std::runtime_error("Could not map shared memory.");

        while(true)
        {
            unsigned int ms = 100;
//            usleep(1000 * ms);

            std::cout << "Pending changes? : " << (shm->hasPendingChanges == true ? "true" : "false") << std::endl;
            std::cout << "Num pixel data: " << shm->numPixelData << std::endl;

            if(shm->hasPendingChanges)
            {
//                for(size_t i = 0; i < 5; i++)
//                {
//                    GraphicsLayer::SharedMemoryProtocol::PixelData p = shm->pixelData[i];
//                    std::cout   << "PixelData: " << std::endl
//                                << "\t" << p.textureId << " - " << p.texX << "x" << p.texY << std::endl;
//
//
//                    GraphicsLayer::SharedMemoryProtocol::DrawCall d = shm->drawCall[i];
//                    std::cout   << "DrawCall: " << std::endl
//                                << "\t" << d.textureId << " - " << d.texX << "x" << d.texY << std::endl;
//                }

                if(shm->numPixelData > MAX_NUM_PIXEL_DATA)
                {
                    std::stringstream sstream;
                    sstream << "PIXEL DATA BUFFER OVERFLOW. (" << shm->numPixelData << " of max " << MAX_NUM_PIXEL_DATA << ")";
                    throw std::runtime_error(sstream.str());
                }


                if(shm->numDrawCall > MAX_NUM_DRAW_CALL)
                {
                    std::stringstream sstream;
                    sstream << "DRAW CALL BUFFER OVERFLOW. (" << shm->numDrawCall << " of max " << MAX_NUM_DRAW_CALL << ")";
                    throw std::runtime_error(sstream.str());
                }


                for(size_t i = 0; i < shm->numPixelData; i++)
                    tileBuffers[shm->pixelData[i].textureId - 1].setTile(shm->pixelData[i]);


                for(size_t i = 0; i < shm->numDrawCall; i++)
                {
                    const DrawCall& d = shm->drawCall[i];

                    if(d.textureId > 5)
                    {
//                        std::cout << "Reading: \t" << d.texX << "x" << d.texY << std::endl;
//                        unsigned char* pixels = tileBuffers[d.textureId - 1].getPixels(d.texX, d.texY, d.width, d.height);
//                        unsigned char* rgb = rgbaToRgb(pixels, d.width, d.height);
//                        delete[] pixels;
//                        std::stringstream sstream;
//                        sstream << "drawnTiles/" << d.texY << "x" << d.texX;
//
//                        writePixelsToFile(sstream.str(), rgb, d.width, d.height);
//                        delete[] rgb;

                        unsigned char* pixels = tileBuffers[d.textureId - 1].getPixels(d.texX, d.texY, d.width, d.height);
                        std::cout << "Searching... " << i << std::endl;
                        std::string name;
                        if(db.getName(d.width, d.height, pixels, name))
                        {
                            std::cout << "FOUND NAME!!!!: " << name << std::endl;

                            unsigned char* rgb = rgbaToRgb(pixels, d.width, d.height);
                            delete[] pixels;
                            std::stringstream sstream;
                            sstream << "foundSprites/" << name;

                            writePixelsToFile(sstream.str(), rgb, d.width, d.height);
                            delete[] rgb;
                        }


                    }



                }

                shm->numPixelData = 0;
                shm->numDrawCall = 0;
                shm->hasPendingChanges = false;
            }

            for(size_t i = 15; i < 20; i++)
            {
//                std::cout << "Going to work... " << std::endl;
//                readPipe(pipeFdRead);


//
//                std::stringstream sstream;
//                sstream << "tileBuffers/" << i;
//                unsigned char* rgb = rgbaToRgb(tileBuffers[i].getPixels(), TileBuffer::WIDTH, TileBuffer::HEIGHT);
//                writePixelsToFile(sstream.str(), rgb, TileBuffer::WIDTH, TileBuffer::HEIGHT);
            }

        }
    }
    else
    {
        std::cout << "Fork failed." << std::endl;
    }

}
