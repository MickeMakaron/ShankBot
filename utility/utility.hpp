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
#ifndef SB_UTILITY_HPP
#define SB_UTILITY_HPP


///////////////////////////////////
// STD C++
#include <cstring>
#include <string>
#include <sstream>
#include <list>
#include <istream>
#include <vector>
#include <array>
///////////////////////////////////

///////////////////////////////////
// Internal ShankBot headers
#include "utility/config.hpp"
#include "utility/Buffer.hpp"
namespace sb
{
namespace utility
{
    template<typename T, size_t N, size_t M>
    class Matrix;
}
}
///////////////////////////////////

namespace sb
{
namespace utility
{
    SHANK_BOT_UTILITY_DECLSPEC void writePixelsToFile(std::string fileName, const unsigned char* pixels, size_t width, size_t height);
    SHANK_BOT_UTILITY_DECLSPEC unsigned char* rgbaToRgb(const unsigned char* rgba, size_t width, size_t height);
    SHANK_BOT_UTILITY_DECLSPEC unsigned char* bgraToRgba(const unsigned char* bgra, size_t width, size_t height);
    SHANK_BOT_UTILITY_DECLSPEC unsigned char* rgbaToGrayscale(const unsigned char* rgba, size_t width, size_t height);
    SHANK_BOT_UTILITY_DECLSPEC unsigned char* grayscaleToRgb(const unsigned char* grayscale, size_t width, size_t height);
    SHANK_BOT_UTILITY_DECLSPEC bool isNumeric(std::string str);
    SHANK_BOT_UTILITY_DECLSPEC int strToInt(std::string str);
    SHANK_BOT_UTILITY_DECLSPEC float strToFloat(std::string str);
    SHANK_BOT_UTILITY_DECLSPEC bool isNumeric(unsigned char c);
    SHANK_BOT_UTILITY_DECLSPEC bool isUppercase(unsigned char c);
    SHANK_BOT_UTILITY_DECLSPEC bool isLowercase(unsigned char c);
    SHANK_BOT_UTILITY_DECLSPEC bool isLetter(unsigned char c);
    SHANK_BOT_UTILITY_DECLSPEC unsigned char toLower(unsigned char c);
    SHANK_BOT_UTILITY_DECLSPEC std::string toLower(const std::string& str);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<std::string> split(const std::string& str, char delimiter);
    SHANK_BOT_UTILITY_DECLSPEC int round(float f);


//    void testLoadImages(std::string directory, std::list<PngImage>& images);
    SHANK_BOT_UTILITY_DECLSPEC void readPpm(std::string filePath, unsigned char** data, size_t* width, size_t* height);

    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> bgraToColorTreeSprite(const std::vector<unsigned char>& bgra, size_t width, size_t height, bool* isBlank = nullptr);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> rgbaToColorTreeSprite(const std::vector<unsigned char>& rgba, size_t width, size_t height, bool* isBlank = nullptr);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> rgbaToTransparencyTreeSprite(const std::vector<unsigned char>& rgba, size_t width, size_t height, bool* isBlank = nullptr);

    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> bgraToColorTreeSprite(const unsigned char* bgra, size_t width, size_t height, bool* isBlank = nullptr);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> rgbaToColorTreeSprite(const unsigned char* rgba, size_t width, size_t height, bool* isBlank = nullptr);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> rgbaToTransparencyTreeSprite(const unsigned char* rgba, size_t width, size_t height, bool* isBlank = nullptr);

    SHANK_BOT_UTILITY_DECLSPEC std::string randStr(size_t length);

    template<typename T>
    void stringifyHelper(std::ostream& stream, const T& t)
    {
        stream << t;
    }

    template<typename TCurrent, typename... TRemaining>
    void stringifyHelper(std::ostream& stream, const TCurrent& currArg, const TRemaining&... remainingArgs)
    {
        stringifyHelper(stream, currArg);
        stringifyHelper(stream, remainingArgs...);
    }

