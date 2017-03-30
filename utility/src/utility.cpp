// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
///////////////////////////////////
// Internal ShankBot headers
#include "utility/utility.hpp"
#include "utility/Matrix.hpp"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <algorithm>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <dirent.h>
///////////////////////////////////

namespace sb
{
namespace utility
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
    unsigned char* rgb = new unsigned char[width * height * BYTES_PER_PIXEL_RGB];
    for(size_t iDest = 0, iSrc = 0; iSrc < width * height * BYTES_PER_PIXEL_RGBA; iSrc++)
    {
        if((iSrc + 1) % BYTES_PER_PIXEL_RGBA != 0)
        {
            rgb[iDest] = ((unsigned char*)rgba)[iSrc];
            iDest++;
        }
    }

    return rgb;
}

///////////////////////////////////

unsigned char* bgraToRgba(const unsigned char* bgra, size_t width, size_t height)
{
    unsigned char* rgba = new unsigned char[width * height * BYTES_PER_PIXEL_RGBA];
    for(size_t iDest = 0, iSrc = 0; iSrc < width * height * BYTES_PER_PIXEL_RGBA;)
    {
        rgba[iDest] = bgra[iSrc + 2];
        rgba[iDest + 1] = bgra[iSrc + 1];
        rgba[iDest + 2] = bgra[iSrc];
        rgba[iDest + 3] = bgra[iSrc + 3];
        iDest += BYTES_PER_PIXEL_RGBA;
        iSrc += BYTES_PER_PIXEL_RGBA;
    }

    return rgba;
}

///////////////////////////////////

unsigned char* rgbaToGrayscale(const unsigned char* rgba, size_t width, size_t height)
{
    unsigned char* grayscale = new unsigned char[width * height * BYTES_PER_PIXEL_ALPHA];
    for(size_t iDest = 0, iSrc = 0; iSrc + BYTES_PER_PIXEL_RGBA - 1 < width * height * BYTES_PER_PIXEL_RGBA; iDest++, iSrc += BYTES_PER_PIXEL_RGBA)
    {
        float sum = rgba[iSrc] + rgba[iSrc + 1] + rgba[iSrc + 2];
        float opacity = rgba[iSrc + 3] / 255.f;
        grayscale[iDest] = (float(sum) / 3.f) * opacity;
    }

    return grayscale;
}

///////////////////////////////////

unsigned char* grayscaleToRgb(const unsigned char* grayscale, size_t width, size_t height)
{
    unsigned char* rgb = new unsigned char[width * height * BYTES_PER_PIXEL_RGB];
    for(size_t iDest = 0, iSrc = 0; iSrc < width * height * BYTES_PER_PIXEL_ALPHA; iDest += BYTES_PER_PIXEL_RGB, iSrc++)
        memset(&rgb[iDest], grayscale[iSrc], BYTES_PER_PIXEL_RGB);

    return rgb;
}

///////////////////////////////////

//void testLoadImages(std::string directory, std::list<PngImage>& images)
//{
//    DIR* dir;
//    dir = opendir(directory.c_str());
//    if(dir == nullptr)
//        THROW_RUNTIME_ERROR("Cannot load images. Directory not found: '" + directory + "'.");
//
//    dirent* ent;
//    while((ent = readdir(dir)) != nullptr)
//    {
//        if(ent->d_type == DT_DIR)
//        {
//            if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
//            {
//                std::stringstream sstream;
//                sstream << directory << "/" << ent->d_name;
//                testLoadImages(sstream.str(), images);
//            }
//        }
//        else if(ent->d_type == DT_REG)
//        {
//            std::stringstream sstream;
//            sstream << directory << "/" << ent->d_name;
//            PngImage image = readPng(sstream.str());
//            if(image.bytesPerPixel == TileBuffer::BYTES_PER_PIXEL)
//                images.push_back(image);
//        }
//
//    }
//
//    closedir(dir);
//}

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

std::vector<size_t> rgbaToColorTreeSprite(const std::vector<unsigned char>& rgba, size_t width, size_t height, bool* isBlank)
{
    return rgbaToColorTreeSprite(rgba.data(), width, height, isBlank);
}

///////////////////////////////////

std::vector<size_t> bgraToColorTreeSprite(const std::vector<unsigned char>& bgra, size_t width, size_t height, bool* isBlank)
{
    return bgraToColorTreeSprite(bgra.data(), width, height, isBlank);
}

///////////////////////////////////

std::vector<size_t> rgbaToTransparencyTreeSprite(const std::vector<unsigned char>& rgba, size_t width, size_t height, bool* isBlank)
{
    return rgbaToTransparencyTreeSprite(rgba.data(), width, height, isBlank);
}

