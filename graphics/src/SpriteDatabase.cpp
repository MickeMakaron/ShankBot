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
///////////////////////////////////

///////////////////////////////////
// STD C
#include <dirent.h>
///////////////////////////////////

using namespace GraphicsLayer;

///////////////////////////////////
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
#include <iostream>
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
                sum += j.red[i] + j.blue[i] + j.green[i]; //+ j.alpha[i];
            }
            assert(sum > 0.99999f && sum < 1.000001f);
        }
}

///////////////////////////////////
#include <iostream>
bool SpriteDatabase::getName(size_t width, size_t height, unsigned char* pixels, std::string& name) const
{
    size_t size = width * height;
    auto entries = mHistogramMap.find(size);
    if(entries == mHistogramMap.end())
        return false;

    HistogramEntry sprite;
    if(!createHistogramEntry("", size, pixels, sprite))
        return false;

    HistogramEntry closestSprite;
    float minDelta = std::numeric_limits<float>::max();
    for(const HistogramEntry& e : entries->second)
    {
        float delta = 0;
        for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
        {
            delta += std::fabs(sprite.red[i] - e.red[i]);
            delta += std::fabs(sprite.green[i] - e.green[i]);
            delta += std::fabs(sprite.blue[i] - e.blue[i]);
//            delta += std::fabs(sprite.alpha[i] - e.alpha[i]);
        }

        if(delta < minDelta)
        {
            minDelta = delta;
            closestSprite = e;
        }
    }

    if(minDelta < 0.01f)
    {
        std::cout << "Closest sprite is: " << closestSprite.name << " (delta=" << minDelta << ")" << std::endl;
        name = closestSprite.name;
        return true;
    }
    else
        return false;




//    size_t size = width * height;
//    auto sprites = mSpriteMap.find(size);
//    if(sprites == mSpriteMap.end())
//        return false;
//
//    for(const SpritePtr& sprite : sprites->second)
//    {
//        if(compareSprites(size, sprite->pixels, pixels))
//        {
//            name = sprite->name;
//            return true;
//        }
//    }
//
//    return false;
}

///////////////////////////////////

bool SpriteDatabase::compareSprites(size_t size, unsigned char* s1, unsigned char* s2) const
{
    size_t halfIndex = (size / 2) * TileBuffer::BYTES_PER_PIXEL;
    size_t numBytes = size * TileBuffer::BYTES_PER_PIXEL;

    const int EPSILON = 100;

    for(size_t i = halfIndex; i < numBytes; i += TileBuffer::BYTES_PER_PIXEL * 2)
    {
        for(size_t j = i; j < i + 4; j++)
        {
            int s2Max = s2[j];
            s2Max += EPSILON;

            int s2Min = s2[j];
            s2Min -= EPSILON;
            if(s1[j] > s2Max || s1[j] < s2Min)
                return false;

        }
//        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
//        {
//            return false;
//        }
    }

    for(int i = halfIndex; i >= 0; i -= TileBuffer::BYTES_PER_PIXEL * 2)
    {

        for(size_t j = i; j < i + 4; j++)
        {
            int s2Max = s2[j];
            s2Max += EPSILON;

            int s2Min = s2[j];
            s2Min -= EPSILON;
            if(s1[j] > s2Max || s1[j] < s2Min)
                return false;

        }
//        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
//        {
//            return false;
//        }
    }

    for(size_t i = halfIndex + TileBuffer::BYTES_PER_PIXEL; i < numBytes; i += TileBuffer::BYTES_PER_PIXEL * 2)
    {
        for(size_t j = i; j < i + 4; j++)
        {
            int s2Max = s2[j];
            s2Max += EPSILON;

            int s2Min = s2[j];
            s2Min -= EPSILON;
            if(s1[j] > s2Max || s1[j] < s2Min)
                return false;

        }

//        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
//        {
//            return false;
//        }
    }

    for(int i = halfIndex - TileBuffer::BYTES_PER_PIXEL; i >= 0; i -= TileBuffer::BYTES_PER_PIXEL * 2)
    {
        for(size_t j = i; j < i + 4; j++)
        {
            int s2Max = s2[j];
            s2Max += EPSILON;

            int s2Min = s2[j];
            s2Min -= EPSILON;
            if(s1[j] > s2Max || s1[j] < s2Min)
                return false;

        }

//        if(s1[i] != s2[i] || s1[i + 1] != s2[i + 1] || s1[i + 2] != s2[i + 2] || s1[i + 3] != s2[i + 3])
//        {
//            return false;
//        }
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
    float alpha[NUM_BUCKETS_PER_BYTE];
    size_t numTransparentPixels = 0;

    for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
    {
        red[i] = green[i] = blue[i] = alpha[i] = 0.f;
    }


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

//
//    for(size_t iRed = 0; iRed < NUM_BYTES; iRed += TileBuffer::BYTES_PER_PIXEL)
//    {
//        if(pixels[iRed + 3] < 50)
//        {
//            alpha[0]++;
//        }
//        else
//        {
//            for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
//            {
//                if(pixels[iRed] <= BUCKET_RANGES[iBucket])
//                {
//                    red[iBucket]++;
//                    break;
//                }
//            }
//        }
//
//    }
//
//    for(size_t iGreen = 1; iGreen < NUM_BYTES; iGreen += TileBuffer::BYTES_PER_PIXEL)
//    {
//        for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
//        {
//            if(pixels[iGreen] <= BUCKET_RANGES[iBucket])
//            {
//                green[iBucket]++;
//                break;
//            }
//        }
//    }
//
//    for(size_t iBlue = 2; iBlue < NUM_BYTES; iBlue += TileBuffer::BYTES_PER_PIXEL)
//    {
//        for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
//        {
//            if(pixels[iBlue] <= BUCKET_RANGES[iBucket])
//            {
//                blue[iBucket]++;
//                break;
//            }
//        }
//    }
//
//    for(size_t iAlpha = 3; iAlpha < NUM_BYTES; iAlpha += TileBuffer::BYTES_PER_PIXEL)
//    {
//        for(size_t iBucket = 0; iBucket < NUM_BUCKETS_PER_BYTE; iBucket++)
//        {
//            if(pixels[iAlpha] <= BUCKET_RANGES[iBucket])
//            {
//                alpha[iBucket]++;
//                break;
//            }
//        }
//    }

    if(size <= numTransparentPixels)
        return false;


    entry.name = name;

//    const float NUM_BYTES_FLOAT = (float)(NUM_BYTES - numTransparentPixels * TileBuffer::BYTES_PER_PIXEL);
    const float NUM_BYTES_FLOAT = (float)((size - numTransparentPixels) * (TileBuffer::BYTES_PER_PIXEL - 1));

    for(size_t i = 0; i < NUM_BUCKETS_PER_BYTE; i++)
    {
        red[i] /= NUM_BYTES_FLOAT;
        green[i] /= NUM_BYTES_FLOAT;
        blue[i] /= NUM_BYTES_FLOAT;
        alpha[i] /= NUM_BYTES_FLOAT;

        entry.red[i] = red[i];
        entry.green[i] = green[i];
        entry.blue[i] = blue[i];
        entry.alpha[i] = alpha[i];
    }

    return true;
 }
