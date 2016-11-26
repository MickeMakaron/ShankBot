// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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


#include "ShankBot.hpp"
using namespace GraphicsLayer;

//#include "CatalogContent.hpp"
//#include "SpriteReader.hpp"
//
//#include "GraphicsResourceReader.hpp"

#include "QtGui/QGuiApplication"
#include "QtGui/QPainter"
#include "QtGui/QImage"
#include "QtGui/QSurfaceFormat"
#include "QtGui/QOffscreenSurface"
#include "QtGui/QStaticText"
#include <ctime>

#include "QtGui/QOpenGLContext"
#include "QtOpenGL/QGLFramebufferObject"
#include "QtGui/QFont"
//#include "Constants.hpp"
//#include "utility.hpp"
//
//#include <ctime>
#include <iostream>
//#include <windows.h>
#include <cassert>
//#include "AppearancesReader.hpp"
//
//#include "FunctionDetour.hpp"
//FunctionDetour* subTexImgDetour = nullptr;
//#include <sstream>
//size_t subCount = 0;
//#include "GL/gl.h"
//char currentCharacter = 0;
//float currentPointSize = 0;
//bool isBold = false;
//
//struct Glyph : public TextBuilder::Glyph
//{
//    char character = 0;
//    float pointSize = 0;
//    bool isBold = false;
//    size_t diff = -1;
//};
//
//std::list<Glyph> qtGlyphs;
//
//size_t glyphDiff(const Glyph& lhs, const Glyph& rhs);
//
//void WINAPI subTexImg
//(
//    GLenum target,
//  	GLint level,
//  	GLint xoffset,
//  	GLint yoffset,
//  	GLsizei width,
//  	GLsizei height,
//  	GLenum format,
//  	GLenum type,
//  	const GLvoid* pixels
//)
//{
//    assert(format == GL_ALPHA);
//    assert(type == GL_UNSIGNED_BYTE);
//    Glyph glyph;
//    glyph.width = width;
//    glyph.height = height;
//    glyph.character = currentCharacter;
//    glyph.pointSize = currentPointSize;
//    glyph.isBold = isBold;
//    size_t paddedWidth = width;
//    size_t remainder = paddedWidth % 4;
//    if(remainder != 0)
//        paddedWidth += 4 - remainder;
//
//    size_t paddedRowSize = paddedWidth * glyph.bytesPerPixel;
//    size_t rowSize = glyph.width * glyph.bytesPerPixel;
//    size_t size = rowSize * glyph.height;
//    glyph.data.resize(size);
//    for(size_t y = 0; y < glyph.height; y++)
//    {
//        memcpy(&glyph.data[y * rowSize], (char*)pixels + y * paddedRowSize, rowSize);
//    }
////    glyph.data.assign((unsigned char*)pixels, (unsigned char*)pixels + GLYPH_SIZE);
////    if(currentCharacter == 't' && currentPointSize > 7.89f && currentPointSize < 7.91f && isBold)
////    {
////        QImage img(QString("glyphCompDebug/1_10_1_60.tif"));
////        Glyph unknownT;
////        unknownT.width = img.width();
////        unknownT.height = img.height();
////        assert(img.depth() / 8 == unknownT.bytesPerPixel);
////        rowSize = unknownT.width * unknownT.bytesPerPixel;
////        size = rowSize * unknownT.height;
////        unknownT.data.resize(size);
////        for(size_t y = 0; y < unknownT.height; y++)
////        {
////            const unsigned char* row = img.constScanLine(y);
////            memcpy(&unknownT.data[y * rowSize], row, rowSize);
////        }
////
////        std::cout << glyphDiff(glyph, unknownT) << std::endl;
////        assert(false);
////    }
//    qtGlyphs.push_back(glyph);
//
//    if((currentCharacter >= '0' && currentCharacter <= '9') || (currentCharacter >= 'A' || currentCharacter <= 'Z') || (currentCharacter >= 'a' && currentCharacter <= 'z'))
//    {
////        std::cout << subCount << " subtex: " << format << " " << type << " " << width << "x" << height << std::endl;
////    //    return;
////        QImage img((unsigned char*)pixels, width, height, QImage::Format_Grayscale8);
////        std::stringstream sstream;
////    //    sstream << "qtSubTex/" << subCount++ << ".bmp";
////        sstream << "qtSubTex/" << currentCharacter << "_" << currentPointSize << "_" << (isBold ? "b" : "n") << ".bmp";
////
////        img.save(QString::fromStdString(sstream.str()));
//    }
//    subTexImgDetour->disable();
//    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
//        subTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, type, pixels);
//    subTexImgDetour->enable();
//}
//#include "FontSample.hpp"
//
//void createGlyphSamples()
//{
//
//    int argc = 0;
//    QGuiApplication app(argc, nullptr);
//    QSurfaceFormat surfaceFormat;
//    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
//
//    QOpenGLContext openGLContext;
//    openGLContext.create();
//    openGLContext.setFormat(surfaceFormat);
//
//
//    QOffscreenSurface surface;
//    surface.create();
//    surface.setFormat(openGLContext.format());
//    openGLContext.makeCurrent(&surface);
//
//
//    HINSTANCE opengl32 = GetModuleHandle("OPENGL32.DLL");
//    assert(opengl32 != NULL);
//    void* texSubFunc = (void*)GetProcAddress(opengl32, "glTexSubImage2D");
//    assert(texSubFunc != NULL);
//    subTexImgDetour = new FunctionDetour((void*)texSubFunc, (void*)subTexImg);
//
////    void APIENTRY (*pixelStorei)(GLenum, GLint) = (void APIENTRY (*)(GLenum, GLint))GetProcAddress(opengl32, "glPixelStorei");
////    pixelStorei(GL_PACK_ALIGNMENT, 1);
//
//    QGLFramebufferObject framebuffer(1, 1);
//    QPainter painter(&framebuffer);
//
//
//    QFont font;
//    font.setFamily("Verdana");
//    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);
//
//    for(float pointSize = 6.f; pointSize < 10.f; pointSize += 0.1f)
//    {
//        currentPointSize = pointSize;
//        font.setPointSizeF(currentPointSize);
//
//        font.setBold(true);
//        isBold = true;
//
//        painter.setFont(font);
//        for(char c = 0x20; c <= 0x7e; c++)
//        {
//            currentCharacter = c;
//            painter.drawText(0, 0, QString(currentCharacter));
//        }
//
//        font.setBold(false);
//        isBold = false;
//
//        painter.setFont(font);
//        for(char c = 0x20; c <= 0x7e; c++)
//        {
//            currentCharacter = c;
//            painter.drawText(0, 0, QString(currentCharacter));
//        }
//    }
//}
//
//
//
//size_t glyphDiff(const FontSample::Glyph& lhs, const Glyph& rhs)
//{
//    if(lhs.width > rhs.width || lhs.height > rhs.height)
//        THROW_RUNTIME_ERROR("LHS size cannot be higher than RHS size.");
//
//    const size_t NUM_ITERATIONS_X = rhs.width - lhs.width;
//    const size_t NUM_ITERATIONS_Y = rhs.height - lhs.height;
//    const size_t LHS_ROW_SIZE = lhs.width * lhs.bytesPerPixel;
//    const size_t RHS_ROW_SIZE = rhs.width * rhs.bytesPerPixel;
//    const size_t LHS_SIZE = lhs.height * LHS_ROW_SIZE;
//    const size_t RHS_SIZE = rhs.height * RHS_ROW_SIZE;
//    size_t minDiff = -1;
//    size_t rhsSum = 0;
//    for(size_t i = 0; i < RHS_SIZE; i++)
//    {
//        rhsSum += rhs.data[i];
//    }
//
//    for(size_t x = 0; x < NUM_ITERATIONS_X; x++)
//        for(size_t y = 0; y < NUM_ITERATIONS_Y; y++)
//        {
//            size_t diff = 0;
//            size_t currentRhsSum = rhsSum;
//            for(size_t lhsIndex = 0, rhsIndex = x + y * RHS_ROW_SIZE;
//                lhsIndex < LHS_SIZE && rhsIndex < RHS_SIZE;
//                lhsIndex += LHS_ROW_SIZE, rhsIndex += RHS_ROW_SIZE)
//            {
//                for(size_t i = 0; i < LHS_ROW_SIZE; i++)
//                {
//                    currentRhsSum -= rhs.data[rhsIndex + i];
//                    diff += std::abs((int)lhs.data[lhsIndex + i] - (int)rhs.data[rhsIndex + i]);
//                }
//            }
//
//            diff += currentRhsSum;
//            if(diff == 0)
//                return 0;
//
//            if(diff < minDiff)
//                minDiff = diff;
//        }
//
//    return minDiff;
//}
//
//#include "file.hpp"
//
//
//size_t movingWindowMinDiff(const unsigned char* lhs, size_t lhsWidth, size_t lhsHeight, const unsigned char* rhs, size_t rhsWidth, size_t rhsHeight, size_t rhsSum = -1)
//{
//    if(lhsWidth > rhsWidth || lhsHeight > rhsHeight)
//        THROW_RUNTIME_ERROR("LHS size cannot be higher than RHS size.");
//
//    const size_t NUM_ITERATIONS_X = rhsWidth - lhsWidth + 1;
//    const size_t NUM_ITERATIONS_Y = rhsHeight - lhsHeight + 1;
//    const size_t LHS_ROW_SIZE = lhsWidth;
//    const size_t RHS_ROW_SIZE = rhsWidth;
//    const size_t LHS_SIZE = lhsHeight * LHS_ROW_SIZE;
//    const size_t RHS_SIZE = rhsHeight * RHS_ROW_SIZE;
//    size_t minDiff = -1;
//
//    if(rhsSum == -1)
//    {
//        rhsSum = 0;
//        for(size_t i = 0; i < RHS_SIZE; i++)
//        {
//            rhsSum += rhs[i];
//        }
//    }
//
//    for(size_t x = 0; x < NUM_ITERATIONS_X; x++)
//        for(size_t y = 0; y < NUM_ITERATIONS_Y; y++)
//        {
//            size_t diff = 0;
//            size_t currentRhsSum = rhsSum;
//            for(size_t lhsIndex = 0, rhsIndex = x + y * RHS_ROW_SIZE;
//                lhsIndex < LHS_SIZE && rhsIndex < RHS_SIZE;
//                lhsIndex += LHS_ROW_SIZE, rhsIndex += RHS_ROW_SIZE)
//            {
//                for(size_t i = 0; i < LHS_ROW_SIZE; i++)
//                {
//                    assert(currentRhsSum >= rhs[rhsIndex + i]);
//                    currentRhsSum -= rhs[rhsIndex + i];
//                    int d = (int)lhs[lhsIndex + i] - (int)rhs[rhsIndex + i];
//                    diff += d < 0 ? -d : d;
//                }
//            }
//
//            diff += currentRhsSum;
//            if(diff == 0)
//                return 0;
//
//            if(diff < minDiff)
//                minDiff = diff;
//        }
//
//    return minDiff;
//}
//
//
//FontSample* fontSample = nullptr;
//unsigned char getChar(const unsigned char* pixels, unsigned short width, unsigned short height)
//{
//    size_t sum = 0;
//    for(size_t i = 0; i < width * height; i++)
//        sum += pixels[i];
//
//    size_t minDiff = -1;
//    const FontSample::Glyph* mostSimilarGlyph = nullptr;
//    for(const FontSample::Glyph& lhs : fontSample->getGlyphs())
//    {
//        size_t diff;
//        if(lhs.width <= width && lhs.height <= height)
//            diff = movingWindowMinDiff(lhs.data.data(), lhs.width, lhs.height, pixels, width, height, sum);
//        else if(width <= lhs.width && height <= lhs.height)
//            diff = movingWindowMinDiff(pixels, width, height, lhs.data.data(), lhs.width, lhs.height);
//
//        if(diff < minDiff)
//        {
//            if(minDiff == 0)
//                return lhs.character;
//
//            minDiff = diff;
//            mostSimilarGlyph = &lhs;
//        }
//    }
//
//    assert(mostSimilarGlyph);
//    return mostSimilarGlyph->character;
//}

#include "FunctionDetour.hpp"
FunctionDetour* createFileDetour = nullptr;

HANDLE WINAPI createFile
(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
)
{
    std::cout << "Hello!" << std::endl;
HANDLE retVal = ((WINBASEAPI HANDLE (WINAPI *)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE))createFileDetour->getFunction())
(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

}
#include "utility.hpp"
#include <fstream>
#include "GraphicsResourceReader.hpp"