    template<typename... T>
    typename std::enable_if<sizeof...(T) != 0, std::string>::type stringify(const T&... args)
    {
        std::stringstream sstream;
        stringifyHelper(sstream, args...);
        return sstream.str();
    }

    template<typename T>
    auto tryStringify(std::stringstream& sstream, const T& t) -> decltype(sstream << t, void())
    {
        sstream << t;
    }

    SHANK_BOT_UTILITY_DECLSPEC auto tryStringify(std::stringstream& sstream, ...) -> decltype(void());

    template<typename T>
    std::string tryStringify(const T& t)
    {
        std::stringstream sstream;
        tryStringify(sstream, t);
        return sstream.str();
    }

    template<typename ExceptionType>
    void throwException(size_t line, std::string file, std::string function, std::string message)
    {
        std::stringstream sstream;
        sstream << std::endl
                << "File: " << file << std::endl
                << "Function (line nr): " << function << " (" << line << ")" << std::endl
                << "Message: " << message << std::endl;
        throw ExceptionType(sstream.str());
    }

    template<typename A, typename B>
    void throwExpect(std::string aStr,
                     std::string binOpStr,
                     std::string bStr,
                     const A& a,
                     const B& b,
                     size_t line,
                     std::string file,
                     std::string function)
    {
        std::stringstream sstream;
        sstream << std::endl
                << "Expected \"" + aStr + " " + binOpStr + " " + bStr + "\" to evaluate to true." << std::endl
                << "Got:     \"" + tryStringify(a) + " " + binOpStr + " " + tryStringify(b) + "\"." << std::endl;

        sb::utility::throwException<std::runtime_error>(line, file, function, sstream.str());
    }

    #define SB_THROW(...) sb::utility::throwException<std::runtime_error>(__LINE__, __FILE__, __PRETTY_FUNCTION__, sb::utility::stringify(__VA_ARGS__));

    #define SB_EXPECT(a, binOp, b) if(!((a) binOp (b))) sb::utility::throwExpect(#a, #binOp, #b, a, b, __LINE__, __FILE__, __PRETTY_FUNCTION__);
    #define SB_EXPECT_TRUE(a) SB_EXPECT(a, ==, true)
    #define SB_EXPECT_FALSE(a) SB_EXPECT(a, ==, false)

    template<typename T>
    void readStream(T& t, std::istream& stream, size_t n = 1)
    {
        stream.read((char*)&t, sizeof(T) * n);
    }

    template<typename T>
    void writeStream(const T& t, std::ostream& stream, size_t n = 1)
    {
        stream.write((char*)&t, sizeof(T) * n);
    }

    template<typename T>
    void readStream(T& t, Buffer& b, size_t n = 1)
    {
        const size_t size = sizeof(T) * n;
        memcpy((char*)&t, &b.data[b.curr], size);
        b.curr += size;
    }

    template<typename T>
    void readStream(T& t, const char*& stream, size_t n = 1)
    {
        const size_t size = sizeof(T) * n;
        memcpy((char*)&t, stream, size);
        stream += size;
    }

    template<typename T>
    bool readStreamSafe(T& t, const char*& stream, const char* end, size_t n = 1)
    {
        const size_t size = sizeof(T) * n;
        if(stream + size > end)
            return false;
        memcpy((char*)&t, stream, size);
        stream += size;

        return true;
    }

    template<typename T>
    void readStreamSafe(T& t, std::istream& stream, size_t n = 1)
    {
        if(!stream.good())
        {
            SB_THROW("Failed to read file stream.");
        }
        stream.read((char*)&t, sizeof(T) * n);
    }

    template<typename T>
    void writeStream(const T& t, char*& stream, size_t n = 1)
    {
        const size_t size = sizeof(T) * n;
        memcpy(stream, (char*)&t, size);
        stream += size;
    }

