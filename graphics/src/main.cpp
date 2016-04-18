#include <unistd.h>
#include <iostream>
#include <wait.h>
#include <cstring>

#include <fstream>

#include <fcntl.h>
#include <sstream>

#include "PipeProtocol.hpp"
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
//#include <dirent.h>
//#include "TileBuffer.hpp"


#include "TileBuffer.hpp"
#include <ctime>

TileBuffer* tileBuffers = new TileBuffer[20];

void readPipe(int pipeFd)
{
    GraphicsLayer::PipeProtocol::Header header;
    ssize_t bytesRead = 0;
    while((bytesRead = read(pipeFd, &header, sizeof(GraphicsLayer::PipeProtocol::Header))) > 0)
    {
//                std::cout << "Read " << bytesRead << " bytes from pipe: " << std::endl;

        if(header.type == GraphicsLayer::PipeProtocol::Type::PIXEL_DATA)
        {
            GraphicsLayer::PipeProtocol::PixelData* pixelPackets = new GraphicsLayer::PipeProtocol::PixelData[header.packetCount];
            size_t numBytes = sizeof(GraphicsLayer::PipeProtocol::PixelData) * header.packetCount;
            bytesRead = read(pipeFd, pixelPackets, numBytes);
            if(bytesRead != numBytes)
            {
                std::stringstream sstream;
                sstream << "ERROR READING PIXEL PACKET. EXPECTED " << numBytes << "bytes, got " << bytesRead << ".";
                throw std::runtime_error(sstream.str());
            }
            else
            {
//                        for(size_t i = 0; i < header.packetCount; i++)
//                        {
//                            for(size_t j = 0; j < sizeof(GraphicsLayer::PipeProtocol::PixelData::pixels); j++)
//                                std::cout << (int)pixelPackets[i].pixels[j] << " ";
//
//                            std::cout << std::endl << "--------------" << std::endl;
//                        }

                for(size_t i = 0; i < header.packetCount; i++)
                {
                    tileBuffers[pixelPackets[i].textureId - 1].setTile(pixelPackets[i]);
//                            tileBuffers[0].setTile(pixelPackets[i]);

//                            std::stringstream sstream;
//                            sstream << "pixelPackets/" << numPixelPackets;
//                            unsigned char* rgb = rgbaToRgb(pixelPackets[i].pixels, 32, 32);
//
//                            writePixelsToFile(sstream.str(), rgb, 32, 32);
//                            delete[] rgb;
                }



            }

//                    for(size_t i = 0; i < header.packetCount; i++)
//                    {
//                        delete[] pixelPackets[i].pixels;
//
//                    }
            delete[] pixelPackets;
        }
        else if(header.type == GraphicsLayer::PipeProtocol::Type::DRAW_CALL)
        {
            GraphicsLayer::PipeProtocol::DrawCall* drawCalls = new GraphicsLayer::PipeProtocol::DrawCall[header.packetCount];
            size_t numBytes = sizeof(GraphicsLayer::PipeProtocol::DrawCall) * header.packetCount;
            bytesRead = read(pipeFd, drawCalls, numBytes);

            if(bytesRead != numBytes)
            {
                std::stringstream sstream;
                sstream << "ERROR READING DRAW CALL PACKET. EXPECTED " << numBytes << "bytes, got " << bytesRead << ".";
                throw std::runtime_error(sstream.str());
            }
            else
            {
//                        for(size_t i = 0; i < header.packetCount; i++)
//                        {
//                            std::cout   << "DRAW CALL" << std::endl
//                                        << "\t" << (int)drawCalls[i].textureId << std::endl
//                                        << "\t" << drawCalls[i].texX << std::endl
//                                        << "\t" << drawCalls[i].texY << std::endl
//                                        << "\t" << drawCalls[i].screenX << std::endl
//                                        << "\t" << drawCalls[i].screenY << std::endl;
//                        }

                for(size_t i = 0; i < header.packetCount; i++)
                {
                    if(drawCalls[i].textureId > 5)
                    {
                        std::cout << "Reading: \t" << drawCalls[i].texX << "x" << drawCalls[i].texY << std::endl;
                        unsigned char* pixels = tileBuffers[drawCalls[i].textureId - 1].getPixels(drawCalls[i].texX, drawCalls[i].texY, drawCalls[i].width, drawCalls[i].height);
                        unsigned char* rgb = rgbaToRgb(pixels, drawCalls[i].width, drawCalls[i].height);
                        delete[] pixels;
                        std::stringstream sstream;
                        sstream << "drawnTiles/" << drawCalls[i].texY << "x" << drawCalls[i].texX;

                        writePixelsToFile(sstream.str(), rgb, drawCalls[i].width, drawCalls[i].height);
                        delete[] rgb;
                    }

                }

            }

            delete[] drawCalls;
        }

    }
    if(errno != EWOULDBLOCK) // If errno == EWOULDBLOCK, it means the pipe is just empty.
    {
        std::stringstream sstream;
        sstream << "Error " << errno << " when reading pipe.";
        throw std::runtime_error(sstream.str());
    }

}


