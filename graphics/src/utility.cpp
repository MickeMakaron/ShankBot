///////////////////////////////////
// Internal ShankBot headers
#include "utility.hpp"
#include "TileBuffer.hpp"
#include "PngImage.hpp"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <sstream>
#include <cstring>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <dirent.h>
///////////////////////////////////

namespace GraphicsLayer
{

///////////////////////////////////

void writePixelsToFile(std::string fileName, const unsigned char* pixels, size_t width, size_t height)
{
    std::ofstream file;
    file.open(fileName.c_str(), std::ios::trunc);
    file    << "P3" << std::endl
            << width << " " << height << std::endl
            << "255" << std::endl;

    for(size_t i = 0; i < width * height * 3; i++)
        file << (int)pixels[i] << " ";
}

///////////////////////////////////

unsigned char* rgbaToRgb(const unsigned char* rgba, size_t width, size_t height)
{
    unsigned char* rgb = new unsigned char[width * height * 3];
    for(size_t iDest = 0, iSrc = 0; iSrc < width * height * 4; iSrc++)
    {
        if((iSrc + 1) % 4 != 0)
        {
            rgb[iDest] = ((unsigned char*)rgba)[iSrc];
            iDest++;
        }
    }

    return rgb;
}

///////////////////////////////////

void testLoadImages(std::string directory, std::list<PngImage>& images)
{
    DIR* dir;
    dir = opendir(directory.c_str());
    if(dir == nullptr)
        throw std::runtime_error("Cannot load images. Directory not found: '" + directory + "'.");

    dirent* ent;
    while((ent = readdir(dir)) != nullptr)
    {
        if(ent->d_type == DT_DIR)
        {
            if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
            {
                std::stringstream sstream;
                sstream << directory << "/" << ent->d_name;
                testLoadImages(sstream.str(), images);
            }
        }
        else if(ent->d_type == DT_REG)
        {
            std::stringstream sstream;
            sstream << directory << "/" << ent->d_name;
            PngImage image = readPng(sstream.str());
            if(image.bytesPerPixel == TileBuffer::BYTES_PER_PIXEL)
                images.push_back(image);
        }

    }

    closedir(dir);
}

///////////////////////////////////

void readPpm(std::string filePath, unsigned char** data, size_t* width, size_t* height)
{
    std::ifstream file;
    file.open(filePath.c_str());

    std::string garbage;
    file >> garbage;

    file >> *width;
    file >> *height;

    file >> garbage;

    size_t size = *width * *height * 3;
    *data = new unsigned char[size];
    for(size_t i = 0; i < size; i++)
    {
        size_t byte;
        file >> byte;
        (*data)[i] = byte;
    }
}

///////////////////////////////////

std::string getDateTime()
{
    time_t t = time(0);
    struct tm* now = localtime( & t );
    std::stringstream sstream;
    sstream     << now->tm_year + 1900 << "-"
                << now->tm_mon + 1 << "-"
                << now->tm_mday << "-"
                << now->tm_hour << "-"
                << now->tm_min << "-"
                << now->tm_sec;

    return sstream.str();
}


///////////////////////////////////

std::vector<unsigned char> rgbaToColorTreeSprite(const std::vector<unsigned char>& rgba)
{
    return rgbaToColorTreeSprite(rgba.data(), rgba.size());
}

///////////////////////////////////

std::vector<unsigned char> rgbaToTransparencyTreeSprite(const std::vector<unsigned char>& rgba)
{
    return rgbaToTransparencyTreeSprite(rgba.data(), rgba.size());
}

///////////////////////////////////

std::vector<unsigned char> rgbaToColorTreeSprite(const unsigned char* rgba, size_t numPixels)
{
    std::vector<unsigned char> sprite;
    for(size_t i = 0; i + 3 < numPixels; i += 4)
    {
        if(rgba[i + 3] != 0)
        {
            sprite.push_back(rgba[i]);
            sprite.push_back(rgba[i + 1]);
            sprite.push_back(rgba[i + 2]);
        }
    }

    return sprite;
}

///////////////////////////////////

std::vector<unsigned char> rgbaToTransparencyTreeSprite(const unsigned char* rgba, size_t numPixels)
{
    std::vector<unsigned char> sprite;
    for(size_t i = 0; i + 3< numPixels; i += 4)
    {
        if(rgba[i + 3] != 0)
        {
            unsigned char x = i % 32;
            unsigned char y = i / 32;

            sprite.push_back(x);
            sprite.push_back(y);
        }
    }

    return sprite;
}

///////////////////////////////////

}