///////////////////////////////////
// Qt
#include "QtGui/QGuiApplication"
#include "QtGui/QSurfaceFormat"
#include "QtGui/QOpenGLContext"
#include "QtGui/QOffscreenSurface"
#include "QtOpenGL/QGLFramebufferObject"
#include "QtGui/QOpenGLFramebufferObject"
#include "QtGui/QOpenGLPaintDevice"
#include "QtWidgets/QApplication"
#include "QtGui/QWindow"
#include "QtGui/QPainter"
#include "QtGui/QOpenGLWindow"
#include "QtWidgets/QOpenGLWidget"
#include "QtGui/QOpenGLFunctions"
#include "QtQuick/QQuickPaintedItem"
#include "QtQuick/QQuickView"
#include "QtQml/QQmlEngine"
#include "QtQml/QQmlFileSelector"
///////////////////////////////////

class Painter : public QOpenGLWidget
{
//    Q_OBJECT

    protected:
        void paintEvent(QPaintEvent* event) override;
};

bool doPaint = false;
void Painter::paintEvent(QPaintEvent* event)
{
    if(!doPaint)
        return;
//    QPainter painter(this);
//    QFont font;
//    font.setFamily(QString::fromStdString("Verdana"));
//    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);
//
//    QPen pen;
//    pen.setWidth(2);
//    pen.setColor(Qt::red);
//    painter.setFont(font);
//    painter.setPen(pen);
//
//    QPainterPath path;
//    path.addText(50, 50, font, QString("ABOOOOOOO"));
//    painter.drawPath(path);
//

//    QLinearGradient myGradient;

    QFont myFont;
    myFont.setPointSize(20.f);
    QPointF baseline(50, 50);

    QPainterPath myPath;
    myPath.addText(baseline, myFont, tr("Qt"));

    QPainter painter(this);
    painter.setFont(myFont);
    painter.setPen(Qt::red);
    painter.drawText(baseline, tr("Qt"));
    painter.setPen(Qt::green);
    painter.drawPath(myPath);
}