///////////////////////////////////

std::vector<size_t> rgbaToColorTreeSprite(const unsigned char* rgba, size_t width, size_t height, bool* isBlank)
{
    std::vector<unsigned char> sprite;
    sprite.push_back(width);
    sprite.push_back(height);
    size_t size = width * height * BYTES_PER_PIXEL_RGBA;
    for(size_t i = 0; i + BYTES_PER_PIXEL_RGBA - 1 < size; i += BYTES_PER_PIXEL_RGBA)
    {
        if(rgba[i + BYTES_PER_PIXEL_RGBA - 1] == 255)
        {
            sprite.push_back(rgba[i]);
            sprite.push_back(rgba[i + 1]);
            sprite.push_back(rgba[i + 2]);
        }
    }
    if(isBlank)
        *isBlank = (sprite.size() == 2);
    return packBytes(sprite);
}

///////////////////////////////////

std::vector<size_t> bgraToColorTreeSprite(const unsigned char* bgra, size_t width, size_t height, bool* isBlank)
{
    unsigned char* rgba = bgraToRgba(bgra, width, height);

    std::vector<size_t> sprite = rgbaToColorTreeSprite(rgba, width, height, isBlank);
    delete[] rgba;
    return sprite;
//
//    std::vector<unsigned char> sprite;
//    sprite.push_back(width);
//    sprite.push_back(height);
//    size_t size = width * height * BYTES_PER_PIXEL_RGBA;
//    for(size_t i = 0; i + BYTES_PER_PIXEL_RGBA - 1 < size; i += BYTES_PER_PIXEL_RGBA)
//    {
//        if(bgra[i + BYTES_PER_PIXEL_RGBA - 1] != 0)
//        {
//            sprite.push_back(bgra[i + 2]);
//            sprite.push_back(bgra[i + 1]);
//            sprite.push_back(bgra[i]);
//        }
//    }
//
//    if(isBlank)
//        *isBlank = (sprite.size() == 2);
//    return packBytes(sprite);
}

///////////////////////////////////

std::vector<size_t> rgbaToTransparencyTreeSprite(const unsigned char* rgba, size_t width, size_t height, bool* isBlank)
{
    std::vector<unsigned char> sprite;
    sprite.push_back(width);
    sprite.push_back(height);
    size_t size = width * height * BYTES_PER_PIXEL_RGBA;
    size_t stripSize = 0;
    auto compressor = isBigEndian() ? compressBigEndianMultiByteValue<size_t> : compressSmallEndianMultiByteValue<size_t>;
    for(size_t i = 0; i + BYTES_PER_PIXEL_RGBA - 1 < size; i += BYTES_PER_PIXEL_RGBA)
    {
        if(rgba[i + BYTES_PER_PIXEL_RGBA - 1] == 255)
        {
            if(stripSize == 0)
            {
                compressor(i, sprite);
            }

            stripSize++;
        }
        else if(stripSize > 0)
        {
            compressor(stripSize, sprite);
            stripSize = 0;
        }
    }
    if(isBlank)
        *isBlank = (sprite.size() == 2);
    return packBytes(sprite);
}

///////////////////////////////////

template<typename StreamT>
size_t readTibiaSizeIndicatorHelper(StreamT& stream)
{
    size_t size = 0;
    unsigned char byte;

    readStream(byte, stream);
    size += byte;

    size_t multiplier = 1;
    while(byte >= 0x80)
    {
        multiplier *= 0x80;
        readStream(byte, stream);
        size += (byte - 1) * multiplier;
    }

    return size;
}

__declspec(dllexport) size_t readTibiaSizeIndicator(std::istream& stream)
{
    return readTibiaSizeIndicatorHelper(stream);
}

__declspec(dllexport) size_t readTibiaSizeIndicator(Buffer& stream)
{
    return readTibiaSizeIndicatorHelper(stream);
}

///////////////////////////////////

std::vector<size_t> packBytes(const std::vector<unsigned char>& bytes)
{
    size_t size = bytes.size() / sizeof(size_t);
    if(bytes.size() % sizeof(size_t) != 0)
        size++;

    std::vector<size_t> packed;
    packed.resize(size);
    memcpy(packed.data(), bytes.data(), bytes.size());

    return packed;
}

///////////////////////////////////

bool isBigEndian()
{
    const size_t i = 1;
    return ((char*)&i)[0] == 0;
}

///////////////////////////////////

