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
#include <cassert>
#include <limits>
#include <cmath>
#include <iostream>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <dirent.h>
///////////////////////////////////

using namespace GraphicsLayer;

///////////////////////////////////
//constexpr short SpriteDatabase::BUCKET_RANGES[];
constexpr unsigned char SpriteDatabase::BUCKET_RANGES[];


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

    for(const auto& i : mHistogramMap)
        for(const auto& j : i.second)
        {
            float sum = 0.f;

            for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
            {
                sum += j.red[i] + j.blue[i] + j.green[i];
            }
            assert(sum > 0.99999f && sum < 1.000001f);
            assert(j.alpha < 1.f);
        }

    size_t numNot32x32 = 0;
    for(auto& pair : mHistogramMap)
    {
        if(pair.first != 32 * 32)
            numNot32x32 += pair.second.size();
    }

    std::cout << "Num not 32x32: " << numNot32x32 << std::endl;
}

///////////////////////////////////

bool SpriteDatabase::getName(size_t width, size_t height, unsigned char* pixels, std::string& name) const
{
    HistogramEntry sprite;
    if(findSimilarSprite(width * height, pixels, sprite))
    {
        name = sprite.name;
        return true;
    }

    return false;
}

///////////////////////////////////

bool SpriteDatabase::findSimilarSprite(size_t size, unsigned char* pixels, HistogramEntry& similarSprite) const
{
//    auto entries = mHistogramMap.find(size);
//    if(entries == mHistogramMap.end())
//        return false;

    HistogramEntry sprite;
    if(!createHistogramEntry("", size, pixels, sprite))
        return false;

    HistogramEntry closestSprite;
    float minDelta = std::numeric_limits<float>::max();
    for(const auto& pair : mHistogramMap)
    for(const HistogramEntry& e : pair.second)//entries->second)
    {
//        if(std::fabs(sprite.alpha - e.alpha) < 0.01f)
        {
            float delta = 0;
            for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
            {
                delta += std::fabs(sprite.red[i] - e.red[i]);
                delta += std::fabs(sprite.green[i] - e.green[i]);
                delta += std::fabs(sprite.blue[i] - e.blue[i]);
            }

//            delta += std::fabs(sprite.alpha - e.alpha);

            if(delta < minDelta)
            {
                minDelta = delta;
                closestSprite = e;

                if(minDelta < 0.0000000001f)
                    break;
            }
        }
    }

    if(minDelta < 0.1f)
    {
//        std::cout << "Closest sprite is: " << closestSprite.name << " (delta=" << minDelta << ")" << std::endl;
        similarSprite = closestSprite;
        return true;
    }

    return false;
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

    insertHistogramEntry(name, width * height, pixels);
}


 void SpriteDatabase::insertHistogramEntry(std::string name, size_t size, unsigned char* pixels)
 {
    auto entries = mHistogramMap.insert(std::make_pair(size, std::vector<HistogramEntry>()));

    HistogramEntry entry;
    if(createHistogramEntry(name, size, pixels, entry))
        entries.first->second.push_back(entry);
 }


bool SpriteDatabase::createHistogramEntry(std::string name, size_t size, unsigned char* pixels, HistogramEntry& entry) const
{
    const size_t NUM_BYTES = size * TileBuffer::BYTES_PER_PIXEL;
    float red[NUM_BUCKETS_PER_BYTE];
    float green[NUM_BUCKETS_PER_BYTE];
    float blue[NUM_BUCKETS_PER_BYTE];
    size_t numTransparentPixels = 0;

    for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
        red[i] = green[i] = blue[i] = 0.f;

    for(size_t i = 0; i < NUM_BYTES;)
    {
        if(pixels[i + 3] < 50)
        {
            numTransparentPixels++;
            i += TileBuffer::BYTES_PER_PIXEL;
        }
        else
        {
            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
            {
                if(pixels[i] <= BUCKET_RANGES[iBucket])
                {
                    red[iBucket]++;
                    break;
                }
            }

            i++;
            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
            {
                if(pixels[i] <= BUCKET_RANGES[iBucket])
                {
                    green[iBucket]++;
                    break;
                }
            }

            i++;
            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
            {
                if(pixels[i] <= BUCKET_RANGES[iBucket])
                {
                    blue[iBucket]++;
                    break;
                }
            }

            i++;
            i++;
        }
    }

    if(size <= numTransparentPixels)
        return false;

    entry.name = name;
    const float NUM_OPAQUE_BYTES_FLOAT = (float)((size - numTransparentPixels) * (TileBuffer::BYTES_PER_PIXEL - 1));

    for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
    {
        red[i] /= NUM_OPAQUE_BYTES_FLOAT;
        green[i] /= NUM_OPAQUE_BYTES_FLOAT;
        blue[i] /= NUM_OPAQUE_BYTES_FLOAT;

        entry.red[i] = red[i];
        entry.green[i] = green[i];
        entry.blue[i] = blue[i];
        entry.alpha = ((float)(numTransparentPixels)) / ((float)size);
    }

    return true;
 }



bool SpriteDatabase::createHistogramEntry2(std::string name, size_t size, unsigned char* pixels, HistogramEntry& entry) const
{
    size -= 1;
    const size_t NUM_BYTES = size * TileBuffer::BYTES_PER_PIXEL;
    float red[NUM_BUCKETS_PER_BYTE];
    float green[NUM_BUCKETS_PER_BYTE];
    float blue[NUM_BUCKETS_PER_BYTE];
    size_t numTransparentPixels = 0;

    for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
        red[i] = green[i] = blue[i] = 0.f;

    for(size_t i = 0; i < NUM_BYTES;)
    {
        if(pixels[i + 3] < 50)
        {
            numTransparentPixels++;
            i += TileBuffer::BYTES_PER_PIXEL;
        }
        else
        {
            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
            {
                short delta = pixels[i] - pixels[i + TileBuffer::BYTES_PER_PIXEL];
                if(delta <= BUCKET_RANGES[iBucket])
                {
                    red[iBucket]++;
                    break;
                }
            }

            i++;
            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
            {
                short delta = pixels[i] - pixels[i + TileBuffer::BYTES_PER_PIXEL];
                if(delta <= BUCKET_RANGES[iBucket])
                {
                    green[iBucket]++;
                    break;
                }
            }

            i++;
            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
            {
                short delta = pixels[i] - pixels[i + TileBuffer::BYTES_PER_PIXEL];
                if(delta <= BUCKET_RANGES[iBucket])
                {
                    blue[iBucket]++;
                    break;
                }
            }

            i++;
            i++;
        }
    }

    if(size <= numTransparentPixels)
        return false;

    entry.name = name;
    const float NUM_OPAQUE_BYTES_FLOAT = (float)((size - numTransparentPixels) * (TileBuffer::BYTES_PER_PIXEL - 1));

    for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
    {
        red[i] /= NUM_OPAQUE_BYTES_FLOAT;
        green[i] /= NUM_OPAQUE_BYTES_FLOAT;
        blue[i] /= NUM_OPAQUE_BYTES_FLOAT;

        entry.red[i] = red[i];
        entry.green[i] = green[i];
        entry.blue[i] = blue[i];
        entry.alpha = ((float)(numTransparentPixels)) / ((float)size);
    }

    return true;
 }