int main(int argc, char *args[])
{
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

    //SpriteDatabase db("spriteDatabase");

    putenv((char*)"HOME=./");
    putenv((char*)"LD_PRELOAD=../bin/Debug/libGraphicsMonitor.so");

    int pipeFd[2];
    pipe2(pipeFd, O_NONBLOCK);
    int pipeFdRead = pipeFd[0];
    int pipeFdWrite = pipeFd[1];


    std::cout << fcntl(pipeFdRead, F_SETPIPE_SZ, 1048576) << std::endl;
    std::cout << fcntl(pipeFdWrite, F_SETPIPE_SZ, 1048576) << std::endl;


//    setEnvironmentVar("LD_PRELOAD", "/home/vendrii/Documents/programming/projects/InjectTest/bin/Debug/libInjectTest.so");
    for(char** env = environ; *env != 0; env++)
        std::cout << *env << std::endl;

//    char* argv[] = {"/home/vendrii/programs/Tibia/Tibia", 0};
    char* argv[] = {(char*)"./Tibia", 0};
//    char* argv[] = {"./herpderp", 0};
    pid_t pid = fork();

    if(pid == 0)
    {
        chdir("./tibia");
        std::stringstream sstream;
        sstream << pipeFdWrite;
        std::string pipeWriteEnvVar = std::string("SHANKBOT_PIPE_WRITE=");
        pipeWriteEnvVar += sstream.str();
        putenv((char*)pipeWriteEnvVar.c_str());

        std::cout << "Hello from Child!" << std::endl;
        execve(argv[0], &argv[0], environ);

    }
    else if(pid > 0)
    {
        char* buffer = new char[500];
        memset(buffer, '\0', 500);

        size_t numPixelPackets = 0;
        while(true)
        {
            unsigned int ms = 10;
//            usleep(1000 * ms);
//                std::cout << "Going to work... " << std::endl;


                readPipe(pipeFdRead);


            for(size_t i = 0; i < 20; i++)
            {
//                std::cout << "Going to work... " << std::endl;
//                readPipe(pipeFdRead);



//                std::stringstream sstream;
//                sstream << "tileBuffers/" << i;
//                unsigned char* rgb = rgbaToRgb(tileBuffers[i].getPixels(), TileBuffer::WIDTH, TileBuffer::HEIGHT);
//                writePixelsToFile(sstream.str(), rgb, TileBuffer::WIDTH, TileBuffer::HEIGHT);
            }

        }
        int status;
        waitpid(pid, &status, 0);
        std::cout << "Hello from Parent!" << std::endl;
        std::cout << "Status: " << status << std::endl;
    }
    else
    {
        std::cout << "Fork failed." << std::endl;
    }

}
