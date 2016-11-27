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
///////////////////////////////////
// Internal ShankBot headers
#include "graphics/FontSample.hpp"
#include "utility/FunctionDetour.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
#include <cassert>
#include <iostream>
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QGuiApplication"
#include "QtGui/QSurfaceFormat"
#include "QtGui/QOpenGLContext"
#include "QtGui/QOffscreenSurface"
#include "QtGui/QOpenGLPaintDevice"
#include "QtGui/QPainter"
///////////////////////////////////

///////////////////////////////////
// OpenGL
#include <GL/gl.h>
///////////////////////////////////

namespace FontSamplePrivate
{
    std::list<FontSample::Glyph>* currentGlyphs = nullptr;
    FunctionDetour* subTexImgDetour = nullptr;
    char currentCharacter = 0;
    float currentPointSize = 0;
    FontSample::Style currentStyle = FontSample::Style::NORMAL;

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
        FontSample::Glyph glyph;
        glyph.width = width;
        glyph.height = height;
        glyph.character = currentCharacter;
        glyph.pointSize = currentPointSize;
        glyph.style = currentStyle;
        size_t paddedWidth = width + (4 - (width % 4)) % 4;
        size_t paddedRowSize = paddedWidth * glyph.bytesPerPixel;
        size_t rowSize = glyph.width * glyph.bytesPerPixel;
        size_t size = rowSize * glyph.height;
        glyph.data.resize(size);
        for(size_t y = 0; y < glyph.height; y++)
        {
            memcpy(&glyph.data[y * rowSize], (char*)pixels + y * paddedRowSize, rowSize);
        }

        for(const unsigned char& c : glyph.data)
            glyph.sum += c;



    //    glyph.data.assign((unsigned char*)pixels, (unsigned char*)pixels + GLYPH_SIZE);
    //    if(currentCharacter == 't' && currentPointSize > 7.89f && currentPointSize < 7.91f && isBold)
    //    {
    //        QImage img(QString("glyphCompDebug/1_10_1_60.tif"));
    //        Glyph unknownT;
    //        unknownT.width = img.width();
    //        unknownT.height = img.height();
    //        assert(img.depth() / 8 == unknownT.bytesPerPixel);
    //        rowSize = unknownT.width * unknownT.bytesPerPixel;
    //        size = rowSize * unknownT.height;
    //        unknownT.data.resize(size);
    //        for(size_t y = 0; y < unknownT.height; y++)
    //        {
    //            const unsigned char* row = img.constScanLine(y);
    //            memcpy(&unknownT.data[y * rowSize], row, rowSize);
    //        }
    //
    //        std::cout << glyphDiff(glyph, unknownT) << std::endl;
    //        assert(false);
    //    }
        currentGlyphs->push_back(glyph);

////        if((currentCharacter >= '0' && currentCharacter <= '9') || (currentCharacter >= 'A' || currentCharacter <= 'Z') || (currentCharacter >= 'a' && currentCharacter <= 'z'))
//        {
////            std::cout << subCount << " subtex: " << format << " " << type << " " << width << "x" << height << std::endl;
//        //    return;
//            QImage img((unsigned char*)pixels, width, height, QImage::Format_Grayscale8);
//            std::stringstream sstream;
//        //    sstream << "qtSubTex/" << subCount++ << ".bmp";
//            sstream << "qtSubTex/" << currentCharacter << "_" << currentPointSize << "_" << (currentStyle == FontSample::Style::BOLD ? "b" : "n") << ".bmp";
//
//            img.save(QString::fromStdString(sstream.str()));
//        }

//        subTexImgDetour->disable();
//        ((void APIENTRY (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
//            subTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, type, pixels);
//        subTexImgDetour->enable();
    }
}

using namespace FontSamplePrivate;

FontSample::FontSample(std::string family, float minPointSize, float maxPointSize, unsigned char styleFlags)
{
    createGlyphSamples(family, minPointSize, maxPointSize, styleFlags);
}

void FontSample::createGlyphSamples(std::string family, float minPointSize, float maxPointSize, unsigned char styleFlags)
{
    int argc = 0;
    QGuiApplication app(argc, nullptr);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);

    QOpenGLContext openGLContext;
    openGLContext.create();
    openGLContext.setFormat(surfaceFormat);


    QOffscreenSurface surface;
    surface.create();
    surface.setFormat(openGLContext.format());
    openGLContext.makeCurrent(&surface);


    HINSTANCE opengl32 = GetModuleHandle("OPENGL32.DLL");
    assert(opengl32 != NULL);
    void* texSubFunc = (void*)GetProcAddress(opengl32, "glTexSubImage2D");
    assert(texSubFunc != NULL);
    subTexImgDetour = new FunctionDetour((void*)texSubFunc, (void*)subTexImg);
    currentGlyphs = &mGlyphs;

    QOpenGLPaintDevice framebuffer(1, 1);
    QPainter painter(&framebuffer);

    QFont font;
    font.setFamily(QString::fromStdString(family));
    font.setStyleStrategy(QFont::NoAntialias);

    for(float pointSize = minPointSize; pointSize < maxPointSize; pointSize += 0.1f)
    {
        currentPointSize = pointSize;
        font.setPointSizeF(currentPointSize);

        if(styleFlags & Style::NORMAL)
        {
            currentStyle = Style::NORMAL;
            font.setBold(false);
            font.setItalic(false);
            painter.setFont(font);
            generateGlyphs(painter);
        }

        if(styleFlags & Style::BOLD)
        {
            currentStyle = Style::BOLD;
            font.setBold(true);
            font.setItalic(false);
            painter.setFont(font);
            generateGlyphs(painter);
        }

        if(styleFlags & Style::ITALIC)
        {
            currentStyle = Style::ITALIC;
            font.setBold(false);
            font.setItalic(true);
            painter.setFont(font);
            generateGlyphs(painter);
        }
    }

    delete subTexImgDetour;
    subTexImgDetour = nullptr;
    currentGlyphs = nullptr;
}

void FontSample::generateGlyphs(QPainter& painter)
{
    for(char c = 0x20; c <= 0x7e; c++)
    {
        currentCharacter = c;
        painter.drawText(0, 0, QString(currentCharacter));
    }
}

const std::list<FontSample::Glyph>& FontSample::getGlyphs() const
{
    return mGlyphs;
}

