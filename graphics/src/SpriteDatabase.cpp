///////////////////////////////////
// Internal ShankBot headers
#include "SpriteDatabase.hpp"
#include "PngImage.hpp"
#include "TileBuffer.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstring>
#include <stdexcept>
#include <sstream>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <dirent.h>
///////////////////////////////////

using namespace GraphicsLayer;

///////////////////////////////////

SpriteDatabase::Sprite::Sprite(std::string name, unsigned char* pixels)
: name(name)
, pixels(pixels)
{

}

///////////////////////////////////

SpriteDatabase::Sprite::~Sprite()
{
    delete[] pixels;
}

///////////////////////////////////

SpriteDatabase::SpriteDatabase(std::string directory)
: mSpriteMap()
, mNames()
{
    loadImages(directory);
}

///////////////////////////////////

bool SpriteDatabase::getName(size_t width, size_t height, unsigned char* pixels, std::string& name) const
{
    size_t size = width * height;
    auto sprites = mSpriteMap.find(size);
    if(sprites == mSpriteMap.end())
        return false;

    for(const SpritePtr& sprite : sprites->second)
    {
        if(compareSprites(size, sprite->pixels, pixels))
        {
            name = sprite->name;
            return true;
        }
    }

    return false;
}

///////////////////////////////////

bool SpriteDatabase::compareSprites(size_t size, unsigned char* s1, unsigned char* s2) const
{
    size_t halfIndex = (size / 2) * TileBuffer::BYTES_PER_PIXEL;
    size_t numBytes = size * TileBuffer::BYTES_PER_PIXEL;

    for(size_t i = halfIndex; i < numBytes; i += TileBuffer::BYTES_PER_PIXEL * 2)
    {
        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
        {
            return false;
        }
    }

    for(int i = halfIndex; i >= 0; i -= TileBuffer::BYTES_PER_PIXEL * 2)
    {
        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
        {
            return false;
        }
    }

    for(size_t i = halfIndex + TileBuffer::BYTES_PER_PIXEL; i < numBytes; i += TileBuffer::BYTES_PER_PIXEL * 2)
    {
        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
        {
            return false;
        }
    }

    for(int i = halfIndex - TileBuffer::BYTES_PER_PIXEL; i >= 0; i -= TileBuffer::BYTES_PER_PIXEL * 2)
    {
        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
        {
            return false;
        }
    }

    return true;
}

///////////////////////////////////

void SpriteDatabase::loadImages(std::string directory)
{
    DIR* dir;
    dir = opendir(directory.c_str());
    if(dir == nullptr)
        throw std::runtime_error("Cannot load images. Directory not found: '" + directory + "'.");

    dirent* ent;
    size_t numFolders = 0;
    std::vector<std::string> files;
    while((ent = readdir(dir)) != nullptr)
    {
        if(ent->d_type == DT_DIR)
        {
            if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
            {
                numFolders++;

                std::stringstream sstream;
                sstream << directory << "/" << ent->d_name;
                loadImages(sstream.str());
            }
        }
        else if(ent->d_type == DT_REG && numFolders == 0)
            files.push_back(ent->d_name);
    }

    if(numFolders == 0)
    {
        std::string folderName = directory.substr(directory.find_last_of('/') + 1);

        for(std::string file : files)
            loadImage(directory + "/" + file, folderName);
    }

    closedir(dir);
}

///////////////////////////////////

void SpriteDatabase::loadImage(std::string path, std::string folderName)
{
    PngImage img = readPng(path);

    if(img.bytesPerPixel == TileBuffer::BYTES_PER_PIXEL)
        insert(folderName, img.width, img.height, img.pixelData);
}

///////////////////////////////////

void SpriteDatabase::insert(std::string name, size_t width, size_t height, unsigned char* pixels)
{
    mNames.push_back(name);
    auto sprites = mSpriteMap.insert(std::make_pair(width * height, std::vector<SpritePtr>()));
    sprites.first->second.push_back(SpritePtr(new Sprite(name, pixels)));
}