std::string randStr(size_t length)
{
    static std::string charSet = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string str;
    str.resize(length);
    for(size_t i = 0; i < length; i++)
        str[i] = charSet[rand() % charSet.size()];
    return str;
}

///////////////////////////////////

size_t movingWindowMinDiff
(
    const unsigned char* lhs,
    size_t lhsWidth, size_t lhsHeight,
    const unsigned char* rhs,
    size_t rhsWidth, size_t rhsHeight,
    size_t rhsSum
)
{
    if(lhsWidth > rhsWidth || lhsHeight > rhsHeight)
        THROW_RUNTIME_ERROR("LHS size cannot be higher than RHS size.");

    const size_t NUM_ITERATIONS_X = rhsWidth - lhsWidth + 1;
    const size_t NUM_ITERATIONS_Y = rhsHeight - lhsHeight + 1;
    const size_t LHS_ROW_SIZE = lhsWidth;
    const size_t RHS_ROW_SIZE = rhsWidth;
    const size_t LHS_SIZE = lhsHeight * LHS_ROW_SIZE;
    const size_t RHS_SIZE = rhsHeight * RHS_ROW_SIZE;
    size_t minDiff = -1;

    if(rhsSum == -1)
    {
        rhsSum = 0;
        for(size_t i = 0; i < RHS_SIZE; i++)
        {
            rhsSum += rhs[i];
        }
    }

    for(size_t x = 0; x < NUM_ITERATIONS_X; x++)
        for(size_t y = 0; y < NUM_ITERATIONS_Y; y++)
        {
            size_t diff = 0;
            size_t currentRhsSum = rhsSum;
            for(size_t lhsIndex = 0, rhsIndex = x + y * RHS_ROW_SIZE;
                lhsIndex < LHS_SIZE && rhsIndex < RHS_SIZE;
                lhsIndex += LHS_ROW_SIZE, rhsIndex += RHS_ROW_SIZE)
            {
                for(size_t i = 0; i < LHS_ROW_SIZE; i++)
                {
                    currentRhsSum -= rhs[rhsIndex + i];
                    int d = (int)lhs[lhsIndex + i] - (int)rhs[rhsIndex + i];
                    diff += d < 0 ? -d : d;
                }
            }

            diff += currentRhsSum;
            if(diff == 0)
                return 0;

            if(diff < minDiff)
                minDiff = diff;
        }

    return minDiff;
}

///////////////////////////////////

size_t computeCenter(const unsigned char* b, size_t width, size_t height, size_t* sumOut)
{
    size_t sum = 0;
    size_t indexSum = 0;
    for(size_t i = 0; i < width * height; i++)
    {
        sum += b[i];
        indexSum += b[i] * (i + 1);
    }

    if(sumOut)
        *sumOut = sum;

    if(sum > 0)
        return indexSum / sum;
    return width / 2 + width * (height / 2);
}

///////////////////////////////////

size_t centerDiff
(
    const unsigned char* lhs,
    size_t lhsWidth, size_t lhsHeight,
    size_t lhsCenter,
    const unsigned char* rhs,
    size_t rhsWidth, size_t rhsHeight,
    size_t rhsCenter,
    size_t rhsSum
)
{
    if(lhsWidth > rhsWidth || lhsHeight > rhsHeight)
        THROW_RUNTIME_ERROR("LHS size cannot be higher than RHS size.");

    const size_t LHS_ROW_SIZE = lhsWidth;
    const size_t RHS_ROW_SIZE = rhsWidth;
    const size_t LHS_SIZE = lhsHeight * LHS_ROW_SIZE;
    const size_t RHS_SIZE = rhsHeight * RHS_ROW_SIZE;
    if(rhsSum == -1)
    {
        rhsSum = 0;
        for(size_t i = 0; i < RHS_SIZE; i++)
        {
            rhsSum += rhs[i];
        }
    }

    size_t diff = 0;
    for(size_t lhsIndex = 0, rhsIndex = rhsCenter - (lhsCenter / lhsWidth) * RHS_ROW_SIZE - lhsCenter % lhsWidth;
        lhsIndex < LHS_SIZE && rhsIndex < RHS_SIZE;
        lhsIndex += LHS_ROW_SIZE, rhsIndex += RHS_ROW_SIZE)
    {
        for(size_t i = 0; i < LHS_ROW_SIZE; i++)
        {
            rhsSum -= rhs[rhsIndex + i];
            int d = (int)lhs[lhsIndex + i] - (int)rhs[rhsIndex + i];
            diff += d < 0 ? -d : d;
        }
    }

    diff += rhsSum;
    return diff;
}

///////////////////////////////////