#include <sstream>
namespace Merp
{
    size_t count = 0;
    FunctionDetour* subTexImgDetour = nullptr;
void APIENTRY subTexImg
(
    GLenum target,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid* pixels
)
{
    assert(format == GL_ALPHA);
    assert(type == GL_UNSIGNED_BYTE);

////        if((currentCharacter >= '0' && currentCharacter <= '9') || (currentCharacter >= 'A' || currentCharacter <= 'Z') || (currentCharacter >= 'a' && currentCharacter <= 'z'))
//    {
////            std::cout << subCount << " subtex: " << format << " " << type << " " << width << "x" << height << std::endl;
//    //    return;
//        QImage img((unsigned char*)pixels, width, height, QImage::Format_Grayscale8);
//        std::stringstream sstream;
//    //    sstream << "qtSubTex/" << subCount++ << ".bmp";
//        sstream << "qtSubTexOutline/" <<  "qt" << count++ << ".bmp";
//
//        img.save(QString::fromStdString(sstream.str()));
//    }

    std::cout << "Subtex " << std::endl;

    subTexImgDetour->disable();
    ((void APIENTRY (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
        subTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, type, pixels);
    subTexImgDetour->enable();
}

}

//
//unsigned char getChar(const unsigned char* pixels, unsigned short width, unsigned short height, const std::list<FontSample::Glyph>& glyphs)
//{
//    size_t sum = 0;
//    for(size_t i = 0; i < width * height; i++)
//        sum += pixels[i];
//
//    size_t minDiff = -1;
//    const FontSample::Glyph* mostSimilarGlyph = nullptr;
//    for(const FontSample::Glyph& lhs : glyphs)
//    {
//        size_t diff = -1;
//        if(lhs.width <= width && lhs.height <= height)
//            diff = movingWindowMinDiff(lhs.data.data(), lhs.width, lhs.height, pixels, width, height, sum);
//        else if(width <= lhs.width && height <= lhs.height)
//            diff = movingWindowMinDiff(pixels, width, height, lhs.data.data(), lhs.width, lhs.height);
//
//        if(diff < minDiff)
//        {
//            if(minDiff == 0)
//                return lhs.character;
//
//            minDiff = diff;
//            mostSimilarGlyph = &lhs;
//        }
//    }
//
//    assert(mostSimilarGlyph);
//    return mostSimilarGlyph->character;
//}
//
//
//FrameParser::TileNumber getTileNumber(const unsigned char* pixels, unsigned short width, unsigned short height, SharedMemoryProtocol::PixelData::PixelFormat format, const std::list<FontSample::Glyph>& glyphs)
//{
//    static const size_t MAX_GLYPH_WIDTH = 9;
//    static const size_t MIN_GLYPH_WIDTH = 8;
//    static const size_t MAX_GLYPH_HEIGHT = 11;
//    static const size_t MIN_GLYPH_HEIGHT = 10;
//
//    if(width < MIN_GLYPH_WIDTH || height < MIN_GLYPH_HEIGHT || height > MAX_GLYPH_HEIGHT)
//        return FrameParser::TileNumber();
//
//    typedef SharedMemoryProtocol::PixelData::PixelFormat Format;
//    assert(format == Format::RGBA);
//
//    size_t numPixelsCounted = 0;
//    size_t rSum = 0;
//    size_t gSum = 0;
//    size_t bSum = 0;
//    for(size_t i = 0; i < width * height * Constants::BYTES_PER_PIXEL_RGBA; i += Constants::BYTES_PER_PIXEL_RGBA)
//    {
//        if(pixels[i + 3] == 255)
//        {
//            unsigned char r = pixels[i];
//            unsigned char g = pixels[i + 1];
//            unsigned char b = pixels[i + 2];
//            static const unsigned char PIXEL_SUM_THRESHOLD = 10;
//            if(r + g + b >= PIXEL_SUM_THRESHOLD)
//            {
//                rSum += r;
//                gSum += g;
//                bSum += b;
//                numPixelsCounted++;
//            }
//        }
//    }
//    float sum = rSum + gSum + bSum;
//    float rPerc = float(rSum) / sum;
//    float gPerc = float(gSum) / sum;
//    float bPerc = float(bSum) / sum;
//
//    FrameParser::TileNumber tileNumber;
//    if(gPerc >= 0.95f)
//    {
//        tileNumber.type = FrameParser::TileNumber::Type::POISON_DAMAGE;
//    }
//    else if(rPerc >= 0.95f)
//    {
//        tileNumber.type = FrameParser::TileNumber::Type::PHYSICAL_DAMAGE;
//    }
//    else if(gPerc >= 0.38f && gPerc <= 0.40f && bPerc >= 0.38f && bPerc <= 0.40f)
//    {
//        tileNumber.type = FrameParser::TileNumber::Type::MAGIC_DAMAGE;
//    }
//    else if(rPerc >= 0.30f && rPerc <= 0.35f && gPerc >= 0.30f && gPerc <= 0.35f && bPerc >= 0.30f && bPerc <= 0.35f)
//    {
//        tileNumber.type = FrameParser::TileNumber::Type::XP_GAIN;
//    }
//    else if(rPerc >= 0.50f && rPerc <= 0.60f && gPerc >= 0.20f && gPerc <= 0.30f && bPerc >= 0.20f && bPerc <= 0.30f)
//    {
//        tileNumber.type = FrameParser::TileNumber::Type::HP_GAIN;
//    }
//    else if(rPerc >= 0.15f && rPerc <= 0.25f && gPerc >= 0.25f && gPerc <= 0.35f && bPerc >= 0.45f && bPerc <= 0.55f)
//    {
//        tileNumber.type = FrameParser::TileNumber::Type::MANA_GAIN;
//    }
//
//    if(tileNumber.type == FrameParser::TileNumber::Type::INVALID)
//        return FrameParser::TileNumber();
//
//    QImage img(pixels, width, height, QImage::Format_RGBA8888);
//    img = img.convertToFormat(QImage::Format_Grayscale8);
//    std::vector<unsigned char> grayPixels(width * height);
//    for(size_t i = 0; i < height; i++)
//        memcpy(&grayPixels[i * width], img.scanLine(i), width);
//
//    numPixelsCounted = 0;
//    sum = 0;
//    static const unsigned char PIXEL_GRAY_THRESHOLD = 10;
//    for(size_t i = 0; i < width * height; i++)
//    {
//        if(grayPixels[i] > PIXEL_GRAY_THRESHOLD)
//        {
//            sum += grayPixels[i];
//            numPixelsCounted++;
//        }
//    }
//
//    float averagePixelValue = float(sum) / float(numPixelsCounted);
//    if(!std::isfinite(averagePixelValue))
//        return FrameParser::TileNumber();
//
//    static const float DESIRED_AVERAGE = 175.f;
//    float modifier = DESIRED_AVERAGE / averagePixelValue;
//    if(!std::isfinite(modifier))
//        return FrameParser::TileNumber();
//
//    for(size_t i = 0; i < width * height; i++)
//        if(grayPixels[i] > PIXEL_GRAY_THRESHOLD)
//            grayPixels[i] = std::min(int(grayPixels[i] * modifier), 255);
//
//
//    if(width <= MAX_GLYPH_WIDTH)
//    {
//        unsigned char character = getChar(grayPixels.data(), width, height, glyphs);
//        if(!isNumeric(character))
//            return FrameParser::TileNumber();
//
//        tileNumber.number = character - '0';
//        return tileNumber;
//    }
//
//    std::list<unsigned int> digits;
//    for(size_t x = 0;;)
//    {
//        if(x >= width - 1)
//        {
//            assert(x == width - 1);
//            break;
//        }
//
//        size_t xRemaining = width - x;
//        size_t currWidth;
//        if(xRemaining >= MAX_GLYPH_WIDTH)
//        {
//            currWidth = MAX_GLYPH_WIDTH;
//        }
//        else
//        {
//            assert(xRemaining == MIN_GLYPH_WIDTH);
//            currWidth = MIN_GLYPH_WIDTH;
//        }
//        std::vector<unsigned char> window(height * currWidth);
//        for(size_t iSrc = x, iDest = 0; iSrc + currWidth <= height * width; iSrc += width, iDest += currWidth)
//            memcpy(&window[iDest], &grayPixels[iSrc], currWidth);
//
//        unsigned char character = getChar(window.data(), currWidth, height, glyphs);
//        if(!isNumeric(character))
//            return FrameParser::TileNumber();
//
//        unsigned int digit = character - '0';
//        digits.push_back(digit);
//
//        if(digit == 1)
//            x += MIN_GLYPH_WIDTH - 1;
//        else
//            x += MAX_GLYPH_WIDTH - 1;
//    }
//
//    unsigned int number = 0;
//    unsigned int magnitude = 1;
//    for(auto it = digits.rbegin(); it != digits.rend(); it++)
//    {
//        number += *it * magnitude;
//        magnitude *= 10;
//    }
//
//    tileNumber.number = number;
//    return tileNumber;
//}
//

//template<typename T>
//void stringifyHelper(std::ostream& stream, const T& t)
//{
//    stream << t;
//}
//
//template<typename TCurrent, typename... TRemaining>
//void stringifyHelper(std::ostream& stream, const TCurrent& currArg, const TRemaining&... remainingArgs)
//{
//    ::stringifyHelper(stream, currArg);
//    ::stringifyHelper(stream, remainingArgs...);
//}
//
//
//template<typename... T>
//std::string stringify(const T&... args)
//{
//    std::stringstream sstream;
//    ::stringifyHelper(sstream, args...);
//    return sstream.str();
//}
//
//template<>
//std::string stringify()
//{
//    return "";
//}

#include "SpriteReader.hpp"
#include "file.hpp"

#include "QtCore/QBuffer"

unsigned int fnv1a32(const unsigned char* data, size_t size)
{
    static const unsigned int FNV_PRIME_32 = 16777619;
    static const unsigned int OFFSET_BASIS_32 = 2166136261;
    unsigned int hash = OFFSET_BASIS_32;
    for(size_t i = 0; i < size; i++)
    {
        hash ^= data[i];
        hash *= FNV_PRIME_32;
    }

    return hash;
}

#include "OutfitAddonMerger.hpp"
#include "CombatSquareSample.hpp"

using namespace sb::utility;
void convertTileBufferDump(size_t number)
{
    std::string readPath = stringify("tibia/packages/Tibia/bin/tileBuffers/", number, ".ppm");
    std::string writePath = stringify("tibia/packages/Tibia/bin/tileBuffers/", number, ".png");
    std::ifstream tileBuffer(readPath, std::ios::binary);
    unsigned char* rgba = new unsigned char[2048 * 2048 * 4];
    tileBuffer.read((char*)rgba, 2048 * 2048 * 4);
    tileBuffer.close();
    QImage img(rgba, 2048, 2048, QImage::Format_RGBA8888);
    img.save(QString::fromStdString(writePath));
    delete[] rgba;
    return;
}


#include "DynamicLibrary.hpp"
#include "DynQString.hpp"
#include "DynQImage.hpp"
#include "DynQFile.hpp"
int main(int argc, char** argv)
{
//    DynamicLibrary qtLib;
//    qtLib.loadDynamicClasses
//    <
//        DynQString,
//        DynQImage,
//        DynQFile
//    >
//    ({
//        "C:/Users/Vendrii/Documents/programming/projects/ShankBot/graphics/tibia/packages/Tibia/bin/Qt5Core.dll",
//        "C:/Users/Vendrii/Documents/programming/projects/ShankBot/graphics/tibia/packages/Tibia/bin/Qt5Gui.dll"
//    });
//    QImage imgPrev("miniMapDebug/a.png");
//    QImage imgCurr("miniMapDebug/b.png");
//
//    assert(imgPrev.format() == QImage::Format_ARGB32);
//    assert(imgCurr.format() == QImage::Format_ARGB32);
//
//    assert(imgPrev.width() == imgCurr.width());
//    assert(imgPrev.height() == imgCurr.height());
//
//    assert(imgPrev.depth() == imgPrev.depth());
//    assert(imgPrev.depth() == 32);
//
//    size_t size = imgPrev.width() * imgPrev.height();
//    const uint32_t* pCurr = (const uint32_t*)imgCurr.constBits();
//    const uint32_t* pPrev = (const uint32_t*)imgPrev.constBits();
//
//
//
//    std::cout << (int)isMiniMapMatch(pPrev, pCurr, size) << std::endl;
//
//    return 0;
//    convertTileBufferDump(3);
//    return 0;

////    size_t size = 256 * 256 * 4;
//    size_t size = 256 * 256;
////    std::vector<unsigned char> pixels(size);
//    std::vector<size_t> pixels(size);
//
//
//
//
//
//
//    return 0;
//    struct Merp
//    {
//        unsigned int x;
//        unsigned int y;
//    };
//
//    std::list<Merp> prevMerps = {{2, 1}, {2, 2}};
//    std::list<Merp> merps = {{1, 1}, {2, 2}, {1, 2}, {2, 1}};
//
//    for(auto merp : merps)
//        std::cout << merp.x << "x" << merp.y << std::endl;
//
//    std::cout << "--" << std::endl;
//
//    merps.sort([](const Merp& m1, const Merp& m2){return m1.x < m2.x;});
//
//    for(auto merp : merps)
//        std::cout << merp.x << "x" << merp.y << std::endl;
//
//
//    std::cout << "--" << std::endl;
//
//    merps.sort([](const Merp& m1, const Merp& m2){return m1.y < m2.y;});
//
//    for(auto merp : merps)
//        std::cout << merp.x << "x" << merp.y << std::endl;
//
//    std::cout << "--" << std::endl;
//
//    merps.unique([](const Merp& m1, const Merp& m2){return m1.x == m2.x && m1.y == m2.y;});
//
//    assert(merps.size() >= 4);
//
//    int maxDeltaX = 0;
//    for(const Merp& m : merps)
//    {
////        if(m)
//    }
//
//    if(merps.size() == 3)
//    {
//        for(const Merp& m : merps)
//        {
//
//        }
//
//        auto it1 = merps.begin();
//        auto it2 = it1;
//        it2++;
//        if(it1->y == it2->y)
//        {
//            int dX =
//        }
//        else
//        {
//            it2++;
//            assert(it1->x == it2->x);
//
//            int dX =
//        }
//    }
//
//    while(it2 != merps.end())
//
//    return 0;

    GraphicsLayer::ShankBot sb("tibia", "version-control");
    sb.run();

    return 0;


//    auto squares = CombatSquareSample::generateSamples();
//    for(const auto& s : squares)
//    {
//        QImage img(s.pixels.data(), s.width, s.height, QImage::Format_RGBA8888);
//        img.save(QString::fromStdString(stringify("combatSquareDebug/", (int)s.type, "-out.png")));
//    }
//
//    return 0;

    const std::string CATALOG_CONTENT_PATH = "tibia/packages/Tibia/assets/catalog-content.json";
    const std::string GRAPHICS_RESOURCES_PATH = "tibia/packages/Tibia/bin/graphics_resources.rcc";
    const std::string SPRITE_COLOR_TREE_PATH = "version-control/current/tree-sprite-color.bin";
    const std::string SPRITE_TRANSPARENCY_TREE_PATH = "version-control/current/tree-sprite-transparency.bin";
    const std::string SPRITE_OBJECT_BINDINGS_PATH = "version-control/current/sprite-object-bindings.bin";



//
//    {
//        SequenceTree transTree(SPRITE_TRANSPARENCY_TREE_PATH);
//
//        forEachFile("addonDebug", [&transTree](const std::string& file)
//        {
//            std::list<unsigned int> ids;
//            QImage img(QString::fromStdString(file));
//            std::vector<size_t> trans = rgbaToTransparencyTreeSprite(img.constBits(), img.width(), img.height());
//            std::cout << "Processing " << file << std::endl;
//            if(!transTree.find(trans, ids))
//            {
//                std::cout << "FAILED: " << file << std::endl;
//            }
//            for(unsigned int id : ids)
//                std::cout << "\t" << id << std::endl;
//        });
//
//        return 0;
//    }


    CatalogContent cat(CATALOG_CONTENT_PATH);
    SpriteReader reader(cat);
    AppearancesReader aReader(cat.getAppearances().front().path);
    SpriteObjectBindings bindings(aReader.getObjects());

    {
        size_t colorSize = 0;
        size_t transSize = 0;
        size_t outfitColorSize = 0;
        size_t outfitTransSize = 0;
        std::vector<std::vector<size_t>> ts;
        std::vector<std::vector<size_t>> cs;
        std::vector<unsigned int> ids;
        reader.forEachSprite([&](const SpriteReader::Sprite& spr)
        {

            bool isBlank = false;
            size_t width = spr.tileWidth * Constants::TILE_PIXEL_WIDTH;
            size_t height = spr.tileHeight * Constants::TILE_PIXEL_HEIGHT;
            std::vector<size_t> trans = rgbaToTransparencyTreeSprite(spr.pixels, width, height, &isBlank);
            if(!isBlank)
            {
                std::vector<size_t> color = rgbaToColorTreeSprite(spr.pixels, width, height);
                transSize += trans.size() * sizeof(size_t);
                colorSize += color.size() * sizeof(size_t);

                std::list<const Object*> objects = bindings.getObjects(spr.id);
                bool isOnlyOutfit = false;
                for(const Object* o : objects)
                {
                    const Object::SpriteInfo& info = o->someInfos.front().spriteInfo;
                    if(o->type == Object::Type::OUTFIT && (info.numAddons > 1 || info.numMounts > 1 || info.numBlendFrames > 1))
                        isOnlyOutfit = true;
                    else
                    {
                        isOnlyOutfit = false;
                        break;
                    }
                }

                if(isOnlyOutfit)
                {
                    outfitTransSize += trans.size() * sizeof(size_t);
                    outfitColorSize += color.size() * sizeof(size_t);
                }
                else
                {
                    ts.push_back(trans);
                    cs.push_back(color);
                    ids.push_back(0);
                }
            }

            return true;
        });

        std::cout   << colorSize << std::endl
                    << transSize << std::endl
                    << outfitColorSize << std::endl
                    << outfitTransSize << std::endl
                    << 100.f * float(outfitColorSize) / float(colorSize) << "%" << std::endl
                    << 100.f * float(outfitTransSize) / float(transSize) << "%" << std::endl;

        SequenceTree color(cs, ids);
        color.writeToBinaryFile("seqcolortest.bin");

        SequenceTree trans(ts, ids);
        trans.writeToBinaryFile("seqtranstest.bin");
        return 0;
    }


    OutfitAddonMerger merger;
    for(const Object& o : aReader.getObjects())
    {
        if(o.type == Object::Type::OUTFIT)
            merger.addOutfit(o);
    }

    std::vector<std::vector<size_t>> transparencies;
    std::vector<unsigned int> transIds;
    reader.forEachSprite([&merger, &transparencies, &transIds](const SpriteReader::Sprite& spr)
    {
//        if(!merger.processSprite(spr).empty())
//            return false;
        using namespace Constants;
        std::vector<std::shared_ptr<Sprite>> merges =
            merger.processSprite(spr.id, spr.tileWidth * TILE_PIXEL_WIDTH, spr.tileHeight * TILE_PIXEL_HEIGHT, spr.pixels);
        for(const std::shared_ptr<Sprite>& m : merges)
        {
            bool isBlank = false;
            std::vector<size_t> transparency = rgbaToTransparencyTreeSprite(m->pixels, m->width, m->height, &isBlank);
            if(!isBlank)
            {
                transparencies.push_back(transparency);
                transIds.push_back(m->id);
            }
        }

        return true;
    });

    assert(merger.isEmpty());

    SequenceTree transTree(transparencies, transIds);

    using namespace sb::utility::file;
    forEachFile("addonDebug", [&transTree](const std::string& file)
    {
        std::list<unsigned int> ids;
        QImage img(QString::fromStdString(file));
        std::vector<size_t> trans = rgbaToTransparencyTreeSprite(img.constBits(), img.width(), img.height());
        std::cout << "Processing " << file << std::endl;
        if(!transTree.find(trans, ids))
        {
            std::cout << "FAILED: " << file << std::endl;
        }
        for(unsigned int id : ids)
            std::cout << "\t" << id << std::endl;
    });


    return 0;


//
//    SequenceTree colorTree(SPRITE_COLOR_TREE_PATH);
//    size_t numSprites = 0;
//    reader.forEachSprite([&](const SpriteReader::Sprite& spr)
//    {
////        std::list<unsigned int> ids;
////        if(!colorTree.find(rgbaToColorTreeSprite(spr.pixels, spr.tileWidth * 32, spr.tileHeight * 32), ids))
////           std::cout << "Failed find..." << std::endl;
//
//        numSprites++;
//
//
//
//        return true;
//    });
//
//    std::cout << numSprites << std::endl;
//
//    return 0;
////    std::vector<std::vector<size_t>> colorTreeSprites;
////    reader.forEachSprite([&](const SpriteReader::Sprite& spr)
////    {
//////        QImage img(spr.pixels, spr.tileWidth * 32, spr.tileHeight * 32, QImage::Format_RGBA8888);
//////        QByteArray array;
//////        QBuffer buffer(&array);
//////        buffer.open(QIODevice::WriteOnly);
//////        img.save(&buffer, "PNG");
//////
//////        colorTreeSprites.emplace_back();
//////        std::vector<size_t>& ctp = colorTreeSprites.back();
//////        size_t numElements = array.size() / sizeof(size_t);
//////        if(array.size() % sizeof(size_t) != 0)
//////            numElements++;
//////
//////        ctp.resize(numElements);
//////        ctp.back() = 0;
//////        memcpy(ctp.data(), buffer.data(), array.size());
////        colorTreeSprites.push_back(rgbaToColorTreeSprite(spr.pixels, spr.tileWidth * 32, spr.tileHeight * 32));
////
////        return true;
////    });
////
////    std::cout << "Building tree..." << std::endl;
////    std::vector<unsigned int> ids(colorTreeSprites.size());
////    SequenceTree colorTree(colorTreeSprites, ids);
////
////
////
////    return 0;
//
//
////    size_t unCompSize = 0;
////    size_t colorTreeSpriteSize = 0;
////    size_t pngCompSize = 0;
////    reader.forEachSprite([&](const SpriteReader::Sprite& spr)
////    {
////        if(spr.id > 2682)
////            return false;
////
////        unCompSize += spr.tileWidth * 32 * spr.tileHeight * 32 * 4;
////        colorTreeSpriteSize += rgbaToColorTreeSprite(spr.pixels, spr.tileWidth * 32, spr.tileHeight * 32).size() * sizeof(size_t);
////
////        QImage img(spr.pixels, spr.tileWidth * 32, spr.tileHeight * 32, QImage::Format_RGBA8888);
////        QByteArray array;
////        QBuffer buffer(&array);
////        buffer.open(QIODevice::WriteOnly);
////        img.save(&buffer, "PNG", 0);
////        pngCompSize += array.size();
//////        img.save(QString::fromStdString(stringify("spritesMaxComp/", spr.id, ".png")), 0, 0);
////        return true;
////    });
////
////    std::cout << "Uncompressed size: " << unCompSize << std::endl;
////    std::cout << "Color tree sprite size: " << colorTreeSpriteSize << std::endl;
////    std::cout << "PNG comp size: " << pngCompSize << std::endl;
////
////    return 0;
//
//    std::map<unsigned int, std::list<unsigned int>> map;
//    std::map<unsigned int, std::list<unsigned int>> transparencyMap;
//    std::map<unsigned int, std::list<unsigned int>> colorMap;
//    reader.forEachSprite([&](const SpriteReader::Sprite& spr)
//    {
////        QImage img(spr.pixels, spr.tileWidth * 32, spr.tileHeight * 32, QImage::Format_RGBA8888);
////        img.save(QString::fromStdString(stringify("sprites/", spr.id, ".png")));
//        unsigned int width = spr.tileWidth * 32;
//        unsigned int height = spr.tileHeight * 32;
//        size_t size = width * height * 4;
//        std::vector<unsigned char> data;
//        data.insert(data.end(), (char*)&width, (char*)&width + sizeof(width));
//        data.insert(data.end(), (char*)&height, (char*)&height + sizeof(height));
//        data.insert(data.end(), spr.pixels, spr.pixels + size);
//        unsigned int hash = fnv1a32(data.data(), data.size());
//        map[hash].push_back(spr.id);
//
//        std::vector<size_t> color = rgbaToColorTreeSprite(spr.pixels, width, height);
//        std::vector<size_t> transparency = rgbaToTransparencyTreeSprite(spr.pixels, width, height);
//        colorMap[fnv1a32((unsigned char*)color.data(), sizeof(size_t) * color.size())].push_back(spr.id);
//        transparencyMap[fnv1a32((unsigned char*)transparency.data(), sizeof(size_t) * transparency.size())].push_back(spr.id);
//        return true;
//    });
//
//    GraphicsResourceReader gReader(GRAPHICS_RESOURCES_PATH);
//    const std::vector<GraphicsResourceReader::GraphicsResource>& gResources = gReader.getGraphicsResources();
//    for(size_t i = 0; i < gResources.size(); i++)
//    {
//        const GraphicsResourceReader::GraphicsResource& res = gResources[i];
//        size_t size = res.width * res.height * 4;
//        std::vector<unsigned char> data;
//        data.insert(data.end(), (char*)&res.width, (char*)&res.width + sizeof(res.width));
//        data.insert(data.end(), (char*)&res.height, (char*)&res.height + sizeof(res.height));
//        data.insert(data.end(), res.pixels, res.pixels + size);
//        unsigned int hash = fnv1a32(data.data(), data.size());
//        map[hash].push_back(i + Constants::GRAPHICS_RESOURCE_ID_START);
//
//        unsigned int id = i + Constants::GRAPHICS_RESOURCE_ID_START;
//        std::vector<size_t> color = rgbaToColorTreeSprite(res.pixels, res.width, res.height);
//        std::vector<size_t> transparency = rgbaToTransparencyTreeSprite(res.pixels, res.width, res.height);
//        colorMap[fnv1a32((unsigned char*)color.data(), sizeof(size_t) * color.size())].push_back(id);
//        transparencyMap[fnv1a32((unsigned char*)transparency.data(), sizeof(size_t) * transparency.size())].push_back(id);
//        return true;
//    }
//
//    std::ofstream file("image-map.bin", std::ios::binary);
//    std::list<std::map<unsigned int, std::list<unsigned int>>::const_iterator> collisions;
//    std::list<std::map<unsigned int, std::list<unsigned int>>::const_iterator> nonCollisions;
//    for(auto it = map.begin(); it != map.end(); it++)
//    {
//        const auto& pair = *it;
//        if(pair.second.size() > 1)
//            collisions.push_back(it);
//        else
//        {
//            nonCollisions.push_back(it);
//
//        }
//    }
//
//    writeStream((unsigned int)nonCollisions.size(), file);
//    for(const auto& it : nonCollisions)
//    {
//        writeStream(it->first, file);
//        for(const auto& i : it->second)
//            writeStream(i, file);
//    }
//
//    writeStream((unsigned int)collisions.size(), file);
//    for(const auto& it : collisions)
//    {
//        writeStream(it->first, file);
//        writeStream((unsigned char)it->second.size(), file);
//        for(const auto& i : it->second)
//            writeStream(i, file);
//    }
//
//    file.close();
//    return 0;
//
//
//    std::cout << "color" << std::endl;
//    for(const auto& pair : colorMap)
//        if(pair.second.size() > 1)
//        {
//            std::vector<QImage> images;
//            for(unsigned int sprId : pair.second)
//            {
//                if(sprId >= Constants::GRAPHICS_RESOURCE_ID_START)
//                {
//                    sprId -= Constants::GRAPHICS_RESOURCE_ID_START;
//                    std::cout << gResources[sprId].name << " ";
//                    images.emplace_back(QString::fromStdString(stringify("graphicsResourcesOutTest/", basename(gResources[sprId].name))));
//                }
//                else
//                {
//                    std::cout << sprId << " ";
//                    images.emplace_back(QString::fromStdString(stringify("sprites/", sprId, ".png")));
//                }
//            }
//
//
//            size_t byteCount = images.front().byteCount();
//            for(size_t i = 1; i < images.size(); i++)
//            {
//                if(images[i].width() != images.front().width())
//                    THROW_RUNTIME_ERROR("Width inequal");
//
//                if(images[i].height() != images.front().height())
//                    THROW_RUNTIME_ERROR("Height inequal");
//
//                if(byteCount != images[i].byteCount())
//                {
//                    THROW_RUNTIME_ERROR(stringify("Byte count inequal"));
//                }
//
//                const unsigned char* data = images[i].constBits();
//                for(size_t j = 0; j < images[i].byteCount(); j++)
//                {
//                    if(data[j] != images.front().constBits()[j])
//                        THROW_RUNTIME_ERROR("Bytes inequal");
//                }
//            }
//
//
//
//            std::cout << std::endl;
//        }
//
//    std::cout << "transparency" << std::endl;
//    for(const auto& pair : transparencyMap)
//        if(pair.second.size() > 1)
//        {
//            std::vector<QImage> images;
//            for(unsigned int sprId : pair.second)
//            {
//                if(sprId >= Constants::GRAPHICS_RESOURCE_ID_START)
//                {
//                    sprId -= Constants::GRAPHICS_RESOURCE_ID_START;
//                    std::cout << gResources[sprId].name << " ";
//                    images.emplace_back(QString::fromStdString(stringify("graphicsResourcesOutTest/", basename(gResources[sprId].name))));
//                }
//                else
//                {
//                    std::cout << sprId << " ";
//                    images.emplace_back(QString::fromStdString(stringify("sprites/", sprId, ".png")));
//                }
//            }
//
//
//            size_t byteCount = images.front().byteCount();
//            for(size_t i = 1; i < images.size(); i++)
//            {
//                if(images[i].width() != images.front().width())
//                    THROW_RUNTIME_ERROR("Width inequal");
//
//                if(images[i].height() != images.front().height())
//                    THROW_RUNTIME_ERROR("Height inequal");
//
//                if(byteCount != images[i].byteCount())
//                {
//                    THROW_RUNTIME_ERROR(stringify("Byte count inequal"));
//                }
//
//                const unsigned char* data = images[i].constBits();
//                for(size_t j = 0; j < images[i].byteCount(); j++)
//                {
//                    if(data[j] != images.front().constBits()[j])
//                        THROW_RUNTIME_ERROR("Bytes inequal");
//                }
//            }
//
//
//
//            std::cout << std::endl;
//        }

//    for(const auto& pair : map)
//        if(pair.second.size() > 1)
//        {
//            std::vector<QImage> images;
//            for(unsigned int sprId : pair.second)
//            {
//                if(sprId >= Constants::GRAPHICS_RESOURCE_ID_START)
//                {
//                    sprId -= Constants::GRAPHICS_RESOURCE_ID_START;
//                    std::cout << gResources[sprId].name << " ";
//                    images.emplace_back(QString::fromStdString(stringify("graphicsResourcesOutTest/", basename(gResources[sprId].name))));
//                }
//                else
//                {
//                    std::cout << sprId << " ";
//                    images.emplace_back(QString::fromStdString(stringify("sprites/", sprId, ".png")));
//                }
//            }
//
//
//            size_t byteCount = images.front().byteCount();
//            for(size_t i = 1; i < images.size(); i++)
//            {
//                if(images[i].width() != images.front().width())
//                    THROW_RUNTIME_ERROR("Width inequal");
//
//                if(images[i].height() != images.front().height())
//                    THROW_RUNTIME_ERROR("Height inequal");
//
//                if(byteCount != images[i].byteCount())
//                {
//                    THROW_RUNTIME_ERROR(stringify("Byte count inequal"));
//                }
//
//                const unsigned char* data = images[i].constBits();
//                for(size_t j = 0; j < images[i].byteCount(); j++)
//                {
//                    if(data[j] != images.front().constBits()[j])
//                        THROW_RUNTIME_ERROR("Bytes inequal");
//                }
//            }
//
//
//
//            std::cout << std::endl;
//        }

//    return 0;


//    QGuiApplication app(argc, argv);
//    QOpenGLContext context;
//    context.create();
//    QOffscreenSurface surface;
//    surface.create();
//    context.makeCurrent(&surface);
//
//    QOpenGLPaintDevice dev(20, 20);
//    QOpenGLFramebufferObject fb(20, 20);
//
//    if(!fb.bind())
//        throw std::runtime_error("Failed to bind fb");
//    assert(fb.isValid());
//
//    QPainter painter(&dev);
//    painter.fillRect(0, 0, 20, 20, Qt::black);
//    painter.setPen(Qt::red);
//    painter.drawRect(1, 1, 10, 10);
//    painter.end();
//
//    fb.toImage().save("paintertest.png");
//
//    return 0;


//    QApplication app(argc, argv);
//    Painter widget;
//    widget.show();
////    widget.bindTexture(QImage());
//
////    QPainter merp(&widget);
////    merp.setPen(Qt::red);
////    merp.drawRect(0, 0, 10, 10);
////    merp.end();
////    widget.swapBuffers();
//    using namespace Merp;
//    HINSTANCE opengl32 = GetModuleHandle("OPENGL32.DLL");
//    assert(opengl32 != NULL);
//    void* texSubFunc = (void*)GetProcAddress(opengl32, "glTexSubImage2D");
//    assert(texSubFunc != NULL);
//    subTexImgDetour = new FunctionDetour((void*)texSubFunc, (void*)subTexImg);
//    doPaint = true;
//    while(true)
//    {
//        widget.update();
//        QApplication::processEvents();
//    }
//    return 0;
//
//    QFont font;
//    font.setFamily("Verdana");
//
//    font.setBold(true);
//    font.setPointSizeF(8.f);
//    font.setStyleStrategy(QFont::PreferAntialias);
//
//    std::unique_ptr<QFontMetrics> metrics;
//
//    metrics.reset(new QFontMetrics(font));
//    QRect rect = metrics->boundingRect('5');
//
//    QImage img(metrics->width('5') + 1, metrics->height(), QImage::Format_RGBA8888);
//    img.fill(QColor(255, 255, 255, 0));
//    QPainter painter(&img);
//
//
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
//    painter.setRenderHint(QPainter::TextAntialiasing, true);
//    painter.setFont(font);
//    QPainterPath path;
//
////    font.setPointSizeF(9.f);
//    metrics.reset(new QFontMetrics(font));
//    QRect pathRect = metrics->boundingRect('5');
//    path.addText(0, 1-pathRect.y(), font, QString('5'));
////    font.setStyleStrategy(QFont::PreferAntialias);
////    painter.setFont(font);
//
////    painter.drawPath(path);
//    QPen pen(Qt::black, 1.5f);
//    painter.setPen(pen);
//    painter.drawPath(path);
//    painter.setPen(QColor(153, 255, 255));
//    painter.drawText(0, 1 - rect.y(), QString('5'));
////    painter.fillPath(path, QColor(153, 255, 255));
//
//    font.setPointSizeF(8.f);
//    metrics.reset(new QFontMetrics(font));
//    rect = metrics->boundingRect('5');
//    painter.setFont(font);
//
////    painter.setPen(QColor(153, 255, 255));
////    painter.drawText(1, rect.height() - metrics->descent() + 1, QString('5'));
////    painter.setPen(Qt::black);
//
////    painter.drawPath(path);
////    font.setStyleStrategy(QFont::ForceOutline);
////    font.setPointSizeF(10.f);
////    painter.setFont(font);
////    painter.drawText(-rect.x(), -rect.y(), QString('5'));
//    painter.end();
//    img.save("paintertest.png");
//    return 0;


//    QGuiApplication app(argc, argv);
//
//    std::list<FontSample::Glyph> glyphs;
//
//    QFont font;
//    font.setFamily("Verdana");
//    font.setStyleStrategy(QFont::NoAntialias);
//    for(float pSize = 6.f; pSize <= 16.f; pSize += 0.1f)
//    {
//        std::unique_ptr<QFontMetrics> metrics;
//        QRectF rect;
//
//        font.setPointSizeF(pSize);
//
//        font.setBold(false);
//        metrics.reset(new QFontMetrics(font));
//        for(char c = 0x20; c <= 0x7e; c++)
//        {
////            rect = metrics->boundingRect(QString(c));
//            rect = metrics->boundingRect(c);
//            QImage img(rect.width(), rect.height(), QImage::Format_Grayscale8);
//            QPainter painter(&img);
//            painter.fillRect(0, 0, img.width(), img.height(), Qt::black);
//            painter.setPen(Qt::white);
//            painter.drawText(-rect.x(), -rect.y(), QString(c));
//            painter.end();
//
////            std::stringstream sstream;
////            sstream << "fontOutDraw/" << c << "_" << pSize << "_" << "n" << ".png";
////            img.save(QString::fromStdString(sstream.str()));
//
//            glyphs.emplace_back();
//            FontSample::Glyph& g = glyphs.back();
//            g.character = c;
//            g.width = img.width();
//            g.height = img.height();
//            g.pointSize = pSize;
//            g.style = FontSample::Style::NORMAL;
//            g.data.resize(g.width * g.height);
//
//            for(size_t i = 0; i < g.height; i++)
//            {
//                const unsigned char* row = img.scanLine(i);
//                memcpy(&g.data[i * g.width], row, g.width);
//            }
//        }
//
//        font.setBold(true);
//        metrics.reset(new QFontMetrics(font));
//        for(char c = 0x20; c <= 0x7e; c++)
//        {
////            rect = metrics->boundingRect(QString(c));
//            rect = metrics->boundingRect(c);
//            QImage img(rect.width(), rect.height(), QImage::Format_Grayscale8);
//            QPainter painter(&img);
//            painter.fillRect(0, 0, img.width(), img.height(), Qt::black);
//            painter.setPen(Qt::white);
//            painter.drawText(-rect.x(), -rect.y(), QString(c));
//            painter.end();
//
////            std::stringstream sstream;
////            sstream << "fontOutDraw/" << c << "_" << pSize << "_" << "b" << ".png";
////            img.save(QString::fromStdString(sstream.str()));
//
//            glyphs.emplace_back();
//            FontSample::Glyph& g = glyphs.back();
//            g.character = c;
//            g.width = img.width();
//            g.height = img.height();
//            g.pointSize = pSize;
//            g.style = FontSample::Style::BOLD;
//            g.data.resize(g.width * g.height);
//
//            for(size_t i = 0; i < g.height; i++)
//            {
//                const unsigned char* row = img.scanLine(i);
//                memcpy(&g.data[i * g.width], row, g.width);
//            }
//        }
//    }

//    using namespace Merp;
//    QApplication app(argc, argv);
//    QSurfaceFormat surfaceFormat;
//    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
//    QSurfaceFormat::setDefaultFormat(surfaceFormat);
//    QOpenGLContext openGLContext;
//    openGLContext.setFormat(surfaceFormat);
//    openGLContext.create();
//
//    QQuickView view;
//    view.setFormat(surfaceFormat);
//    view.connect(view.engine(), &QQmlEngine::quit, &app, &QCoreApplication::quit);
//    new QQmlFileSelector(view.engine(), &view);
//    view.setSource(QUrl("test.qml"));
//    if (view.status() == QQuickView::Error)
//        return -1;
//    view.setResizeMode(QQuickView::SizeRootObjectToView);
//
//    HINSTANCE opengl32 = GetModuleHandle("OPENGL32.DLL");
//    assert(opengl32 != NULL);
//    void* texSubFunc = (void*)GetProcAddress(opengl32, "glTexSubImage2D");
//    assert(texSubFunc != NULL);
//    subTexImgDetour = new FunctionDetour((void*)texSubFunc, (void*)subTexImg);
//    view.show();
//    return app.exec();
//
//
//
//    Painter widget;
//
//
//    widget.show();
//
//
//    app.exec();
//
//    return 0;

//    QGuiApplication app(argc, argv);
//
//
//
////    QOpenGLContext openGLContext;
////    openGLContext.setFormat(surfaceFormat);
////    openGLContext.create();
//
//    QOpenGLWindow window;
//    QSurfaceFormat surfaceFormat;
//    surfaceFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
//    QSurfaceFormat::setDefaultFormat(surfaceFormat);
//    window.setFormat(surfaceFormat);
//    window.create();
////    openGLContext.makeCurrent(&window);
//
////    assert(openGLContext.isValid());
//
//
//
//    window.show();
//    window.makeCurrent();
//
////    assert(window.isValid());
////    assert(window.context() != nullptr);
//    QPainter painter(&window);
//
//
//    QFont font;
//    font.setFamily("Verdana");
//    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);
//    font.setBold(false);
//    font.setItalic(false);
//    painter.setFont(font);
//    QPen pen;
//    pen.setWidth(2);
//    pen.setColor(Qt::white);
//    painter.setPen(pen);
//    QPainterPath path;
//
//    QLinearGradient gradient;
//    path.addText(0, 0, font, QString("Hallo"));
//    painter.setBrush(gradient);
//    painter.setPen(pen);
//    painter.drawPath(path);
//
//    painter.end();
//    app.exec();
//    return 0;

    FontSample fontSample("Verdana", 6.f, 10.f, FontSample::Style::BOLD | FontSample::Style::NORMAL);
    FontSample bigFontSample("Verdana", 13.f, 15.f, FontSample::Style::BOLD | FontSample::Style::NORMAL);
    std::list<FontSample::Glyph> glyphs;
    glyphs.insert(glyphs.end(), fontSample.getGlyphs().begin(), fontSample.getGlyphs().end());
    glyphs.insert(glyphs.end(), bigFontSample.getGlyphs().begin(), bigFontSample.getGlyphs().end());
    std::vector<unsigned char> correctGlyphs =
    {
       'W', 'i',  'l', ' ', 'P',
       'a', 's', 'e', 't', 'f',
       'g', 'L', 'm', 'n', 'o',
       'd', 'u', 'r', 'F', '0',
       'o', 'p', 'S', 't', 'u',
       'i', 'l', 'o', 'S', 'C',
       'a', 'P', 'n', 'C', 'r',
       'e', 'u', 'g', 'M', ' ',
       'A', 'a', 'c', 'i', 'l',
       's', 'O', 'Q', 'h', 'y',
       'M', 'D', ':', 'A', 'c',
       'w', '/', 'p', 'C', 'L',
       'f', 'g', 'w', 'L', 'l',
       '.', ' ', '0', 'm', 'o',
       'a', 'p', '(', 's', ')',
       'h', 'H', 'I', 'v', ',',
       'k', '.', '!', 'N', '3',
       'b', 'T', 'o', 'Y', 'Z',
       'x', 'R', 'E', 'S', 'm',
       'P', 'G', 'k', 'v', 'S',
       'r', 'e', 'h', 'C', 'c',
       't', 'u', 'i', 'M', '2',
       '6', 'd', '9', ':', 'Y',
       'O', '1', 'n', '3', 'b',
       '5', 'T', 'E', 'D', 'k',
       '0', '8', '9', '4', '1',
       '6', '3', '5', '1', '4',
       '7', '%', '2', 'O', '5',
       'X', 'G', '+', '(', ')',
       'H', 'j', 'U', ',', 'I',
       '-', 'f', 'i', 'L', '-',
       'k', 'l', ' ', 'm', 'a',
       'C', 'S', 'r', 'c', 'e',
       't', 'K', 'B', 'H', 'y',
       'Q', '.', 'y', '?', '!',
       '?', '*', 'g', 'h', 'K',
       'n', 'd', 'R', 'v', 'f',
       'D', 'j', 'b', 'q', 'J',
       '>', 'V', '8', 'B', 's',
       'u', 'o', '(', ')', '\'',
       'T', 'V', 'F', 'I', '7',
       'w', '.', 'W', 'P', 'R',
       'G'
    };
//
    const size_t NUM_GLYPHS = 210 + 1;
    assert(correctGlyphs.size() == NUM_GLYPHS);
    std::vector<QImage> fontOut;
    fontOut.reserve(NUM_GLYPHS);
    for(size_t i = 0; i < NUM_GLYPHS; i++)
    {
        std::stringstream sstream;
        sstream << "fontOut/" << i << ".png";
        fontOut.emplace_back(QString::fromStdString(sstream.str()));
    }
////
//    std::vector<QImage> fontOut =
//    {
//        QImage("glyphCompDebug/1g.png"),
//        QImage("glyphCompDebug/5g.png"),
//        QImage("glyphCompDebug/1r.png"),
//        QImage("glyphCompDebug/2r.png"),
//        QImage("glyphCompDebug/3r.png"),
//        QImage("glyphCompDebug/4r.png"),
//        QImage("glyphCompDebug/5r.png"),
//        QImage("glyphCompDebug/6r.png"),
//        QImage("glyphCompDebug/7r.png"),
//        QImage("glyphCompDebug/9r.png"),
//        QImage("glyphCompDebug/2c.png"),
//        QImage("glyphCompDebug/3c.png"),
//        QImage("glyphCompDebug/4c.png"),
//        QImage("glyphCompDebug/5c.png"),
//        QImage("glyphCompDebug/6c.png"),
//        QImage("glyphCompDebug/7c.png"),
//        QImage("glyphCompDebug/8c.png"),
//        QImage("glyphCompDebug/4w.png"),
//        QImage("glyphCompDebug/17w.png"),
//        QImage("glyphCompDebug/23w.png"),
//        QImage("glyphCompDebug/45w.png"),
//        QImage("glyphCompDebug/65p.png"),
//        QImage("glyphCompDebug/68b.png"),
//    };
//    const size_t NUM_GLYPHS = fontOut.size();
//    assert(NUM_GLYPHS == 23);
//
//    for(QImage& img : fontOut)
//    {
//        assert(img.format() == QImage::Format_RGBA8888 || img.format() == QImage::Format_ARGB32);
//        typedef SharedMemoryProtocol::PixelData::PixelFormat Format;
//        if(img.format() == QImage::Format_ARGB32)
//            img = img.rgbSwapped();
//
//        FrameParser::TileNumber n = getTileNumber(img.constBits(), img.width(), img.height(), Format::RGBA, fontSample.getGlyphs());
//
//        std::cout << (int)n.type << "\t\t" << n.number << std::endl;
//    }
//    return 0;

//    for(QImage& img : fontOut)
//    {
//        {
//            size_t numPixels = 0;
//            size_t rSum = 0;
//            size_t gSum = 0;
//            size_t bSum = 0;
//            for(size_t x = 0; x < img.width(); x++)
//                for(size_t y = 0; y < img.height(); y++)
//                {
//                            QColor c = img.pixelColor(x, y);
//                            if(c.alpha() == 255)
//                            {
//                                size_t pixelSum = c.red() + c.green() + c.blue();
//                                if(pixelSum >= 10)
//                                {
//                                    numPixels++;
//                                    rSum += c.red();
//                                    gSum += c.green();
//                                    bSum += c.blue();
//                                }
//                            }
//                }
//
//            size_t sum = rSum + gSum + bSum;
//            float rPerc = float(rSum) / float(sum);
//            float gPerc = float(gSum) / float(sum);
//            float bPerc = float(bSum) / float(sum);
//
//            std::string color = "FREAKING UNKNOOOOOWN";
//            if(gPerc >= 0.95f)
//            {
//                color = "Green";
//            }
//            else if(rPerc >= 0.95f)
//            {
//                color = "Red";
//            }
//            else if(gPerc >= 0.38f && gPerc <= 0.40f && bPerc >= 0.38f && bPerc <= 0.40f)
//            {
//                color = "Cyan";
//            }
//            else if(rPerc >= 0.30f && rPerc <= 0.35f && gPerc >= 0.30f && gPerc <= 0.35f && bPerc >= 0.30f && bPerc <= 0.35f)
//            {
//                color = "White";
//            }
//            else if(rPerc >= 0.50f && rPerc <= 0.60f && gPerc >= 0.20f && gPerc <= 0.30f && bPerc >= 0.20f && bPerc <= 0.30f)
//            {
//                color = "Pink";
//            }
//            else if(rPerc >= 0.15f && rPerc <= 0.25f && gPerc >= 0.25f && gPerc <= 0.35f && bPerc >= 0.45f && bPerc <= 0.55f)
//            {
//                color = "Blue";
//            }
//            std::cout << color << std::endl;
////            std::cout << float(rSum) / float(sum) << " " << float(gSum) / float(sum) << " " << float(bSum) / float(sum) << std::endl;
////            std::cout << float(rSum) / float(numPixels) << " " << float(gSum) / float(numPixels) << " " << float(bSum) / float(numPixels) << std::endl;
//        }
//
//        img = img.convertToFormat(QImage::Format_Grayscale8);
//        static size_t merp = 0;
//
//        size_t numPixels = 0;
//        size_t sum = 0;
//        for(size_t i = 0; i < img.height(); i++)
//        {
//            unsigned char* row = (unsigned char*)img.scanLine(i);
//            for(size_t x = 0; x < img.width(); x++)
//            {
//                if(row[x] > 10)
//                {
//                    sum += row[x];
//                    numPixels++;
//                }
////                row[x] = std::min(int(row[x] * 2.f), 255);
//            }
//        }
//
//        static const float idealAverage = 175.f;
//        float averagePixelValue = float(sum) / float(numPixels);
//        assert(std::isfinite(averagePixelValue));
//        float modifier = idealAverage / averagePixelValue;
//        assert(std::isfinite(modifier));
//        for(size_t i = 0; i < img.height(); i++)
//        {
//            unsigned char* row = (unsigned char*)img.scanLine(i);
//            for(size_t x = 0; x < img.width(); x++)
//            {
//                if(row[x] > 10)
//                {
//                    row[x] = std::min(int(row[x] * modifier), 255);
//                }
//        //                row[x] = std::min(int(row[x] * 2.f), 255);
//            }
//        }
////        std::cout << averagePixelValue << std::endl;
//
////
//        std::stringstream sstream;
//        sstream << "glyphCompDebug/" << merp++ << ".png";
//        img.save(QString::fromStdString(sstream.str()));
//    }


//
//    {
//        QImage img = fontOut.back();
//        size_t r = 0;
//        size_t g = 0;
//        size_t b = 0;
//        size_t numPixels = 0;
//        for(size_t x = 0; x < img.width(); x++)
//            for(size_t y = 0; y < img.height(); y++)
//            {
//                QColor c = img.pixelColor(x, y);
//                if(c.alpha() != 0)
//                {
//                    r += c.red();
//                    g += c.green();
//                    b += c.blue();
//                    numPixels++;
//                }
//            }
//
//
//
//        std::cout   << "r: " << float(r) / float(numPixels) << std::endl
//                    << "g: " << float(g) / float(numPixels) << std::endl
//                    << "b: " << float(b) / float(numPixels) << std::endl;
//
//    }
//
//
//    fontOut.back() = fontOut.back().convertToFormat(QImage::Format_Grayscale8);


//
    size_t fontOutCount = 0;
    size_t totalDelta = 0;
    struct Wrong
    {
        unsigned char cpu;
        unsigned char human;
        size_t id;
    };
    std::list<Wrong> wrongs;
////    std::vector<size_t> glyphCenters;
////    glyphCenters.reserve(glyphs.size());
////
////    std::vector<size_t> glyphSums;
////    glyphSums.reserve(glyphs.size());
////
////    for(const FontSample::Glyph& g : glyphs)
////    {
////        size_t center;
////        size_t sum;
////
////        center = computeCenter(g.data.data(), g.width, g.height, &sum);
////        glyphCenters.push_back(center);
////        glyphSums.push_back(sum);
////    }
//
//
    std::vector<std::pair<unsigned char, size_t>> diffs;
    clock_t preTime;
    clock_t endTime;

    preTime = clock();
    for(size_t iGlyph = 0; iGlyph < fontOut.size(); iGlyph++)
    {
        const QImage& img = fontOut[iGlyph];
        assert(img.depth() == 8);
        size_t width = img.width();
        size_t height = img.height();
        std::vector<unsigned char> pixels(width * height);
        for(size_t i = 0; i < height; i++)
        {
            const unsigned char* row = img.scanLine(i);
            memcpy(&pixels[width * i], row, img.width());
        }
//
////        size_t sum;
////        size_t center;
////        center = computeCenter(pixels.data(), width, height, &sum);
////
////        size_t minDiff = -1;
////        const FontSample::Glyph* mostSimilarGlyph = nullptr;
////        size_t iLhs = 0;
////        for(const FontSample::Glyph& lhs : glyphs)
////        {
//////            size_t diff = -1;
//////            for(int x = -2; x <= 2; x++)
//////            {
//////                for(int y = -2; y <= 2; y++)
//////                {
//////                    int newCenter = center;
//////                    newCenter + x + y * width;
//////                    if(newCenter >= 0)
//////                    {
//////                        size_t cDiff = -1;
//////                        if(lhs.width <= width && lhs.height <= height)
//////                            cDiff = centerDiff(lhs.data.data(), lhs.width, lhs.height, glyphCenters[iLhs], pixels.data(), width, height, newCenter, sum);
//////                        else if(width <= lhs.width && height <= lhs.height)
//////                            cDiff = centerDiff(pixels.data(), width, height, newCenter, lhs.data.data(), lhs.width, lhs.height, glyphCenters[iLhs], glyphSums[iLhs]);
//////
//////                        if(cDiff < diff)
//////                        {
//////                            diff = cDiff;
//////                        }
//////                    }
//////
//////                }
//////            }
////
////            size_t diff = -1;
////            if(lhs.width <= width && lhs.height <= height)
////                diff = centerDiff(lhs.data.data(), lhs.width, lhs.height, glyphCenters[iLhs], pixels.data(), width, height, center, sum);
////            else if(width <= lhs.width && height <= lhs.height)
////                diff = centerDiff(pixels.data(), width, height, center, lhs.data.data(), lhs.width, lhs.height, glyphCenters[iLhs], glyphSums[iLhs]);
////
////            if(diff < minDiff)
////            {
////                if(minDiff == 0)
////                    return lhs.character;
////
////                minDiff = diff;
////                mostSimilarGlyph = &lhs;
////            }
////
////            iLhs++;
////        }
////
//
        size_t sum = 0;
        for(size_t i = 0; i < width * height; i++)
            sum += pixels[i];

        size_t minDiff = -1;
        const FontSample::Glyph* mostSimilarGlyph = nullptr;
        for(const FontSample::Glyph& lhs : glyphs)
        {
            size_t diff = -1;
            #define SUM_EPSILON 4
//            if(!(sum > lhs.sum * SUM_EPSILON || lhs.sum > sum * SUM_EPSILON))
            {
                if(lhs.width <= width && lhs.height <= height)
                    diff = movingWindowMinDiff(lhs.data.data(), lhs.width, lhs.height, pixels.data(), width, height, sum);
                else if(width <= lhs.width && height <= lhs.height)
                    diff = movingWindowMinDiff(pixels.data(), width, height, lhs.data.data(), lhs.width, lhs.height, lhs.sum);
            }

            if(diff < minDiff)
            {
                diffs.emplace_back(lhs.character, diff);
                if(minDiff == 0)
                    return lhs.character;

                minDiff = diff;
                mostSimilarGlyph = &lhs;
            }
        }

//        assert(mostSimilarGlyph);
//        std::cout << iGlyph << ": " << mostSimilarGlyph->character << "\t\t" << "(" << minDiff << ")";
//        std::cout << "\t\treal: " << correctGlyphs[iGlyph];
//        if(mostSimilarGlyph->character != correctGlyphs[iGlyph])
//        {
//            std::cout << "\t" << "WRONG";
//            wrongs.push_back({mostSimilarGlyph->character, correctGlyphs[iGlyph], iGlyph});
//        }
//        std::cout << std::endl;
//        totalDelta += minDiff;
    }
    endTime = clock();

    std::cout << "Time: " << float(endTime - preTime) / float(CLOCKS_PER_SEC) << std::endl;

    std::cout << "Total delta: " << totalDelta << std::endl;
    std::cout << "Average delta: " << float(totalDelta) / float(NUM_GLYPHS) << std::endl;
    std::cout << "Num wrongs: " << wrongs.size() << std::endl;
    for(const Wrong& w : wrongs)
        std::cout << w.id << ":\t" << w.cpu << "\t" << w.human << std::endl;
//
////    std::sort(diffs.begin(), diffs.end(), [](const std::pair<unsigned char, size_t>& lhs, const std::pair<unsigned char, size_t>& rhs){return lhs.second < rhs.second;});
////    std::cout << "Diffs: " << std::endl;
////    for(const auto& pair : diffs)
////        std::cout << pair.first << "\t\t" << pair.second << std::endl;
//
    return 0;


//    GraphicsResourceReader reader("tibia/packages/Tibia/bin/graphics_resources.rcc");
//
//    size_t size = 196 * 112 * 4;
//    unsigned char* pixels = new unsigned char[size];
//
//    std::ifstream file;
//    file.open("dragonsubtex.bin", std::ios::binary);
//    file.read((char*)pixels, size);
//    file.close();
//    std::vector<unsigned char> pixelsSubTex(pixels, pixels + size);
//    std::vector<size_t> colorSpriteSubTex = rgbaToColorTreeSprite(pixels, 196, 112);
////    QImage imgSubTex(pixelsSubTex.data(), 196, 112, QImage::Format_RGBA8888);
////    imgSubTex.save("dragonsubtex.png");
//
//    file.open("dragon-dragon-logo.png.bin", std::ios::binary);
//    file.read((char*)pixels, size);
//    file.close();
//    std::vector<unsigned char> pixelsGui1(pixels, pixels + size);
//    std::vector<size_t> colorSpriteGui1 = rgbaToColorTreeSprite(pixels, 196, 112);
//
//    assert(colorSpriteGui1.size() == colorSpriteSubTex.size());
//    for(size_t i = 0; i < colorSpriteGui1.size(); i++)
//    {
//        std::cout << i << std::endl;
//        assert(colorSpriteGui1[i] == colorSpriteSubTex[i]);
////        assert(colorSpriteGui1[i] == colorSpriteGui2[i]);
//    }
//
////    QImage imgGui1(pixelsGui1.data(), 196, 112, QImage::Format_RGBA8888);
////    imgGui1.save("dragongui1.png");
//
//    file.open("dragonpre-dragon-logo.png.bin", std::ios::binary);
//    file.read((char*)pixels, size);
//    file.close();
//    std::vector<unsigned char> pixelsGuiPre1(pixels, pixels + size);
//    std::vector<size_t> colorSpriteGuiPre1 = rgbaToColorTreeSprite(pixels, 196, 112);
//
//
//    std::vector<unsigned char> pixelsGuiPreSwapped1 = pixelsGuiPre1;
//    for(size_t i = 0; i < pixelsGuiPreSwapped1.size(); i += 4)
//        std::swap(pixelsGuiPreSwapped1[i], pixelsGuiPreSwapped1[i + 2]);
//
//    std::vector<size_t> colorSpriteGuiPreSwapped1 = rgbaToColorTreeSprite(pixelsGuiPreSwapped1, 196, 112);
//
////    QImage imgGuiPreSwapped1(pixelsGuiPreSwapped1.data(), 196, 112, QImage::Format_RGBA8888);
////    imgGuiPreSwapped1.save("dragonguipreswapped1.png");
//
//    assert(pixelsGuiPreSwapped1.size() == pixelsGui1.size());
//    for(size_t i = 0; i < pixelsGui1.size(); i += 4)
//        assert(pixelsGuiPreSwapped1[i] == pixelsGui1[i]);
//
//    assert(pixelsGui1.size() == pixelsSubTex.size());
//    for(size_t i = 0; i < pixelsGui1.size(); i += 4)
//    {
////        std::cout << i << std::endl;
//        assert(pixelsGui1[i + 3] == pixelsSubTex[i + 3]);
//        if(pixelsGui1[i + 3] == 255)
//        {
//            assert(pixelsGui1[i] == pixelsSubTex[i]);
//            assert(pixelsGui1[i + 1] == pixelsSubTex[i + 1]);
//            assert(pixelsGui1[i + 2] == pixelsSubTex[i + 2]);
//        }
//    }
//
//    file.open("dragon-client-dragon-logo.png.bin", std::ios::binary);
//    file.read((char*)pixels, size);
//    file.close();
//    std::vector<unsigned char> pixelsGui2(pixels, pixels + size);
//    std::vector<size_t> colorSpriteGui2 = rgbaToColorTreeSprite(pixels, 196, 112);
//
//    assert(pixelsGui1.size() == pixelsGui2.size());
//
//    file.open("dragonpre-client-dragon-logo.png.bin", std::ios::binary);
//    file.read((char*)pixels, size);
//    file.close();
//    std::vector<unsigned char> pixelsGuiPre2(pixels, pixels + size);
//    std::vector<size_t> colorSpriteGuiPre2 = rgbaToColorTreeSprite(pixels, 196, 112);
//
//
//    std::vector<unsigned char> pixelsGuiPreSwapped2 = pixelsGuiPre2;
//    for(size_t i = 0; i < pixelsGuiPreSwapped2.size(); i += 4)
//        std::swap(pixelsGuiPreSwapped2[i], pixelsGuiPreSwapped2[i + 2]);
//
//    std::vector<size_t> colorSpriteGuiPreSwapped2 = rgbaToColorTreeSprite(pixelsGuiPreSwapped2, 196, 112);
//
////    QImage imgGuiPreSwapped1(pixelsGuiPreSwapped1.data(), 196, 112, QImage::Format_RGBA8888);
////    imgGuiPreSwapped1.save("dragonguipreswapped1.png");
//
//    assert(pixelsGuiPreSwapped2.size() == pixelsGui2.size());
//    for(size_t i = 0; i < pixelsGui2.size(); i += 4)
//        assert(pixelsGuiPreSwapped2[i] == pixelsGui2[i]);
//
//    assert(pixelsGui1.size() == pixelsSubTex.size());
//
//
////    for(size_t i = 0; i < pixelsGui1.size(); i++)
////        assert(pixelsGui1[i] == pixelsGui2[i]);
//
//    delete[] pixels;
//
//    assert(colorSpriteGui2.size() == colorSpriteGui1.size());
//
//
//
////    std::vector<std::vector<size_t>> colorSpritesGui = {colorSpriteGuiPreSwapped1, colorSpriteGuiPreSwapped2};
//    std::vector<std::vector<size_t>> colorSpritesGui = {colorSpriteGui1, colorSpriteGui2};
//    std::vector<unsigned int> ids = {1, 2};
////    std::vector<unsigned int> ids = {1};
//    SequenceTree tree(colorSpritesGui, ids);
//
//    std::list<unsigned int> foundIds;
//    assert(tree.find(colorSpriteSubTex, foundIds));
//    for(unsigned int i : foundIds)
//        std::cout << "Found: " << i << std::endl;
//
//    return 0;


////    QGuiApplication* merp = new QGuiApplication(argc, argv);
////    delete merp;
////
//    std::cout << (unsigned int)QCoreApplication::instance() << std::endl;
//    std::cout << __LINE__ << std::endl;
//    {
////        QImage test(QSize(10, 10), QImage::Format_Grayscale8);
////        test.scaledToWidth(16);
////
////        QPainter p(&test);
////        QGuiApplication::palette();
//
//        std::cout << (unsigned int)QCoreApplication::instance() << std::endl;
//    }
//    std::cout << (unsigned int)QCoreApplication::instance() << std::endl;
//    std::cout << __LINE__ << std::endl;
//
//    {
//        QGuiApplication app(argc, argv);
//    }
//    std::cout << (unsigned int)QCoreApplication::instance() << std::endl;
//    std::cout << __LINE__ << std::endl;
//
//
//
//    {
//        QPalette();
////        QGuiApplication::palette();
////        QImage test(QSize(10, 10), QImage::Format_Grayscale8);
//        QPainter p;
//        p.set
////        QTransform transform = QTransform::fromScale(1, 2);
////        test.transformed(transform);
////        test.scaledToWidth(16);
//    }
//
//    return 0;


//    createFileDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateFileA"), (void*)createFile);
////    CreateFileA("derp.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
//HANDLE retVal = ((WINBASEAPI HANDLE (WINAPI *)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE))createFileDetour->getFunction())
//("derp.txt", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
//
////    return 0;
//    delete createFileDetour;
//    return 0;
//    Map2d<char, char, size_t> map;
//    map.set(1, 5, 999);
//    map.set(2, 5, 1);
//    map.set(1, 4, 6);
//    for(auto i : map)
//        std::cout << (int)i.x << "x" << (int)i.y << " " << i.v << std::endl;
//
//    auto it = map.find(1, 1);
//    if(it == map.end())
//        std::cout << ":(" << std::endl;
//    it = map.find(2, 5);
//    if(it != map.end())
//        std::cout << (*it).v << std::endl;
//    return 0;

//    while(true)
//    {
//        SHORT state = GetKeyState(VK_CONTROL);
//        SHORT isToggled = state & 1;
//        SHORT isDown = (state >> (sizeof(SHORT) * 8 - 1)) & 1;
//        std::cout << "Is down? " << isDown << std::endl;
//        std::cout << "Is toggled? " << isToggled << std::endl;
//    }
//    FontSample fontSample("Verdana", 39.f, 40.f, FontSample::Style::BOLD);
//    FontSample fontSample("Verdana", 13.f, 15.f, FontSample::Style::BOLD);
////    int merpc = 0;
////    QGuiApplication app(merpc, nullptr);
//    std::list<QImage> imgs =
//    {
//        QImage(QString("glyphCompDebug/0_4_0x0_47x68.tif")),
//        QImage(QString("glyphCompDebug/1_4_47x0_47x68.tif")),
//        QImage(QString("glyphCompDebug/2_4_94x0_45x68.tif")),
//        QImage(QString("glyphCompDebug/3_4_139x0_41x68.tif"))
//    };
//
//    for(QImage& img : imgs)
//        img = img.scaledToWidth(16);
//
//    return 0;
//
////    QImage img(QString("glyphCompDebug/0_4_0x0_47x68.tif"));
//
//    for(const QImage& img : imgs)
//    {
//        assert(img.depth() / 8 == 1);
//
//    //
//        unsigned char* unpaddedPixels = new unsigned char[img.width() * img.height()];
//        for(size_t y = 0; y < img.height(); y++)
//        {
//            const unsigned char* row = img.constScanLine(y);
//            memcpy(&unpaddedPixels[y * img.width()], row, img.width());
//        }
//
//        size_t width = img.width();
//        size_t height = img.height();
//        unsigned char* pixels = unpaddedPixels;
//        size_t sum = 0;
//        for(size_t i = 0; i < width * height; i++)
//            sum += pixels[i];
//
//        size_t minDiff = -1;
//        const FontSample::Glyph* mostSimilarGlyph = nullptr;
//        for(const FontSample::Glyph& lhs : fontSample.getGlyphs())
//        {
//            size_t diff;
//            if(lhs.width <= width && lhs.height <= height)
//                diff = movingWindowMinDiff(lhs.data.data(), lhs.width, lhs.height, pixels, width, height, sum);
//            else if(width <= lhs.width && height <= lhs.height)
//                diff = movingWindowMinDiff(pixels, width, height, lhs.data.data(), lhs.width, lhs.height);
//
//            if(diff < minDiff)
//            {
//                if(minDiff == 0)
//                    return lhs.character;
//
//                minDiff = diff;
//                mostSimilarGlyph = &lhs;
//            }
//        }
//
//        assert(mostSimilarGlyph);
//        std::cout << mostSimilarGlyph->character << std::endl;
//        std::cout << "Ps: " << mostSimilarGlyph->pointSize << std::endl;
//    }
//
//    return 0;


//    QImage knownF(QString("glyphCompDebug/f_7.9_b.bmp"));
//    assert(knownF.depth() / 8 == 1);
//    unsigned char* unpaddedPixelsF = new unsigned char[knownF.width() * knownF.height()];
//    for(size_t y = 0; y < knownF.height(); y++)
//    {
//        const unsigned char* row = knownF.constScanLine(y);
//        memcpy(&unpaddedPixelsF[y * knownF.width()], row, knownF.width());
//    }

//    unsigned char character = getChar(unpaddedPixels, img.width(), img.height());
////    size_t diff = movingWindowMinDiff(unpaddedPixelsF, knownF.width(), knownF.height(), unpaddedPixels, img.width(), img.height());
////    std::cout << diff << std::endl;
//    std::cout << character << std::endl;
//    return 0;

//    Glyph unknownT;
//    unknownT.width = img.width();
//    unknownT.height = img.height();
//    size_t rowSize = unknownT.width * unknownT.bytesPerPixel;
//    size_t size = rowSize * unknownT.height;
//    unknownT.data.resize(size);
//    for(size_t y = 0; y < unknownT.height; y++)
//    {
//        const unsigned char* row = img.constScanLine(y);
//        memcpy(&unknownT.data[y * rowSize], row, rowSize);
//    }
//
//    img.load(QString("glyphCompDebug/1_6.4_n.bmp"));
//    Glyph known1;
//    known1.width = img.width();
//    known1.height = img.height();
//    assert(img.depth() / 8 == known1.bytesPerPixel);
//    rowSize = known1.width * known1.bytesPerPixel;
//    size = rowSize * known1.height;
//    known1.data.resize(size);
//    for(size_t y = 0; y < known1.height; y++)
//    {
//        const unsigned char* row = img.constScanLine(y);
//        memcpy(&known1.data[y * rowSize], row, rowSize);
//    }
//
//    img.load(QString("glyphCompDebug/t_7.9_b.bmp"));
//    Glyph knownT;
//    knownT.width = img.width();
//    knownT.height = img.height();
//    assert(img.depth() / 8 == knownT.bytesPerPixel);
//    rowSize = knownT.width * knownT.bytesPerPixel;
//    size = rowSize * knownT.height;
//    knownT.data.resize(size);
//    for(size_t y = 0; y < knownT.height; y++)
//    {
//        const unsigned char* row = img.constScanLine(y);
//        memcpy(&knownT.data[y * rowSize], row, rowSize);
//    }
//
//    size_t diffKnown1 = glyphDiff(known1, unknownT);
//    size_t diffKnownT = glyphDiff(knownT, unknownT);
//
//
//    return 0;
//

//    std::list<Glyph> tibiaGlyphs;
//    std::list<std::string> names;
//    forEachFile("glyphs", [&](const std::string& file)
//    {
//        QImage img(QString::fromStdString(file));
//        Glyph g;
//        g.width = img.width();
//        g.height = img.height();
//        assert(img.depth() / 8 == g.bytesPerPixel);
//        const size_t ROW_SIZE = g.width * g.bytesPerPixel;
//        const size_t SIZE = ROW_SIZE * g.height;
//        g.data.resize(SIZE);
//        for(size_t y = 0; y < g.height; y++)
//        {
//            const unsigned char* row = img.constScanLine(y);
//            memcpy(&g.data[y * ROW_SIZE], row, ROW_SIZE);
//        }
//
//        tibiaGlyphs.push_back(g);
//        names.push_back(file);
//    });
//
////    createGlyphSamples();
//    FontSample verdana("Verdana", 6.f, 10.f, FontSample::Style::BOLD);
//
//    for(Glyph& rhs : tibiaGlyphs)
//    {
//        size_t minDiff = -1;
//        const FontSample::Glyph* mostSimilarGlyph = nullptr;
//        for(const FontSample::Glyph& lhs : verdana.getGlyphs())
//        {
//            if(lhs.width <= rhs.width && lhs.height <= rhs.height)
//            {
//                size_t diff = glyphDiff(lhs, rhs);
//
//                if(diff < minDiff)
//                {
//                    minDiff = diff;
//                    mostSimilarGlyph = &lhs;
//                    if(minDiff == 0)
//                        break;
//                }
//            }
//        }
//
//        assert(mostSimilarGlyph);
//        rhs.character = mostSimilarGlyph->character;
//        rhs.isBold = mostSimilarGlyph->style & FontSample::Style::BOLD;
//        rhs.pointSize = mostSimilarGlyph->pointSize;
//        rhs.diff = minDiff;
//    }
//
//    auto nameIt = names.begin();
//    auto tibiaGlyphIt = tibiaGlyphs.begin();
//    while(nameIt != names.end())
//    {
//        if(tibiaGlyphIt->diff > 0)
////        if(!tibiaGlyphIt->isBold)
//            std::cout << *nameIt << " (diff " << tibiaGlyphIt->diff << "): " << tibiaGlyphIt->character << "_" << tibiaGlyphIt->pointSize << "_" << (tibiaGlyphIt->isBold ? "b" : "n") << std::endl;
//
//        nameIt++;
//        tibiaGlyphIt++;
//    }
//
//
//    while(true);
//
//    return 0;

//    // 0x0
//    // 0x448
//    // 611x0
//    // 611x448
//    float worldPos[4];
//    worldPos[0] = 611;
//    worldPos[1] = 448;
//    worldPos[2] = 0.f;
//    worldPos[3] = 1.f;
//
//    float transform[4][4] =
//    {
//        {0.0025f,   0.f,            0.f, 0.f},
//        {0.f,       -0.00333333f,   0.f, 0.f},
//        {0.f,       0.f,            1.f, 0.f},
//        {-0.985f,   0.983333,       0.f, 1.f}
//    };
//
//    std::array<float, 4> ndcPos = mulTransposedMatVec(transform, worldPos);
//    for(float& f : ndcPos)
//        f /= ndcPos[3];
//
//    unsigned short posX = 400 * (ndcPos[0] + 1.f);
//    unsigned short posY = 300 * (1.f - ndcPos[1]);
//
//    std::cout << "View: " << posX << "x" << posY << std::endl;
//
//    return 0;

//    int l = 21692417;
//    int l = 1095434241;
//    int fakeLDown = 19529729;
//    int realLDown = 2752513;
//    int fakeLUp = 19529729;
//    int realLUp = -1070989311;
//
//    auto printEventStuff = [](int l)
//    {
//        int repeatCount = (0xffff & l);
//        int scanCode = ((0xff << 16) & l) >> 16;
//        int isExtended = ((1 << 24) & l) >> 24;
//        int reserved = ((0xf << 25) & l) >> 25;
//        int contextCode = ((1 << 29) & l) >> 29;
//        int previousKeyState = ((1 << 30) & l) >> 30;
//        int transitionState = ((1 << 31) & l) >> 31;
//
//        std::cout << MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC) << std::endl;
//        std::cout << MapVirtualKey(scanCode, MAPVK_VSC_TO_VK) << std::endl;
//        std::cout   << "Repeat count: " << repeatCount << std::endl
//                    << "Scan code: " << scanCode << std::endl
//                    << "Is extended: " << isExtended << std::endl
//                    << "Reserved: " << reserved << std::endl
//                    << "Context code: " << contextCode << std::endl
//                    << "Previous key state: " << previousKeyState << std::endl
//                    << "Transition state: " << transitionState << std::endl;
//    };
//
//    std::cout << "FAKE L DOWN: " << std::endl;
//    printEventStuff(fakeLDown);
//    std::cout << std::endl;
//
//    std::cout << "REAL L DOWN: " << std::endl;
//    printEventStuff(realLDown);
//    std::cout << std::endl;
//
//    std::cout << "FAKE L UP: " << std::endl;
//    printEventStuff(fakeLUp);
//    std::cout << std::endl;
//
//    std::cout << "REAL L UP: " << std::endl;
//    printEventStuff(realLUp);
//    std::cout << std::endl;
//
//
//
//    return 0;


//    CatalogContent cat("tibia/packages/Tibia/assets/catalog-content.json");
//
//    const std::list<CatalogContent::Appearances>& appearances = cat.getAppearances();
//    assert(appearances.size() == 1);
//    AppearancesReader reader(appearances.front().path);


//    SpriteReader reader(cat);
//
//
//    std::vector<std::vector<size_t>> colorSprites;
//    std::vector<std::vector<size_t>> transparencySprites;
//    std::vector<size_t> ids;
//
//
//    GraphicsResourceReader* graphicsResourceReader = new GraphicsResourceReader("tibia/packages/Tibia/bin/graphics_resources.rcc");
//    const std::vector<GraphicsResourceReader::GraphicsResource>& gResources = graphicsResourceReader->getGraphicsResources();
//
//    for(size_t i = 0; i < gResources.size(); i++)
//    {
//        const GraphicsResourceReader::GraphicsResource& gRes = gResources[i];
//        const size_t ID = i + Constants::GRAPHICS_RESOURCE_ID_START;
//        assert(ID >= Constants::GRAPHICS_RESOURCE_ID_START && ID <= Constants::GRAPHICS_RESOURCE_ID_END);
//
//        std::vector<size_t> colorTreeSprite = rgbaToColorTreeSprite(gRes.pixels, gRes.width, gRes.height);
//        if(colorTreeSprite.size() > 0)
//        {
//            bool isDuplicate = false;
//            for(const std::vector<size_t>& spr1 : colorSprites)
//            {
//                if(spr1.size() == colorTreeSprite.size())
//                {
//                    size_t j = 0;
//                    while(j < spr1.size())
//                    {
//                        if(spr1[j] != colorTreeSprite[j])
//                            break;
//
//                        j++;
//                    }
//
//                    if(j == spr1.size())
//                    {
//                        isDuplicate = true;
//                        break;
//                    }
//                }
//            }
//
//            if(!isDuplicate)
//            {
//
//                colorSprites.push_back(colorTreeSprite);
//                transparencySprites.push_back(packBytes(rgbaToTransparencyTreeSprite(gRes.pixels, gRes.width, gRes.height)));
//                ids.push_back(ID);
//            }
//        }
//    }
//
//    std::cout << "Done with graphics resources." << std::endl;
//
//
//    reader.forEachSprite([&](const SpriteReader::Sprite& spr)
//    {
//        size_t pixelWidth = spr.tileWidth * Constants::TILE_PIXEL_WIDTH;
//        size_t pixelHeight = spr.tileHeight * Constants::TILE_PIXEL_HEIGHT;
//        if(spr.id % 1000 == 0)
//            std::cout << spr.id << std::endl;
//
//        assert(spr.id >= Constants::SPRITE_ID_START && spr.id <= Constants::SPRITE_ID_END);
//
//        std::vector<size_t> colorSprite = rgbaToColorTreeSprite(spr.pixels, pixelWidth, pixelHeight);
//        if(colorSprite.size() > 0)
//        {
//            colorSprites.push_back(colorSprite);
//            transparencySprites.push_back(packBytes(rgbaToTransparencyTreeSprite(spr.pixels, pixelWidth, pixelHeight)));
//            ids.push_back(spr.id);
//        }
//    });
//
//
//    std::cout << "Building colorTree... ";
//    SequenceTree* colorTree = new SequenceTree(colorSprites, ids);
//    std::cout << "Done " << std::endl;
//    colorTree->writeToBinaryFile("color-tree.bin");
//    delete colorTree;
//
//    std::cout << "Building transparencyTree... ";
//    SequenceTree* transparencyTree = new SequenceTree(transparencySprites, ids);
//    std::cout << "Done" << std::endl;
//    transparencyTree->writeToBinaryFile("transparency-tree.bin");
//    delete transparencyTree;
//
//
//    colorTree = new SequenceTree("color-tree.bin");
//    transparencyTree = new SequenceTree("transparency-tree.bin");
//    std::cout << "Testing..." << std::endl;
//    clock_t start;
//    clock_t end;
//
//    start = clock();
//    for(size_t i = 0; i < colorSprites.size(); i++)
//    {
//        const std::vector<size_t>& colorSprite = colorSprites[i];
//        std::list<unsigned int> foundIds;
//        if(!colorTree->find(colorSprite, foundIds))
//        {
//            std::cout << "FAILED COLOR: " << i << std::endl;
//            std::cout << "Sprite id: " << ids[i] << std::endl;
//            throw 1;
//        }
//        else if(foundIds.size() > 1)
//        {
////            std::cout << "DUPLICATE COLOR: " << i << std::endl;
////            std::cout << "Sprite ids: ";
////            for(size_t id : foundIds)
////                std::cout << id << " ";
////            std::cout << std::endl;
//
//            const std::vector<size_t>& transparencySprite = transparencySprites[i];
//            std::list<size_t> foundTransparencyIds;
//            if(!transparencyTree->find(transparencySprite, foundTransparencyIds))
//            {
//                std::cout << "FAILED TRANSPARENCY: " << i << std::endl;
//                std::cout << "Sprite id: " << ids[i] << std::endl;
//                throw 1;
//            }
//            else
//            {
//                std::list<size_t> matchingIds;
//                for(size_t id : foundIds)
//                {
//                    auto result = std::find(foundTransparencyIds.begin(), foundTransparencyIds.end(), id);
//                    if(result != foundTransparencyIds.end())
//                        matchingIds.push_back(id);
//                }
//
////                if(matchingIds.size() > 1)
////                {
////                    std::cout << "NON-UNIQUE SPRITEs: " << i << std::endl;
////                    std::cout << "Sprite ids: ";
////                    for(size_t id : matchingIds)
////                        std::cout << id << " ";
////                    std::cout << std::endl;
////                }
//                if(matchingIds.size() == 0)
//                {
//                    std::cout << "MISMATCHING COLOR AND TRANSPARENCY: " << i << std::endl;
//                    throw 1;
//                }
//            }
//        }
////
////        const std::vector<size_t>& transparencySprite = transparencySprites[i];
////        if(!transparencyTree->find(transparencySprite, foundIds))
////        {
////            std::cout << "FAILED TRANSPARENCY: " << i << std::endl;
////            std::cout << "Sprite id: " << ids[i] << std::endl;
////        }
////        else if(foundIds.size() > 1)
////        {
//////            std::cout << "DUPLICATE TRANSPARENCY: " << i << std::endl;
//////            std::cout << "Sprite ids: ";
//////            for(size_t id : foundIds)
//////                std::cout << id << " ";
//////            std::cout << std::endl;
////        }
//    }
//    end = clock();
//
//    std::cout << "Done" << std::endl;
//    std::cout << "Took " << end - start << " cycles." << std::endl;
//
//    colorSprites.resize(0);
//    transparencySprites.resize(0);
//    ids.resize(0);
//    delete graphicsResourceReader;
//    delete colorTree;
//    delete transparencyTree;
//    Sleep(10000);
//    reader.forEachSprite([](const SpriteReader::Sprite& spr)
//    {
//        size_t pixelWidth = spr.tileWidth * Constants::TILE_PIXEL_WIDTH;
//        size_t pixelHeight = spr.tileHeight * Constants::TILE_PIXEL_HEIGHT;
//        QImage sprite(spr.pixels, pixelWidth, pixelHeight, QImage::Format_RGBA8888);
//
//        if(sprite.isNull())
//            THROW_RUNTIME_ERROR("Failed to create image object.");
//
//        std::stringstream sstream;
//        sstream << "spriteOutTest/" << spr.id << ".png";
//        if(!sprite.save(QString::fromStdString(sstream.str())))
//            THROW_RUNTIME_ERROR("Failed to save sprite image to '" + sstream.str() + "'.");
//    });

//    GraphicsResourceReader graphicsResourceReader("tibia/packages/Tibia/bin/graphics_resources.rcc");
//
//    const std::vector<GraphicsResourceReader::GraphicsResource>& resources = graphicsResourceReader.getGraphicsResources();
//    const GraphicsResourceReader::GraphicsResource& resource = resources[1];
//    std::cout << resource.name << std::endl;
//    for(size_t y = 0, i = 0; y < resource.height; y++)
//    {
//        for(size_t x = 0; x < resource.width * Constants::BYTES_PER_PIXEL_RGBA; x++)
//        {
//            if(resource.pixels[i] == 0)
//                std::cout << "000" << " ";
//            else
//                std::cout << (int)resource.pixels[i] << " ";
//
//            i++;
//        }
//        std::cout << std::endl;
//    }


}