    template<typename T>
    void writeStream(const T& t, std::vector<char>& stream, size_t n = 1)
    {
        const size_t size = sizeof(T) * n;
        stream.insert(stream.end(), (char*)&t, (char*)&t + size);
    }


    SHANK_BOT_UTILITY_DECLSPEC std::array<float, 4> mulTransposedMatVec(const float lhs[4][4], const float rhs[4]);

    SHANK_BOT_UTILITY_DECLSPEC size_t readTibiaSizeIndicator(std::istream& stream);
    SHANK_BOT_UTILITY_DECLSPEC size_t readTibiaSizeIndicator(Buffer& stream);

    SHANK_BOT_UTILITY_DECLSPEC std::string getDateTime();

    SHANK_BOT_UTILITY_DECLSPEC std::vector<size_t> packBytes(const std::vector<unsigned char>& bytes);
    constexpr int packBytes(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4)
    {
        return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
    }

   SHANK_BOT_UTILITY_DECLSPEC  bool isBigEndian();


    template<typename T>
    void compressBigEndianMultiByteValue(const T& val, std::vector<unsigned char>& out)
    {
        unsigned char* byte = (unsigned char*)&val;
        int j = sizeof(T);
        do
        {
            out.push_back(byte[j--]);
        } while(byte[j] != 0 && j >= 0);
    }

    template<typename T>
    void compressSmallEndianMultiByteValue(const T& val, std::vector<unsigned char>& out)
    {
        unsigned char* byte = (unsigned char*)&val;
        unsigned int j = 0;
        do
        {
            out.push_back(byte[j++]);
        } while(byte[j] != 0 && j < sizeof(T));
    }


    SHANK_BOT_UTILITY_DECLSPEC void worldToScreenCoords
    (
        float worldX, float worldY,
        const Matrix<float, 4, 4>& transform,
        float halfScreenWidth, float halfScreenHeight,
        float& screenX, float& screenY
    );

    SHANK_BOT_UTILITY_DECLSPEC size_t movingWindowMinDiff
    (
        const unsigned char* lhs,
        size_t lhsWidth, size_t lhsHeight,
        const unsigned char* rhs,
        size_t rhsWidth, size_t rhsHeight,
        size_t rhsSum = -1
    );
    SHANK_BOT_UTILITY_DECLSPEC size_t centerDiff
    (
        const unsigned char* lhs,
        size_t lhsWidth, size_t lhsHeight,
        size_t lhsCenter,
        const unsigned char* rhs,
        size_t rhsWidth, size_t rhsHeight,
        size_t rhsCenter,
        size_t rhsSum = -1
    );


    SHANK_BOT_UTILITY_DECLSPEC size_t computeCenter(const unsigned char* b, size_t width, size_t height, size_t* sumOut = nullptr);

    SHANK_BOT_UTILITY_DECLSPEC std::vector<unsigned char> rgbaAlphaUnion(const unsigned char* lPixels, const unsigned char* rPixels, size_t width, size_t height);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<std::vector<size_t>> generateCommutativeCombinations(size_t numNumbers, size_t combinationSize);
    SHANK_BOT_UTILITY_DECLSPEC std::vector<std::vector<size_t>> generateCommutativeCombinations(size_t numNumbers);


    template<typename T1, typename T2>
    bool isCoordinateLessThan(const T1& x1, const T1& y1, const T2& x2, const T2& y2)
    {
        if(y1 < y2)
            return true;

        if(y1 > y2)
            return false;

        return x1 < x2;
    }

    const unsigned char BYTES_PER_PIXEL_RGBA = 4;
    const unsigned char BYTES_PER_PIXEL_BGRA = BYTES_PER_PIXEL_RGBA;
    const unsigned char BYTES_PER_PIXEL_RGB = 3;
    const unsigned char BYTES_PER_PIXEL_ALPHA = 1;
}
}

#endif // SB_UTILITY_HPP