void worldToScreenCoords
(
    float worldX, float worldY,
    const Matrix<float, 4, 4>& transform,
    float halfScreenWidth, float halfScreenHeight,
    float& screenX, float& screenY
)
{
    const Matrix<float, 4, 1> WORLD_POS({{worldX}, {worldY}, {0.f}, {1.f}});
    Matrix<float, 4, 1> ndcPos = transform.transposedMul(WORLD_POS);
    for(size_t i = 0; i < 4; i++)
        ndcPos[i][0] /= ndcPos[3][0];

    screenX = halfScreenWidth * (ndcPos[0][0] + 1.f);
    screenY = halfScreenHeight * (1.f - ndcPos[1][0]);
}

bool isNumeric(std::string str)
{
    if(str.empty())
        return false;

    size_t decimalIndex = str.find('.');
    if(decimalIndex != str.npos)
    {
        for(size_t i = decimalIndex + 1; i < str.size(); i++)
            if(!isNumeric(str[i]))
                return false;
    }

    str = str.substr(0, decimalIndex);

    for(size_t i = 0; i < str.size(); i++)
    {
        const unsigned char& c = str[i];
        if(!isNumeric(c))
        {
            size_t iReverse = str.size() - i;
            if(iReverse % 4 != 0)
                return false;

            if(c != ',')
                return false;
        }
    }

    return true;
}

int strToInt(std::string str)
{
    str.erase(std::remove(str.begin(), str.end(), ','), str.end());
    std::stringstream sstream(str);
    int i;
    sstream >> i;
    return i;
}

float strToFloat(std::string str)
{
    str.erase(std::remove(str.begin(), str.end(), ','), str.end());
    std::stringstream sstream(str);
    float f;
    sstream >> f;
    return f;
}

bool isNumeric(unsigned char c)
{
    return c >= '0' && c <= '9';
}

bool isUppercase(unsigned char c)
{
    return c >= 'A' && c <= 'Z';
}

bool isLowercase(unsigned char c)
{
    return c >= 'a' && c <= 'z';
}

bool isLetter(unsigned char c)
{
    return isLowercase(c) || isUppercase(c);
}

unsigned char toLower(unsigned char c)
{
    if(isUppercase(c))
        return c + 32;

    return c;
}

std::string toLower(const std::string& str)
{
    std::string lowerCase;
    lowerCase.resize(str.size());
    for(size_t i = 0; i < str.size(); i++)
    {
        lowerCase[i] = toLower(str[i]);
    }

    return lowerCase;
}


std::vector<unsigned char> rgbaAlphaUnion(const unsigned char* lPixels, const unsigned char* rPixels, size_t width, size_t height)
{
    size_t size = width * height * BYTES_PER_PIXEL_RGBA;
    std::vector<unsigned char> alphaUnion(size);
    for(size_t i = 3; i < size; i += BYTES_PER_PIXEL_RGBA)
        alphaUnion[i] = lPixels[i] | rPixels[i];

    return alphaUnion;
}

std::vector<std::vector<size_t>> generateCommutativeCombinations(size_t numNumbers, size_t combinationSize) // From rosettacode.org
{
    std::string bitmask(combinationSize, 1);
    bitmask.resize(numNumbers, 0);

    std::vector<std::vector<size_t>> combos;
    do
    {
        combos.emplace_back();
        auto& c = combos.back();
        for(size_t i = 0; i < numNumbers; ++i)
            if(bitmask[i])
                c.push_back(i);
    } while(std::prev_permutation(bitmask.begin(), bitmask.end()));

    return combos;
}

std::vector<std::vector<size_t>> generateCommutativeCombinations(size_t numNumbers)
{
    std::vector<std::vector<size_t>> combos;
    for(size_t i = 1; i <= numNumbers; i++)
        for(const std::vector<size_t>& combo : generateCommutativeCombinations(numNumbers, i))
            combos.push_back(combo);

    return combos;
}

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<int> splitPoints;
    splitPoints.push_back(-1);
    for(int i = 0; i < str.size(); i++)
    {
        if(str[i] == delimiter)
        {
            splitPoints.push_back(i);
        }
    }
    splitPoints.push_back(str.size());

    std::vector<std::string> parts;
    parts.reserve(splitPoints.size() - 1);
    for(size_t i = 0; i + 1 < splitPoints.size(); i++)
    {
        if(splitPoints[i] + 1 < splitPoints[i + 1])
        {
            parts.emplace_back(&str[splitPoints[i] + 1], &str[splitPoints[i + 1]]);
        }
    }
    return parts;
}

}
}
