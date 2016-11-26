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
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cstdio>
#include "FunctionDetour.hpp"
#include "SharedMemoryProtocol.hpp"
#include "Vertex.hpp"
//
////#include <GL/gl.h>
////#include <GL/glext.h>
#include "GL/glew.h"

#include <cassert>
#include <fstream>
#include <algorithm>
#include <list>

#include <sstream>
#define CHECK_GL_ERROR() {GLenum GL_ERROR_NR = getError(); if(GL_ERROR_NR != 0){std::cout << "GL ERROR " << "(" << __LINE__ << "): " << GL_ERROR_NR << std::endl; assert(false);}}

using namespace GraphicsLayer;

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


FunctionDetour* loadLibDetour = nullptr;
FunctionDetour* genTexDetour = nullptr;
FunctionDetour* swapBufDetour = nullptr;
FunctionDetour* delTexDetour = nullptr;
FunctionDetour* texImgDetour = nullptr;
FunctionDetour* subTexImgDetour = nullptr;
FunctionDetour* compTexImgDetour = nullptr;
FunctionDetour* subCompTexImgDetour = nullptr;
FunctionDetour* bufDatDetour = nullptr;
FunctionDetour* bufSubDatDetour = nullptr;
FunctionDetour* bindTexDetour = nullptr;
FunctionDetour* drawArraysDetour = nullptr;
FunctionDetour* drawElementsDetour = nullptr;
FunctionDetour* bindFramebufferDetour = nullptr;
FunctionDetour* framebufferTexDetour = nullptr;
FunctionDetour* drawBufferDetour = nullptr;
FunctionDetour* activeTextureDetour = nullptr;
FunctionDetour* vertexAttribPointerDetour = nullptr;
FunctionDetour* bindBufferDetour = nullptr;
FunctionDetour* createContextDetour = nullptr;
FunctionDetour* makeCurrentDetour = nullptr;
FunctionDetour* viewportDetour = nullptr;
FunctionDetour* framebufferRenderbufferDetour = nullptr;
FunctionDetour* useProgramDetour = nullptr;
FunctionDetour* disableDetour = nullptr;
FunctionDetour* enableiDetour = nullptr;
FunctionDetour* enableDetour = nullptr;
FunctionDetour* enableVertexAttribArrayDetour = nullptr;
FunctionDetour* createProgramDetour = nullptr;
FunctionDetour* shaderSourceDetour = nullptr;
FunctionDetour* attachShaderDetour = nullptr;
FunctionDetour* uniform3fvDetour = nullptr;
FunctionDetour* uniform4fvDetour = nullptr;
FunctionDetour* uniform3uivDetour = nullptr;
FunctionDetour* uniform4uivDetour = nullptr;
FunctionDetour* uniformMatrix3fvDetour = nullptr;
FunctionDetour* uniformMatrix4fvDetour = nullptr;
FunctionDetour* copyTexImageDetour = nullptr;
FunctionDetour* copyTexSubImageDetour = nullptr;
FunctionDetour* readBufferDetour = nullptr;
FunctionDetour* bindBufferBaseDetour = nullptr;
FunctionDetour* bindBufferRangeDetour = nullptr;
FunctionDetour* bindVertexArrayDetour = nullptr;
FunctionDetour* bindVertexBufferDetour = nullptr;
FunctionDetour* vertexArrayVertexBufferDetour = nullptr;
FunctionDetour* blendColorDetour = nullptr;
FunctionDetour* vertexPointerDetour = nullptr;
FunctionDetour* enableClientStateDetour = nullptr;
FunctionDetour* scissorDetour = nullptr;
FunctionDetour* pixelStoreiDetour = nullptr;
FunctionDetour* pixelStorefDetour = nullptr;

FunctionDetour* createFileDetour = nullptr;
FunctionDetour* readFileDetour = nullptr;
FunctionDetour* readFileExDetour = nullptr;
FunctionDetour* writeFileDetour = nullptr;
FunctionDetour* writeFileExDetour = nullptr;
FunctionDetour* virtualProtectDetour = nullptr;
FunctionDetour* virtualProtectExDetour = nullptr;
FunctionDetour* virtualAllocDetour = nullptr;
FunctionDetour* virtualAllocExDetour = nullptr;

FunctionDetour* createWindowExWDetour = nullptr;
FunctionDetour* createWindowExADetour = nullptr;
FunctionDetour* createWindowADetour = nullptr;
FunctionDetour* createWindowWDetour = nullptr;
FunctionDetour* peekMessageDetour = nullptr;
FunctionDetour* getCursorPosDetour = nullptr;
FunctionDetour* getMessageDetour = nullptr;
FunctionDetour* getKeyStateDetour = nullptr;
FunctionDetour* getAsyncKeyStateDetour = nullptr;
FunctionDetour* getKeyboardStateDetour = nullptr;
FunctionDetour* translateMessageDetour = nullptr;
FunctionDetour* translateAcceleratorDetour = nullptr;
FunctionDetour* getQueueStatusDetour = nullptr;
FunctionDetour* getInputStateDetour = nullptr;
FunctionDetour* getClientRectDetour = nullptr;
FunctionDetour* getWindowRectDetour = nullptr;
FunctionDetour* screenToClientDetour = nullptr;
FunctionDetour* isWindowVisibleDetour = nullptr;
FunctionDetour* getFocusDetour = nullptr;
FunctionDetour* registerClassWDetour = nullptr;
FunctionDetour* registerClassADetour = nullptr;
FunctionDetour* registerClassExWDetour = nullptr;
FunctionDetour* registerClassExADetour = nullptr;

PROC (WINAPI *getProc)(LPCSTR) = nullptr;

GLenum APIENTRY (*getError)() = nullptr;
void APIENTRY (*getIntegerv)(GLenum, GLint*) = nullptr;
void APIENTRY (*getTexImage)(GLenum, GLint, GLenum, GLenum, void*) = nullptr;
void APIENTRY (*subBufDat)(GLenum, GLintptr, GLsizeiptr, const GLvoid*) = nullptr;
void APIENTRY (*getSubBufDat)(GLenum, GLintptr, GLsizeiptr, GLvoid*) = nullptr;
void APIENTRY (*getTexLevelParameteriv)(GLenum, GLint, GLenum, GLint*) = nullptr;
//void APIENTRY (*pixelStorei)(GLenum, GLint) = nullptr;
void APIENTRY (*blitNamedFramebuffer)
(
    GLuint readFramebuffer,
    GLuint drawFramebuffer,
    GLint srcX0,
    GLint srcY0,
    GLint srcX1,
    GLint srcY1,
    GLint dstX0,
    GLint dstY0,
    GLint dstX1,
    GLint dstY1,
    GLbitfield mask,
    GLenum filter
) = nullptr;
void APIENTRY (*blitFramebuffer)
(
    GLint srcX0,
    GLint srcY0,
    GLint srcX1,
    GLint srcY1,
    GLint dstX0,
    GLint dstY0,
    GLint dstX1,
    GLint dstY1,
    GLbitfield mask,
    GLenum filter
) = nullptr;
void APIENTRY (*genFramebuffers)(GLsizei, GLuint*) = nullptr;
void APIENTRY (*genRenderbuffers)(GLsizei, GLuint*) = nullptr;
void APIENTRY (*bindRenderbuffer)(GLenum, GLuint) = nullptr;
void APIENTRY (*renderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei) = nullptr;
void APIENTRY (*framebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint) = nullptr;
GLenum APIENTRY (*checkFramebufferStatus)(GLenum) = nullptr;
void APIENTRY (*drawBuffers)(GLsizei, const GLenum*) = nullptr;
void APIENTRY (*texParameteri)(GLenum, GLenum, GLint) = nullptr;
void APIENTRY (*clear)(GLbitfield) = nullptr;


#include <map>
#include <vector>

namespace GraphicsMonitor
{
    struct Texture
    {
        int width;
        int height;
        GLenum format;
    };

    size_t swapCount = 0;
    std::map<GLuint, Texture> textures;
//    std::map<GLenum, GLuint> targets;

    GLuint boundTexture = 0;
    GLuint boundFramebuffer = 0;
    GLuint boundBuffer = 0;
    GLuint boundRenderbuffer = 0;
    GLuint boundProgram = 0;
    Color currentBlendColor;


    struct TextureUnit
    {
        std::map<GLenum, GLuint> targets;
        GLenum id;
    };

    class TextureUnitHolder
    {
        public:
            explicit TextureUnitHolder();

            void setTexture(GLenum target, GLuint texture);
            GLuint getTexture(GLenum target) const;
            void setActiveTextureUnit(GLenum unit);

            // DEBUG
            const std::vector<TextureUnit>& getTextureUnits() const;

        private:
            std::vector<TextureUnit> mTextureUnits;
            TextureUnit* mActiveTextureUnit;
    };

    TextureUnitHolder::TextureUnitHolder()
    {
        TextureUnit unit0;
        unit0.id = GL_TEXTURE0;
        mTextureUnits.push_back(unit0);
        mActiveTextureUnit = &mTextureUnits[0];
    }

    void TextureUnitHolder::setTexture(GLenum target, GLuint texture)
    {
        mActiveTextureUnit->targets[target] = texture;
    }

    void TextureUnitHolder::setActiveTextureUnit(GLenum unit)
    {
        if(unit < GL_TEXTURE0)
            throw std::runtime_error("Invalid argument in TextureUnitHolder::setActiveTextureUnit");

        size_t index = unit - GL_TEXTURE0;
        if(index >= mTextureUnits.size())
        {
            size_t oldSize = mTextureUnits.size();
            mTextureUnits.resize(index + 1);
            for(size_t i = oldSize; i < mTextureUnits.size(); i++)
                mTextureUnits[i].id = i + GL_TEXTURE0;
        }
        mActiveTextureUnit = &mTextureUnits[index];
    }

    GLuint TextureUnitHolder::getTexture(GLenum target) const
    {
        return mActiveTextureUnit->targets[target];
    }

    const std::vector<TextureUnit>& TextureUnitHolder::getTextureUnits() const
    {
        return mTextureUnits;
    }



    std::map<GLuint, GLuint> framebufferTextureAttachment;
    TextureUnitHolder textureUnits;
    GLuint sceneBuffer = 0;

//    std::map<GLuint, SharedMemoryProtocol::VertexBuffer> buffers;
//    std::list<GLuint> updatedBuffers;

//    std::vector<SharedMemoryProtocol::PixelData> pixelData;
//    std::vector<unsigned char> pixelBuffer;
//
//    std::vector<SharedMemoryProtocol::VertexBufferWrite> bufferWrites;
//    std::vector<char> vertexBuffer;
//    std::vector<SharedMemoryProtocol::VertexBufferInfo> bufferInfos;
//    std::vector<SharedMemoryProtocol::DrawCall> drawCalls;
//    std::vector<SharedMemoryProtocol::TextureData> textureData;
//
//    std::vector<SharedMemoryProtocol::Frame> frames;
    std::vector<char> dataBuffer;
    size_t currentFrameIndex = 0;


    std::map<GLuint, SharedMemoryProtocol::VertexBufferWrite> vertexBuffers;

    GLuint tileSheetTextureId = 0;
    GLuint unshadedViewTextureId = 0;

    enum TextureBuffers : unsigned char
    {
        SHADED,
        UNSHADED,
        SHADING_MAP,
        NUM_BUFFERS
    };
    std::vector<GLuint> textureBuffers;

    bool areExtendedOpenGlFunctionsHooked = false;

    SharedMemoryProtocol::SharedMemorySegment* shm = nullptr;

//    float peakDrawCalls = 0.f;
//    float peakPixelData = 0.f;
//    float peakBufferInfo = 0.f;
//    float peakBufferWrite = 0.f;
//    float peakBuffer = 0.f;
//    float peakPixel = 0.f;
    float peakDataOccupancy = 0.f;


    unsigned short currentViewportWidth = 0;
    unsigned short currentViewportHeight = 0;
    unsigned int currentUnpackAlignment = 4;

    std::map<GLuint, GLuint> vaoToVbo;
    GLuint boundVertexArray = 0;

    std::ofstream log;


    GLuint rescaleFramebuffer = 0;
    GLuint rescaleRenderbufferColor = 0;
    GLuint rescaleRenderbufferDepth = 0;


    int windowX = 0;
    int windowY = 0;
    unsigned int windowWidth = 0;
    unsigned int windowHeight = 0;
    unsigned int clientWidth = 0;
    unsigned int clientHeight = 0;
    unsigned int widthOffset = 0;
    unsigned int heightOffset = 0;

    int scissorX = 0;
    int scissorY = 0;
    unsigned int scissorWidth = 0;
    unsigned int scissorHeight = 0;

    std::map<HANDLE, std::string> fileHandleToPath;
    std::vector<char> ioBuffer;
    HANDLE ioBufferLock = NULL;
    bool isIoBufferLocked = false;
}

#ifdef NDEBUG
#define WRITE_LOG() ((void)0)
#define WRITE_LOG_STEP() ((void)0)
#else
#define WRITE_LOG() GraphicsMonitor::log << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")" << std::endl
#define WRITE_LOG_STEP() GraphicsMonitor::log << "\t" << __LINE__ << std::endl
#endif

#define assert(v) if(!(v)) {std::cout << __LINE__ << std::endl; throw 1;}

SharedMemoryProtocol::Frame* getCurrentFrame()
{
    using namespace GraphicsMonitor;
    return (SharedMemoryProtocol::Frame*)((char*)dataBuffer.data() + currentFrameIndex);
}

void createNewFrame()
{
    using namespace GraphicsMonitor;
    SharedMemoryProtocol::Frame f;
    currentFrameIndex = dataBuffer.size();
    dataBuffer.insert(dataBuffer.end(), (char*)&f, (char*)&f + sizeof(f));
}

void appendToDataBuffer(const char* data, size_t size)
{
    using namespace GraphicsMonitor;
    getCurrentFrame()->size += size;
    dataBuffer.insert(dataBuffer.end(), data, data + size);
}

template<typename T>
void appendToDataBuffer(const T& message)
{
    appendToDataBuffer((char*)&message, sizeof(message));
}

void APIENTRY viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;
    assert(x == 0 && y == 0);
    currentViewportWidth = width;
    currentViewportHeight = height;

//    if(width > 1500 && height > 900)
//    {
//        width -= 200;
//        height -= 200;
//    }
//
//    std::cout << x << "x" << y << " (" << width << "x" << height << ")" << std::endl;
    viewportDetour->disable();
    ((void APIENTRY (*)(GLint, GLint, GLsizei, GLsizei))viewportDetour->getFunction())(x, y, width, height);
    viewportDetour->enable();
}

void APIENTRY pixelStoref(GLenum pname, GLfloat param)
{
    std::cout << "PIXEL STORE F: " << pname << " " << param << std::endl;

    pixelStorefDetour->disable();
    ((void APIENTRY (*)(GLenum, GLfloat))pixelStorefDetour->getFunction())(pname, param);
    pixelStorefDetour->enable();
}

void APIENTRY pixelStorei(GLenum pname, GLint param)
{
    if(pname == GL_UNPACK_ALIGNMENT)
        GraphicsMonitor::currentUnpackAlignment = param;

    pixelStoreiDetour->disable();
    ((void APIENTRY (*)(GLenum, GLint))pixelStoreiDetour->getFunction())(pname, param);
    pixelStoreiDetour->enable();
}

void APIENTRY useProgram(GLuint program)
{
    WRITE_LOG();
//    std::cout << "Using program " << program << std::endl;

    GraphicsMonitor::boundProgram = program;

    useProgramDetour->disable();
    ((void APIENTRY (*)(GLuint))useProgramDetour->getFunction())(program);
    useProgramDetour->enable();
}

GLuint APIENTRY createProgram()
{
    WRITE_LOG();
    createProgramDetour->disable();
    GLuint result = ((GLuint APIENTRY (*)())createProgramDetour->getFunction())();
    createProgramDetour->enable();
    return result;
}

void APIENTRY uniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
    WRITE_LOG();
    assert(false);

    uniform3fvDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, const GLfloat*))uniform3fvDetour->getFunction())(location, count, value);
    uniform3fvDetour->enable();
}

void APIENTRY uniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;
    assert(count == 1);

//    std::cout << "uniform4f ->" << location << std::endl;
    Uniform4f u;
    u.messageType = Message::MessageType::UNIFORM_4_F;
    u.programId = boundProgram;
    u.location = location;
    memcpy(u.values, value, sizeof(u.values));
    appendToDataBuffer(u);

    uniform4fvDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, const GLfloat*))uniform4fvDetour->getFunction())(location, count, value);
    uniform4fvDetour->enable();
}

void APIENTRY uniform3uiv(GLint location, GLsizei count, const GLuint* value)
{
    WRITE_LOG();
    assert(false);

    uniform3uivDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, const GLuint*))uniform3uivDetour->getFunction())(location, count, value);
    uniform3uivDetour->enable();
}



void APIENTRY uniform4uiv(GLint location, GLsizei count, const GLuint* value)
{
    WRITE_LOG();
    assert(false);

    uniform4uivDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, const GLuint*))uniform4uivDetour->getFunction())(location, count, value);
    uniform4uivDetour->enable();
}

void APIENTRY uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    WRITE_LOG();
    std::cout << "UNIFORM MATRIX 3FV" << std::endl;
    assert(false);

    uniformMatrix3fvDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, GLboolean, const GLfloat*))uniformMatrix3fvDetour->getFunction())(location, count, transpose, value);
    uniformMatrix3fvDetour->enable();
}

void APIENTRY uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    WRITE_LOG();
    assert(count == 1);
    assert(!transpose);
//
//    GLfloat (*vals)[4] = (GLfloat (*)[4])value;
////    for(size_t y = 0; y < 4; y++)
////    {
////        for(size_t x = 0; x < 4; x++)
////            std::cout << vals[y][x] << "\t\t";
////
////        std::cout << std::endl;
////    }
//
//    vals[3][0] = 0.1f;

    using namespace GraphicsMonitor;
    using namespace GraphicsLayer::SharedMemoryProtocol;
    TransformationMatrix m;
    m.messageType = Message::MessageType::TRANSFORMATION_MATRIX;
    memcpy(m.matrix[0], value, sizeof(m.matrix));
    m.programId = boundProgram;
    appendToDataBuffer(m);

    uniformMatrix4fvDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, GLboolean, const GLfloat*))uniformMatrix4fvDetour->getFunction())(location, count, transpose, value);
    uniformMatrix4fvDetour->enable();
}

void APIENTRY attachShader(GLuint program, GLuint shader)
{
    WRITE_LOG();
    std::cout << "Attach shader to program: " << shader << "->" << program << std::endl;

    attachShaderDetour->disable();
    ((void APIENTRY (*)(GLuint, GLuint))attachShaderDetour->getFunction())(program, shader);
    attachShaderDetour->enable();
}

void APIENTRY shaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
{
    WRITE_LOG();
    std::cout << "Shader source: " << shader << std::endl;
    assert(length != nullptr);
    for(size_t i = 0; i < count; i++)
    {
        if(length[i] < 0)
            std::cout << string[i];
        else
        {
            std::string str;
            str.assign(string[i], length[i]);
            std::cout << str;
        }
    }
    std::cout << std::endl << std::endl;

    shaderSourceDetour->disable();
    ((void APIENTRY (*)(GLuint, GLsizei, const GLchar**, const GLint*))shaderSourceDetour->getFunction())(shader, count, string, length);
    shaderSourceDetour->enable();
}

void APIENTRY disable(GLenum cap)
{
    WRITE_LOG();
//    if(cap == GL_BLEND)
//        std::cout << "BLEND DISABLE" << std::endl;
//    switch(cap)
//    {
//        case GL_DEPTH_TEST:
//        case GL_SCISSOR_TEST:
//        case GL_STENCIL_TEST:
//        case GL_CULL_FACE:
//            break;
//        case GL_BLEND:
//            std::cout << "Disabled BLEND" << std::endl;
//            break;
//
//        default:
//            std::cout << "Disabled: " << cap << std::endl;
//    }

    disableDetour->disable();
    ((void APIENTRY (*)(GLenum))disableDetour->getFunction())(cap);
    disableDetour->enable();
}

void APIENTRY enablei(GLenum cap, GLuint index)
{
    WRITE_LOG();
    assert(false);
    switch(cap)
    {
        case GL_DEPTH_TEST:
        case GL_SCISSOR_TEST:
            break;
        case GL_BLEND:
            std::cout << "Enabled BLEND ->" << index << std::endl;
            return;
            break;

        default:
            std::cout << "Enabled: " << cap << std::endl;
    }

    enableiDetour->disable();
    ((void APIENTRY (*)(GLenum, GLuint))enableiDetour->getFunction())(cap, index);
    enableiDetour->enable();
}

void APIENTRY enable(GLenum cap)
{
    WRITE_LOG();
//    switch(cap)
//    {
//        case GL_DEPTH_TEST:
//        case GL_SCISSOR_TEST:
//            break;
//        case GL_BLEND:
//            std::cout << "Enabled BLEND" << std::endl;
//            return;
//            break;
//
//        default:
//            std::cout << "Enabled: " << cap << std::endl;
//    }

//    if(cap == GL_BLEND)
//        std::cout << "BLEND ENABLE" << std::endl;

    enableDetour->disable();
    ((void APIENTRY (*)(GLenum))enableDetour->getFunction())(cap);
    enableDetour->enable();
}

void APIENTRY blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    WRITE_LOG();

    GraphicsMonitor::currentBlendColor = Color(red * 255, green * 255, blue * 255, alpha * 255);

    blendColorDetour->disable();
    ((void APIENTRY (*)(GLfloat, GLfloat, GLfloat, GLfloat))blendColorDetour->getFunction())(red, green, blue, alpha);
    blendColorDetour->enable();
}

void APIENTRY enableVertexAttribArray(GLuint index)
{
    WRITE_LOG();
//    std::cout << "ATTRIB ARRAY ON: " << index << std::endl;

    enableVertexAttribArrayDetour->disable();
    ((void APIENTRY (*)(GLuint))enableVertexAttribArrayDetour->getFunction())(index);
    enableVertexAttribArrayDetour->enable();
}


void writeBoundTextureToFile(std::string dir, std::string name = "")
{
    GLint id = GraphicsMonitor::textureUnits.getTexture(GL_TEXTURE_2D);

    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[id];

    unsigned char* rgba = new unsigned char[texture.width * texture.height * 4];

    std::stringstream sstream;
    if(name == "")
        sstream << dir << "/" << id << ".ppm";
    else
        sstream << dir << "/" << name << ".ppm";

    getTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    std::ofstream file(sstream.str(), std::ios::binary);
    file.write((const char*)rgba, texture.width * texture.height * 4);
    file.close();
    delete[] rgba;

    return;

    unsigned char* rgb = new unsigned char[texture.width * texture.height * 3];
    for(size_t iDest = 0, iSrc = 0; iSrc + 3 < texture.width * texture.height * 4; iSrc += 4, iDest += 3)
    {
        if(rgba[iSrc + 3] == 0)
        {
            rgb[iDest] = 0;
            rgb[iDest + 1] = 255;
            rgb[iDest + 2] = 0;
        }
        else
        {
            memcpy(&rgb[iDest], &rgba[iSrc], 3);
        }
    }
    writePixelsToFile(sstream.str(), rgb, texture.width, texture.height);

    delete[] rgba;
    delete[] rgb;

}

void WINAPI bindTex(GLenum target, GLuint texture)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;

    textureUnits.setTexture(target, texture);
    WRITE_LOG_STEP();
    bindTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))
        bindTexDetour->getFunction())(target, texture);
    bindTexDetour->enable();
    WRITE_LOG_STEP();
}

WINBOOL WINAPI getClientRect(HWND hWnd, LPRECT lpRect)
{
    WRITE_LOG();

    getClientRectDetour->disable();
    WINBOOL retVal = ((WINBOOL WINAPI (*)(HWND, LPRECT))
        getClientRectDetour->getFunction())(hWnd, lpRect);
    getClientRectDetour->enable();

    using namespace GraphicsMonitor;
//    if(hWnd == shm->window)
//    {
//        clientWidth = lpRect->right;
//        clientHeight = lpRect->bottom;
//        lpRect->bottom += widthOffset;
//        lpRect->right += heightOffset;
//    }
    return retVal;
}

WINBOOL WINAPI getWindowRect(HWND hWnd, LPRECT lpRect)
{
    WRITE_LOG();

    getWindowRectDetour->disable();
    WINBOOL retVal = ((WINBOOL WINAPI (*)(HWND, LPRECT))
        getWindowRectDetour->getFunction())(hWnd, lpRect);
    getWindowRectDetour->enable();

    using namespace GraphicsMonitor;
    if(hWnd == shm->window)
    {
        tagPOINT clientScreenCoords = {0};
        ClientToScreen(shm->window, &clientScreenCoords);
        clientWidth = lpRect->right - clientScreenCoords.x;
        clientHeight = lpRect->bottom - clientScreenCoords.y;
        windowX = lpRect->left;
        windowY = lpRect->top;
        windowWidth = lpRect->right - lpRect->left;
        windowHeight = lpRect->bottom - lpRect->top;

        lpRect->bottom += widthOffset;
        lpRect->right += heightOffset;
    }

    return retVal;
}
#include <set>
std::set<GLuint> weirdGlyphBufferIds;
void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;



    SharedMemoryProtocol::DrawCall drawCall;
    drawCall.bufferId = (boundVertexArray == 0 ? boundBuffer : vaoToVbo[boundVertexArray]);
    drawCall.programId = boundProgram;
    drawCall.sourceTextureId = textureUnits.getTexture(GL_TEXTURE_2D);
    drawCall.blendColor = currentBlendColor;
    drawCall.indicesOffset = first;
    drawCall.numIndices = count;

    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        assert(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }
//    if(weirdGlyphBufferIds.find(drawCall.sourceTextureId) != weirdGlyphBufferIds.end())
//    {
//        std::cout << "DRAWARRAYS" << std::endl;
//        std::cout << "vao: " << boundVertexArray << std::endl;
//        std::cout << "buffer: " << boundBuffer << std::endl;
//        std::cout << "boundProgram: " << boundProgram << std::endl;
//        std::cout << "offset: " << (int)first << std::endl;
//        std::cout << drawCall.sourceTextureId << "->" << drawCall.targetTextureId << " (" << mode << " " << count << ")" << std::endl;
//        std::cout << "NUM WEIRD: " << weirdGlyphBufferIds.size() << std::endl;
////            return;
//    }
    switch(mode)
    {
        case GL_TRIANGLE_FAN:
            drawCall.type = SharedMemoryProtocol::DrawCall::PrimitiveType::TRIANGLE_FAN;
            break;

        default:
            std::stringstream sstream;
            sstream << "Unsupported primitive type: " << mode << ".";
            std::cout << sstream.str();
            throw std::runtime_error(sstream.str());
            break;
    }
    drawCall.messageType = SharedMemoryProtocol::Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);

//
//    unsigned int sourceTextureId = textureUnits.getTexture(GL_TEXTURE_2D);
//    unsigned int targetTextureId = 0;
//    if(boundFramebuffer != 0)
//    {
//        auto it = framebufferTextureAttachment.find(boundFramebuffer);
//        assert(it != framebufferTextureAttachment.end());
//
//        targetTextureId = it->second;
//    }
//
//    std::cout << "DRAW ARRAYS: " << drawCall.sourceTextureId << "->" << drawCall.targetTextureId << std::endl;
//    std::cout   << "\tmode: " << mode << std::endl
//                << "\tfirst: " << first << std::endl
//                << "\tcount: " << count << std::endl
//                << "\tbuffer: " << drawCall.bufferId << std::endl;


    drawArraysDetour->disable();
    ((void APIENTRY (*)(GLenum, GLint, GLsizei))drawArraysDetour->getFunction())(mode, first, count);
    drawArraysDetour->enable();

//    if(drawCall.targetTextureId != 0)
//    {
//        GLuint textureBackup = textureUnits.getTexture(GL_TEXTURE_2D);
//        bindTex(GL_TEXTURE_2D, drawCall.targetTextureId);
//        writeBoundTextureToFile("tileBuffers");
//        bindTex(GL_TEXTURE_2D, textureBackup);
//    }
}

void APIENTRY enableClientState(GLenum cap)
{
    WRITE_LOG();
    std::cout << "ENABLE CLIENT STATE: " << cap << std::endl;

    enableClientStateDetour->disable();
    ((void APIENTRY (*)(GLenum))enableClientStateDetour->getFunction())(cap);
    enableClientStateDetour->enable();
}

void APIENTRY vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;
    std::cout << "vertexPointer to buffer: " << boundBuffer << std::endl;

    vertexPointerDetour->disable();
    ((void APIENTRY (*)(GLint, GLenum, GLsizei, const GLvoid*))vertexPointerDetour->getFunction())(size, type, stride, pointer);
    vertexPointerDetour->enable();
}

void WINAPI vertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
{
    WRITE_LOG();
    std::cout << "bind vbuffer to varra: " << buffer << "->" << vaobj << std::endl;
    vertexArrayVertexBufferDetour->disable();
    ((void APIENTRY (*)(GLuint, GLuint, GLuint, GLintptr, GLsizei))vertexArrayVertexBufferDetour->getFunction())
        (vaobj, bindingindex, buffer, offset, stride);
    vertexArrayVertexBufferDetour->enable();
}

void WINAPI bindVertexArray(GLuint array)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;
    boundVertexArray = array;

    bindVertexArrayDetour->disable();
    ((void WINAPI (*)(GLuint))bindVertexArrayDetour->getFunction())(array);
    bindVertexArrayDetour->enable();
}

void WINAPI bindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLintptr stride)
{
    WRITE_LOG();
    std::cout << "bind vbuffer: " << buffer << std::endl;

    bindVertexBufferDetour->disable();
    ((void WINAPI (*)(GLuint, GLuint, GLintptr, GLintptr))bindVertexBufferDetour->getFunction())(bindingindex, buffer, offset, stride);
    bindVertexBufferDetour->enable();
}

void WINAPI bindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
    WRITE_LOG();
    assert(false);
    bindBufferBaseDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint, GLuint))bindBufferBaseDetour->getFunction())(target, index, buffer);
    bindBufferBaseDetour->enable();
}

void WINAPI bindBufferRange(GLenum target, GLuint index, GLuint buffer, GLint ptroffset, GLsizei ptrsize)
{
    WRITE_LOG();
    assert(false);
    bindBufferRangeDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint, GLuint, GLint, GLsizei))bindBufferRangeDetour->getFunction())(target, index, buffer, ptroffset, ptrsize);
    bindBufferRangeDetour->enable();
}

void WINAPI bindBuffer(GLenum target, GLuint buffer)
{
    WRITE_LOG();
//    std::cout << "bindbuffer: " << buffer << "->" << target << std::endl;

    GraphicsMonitor::boundBuffer = buffer;

    assert(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER);

    bindBufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindBufferDetour->getFunction())(target, buffer);
    bindBufferDetour->enable();
}

//SharedMemoryProtocol::VertexBufferInfo::VertexType currentVertexBufferInfoType;
//SharedMemoryProtocol::VertexBufferInfo currentVertexBufferInfo;
void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    WRITE_LOG();

    using namespace SharedMemoryProtocol;
    using namespace GraphicsMonitor;

    if(boundVertexArray != 0)
        vaoToVbo[boundVertexArray] = boundBuffer;

    VertexAttribPointer p;
    p.bufferId = boundBuffer;
    p.index = index;
    p.stride = stride;
    p.offset = (unsigned int)pointer;
    p.messageType = Message::MessageType::VERTEX_ATTRIB_POINTER;
    appendToDataBuffer(p);
//
//    typedef VertexBufferInfo::VertexType Type;
//    if(index == 0)
//    {
//        currentVertexBufferInfo = VertexBufferInfo();
//        switch(stride)
//        {
//            case 0:
//                currentVertexBufferInfoType = Type::TEXTURED_NO_ORDER;
//                break;
//            case 12:
//                currentVertexBufferInfoType = Type::COLORED;
//                break;
//            case 16:
//                if((unsigned int)pointer != 0)
//                    currentVertexBufferInfoType = Type::TEXTURED_OFFSET;
//                else
//                    currentVertexBufferInfoType = Type::TEXTURED;
//                break;
//            case 20:
//                currentVertexBufferInfoType = Type::TEXT;
//                break;
//            default:
//                std::stringstream sstream;
//                sstream << "Unimplemented stride value " << stride << ".";
//                std::cout << sstream.str() << std::endl;
//                throw 1;
//        }
//
//    }
//
//
//    if(index == 1)
//    {
//        if(currentVertexBufferInfoType == Type::TEXTURED_NO_ORDER)
//        {
//            VertexBufferInfo info;
//            info.bufferId = boundBuffer;
//            info.vertexType = currentVertexBufferInfoType;
//            info.unknownOffset = info.numUnknown = 0;
//            info.indicesOffset = info.numIndices = 0;
//            info.numVertices = (unsigned int)pointer / sizeof(Vertex);
//            info.messageType = SharedMemoryProtocol::Message::MessageType::VERTEX_BUFFER_INFO;
//            std::cout << "TEXTURED_NO_ORDER->" << boundBuffer << std::endl;
//            std::cout << boundBuffer << " " << boundVertexArray << std::endl;
//            appendToDataBuffer(info);
//        }
//        else if(currentVertexBufferInfoType == Type::TEXTURED_OFFSET)
//        {
//            std::cout << "TEXTURED_OFFSET" << std::endl;
//            VertexBufferInfo info;
//            info.bufferId = boundBuffer;
//            info.vertexType = currentVertexBufferInfoType;
//            info.verticesOffset = (unsigned int)pointer - stride / 2;
//            info.numVertices = info.verticesOffset / sizeof(TexturedVertex);
//            info.messageType = SharedMemoryProtocol::Message::MessageType::VERTEX_BUFFER_INFO;
//            appendToDataBuffer(info);
//        }
//    }
//    else if
//    (
//        (
//            index == 2 &&
//            (currentVertexBufferInfoType == Type::COLORED || currentVertexBufferInfoType == Type::TEXTURED)
//        ) ||
//        (
//            index == 3 && currentVertexBufferInfoType == Type::TEXT
//        )
//    )
//    {
//        VertexBufferInfo info;
//        info.bufferId = boundBuffer;
//        info.vertexType = currentVertexBufferInfoType;
//
//
//
//
//        unsigned int vertexSize;
//        switch(info.vertexType)
//        {
//            case Type::COLORED:
//                vertexSize = sizeof(ColoredVertex);
//                break;
//            case Type::TEXTURED:
//                vertexSize = sizeof(TexturedVertex);
//                break;
//            case Type::TEXT:
//                vertexSize = sizeof(UnknownVertex);
//                break;
//            default:
//                std::stringstream sstream;
//                sstream << "Unimplemented vertex type " << (int)info.vertexType << ".";
//                std::cout << sstream.str() << std::endl;
//                throw 1;
//        }
//
//        info.unknownOffset = (int)pointer;
//        info.numVertices = info.unknownOffset / vertexSize;
//        info.numUnknown = info.numVertices;
//
//        typedef SharedMemoryProtocol::VertexBufferInfo::Unknown Unknown;
//        unsigned int numBytes = vertexBuffers[boundBuffer].numBytes;
//        info.indicesOffset = info.unknownOffset + info.numUnknown * sizeof(Unknown);
//        unsigned int remainingBytes = numBytes - (int)info.indicesOffset;
//
//        info.numIndices = remainingBytes / sizeof(SharedMemoryProtocol::VertexBufferInfo::Index);
//
////        size_t unknownSize = info.numUnknown * sizeof(Unknown);
////        Unknown* replacement = new Unknown[info.numUnknown];
////        getSubBufDat(GL_ARRAY_BUFFER, info.unknownOffset, unknownSize, replacement);
////        for(size_t i = 0; i < info.numUnknown; i++)
////            replacement[i] /= 10.f;
////        subBufDat(GL_ARRAY_BUFFER, info.unknownOffset, unknownSize, replacement);
////        delete[] replacement;
//
////        bufferInfos.push_back(info);
////        currentFrame.numVertexBufferInfos++;
//
//        info.messageType = SharedMemoryProtocol::Message::MessageType::VERTEX_BUFFER_INFO;
//        appendToDataBuffer(info);
//    }

//    std::cout << "VertexAttribPointer " << index << "->" << boundBuffer << ": " << size << " " << type << " " << normalized << " " << stride << " " << (int)pointer << std::endl;

    vertexAttribPointerDetour->disable();
    ((void WINAPI (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))
     vertexAttribPointerDetour->getFunction())(index, size, type, normalized, stride, pointer);
    vertexAttribPointerDetour->enable();
}

void APIENTRY scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    using namespace GraphicsMonitor;
    scissorX = x;
    scissorY = y;
    scissorWidth = width;
    scissorHeight = height;

    scissorDetour->disable();
    ((void WINAPI (*)(GLint, GLint, GLsizei, GLsizei))
     scissorDetour->getFunction())(x, y, width, height);
    scissorDetour->enable();
}

#include <set>
std::set<unsigned int> glyphBufferIds;

std::set<unsigned int> tex1024x16;
std::set<unsigned int> tex512x16;
size_t merpCount = 0;
void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer);
void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;

    assert(boundBuffer != 1);


//    std::cout << "drawElements " << mode << " " << count << " " << type << " " << (int)indices << " (buf: " << boundBuffer << std::endl;

    SharedMemoryProtocol::DrawCall drawCall;
    drawCall.bufferId = boundBuffer;
    drawCall.programId = boundProgram;
    drawCall.sourceTextureId = textureUnits.getTexture(GL_TEXTURE_2D);
    drawCall.blendColor = currentBlendColor;
    drawCall.indicesOffset = (unsigned int)indices;
    drawCall.numIndices = count;

    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        assert(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

//    if(weirdGlyphBufferIds.find(drawCall.sourceTextureId) != weirdGlyphBufferIds.end())
//    {
//        std::cout << "buffer: " << boundBuffer << std::endl;
//        std::cout << "boundProgram: " << boundProgram << std::endl;
//        std::cout << "offset: " << (int)indices << std::endl;
//        std::cout << drawCall.sourceTextureId << "->" << drawCall.targetTextureId << " (" << mode << " " << count << " " << type << ")" << std::endl;
//        std::cout << "NUM WEIRD: " << weirdGlyphBufferIds.size() << std::endl;
////            return;
//    }

//    if(drawCall.sourceTextureId == tileSheetTextureId && drawCall.targetTextureId != 0)
//        unshadedViewTextureId = drawCall.targetTextureId;
//
//    if(drawCall.sourceTextureId == tileSheetTextureId && drawCall.targetTextureId == 0)
//        return;


//    if(drawCall.sourceTextureId == unshadedViewTextureId && drawCall.targetTextureId == 0)
//    {
////        TexturedVertex v;
////        v.texX = 0.f;
////        v.texY = 0.f;
////        v.x = 0.f;
////        v.y = 0.f;
//        float offset = 0;
////        float xRatio = (clientWidth + 1000.f) / clientWidth;
////        float yRatio = (clientHeight + 1000.f) / clientHeight;
//        float xRatio = clientWidth / (clientWidth + 1000.f);
//        float yRatio = clientHeight / (clientHeight + 1000.f);
//
//        float v[4 * 4] =
//        {
//          -offset, -offset, 0.f, 0.f,
//          -offset, 1555 - offset, 0, 1,
//          2120 - offset, -offset, 1, 0,
//          2120 - offset, 1555 - offset, 1, 1
//        };
////        float v[4 * 4] =
////        {
////          0, 0, 0.f, 0.f,
////          0, 1555 * yRatio, 0, 1,
////          2120 * xRatio, 0, 1, 0,
////          2120 * xRatio, 1555 * yRatio, 1, 1
////        };
////        float v[4 * 4] =
////        {
////            0.f, 0.f, -1000.f, -1000.f,
////            0, 1, -1000, 1555 - 1000,
////            1, 0, 2120 - 1000, -1000,
////            1, 1, 2120 - 1000, 1555 - 1000,
////        };
//        WRITE_LOG_STEP();
//        bufSubDatDetour->disable();
//        WRITE_LOG_STEP();
//        ((void WINAPI (*)(GLenum, GLint, GLsizei, const GLvoid*))
////            bufSubDatDetour->getFunction())(GL_ARRAY_BUFFER, sizeof(TexturedVertex), sizeof(TexturedVertex), &v);
//            bufSubDatDetour->getFunction())(GL_ARRAY_BUFFER, 0, sizeof(v), v);
//        WRITE_LOG_STEP();
//        bufSubDatDetour->enable();
//        WRITE_LOG_STEP();
//    }
//    std::cout << drawCall.sourceTextureId << "->" << drawCall.targetTextureId << std::endl;


//    if(drawCall.targetTextureId == 0 && currentVertexBufferInfoType == SharedMemoryProtocol::VertexBufferInfo::VertexType::COLORED)
//        return;
//    if(drawCall.targetTextureId == 0 && glyphBufferIds.find(drawCall.sourceTextureId) != glyphBufferIds.end())
//    {
//        std::cout << "Glyph->0: " << std::endl;
//    }
    //    if(drawCall.targetTextureId == 0 && drawCall.sourceTextureId == tex512x16)
//    if(drawCall.targetTextureId == 0 && glyphBufferIds.find(drawCall.sourceTextureId) != glyphBufferIds.end())
//    if(drawCall.targetTextureId == 0 && tex512x16.find(drawCall.sourceTextureId) != tex512x16.end())
//        return;

//    if(drawCall.sourceTextureId == 2 && drawCall.targetTextureId == 0)
//        return;

//    if(coloredBuffers.find(boundBuffer) != coloredBuffers.end())
//        return;
//    if(drawCall.sourceTextureId < 6 && drawCall.targetTextureId == 0)
//        return;

//    if(drawCall.targetTextureId == 0 && (drawCall.sourceTextureId == 16 || drawCall.sourceTextureId == 17))
//    {
//        std::cout << drawCall.sourceTextureId << "->" << drawCall.targetTextureId << " (" << boundBuffer << ")" << std::endl;
//    }//&& boundFramebuffer != 0)
////        if(/*drawCall.sourceTextureId == 16 || */drawCall.sourceTextureId == 17)
//        {
////            count = 0;
////            if(drawCall.sourceTextureId != 2 && drawCall.sourceTextureId != 15)
//        }

    switch(mode)
    {
        case GL_TRIANGLES:
            drawCall.type = SharedMemoryProtocol::DrawCall::PrimitiveType::TRIANGLE;
            break;

        case GL_TRIANGLE_STRIP:
            drawCall.type = SharedMemoryProtocol::DrawCall::PrimitiveType::TRIANGLE_STRIP;
            break;

        default:
            std::stringstream sstream;
            sstream << "Unsupported primitive type: " << mode << ".";
            std::cout << sstream.str();
            throw std::runtime_error(sstream.str());
            break;
    }
//    drawCalls.push_back(drawCall);
//    currentFrame.numDrawCalls++;

    drawCall.messageType = SharedMemoryProtocol::Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);

    if(boundFramebuffer != 0 && framebufferTextureAttachment.find(boundFramebuffer) != framebufferTextureAttachment.end())
    {
        if(textureBuffers.size() == 3 && framebufferTextureAttachment[boundFramebuffer] == textureBuffers[TextureBuffers::UNSHADED])
        {


//
//            std::cout << "drawElements " << boundTexture << "->" << framebufferTextureAttachment[boundFramebuffer] << std::endl;
//            std::cout << "buffer: " << boundBuffer << std::endl;
//            std::cout << "framebuf: " << boundFramebuffer << std::endl;
//            std::cout   << "\tmode: " << mode << std::endl
//                        << "\tcount: " << count << std::endl
//                        << "\ttype: " << type << std::endl;
//
//            std::cout << "Texture units: " << std::endl;
//            sceneBuffer = boundBuffer;
//            for(const TextureUnit& unit : textureUnits.getTextureUnits())
//            {
//                std::cout << unit.id << std::endl;
//                for(auto pair2 : unit.targets)
//                {
//                    GLenum target = pair2.first;
//                    GLuint texture = pair2.second;
//
//                    std::cout   << "\ttarget: " << target << std::endl
//                                << "\ttexture: " << texture << std::endl;
//                }
//                std::cout << "------------" << std::endl;
//            }
//            Sleep(1000);
        }
    }

//    if(drawCall.targetTextureId == 0 && drawCall.sourceTextureId == 3)//&& drawCall.sourceTextureId != 2 && glyphBufferIds.find(drawCall.sourceTextureId) != glyphBufferIds.end())
//    {
//        GLuint textureBackup = textureUnits.getTexture(GL_TEXTURE_2D);
//        bindTex(GL_TEXTURE_2D, drawCall.sourceTextureId);
//        std::stringstream sstream;
//        sstream << merpCount++ << "-" << drawCall.sourceTextureId;
//        writeBoundTextureToFile("tileBuffers", sstream.str());
//        bindTex(GL_TEXTURE_2D, textureBackup);
//    }

    GLuint framebufferBackup = boundFramebuffer;
    GLuint programBackup = boundProgram;
    unsigned short viewportWidthBackup = currentViewportWidth;
    unsigned short viewportHeightBackup = currentViewportHeight;
    int scissorXBackup = scissorX;
    int scissorYBackup = scissorY;
    unsigned int scissorWidthBackup = scissorWidth;
    unsigned int scissorHeightBackup = scissorHeight;

    if(drawCall.targetTextureId == 0)
    {
//        viewport(0, 0, viewportWidthBackup - widthOffset, viewportHeightBackup - heightOffset);
//        CHECK_GL_ERROR();
//        scissor(0, 0, viewportWidthBackup, viewportHeightBackup);

//        disable(GL_SCISSOR_TEST);
//        bindFramebuffer(GL_DRAW_FRAMEBUFFER, rescaleFramebuffer);
//        CHECK_GL_ERROR();
//        ((void WINAPI (*)(GLenum))drawBufferDetour->getFunction())(GL_COLOR_ATTACHMENT0);
//        GLenum status = checkFramebufferStatus(GL_DRAW_FRAMEBUFFER);
//        if(status != GL_FRAMEBUFFER_COMPLETE)
//        {
//            std::cout << "Draw fb status: " << status << std::endl;
////            assert(false);
//        }
////        GLenum b[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
////        drawBuffers(2, b);

    }

    drawElementsDetour->disable();
    ((void APIENTRY (*)(GLenum, GLsizei, GLenum, const GLvoid*))drawElementsDetour->getFunction())(mode, count, type, indices);
    drawElementsDetour->enable();

    if(drawCall.targetTextureId == 0)
    {
//        viewport(0, 0, viewportWidthBackup, viewportHeightBackup);
//        scissor(scissorXBackup, scissorYBackup, scissorWidthBackup, scissorHeightBackup);
//        CHECK_GL_ERROR();

//        CHECK_GL_ERROR();
////        std::cout << "DRAW: " << getError() << std::endl;
//        bindFramebuffer(GL_FRAMEBUFFER, framebufferBackup);
//        CHECK_GL_ERROR();
    }
}

void WINAPI compTexImg
(
    GLenum target,
    GLint level,
    GLenum internalformat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLsizei imageSize,
    const GLvoid* data
)
{
    WRITE_LOG();
//    std::cout << "comptexImg " << width << "x" << height << std::endl;

    compTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*))
        compTexImgDetour->getFunction())(target, level, internalformat, width, height, border, imageSize, data);
    compTexImgDetour->enable();
}

void WINAPI subCompTexImg
(
    GLenum target,
  	GLint level,
  	GLint xoffset,
  	GLint yoffset,
  	GLsizei width,
  	GLsizei height,
  	GLenum format,
  	GLsizei imageSize,
  	const GLvoid* data
)
{
    WRITE_LOG();
//    std::cout << "subcomptexImg " << width << "x" << height << " - pos: " << xoffset << "x" << yoffset << std::endl;

    subCompTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*))
        subCompTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    subCompTexImgDetour->enable();
}

void WINAPI bufSubDat
(
    GLenum target,
  	GLint offset,
  	GLsizei size,
  	const GLvoid* data
)
{
    WRITE_LOG();
//    std::cout << "glBufferSubData: " << size << " bytes" << std::endl;

    bufSubDatDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLsizei, const GLvoid*))
        bufSubDatDetour->getFunction())(target, offset, size, data);
    bufSubDatDetour->enable();
}


void WINAPI bufDat
(
    GLenum target,
    GLsizei size,
    const GLvoid* data,
    GLenum usage
)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;

    GLuint buf = boundBuffer;
//    std::cout << "glBufferData: " << target << " " << size << " " << usage << " -> " << buf << std::endl;

    SharedMemoryProtocol::VertexBufferWrite& buffer = vertexBuffers[buf];
    buffer.bufferId = buf;
    buffer.numBytes = size;

//    bufferWrites.push_back(buffer);
//    currentFrame.numVertexBufferWrites++;

//    vertexBuffer.insert(vertexBuffer.end(), (char*)data, (char*)data + size);
//    currentFrame.numVertexBytes += size;

    buffer.messageType = SharedMemoryProtocol::Message::MessageType::VERTEX_BUFFER_WRITE;
    appendToDataBuffer(buffer);
    appendToDataBuffer((char*)data, size);


//    bufferData.data = malloc(size);
//    bufferData.numBytes = size;
//    memcpy(bufferData.data, data, size);
//
//    updatedBuffers.push_back(buf);



    bufDatDetour->disable();
    ((void WINAPI (*)(GLenum, GLsizei, const GLvoid*, GLenum))
        bufDatDetour->getFunction())(target, size, data, usage);
    bufDatDetour->enable();
}

void insertTextureData(unsigned short width, unsigned short height, unsigned int id)
{
    if(id == 0)
        return;

    using namespace GraphicsLayer::SharedMemoryProtocol;
    using namespace GraphicsMonitor;
    TextureData data;
    data.width = width;
    data.height = height;
    data.id = id;
//    textureData.push_back(data);
//    currentFrame.numTextureData++;

    data.messageType = Message::MessageType::TEXTURE_DATA;
    appendToDataBuffer(data);
}

void insertPixelData
(
    unsigned char* data,
    unsigned int targetTextureId,
    unsigned short x,
    unsigned short y,
    unsigned short width,
    unsigned short height,
    GLenum format,
    GLenum type
)
{
    WRITE_LOG();

    if(data == nullptr)
        return;

    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    assert(format == GL_BGRA || format == GL_RGBA || format == GL_ALPHA);
    assert(type == GL_UNSIGNED_BYTE);

    PixelData p;
    p.height = height;
    p.width = width;
    p.texX = x;
    p.texY = y;
    p.targetTextureId = targetTextureId;

    unsigned int bytesPerPixel = 4;
    typedef PixelData::PixelFormat PixelFormat;
    switch(format)
    {
        case GL_RGBA:
            p.format = PixelFormat::RGBA;
            bytesPerPixel = 4;
            break;

        case GL_BGRA:
            p.format = PixelFormat::BGRA;
            bytesPerPixel = 4;
            break;

        case GL_ALPHA:
            p.format = PixelFormat::ALPHA;
            bytesPerPixel = 1;
            break;

        default:
            std::stringstream sstream;
            sstream << "Unsupported pixel format: " << format << ".";
            std::cout << sstream.str();
            throw std::runtime_error(sstream.str());
    }

//    pixelData.push_back(p);
//    currentFrame.numPixelData++;

    size_t size = width * height * bytesPerPixel;
//    pixelBuffer.insert
//    (
//        pixelBuffer.end(),
//        data,
//        data + size
//    );
//    currentFrame.numPixelBytes += size;

    p.messageType = Message::MessageType::PIXEL_DATA;
    WRITE_LOG_STEP();
    appendToDataBuffer(p);
    WRITE_LOG_STEP();
    if(p.format == PixelFormat::ALPHA)
    {
        size_t remainder = width % currentUnpackAlignment;
        if(remainder != 0)
        {

            size_t paddedWidth = width + currentUnpackAlignment - remainder;
            char* unpaddedData = new char[size];
            WRITE_LOG_STEP();
            for(size_t iSrc = 0, iDest = 0; iDest < size; iSrc += paddedWidth, iDest += width)
                memcpy(&unpaddedData[iDest], &data[iSrc], width); // TODO: Sometimes the client crashes here // UPDATE: This might be fixed. May have been due to the current unpack alignment sometimes changing (currentUnpackAlignment was hardcoded to 4 before).
            WRITE_LOG_STEP();
            appendToDataBuffer(unpaddedData, size);
            WRITE_LOG_STEP();
            delete[] unpaddedData;
            WRITE_LOG_STEP();
            return;
        }
    }

    WRITE_LOG_STEP();
    appendToDataBuffer((char*)data, size);
    WRITE_LOG_STEP();
}

void WINAPI copyTexImage(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    WRITE_LOG();
    assert(level == 0);
    using namespace GraphicsMonitor;
    unsigned int targetTextureId = GraphicsMonitor::textureUnits.getTexture(target);
    unsigned int sourceTextureId = 0;
    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        assert(it != framebufferTextureAttachment.end());

        sourceTextureId = it->second;
    }

    using namespace SharedMemoryProtocol;
    CopyTexture copy;
    copy.srcX = 0;
    copy.srcY = 0;
    copy.targetX = x;
    copy.targetY = y;
    copy.width = width;
    copy.height = height;
    copy.sourceTextureId = sourceTextureId;
    copy.targetTextureId = targetTextureId;
    copy.messageType = Message::MessageType::COPY_TEXTURE;
    appendToDataBuffer(copy);

    std::cout << "COPY TEX IMAGE: " << sourceTextureId << "->" << targetTextureId << std::endl;

    copyTexImageDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))
        copyTexImageDetour->getFunction())(target, level, internalformat, x, y, width, height, border);
    copyTexImageDetour->enable();
}

void WINAPI copyTexSubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;
    assert(level == 0);
    unsigned int targetTextureId = GraphicsMonitor::textureUnits.getTexture(target);
    unsigned int sourceTextureId = 0;
    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        assert(it != framebufferTextureAttachment.end());

        sourceTextureId = it->second;
    }

    std::cout   << "COPY TEX SUB IMAGE: " << sourceTextureId << "->" << targetTextureId
                << "(" << xoffset << "x" << yoffset << "->" << x << "x" << y << ") "
                << width << "x" << height << std::endl;

    using namespace SharedMemoryProtocol;
    CopyTexture copy;
    copy.srcX = xoffset;
    copy.srcY = yoffset;
    copy.targetX = x;
    copy.targetY = y;
    copy.width = width;
    copy.height = height;
    copy.sourceTextureId = sourceTextureId;
    copy.targetTextureId = targetTextureId;
    copy.messageType = Message::MessageType::COPY_TEXTURE;
    appendToDataBuffer(copy);

    copyTexSubImageDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
        copyTexSubImageDetour->getFunction())(target, level, xoffset, yoffset, x, y, width, height);
    copyTexSubImageDetour->enable();
}

void WINAPI texImg
(
    GLenum target,
  	GLint level,
  	GLint internalFormat,
  	GLsizei width,
  	GLsizei height,
  	GLint border,
  	GLenum format,
  	GLenum type,
  	const GLvoid* data
)
{
    WRITE_LOG();
    assert(level == 0);
    GLuint textureId = GraphicsMonitor::textureUnits.getTexture(target);
    if(height == 16)
        glyphBufferIds.insert(textureId);
    else
        glyphBufferIds.erase(textureId);

    if(height == 68)
        weirdGlyphBufferIds.insert(textureId);
    else
        weirdGlyphBufferIds.erase(textureId);

    if(width == 1024 && height == 16)
    {
        tex1024x16.insert(textureId);
    }
    else
        tex1024x16.erase(textureId);

    if(width == 512 && height == 16)
    {
        tex512x16.insert(textureId);
    }
    else
        tex512x16.erase(textureId);
//    std::cout << "texImg " << width << "x" << height << std::endl;
//    std::cout << "\ttarget: " << target << std::endl;
//    if(target == GL_PROXY_TEXTURE_2D)
//        std::cout << "\t(GL_PROXY_TEXTURE_2D)" << std::endl;
//    if(data == nullptr)
//        std::cout << "\tNULL DATA" << std::endl;
//    std::cout << "\tlevel: " << level << std::endl;
//    std::cout << "\tid:" << textureId << std::endl;
//    std::cout << "\tintform: " << internalFormat << std::endl;
//    std::cout << "\tformat: " << format << std::endl;
//    std::cout << "\ttype: " << type << std::endl;
//    std::cout << "\tborder: " << border << std::endl;

    if(target == GL_TEXTURE_2D)
    {
        GraphicsMonitor::textures[textureId] = {width, height, format};


        if(width == 480 && height == 352)
        {
            GraphicsMonitor::textureBuffers.push_back(textureId);
            assert(GraphicsMonitor::textureBuffers.size() <= GraphicsMonitor::TextureBuffers::NUM_BUFFERS);
        }

        if(width == 2048 && height == 2048)
        {
            GraphicsMonitor::tileSheetTextureId = textureId;
        }
    }
    insertTextureData(width, height, textureId);
    insertPixelData((unsigned char*)data, textureId, 0, 0, width, height, format, type);


    texImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))
        texImgDetour->getFunction())(target, level, internalFormat, width, height, border, format, type, data);
    texImgDetour->enable();
}


size_t subTexCount = 0;
void WINAPI subTexImg
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
    WRITE_LOG();
    assert(level == 0);
    using namespace GraphicsMonitor;

////        std::cout << "subtex: " << format << " " << type << std::endl;
//        unsigned char* rgb = rgbaToRgb((const unsigned char*)pixels, width, height);
//        WRITE_LOG_STEP();
//        if(format == GL_BGRA)
//        {
//            for(size_t i = 2; i < width * height * 3; i += 3)
//                std::swap(rgb[i - 2], rgb[i]);
//        }
//        std::stringstream sstream;
//        sstream << "tiles/" << subTexCount++ << "-" << xoffset << "x" << yoffset << "_" << width << "x" << height << "-" << textureUnits.getTexture(target) << ".ppm";
////        writePixelsToFile(sstream.str(), rgb, width, height);
//        delete[] rgb;
//        WRITE_LOG_STEP();

//    SharedMemoryProtocol::PixelData p;
//    p.height = height;
//    p.width = width;
//    p.texX = 0;
//    p.texY = 0;
//    p.targetTextureId = textureId;
//
//    pixelData.push_back(p);
//    GraphicsMonitor::currentFrame.numPixelData++;
    WRITE_LOG_STEP();
    insertPixelData
    (
        (unsigned char*)pixels,
        textureUnits.getTexture(target),
        xoffset,
        yoffset,
        width,
        height,
        format,
        type
    );
    WRITE_LOG_STEP();


    subTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
        subTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, type, pixels);
    subTexImgDetour->enable();
    WRITE_LOG_STEP();

}


void WINAPI drawBuffer(GLenum buf)
{
    WRITE_LOG();
    std::cout << "DRAW BUFFER: " << buf << std::endl;

    drawBufferDetour->disable();
    ((void WINAPI (*)(GLenum))drawBufferDetour->getFunction())(buf);
    drawBufferDetour->enable();
}

void WINAPI readBuffer(GLenum mode)
{
    WRITE_LOG();
    std::cout << "READ BUFFER: " << GraphicsMonitor::boundFramebuffer << std::endl;

    readBufferDetour->disable();
    ((void WINAPI (*)(GLenum))readBufferDetour->getFunction())(mode);
    readBufferDetour->enable();
}

void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer)
{
    WRITE_LOG();
//    std::cout << "bindFramebuffer: " << framebuffer << "->" << target << std::endl;

    GraphicsMonitor::boundFramebuffer = framebuffer;

    bindFramebufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindFramebufferDetour->getFunction())(target, framebuffer);
    bindFramebufferDetour->enable();
}

//void WINAPI framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
//{
//    WRITE_LOG();
//    using namespace GraphicsMonitor;
//    std::cout << "BIND RENDERBUFFER TO FRAMEBUFFER: " << renderbuffer << "->" << boundFramebuffer << " (attach: " << attachment << ")" << std::endl;
//
//    framebufferRenderbufferDetour->disable();
//    ((void WINAPI (*)(GLenum, GLenum, GLenum, GLuint))framebufferRenderbufferDetour->getFunction())(target, attachment, renderbuffertarget, renderbuffer);
//    framebufferRenderbufferDetour->enable();
//}

void WINAPI activeTexture(GLenum texture)
{
    WRITE_LOG();
    GraphicsMonitor::textureUnits.setActiveTextureUnit(texture);

    activeTextureDetour->disable();
    ((void WINAPI (*)(GLenum))activeTextureDetour->getFunction())(texture);
    activeTextureDetour->enable();
}


void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    WRITE_LOG();
    GraphicsMonitor::framebufferTextureAttachment[GraphicsMonitor::boundFramebuffer] = texture;
    assert(level == 0);
    framebufferTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLenum, GLenum, GLuint, GLint))framebufferTexDetour->getFunction())(target, attachment, textarget, texture, level);
    framebufferTexDetour->enable();
}

void waitForIoBufferLock()
{
//    while(GraphicsMonitor::isIoBufferLocked);
//    GraphicsMonitor::isIoBufferLocked = true;

    switch(WaitForSingleObject(GraphicsMonitor::ioBufferLock, 2000))
    {
        case WAIT_ABANDONED:
            std::cout << "WAIT_ABANDONED" << std::endl;
            throw std::runtime_error("");

        case WAIT_OBJECT_0:
            return;

        case WAIT_TIMEOUT:
            std::cout << "Waiting for IO buffer lock takes too long..." << std::endl;
            throw std::runtime_error("");

        case WAIT_FAILED:
            std::cout << "WAIT_FAILED" << std::endl;
            throw std::runtime_error("");

        default:
            std::cout << "Unimplemented wait signal." << std::endl;
            throw std::runtime_error("");
            break;
    }
}

void signalIoBufferLock()
{
//    GraphicsMonitor::isIoBufferLocked = false;
    ReleaseMutex(GraphicsMonitor::ioBufferLock);
}

void updateSharedMemory()
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    waitForIoBufferLock();
    appendToDataBuffer(ioBuffer.data(), ioBuffer.size());
    ioBuffer.clear();
    signalIoBufferLock();

    if(dataBuffer.size() < sizeof(Frame))
        return;

    size_t insertionSize = 0;
    if(dataBuffer.size() > DATA_BUFFER_SIZE)
    {
        while(true)
        {
            const Frame& f = *(Frame*)((char*)dataBuffer.data() + insertionSize);
            size_t newInsertionSize = insertionSize + f.size;
            if(newInsertionSize > DATA_BUFFER_SIZE || newInsertionSize > dataBuffer.size())
                break;
            insertionSize = newInsertionSize;
        }
        assert(insertionSize <= dataBuffer.size());
    }
    else
        insertionSize = dataBuffer.size();

    memcpy(shm->data, dataBuffer.data(), insertionSize);
    shm->size = insertionSize;
    if(insertionSize == dataBuffer.size())
        dataBuffer.clear();
    else
        dataBuffer.erase(dataBuffer.begin(), dataBuffer.begin() + insertionSize);
}

void printSharedMemoryBufferDiagnostics()
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    float dataOccupancy = ((float)dataBuffer.size()) / ((float)DATA_BUFFER_SIZE) * 100.f;
    peakDataOccupancy = std::max(peakDataOccupancy, dataOccupancy);
    std::cout   << "Shared memory buffer occupancy: " << dataBuffer.size() << " / " << DATA_BUFFER_SIZE
                << "  (" << dataOccupancy << "%)"
                << " peak: " << peakDataOccupancy << "%" <<  std::endl;
}

bool isParentAlive()
{
    unsigned long code;
    if(GetExitCodeProcess(GraphicsMonitor::shm->parentProcessHandle, &code) == 0)
    {
        std::cout << "Could not get parent process status. Error code: " << GetLastError() << std::endl;
        throw std::runtime_error("");
    }

    return (code == STILL_ACTIVE);
}

void postFrame()
{
    if(ReleaseSemaphore(GraphicsMonitor::shm->semRead, 1, NULL) == 0)
    {
        std::cout << "Could not release shared memory read semaphore. Error code: " << GetLastError() << std::endl;
        throw std::runtime_error("");
    }
}



std::map<std::string, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)> classNameToWndProc;
std::map<HWND, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)> hwndToWndProc;

LRESULT CALLBACK windowClassProcDetour(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_SETFOCUS:
//            std::cout << "got focus" << std::endl;
//            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        case WM_KILLFOCUS:
//            std::cout << "lost focus" << std::endl;
//            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        case WM_SIZE:
//            std::cout << "WM_SIZE" << std::endl;
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        default:
//            std::cout << std::hex << uMsg << std::endl;
            break;
    }


    auto hwndToWndProcIt = hwndToWndProc.find(hwnd);
    if(hwndToWndProcIt != hwndToWndProc.end())
        return hwndToWndProcIt->second(hwnd, uMsg, wParam, lParam);

    char classN[256];
    GetClassNameA(hwnd, classN, 256);
    auto classNameToWndProcIt = classNameToWndProc.find(std::string(classN));
    if(classNameToWndProcIt != classNameToWndProc.end())
    {
        auto& proc = classNameToWndProcIt->second;
        hwndToWndProc[hwnd] = proc;
        return proc(hwnd, uMsg, wParam, lParam);
    }

    assert(false);
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

void waitForFrameRequest()
{
    switch(WaitForSingleObject(GraphicsMonitor::shm->semWrite, 500))
    {
        case WAIT_ABANDONED:
            std::cout << "WAIT_ABANDONED" << std::endl;
            throw std::runtime_error("");

        case WAIT_OBJECT_0:
//            InvalidateRect(GraphicsMonitor::shm->window, NULL, FALSE);
//            UpdateWindow(GraphicsMonitor::shm->window);
            return;

        case WAIT_TIMEOUT:
            if(!isParentAlive())
                ExitProcess(0);

            waitForFrameRequest();
            return;

        case WAIT_FAILED:
            std::cout << "WAIT_FAILED" << std::endl;
            throw std::runtime_error("");

        default:
            std::cout << "Unimplemented wait signal." << std::endl;
            throw std::runtime_error("");
            break;
    }
}




BOOL WINAPI swapBuf(HDC hdc)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    swapCount++;
//    std::cout << "swapbuf: " << swapCount << std::endl;
//    printSharedMemoryBufferDiagnostics();

    assert(getCurrentFrame()->size <= DATA_BUFFER_SIZE);

    getCurrentFrame()->width = currentViewportWidth;
    getCurrentFrame()->height = currentViewportHeight;

    updateSharedMemory();
    createNewFrame();

    GLuint textureBackup = textureUnits.getTexture(GL_TEXTURE_2D);

    for(auto pair : textures)
    {
//        if(pair.first != 3 && pair.first != 4)
            if(pair.first == tileSheetTextureId)
//        if(pair.first >= 9)
//        if(pair.first == 17)
        {
            bindTex(GL_TEXTURE_2D, pair.first);
            writeBoundTextureToFile("tileBuffers");
        }
    }

    bindTex(GL_TEXTURE_2D, textureBackup);

//
//    bindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//    bindFramebuffer(GL_READ_FRAMEBUFFER, rescaleFramebuffer);
//
//    readBufferDetour->disable();
//    ((void WINAPI (*)(GLenum))readBufferDetour->getFunction())(GL_COLOR_ATTACHMENT0);
//    readBufferDetour->enable();
//    ((void WINAPI (*)(GLenum))drawBufferDetour->getFunction())(GL_BACK_LEFT);
//    blitFramebuffer
//    (
//        0, 0,
//        clientWidth + widthOffset, clientHeight + heightOffset,
//        0, 0,
//        clientWidth, clientHeight,
//        GL_COLOR_BUFFER_BIT,
//        GL_LINEAR
//    );
//    std::cout << getError() << std::endl;
//
//    bindFramebuffer(GL_FRAMEBUFFER, 0);

    swapBufDetour->disable();
    BOOL retVal = ((WINGDIAPI BOOL WINAPI (*)(HDC))swapBufDetour->getFunction())(hdc);
    swapBufDetour->enable();

    postFrame();
    waitForFrameRequest();
    return retVal;
}

void WINAPI delTex(GLsizei n, const GLuint* textures)
{
    WRITE_LOG();
//    std::cout << "deltex" << std::endl;

    for(size_t i = 0; i < n; i++)
    {
        GraphicsMonitor::textures.erase(textures[i]);
        weirdGlyphBufferIds.erase(textures[i]);
    }

    delTexDetour->disable();
    ((void WINAPI (*)(GLsizei, const GLuint*))delTexDetour->getFunction())(n, textures);
    delTexDetour->enable();
}

void WINAPI genTexFunc(GLsizei n, GLuint* textures)
{
    WRITE_LOG();
    genTexDetour->disable();
    ((void WINAPI (*)(GLsizei, GLuint*))genTexDetour->getFunction())(n, textures);
    genTexDetour->enable();
}

WINBOOL WINAPI isWindowVisible(HWND hWnd)
{
    std::cout << "iswindowvisible: " << (int)hWnd << std::endl;
    return hWnd == GraphicsMonitor::shm->window;
}


ATOM WINAPI registerClassExW(CONST WNDCLASSEXW* lpWndClass)
{
    char classN[256];
    wcstombs(classN, lpWndClass->lpszClassName, 256);
    classNameToWndProc[std::string(classN)] = lpWndClass->lpfnWndProc;
    ((WNDCLASSEXW*)lpWndClass)->lpfnWndProc = windowClassProcDetour;

    registerClassExWDetour->disable();
    ATOM retVal = ((ATOM (WINAPI *)(CONST WNDCLASSEXW*))registerClassExWDetour->getFunction())(lpWndClass);
    registerClassExWDetour->enable();

    return retVal;
}



ATOM WINAPI registerClassExA(CONST WNDCLASSEXA* lpWndClass)
{
    classNameToWndProc[std::string(lpWndClass->lpszClassName)] = lpWndClass->lpfnWndProc;
    ((WNDCLASSEXA*)lpWndClass)->lpfnWndProc = windowClassProcDetour;

    registerClassExADetour->disable();
    ATOM retVal = ((ATOM (WINAPI *)(CONST WNDCLASSEXA*))registerClassExADetour->getFunction())(lpWndClass);
    registerClassExADetour->enable();
    return retVal;
}


HWND WINAPI createWindowExA
(
    DWORD dwExStyle,
    LPCSTR className,
    LPCSTR windowName,
    DWORD dwStyle,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    LPVOID param
)
{
    WRITE_LOG();
    createWindowExADetour->disable();
    HWND retVal = ((HWND (WINAPI *)(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID))
                   createWindowExADetour->getFunction())
                   (dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);
    createWindowExADetour->enable();

    hwndToWndProc[retVal] = classNameToWndProc[std::string(className)];

    return retVal;
}


HWND WINAPI createWindowExW
(
    DWORD dwExStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD dwStyle,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    LPVOID param
)
{

    WRITE_LOG();
    createWindowExWDetour->disable();
    HWND retVal = ((WINUSERAPI HWND (WINAPI *)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID))
                   createWindowExWDetour->getFunction())
                   (dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);
    createWindowExWDetour->enable();

    char classN[256];
    wcstombs(classN, className, 256);
    hwndToWndProc[retVal] = classNameToWndProc[std::string(classN)];

    char name[256];
    wcstombs(name, windowName, 256);
    if(strcmp(name, "Tibia") == 0)
    {
        using namespace GraphicsMonitor;
        shm->window = retVal;

//        tagRect clientRect;
//        getClientRectDetour->disable();
//        ((WINBOOL WINAPI (*)(HWND, LPRECT))
//            getClientRectDetour->getFunction())(shm->window, &clientRect);
//        getClientRectDetour->enable();
//
//        tagRect winRect;
//        getWindowRectDetour->disable();
//        ((WINBOOL WINAPI (*)(HWND, LPRECT))
//            getWindowRectDetour->getFunction())(shm->window, &winRect);
//        getWindowRectDetour->enable();



        waitForFrameRequest();
    }

    return retVal;
}

WINBOOL WINAPI screenToClient(HWND hWnd, LPPOINT lpPoint)
{
    screenToClientDetour->disable();
    BOOL retVal =
    ((WINBOOL (WINAPI *)(HWND, LPPOINT))screenToClientDetour->getFunction())(hWnd, lpPoint);
    screenToClientDetour->enable();

    std::cout << "screen to client: " << lpPoint->x << "x" << lpPoint->y << std::endl;
//    if(lpPoint->x == windowBotRightX && lpPoint->y == windowBotRightY)
//    {
//        lpPoint->x += GraphicsMonitor::widthOffset;
//        lpPoint->y += GraphicsMonitor::heightOffset;
//    }

    return retVal;
}

tagPOINT currentCursorPos = {0};
WINBOOL WINAPI getCursorPos(LPPOINT lpPoint)
{
    WRITE_LOG();
    getCursorPosDetour->disable();
    BOOL retVal =
    ((WINBOOL (WINAPI *)(LPPOINT))getCursorPosDetour->getFunction())(lpPoint);
    getCursorPosDetour->enable();

    using namespace GraphicsMonitor;
//    std::cout << "Get cursor pos: " << lpPoint->x << "x" << lpPoint->y << std::endl;
//    memcpy(lpPoint, &currentCursorPos, sizeof(currentCursorPos));

//    if(lpPoint->x >= windowX &&
//       lpPoint->y >= windowY &&
//       lpPoint->x <= windowX + windowWidth &&
//       lpPoint->y <= windowY + windowHeight)
//    {
//        float rescaleX = float(lpPoint->x - windowX) * float(windowWidth + widthOffset) / float(windowWidth);
//        float rescaleY = float(lpPoint->y - windowY) * float(windowHeight + heightOffset) / float(windowHeight);
//        lpPoint->x = windowX + rescaleX;
//        lpPoint->y = windowY + rescaleY;
//    }

//    lpPoint->x = ((float)lpPoint->x) * (clientWidth + widthOffset) / clientWidth;
//    lpPoint->y = ((float)lpPoint->y) * (clientHeight + heightOffset) / clientHeight;
//    lpPoint->x += widthOffset;
//    lpPoint->y += heightOffset;
    return retVal;
}

WINBOOL WINAPI getMessage(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax)
{
    WRITE_LOG();
    std::cout << "GET MESSAGE " << std::endl;
    assert(false);
    getMessageDetour->disable();
    BOOL retVal =
    ((WINBOOL (WINAPI *)(LPMSG, HWND, UINT, UINT))getMessageDetour->getFunction())(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    getMessageDetour->enable();

    return retVal;
}

int WINAPI translateAccelerator(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg)
{
    WRITE_LOG();
    std::cout << "TRANSLATE ACCELERATOR" << std::endl;
    assert(false);
    translateAcceleratorDetour->disable();
    int retVal =
    ((WINUSERAPI int (WINAPI *)(HWND, HACCEL, LPMSG))translateAcceleratorDetour->getFunction())(hWnd, hAccTable, lpMsg);
    translateAcceleratorDetour->enable();

    return retVal;
}

DWORD WINAPI getQueueStatus(UINT flags)
{
    WRITE_LOG();
    getQueueStatusDetour->disable();
    DWORD retVal =
    ((WINUSERAPI DWORD (WINAPI *)(UINT))getQueueStatusDetour->getFunction())(flags);
    getQueueStatusDetour->enable();

//    retVal |= 0x10000;
//    std::cout << "queue status: " << flags << "->" << retVal << std::endl;
    return retVal;
}

BOOL WINAPI getInputState()
{
    WRITE_LOG();
    std::cout << "input state" << std::endl;
    assert(false);
    getInputStateDetour->disable();
    BOOL retVal =
    ((WINUSERAPI WINBOOL (WINAPI *)())getInputStateDetour->getFunction())();
    getInputStateDetour->enable();
    return retVal;
}

BOOL WINAPI translateMessage(const MSG* lpMsg)
{
    WRITE_LOG();
    using namespace GraphicsMonitor;

    translateMessageDetour->disable();
    BOOL retVal =
    ((WINUSERAPI WINBOOL (WINAPI *)(const MSG*))translateMessageDetour->getFunction())(lpMsg);
    translateMessageDetour->enable();
    return retVal;

}

SHORT WINAPI getKeyState(int nVirtKey)
{
    WRITE_LOG();
    getKeyStateDetour->disable();
    SHORT retVal =
    ((WINUSERAPI SHORT (WINAPI *)(int))getKeyStateDetour->getFunction())(nVirtKey);
    getKeyStateDetour->enable();

    return retVal;
}

SHORT WINAPI getAsyncKeyState(int nVirtKey)
{
    WRITE_LOG();
    std::cout << "GET ASYNC KEY STATE: " << std::endl;
    assert(false);
    getAsyncKeyStateDetour->disable();
    SHORT retVal =
    ((WINUSERAPI SHORT (WINAPI *)(int))getAsyncKeyStateDetour->getFunction())(nVirtKey);
    getAsyncKeyStateDetour->enable();
}

BOOL WINAPI getKeyboardState(PBYTE lpKeyState)
{
    WRITE_LOG();
    getKeyboardStateDetour->disable();
    BOOL retVal =
    ((WINUSERAPI BOOL (WINAPI *)(PBYTE))getKeyboardStateDetour->getFunction())(lpKeyState);
    getKeyboardStateDetour->enable();

    return retVal;
}


WINBOOL WINAPI peekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    WRITE_LOG();

    assert(lpMsg != nullptr);
    WRITE_LOG_STEP();


    using namespace GraphicsMonitor;
    peekMessageDetour->disable();
    BOOL retVal =
    ((WINUSERAPI WINBOOL (WINAPI *)(LPMSG, HWND, UINT, UINT, UINT))peekMessageDetour->getFunction())(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    peekMessageDetour->enable();
    WRITE_LOG_STEP();

    auto printEventStuff = [](int l)
    {
        int repeatCount = (0xffff & l);
        int scanCode = ((0xff << 16) & l) >> 16;
        int isExtended = ((1 << 24) & l) >> 24;
        int reserved = ((0xf << 25) & l) >> 25;
        int contextCode = ((1 << 29) & l) >> 29;
        int previousKeyState = ((1 << 30) & l) >> 30;
        int transitionState = ((1 << 31) & l) >> 31;

        std::cout   << "Repeat count: " << repeatCount << std::endl
                    << "Scan code: " << scanCode << std::endl
                    << "Is extended: " << isExtended << std::endl
                    << "Reserved: " << reserved << std::endl
                    << "Context code: " << contextCode << std::endl
                    << "Previous key state: " << previousKeyState << std::endl
                    << "Transition state: " << transitionState << std::endl;
    };

//    std::cout << "peek message " << (int)hWnd << ": " << wRemoveMsg << " " << wMsgFilterMin << "->" << wMsgFilterMax << std::endl;

    WRITE_LOG_STEP();

    switch(*((unsigned short*)&lpMsg->message))
    {
//        case WM_MOUSELEAVE:
//        case 58444:
//        case 43524:
//        case 799:
//        case 49360:
//        case 21236:
//        case WM_PAINT:
//        case WM_USER:
//        case 1025:
//        case WM_TIMER:
//        case WM_SETFONT:
//            break;

//        case 275:
//        case 1025:
//            break;

        case WM_CHAR:
//            std::cout << "CHAR: " << lpMsg->wParam << "    " << lpMsg->lParam << std::endl;
//            printEventStuff(lpMsg->lParam);
            break;
        case WM_KEYDOWN:
//            std::cout << "KEYDOWN: " << lpMsg->wParam << "    " << lpMsg->lParam << std::endl;
//            printEventStuff(lpMsg->lParam);

            switch(lpMsg->wParam)
            {
                case VK_CONTROL:
                    {
                        unsigned char state[256];
                        GetKeyboardState(state);
                        state[VK_CONTROL] = state[VK_LCONTROL] = 0x80;
                        SetKeyboardState(state);
                    }
                    break;
                case VK_SHIFT:
                    {
                        unsigned char state[256];
                        GetKeyboardState(state);
                        state[VK_SHIFT] = state[VK_LSHIFT] = 0x80;
                        SetKeyboardState(state);
                    }
                    break;

                case VK_MENU:
                    {
                        unsigned char state[256];
                        GetKeyboardState(state);
                        state[VK_MENU] = state[VK_LMENU] = 0x80;
                        SetKeyboardState(state);
                    }
                    break;

                default:
                    break;
            }

            break;
        case WM_KEYUP:
//            std::cout << "KEYUP:" << lpMsg->wParam << "    " <<  lpMsg->lParam << std::endl;
//            printEventStuff(lpMsg->lParam);
            switch(lpMsg->wParam)
            {
                case VK_CONTROL:
                    {
                        unsigned char state[256];
                        GetKeyboardState(state);
                        state[VK_CONTROL] = state[VK_LCONTROL] = 0;
                        SetKeyboardState(state);
                    }
                    break;
                case VK_SHIFT:
                    {
                        unsigned char state[256];
                        GetKeyboardState(state);
                        state[VK_SHIFT] = state[VK_LSHIFT] = 0;
                        SetKeyboardState(state);
                    }
                    break;
                case VK_MENU:
                    {
                        unsigned char state[256];
                        GetKeyboardState(state);
                        state[VK_MENU] = state[VK_LMENU] = 0;
                        SetKeyboardState(state);
                    }
                    break;
                default:
                    break;
            }
            break;

//        case WM_MOUSEWHEEL:
//            std::cout << "SCROLL AT : " << GET_X_LPARAM(lpMsg->lParam) << "x" << GET_Y_LPARAM(lpMsg->lParam) << std::endl;
//            break;

//        case WM_LBUTTONDOWN:
//            {
//                unsigned short x = lpMsg->lParam & 0x0000ffff;
//                unsigned short y = (lpMsg->lParam >> 16) & 0x0000ffff;
////                std::cout << "LBUTTONDOWN: " << lpMsg->wParam << "    " << x << "x" << y << std::endl;
////                lpMsg->lParam &= 0x0000ffff;
////                lpMsg->lParam |= (y + 1000) << 16;
////                x = ((float)x) * (clientWidth + 1000.f) / clientWidth;
////                y = ((float)y) * (clientHeight + 1000.f) / clientHeight;
////                lpMsg->lParam = ((y + 1000) << 16) | (x + 1000);
//                x = float(x) * float(clientWidth + widthOffset) / float(clientWidth);
//                y = float(y) * float(clientHeight + heightOffset) / float(clientHeight);
//                lpMsg->lParam = (y << 16) | x;
//                break;
//            }
//
//        case WM_LBUTTONUP:
//            {
//                unsigned short x = lpMsg->lParam & 0x0000ffff;
//                unsigned short y = (lpMsg->lParam >> 16) & 0x0000ffff;
////                std::cout << "LBUTTONUP: " << lpMsg->wParam << "    " << x << "x" << y << std::endl;
////                lpMsg->lParam &= 0x0000ffff;
////                lpMsg->lParam |= (y + 1000) << 16;
////                lpMsg->lParam = ((y + 1000) << 16) | (x + 1000);
////                x = ((float)x) * (clientWidth + 1000.f) / clientWidth;
////                y = ((float)y) * (clientHeight + 1000.f) / clientHeight;
//                x = float(x) * float(clientWidth + widthOffset) / float(clientWidth);
//                y = float(y) * float(clientHeight + heightOffset) / float(clientHeight);
//                lpMsg->lParam = (y << 16) | x;
//                break;
//            }
//
//        case WM_RBUTTONDOWN:
//        {
//            unsigned short x = lpMsg->lParam & 0x0000ffff;
//            unsigned short y = (lpMsg->lParam >> 16) & 0x0000ffff;
////            std::cout << "RBUTTONDOWN: " << lpMsg->wParam << "    " << x << "x" << y << std::endl;
//            currentCursorPos.x = x;
//            currentCursorPos.y = y;
////            ClientToScreen(shm->window, &currentCursorPos);
//            break;
//        }
//
//        case WM_RBUTTONUP:
//            {
//                unsigned short x = lpMsg->lParam & 0x0000ffff;
//                unsigned short y = (lpMsg->lParam >> 16) & 0x0000ffff;
////                std::cout << "RBUTTONUP: " << lpMsg->wParam << "    " << x << "x" << y << std::endl;
//                currentCursorPos.x = x;
//                currentCursorPos.y = y;
////                ClientToScreen(shm->window, &currentCursorPos);
//                break;
//            }
//
//        case WM_MOUSEMOVE:
//            {
////                unsigned short x = lpMsg->lParam & 0x0000ffff;
////                unsigned short y = (lpMsg->lParam >> 16) & 0x0000ffff;
//////                std::cout << "MOUSEMOVE: " << lpMsg->wParam << "     " << x << "x" << y << std::endl;
//////                lpMsg->lParam &= 0x0000ffff;
//////                lpMsg->lParam |= (y + 1000) << 16;
//////                lpMsg->lParam = ((y + 1000) << 16) | (x + 1000);
////                x = float(x) * float(clientWidth + widthOffset) / float(clientWidth);
////                y = float(y) * float(clientHeight + heightOffset) / float(clientHeight);
////                lpMsg->lParam = (y << 16) | x;
//
//                std::cout << "MOUSE MOVE: " << GET_X_LPARAM(lpMsg->lParam) << "x" << GET_Y_LPARAM(lpMsg->lParam) << std::endl;
//                break;
//            }
//
//        case WM_SIZE:
//        {
//            unsigned short x = lpMsg->lParam & 0x0000ffff;
//            unsigned short y = (lpMsg->lParam >> 16) & 0x0000ffff;
////            clientWidth = x;
////            clientHeight = y;
////            lpMsg->lParam = ((y + heightOffset) << 16) | (x + widthOffset);
////            std::cout << "WM_SIZE " << x << "x" << y << std::endl;
//            break;
//        }
//        case WM_SIZING:
//        {
//            LPRECT rect = (LPRECT)lpMsg->lParam;
////            std::cout << "WM_SIZING" << rect->left << "x" << rect->top << "->" << rect->right << "x" << rect->bottom << std::endl;
//            break;
//        }
        default:
//            std::cout << "MESSAGE: " << *((unsigned short*)&lpMsg->message) << std::endl;
            break;
    }
    WRITE_LOG_STEP();

    return retVal;
}

HWND WINAPI getFocus()
{
    getFocusDetour->disable();
    HWND retVal = ((HWND (WINAPI *)())getFocusDetour->getFunction())();
    getFocusDetour->enable();

    std::cout << "Get Focus: " << int(retVal) << "\t(" << int(GraphicsMonitor::shm->window) << ")" << std::endl;

    return retVal;
//    return GraphicsMonitor::shm->window;
}

ATOM WINAPI registerClassW(CONST WNDCLASSW* lpWndClass)
{

    registerClassWDetour->disable();
    ATOM retVal = ((ATOM (WINAPI *)(CONST WNDCLASSW*))registerClassWDetour->getFunction())(lpWndClass);
    registerClassWDetour->enable();

    std::cout << "Register classW" << std::endl;

    return retVal;
}

ATOM WINAPI registerClassA(CONST WNDCLASSA* lpWndClass)
{
    registerClassADetour->disable();
    ATOM retVal = ((ATOM (WINAPI *)(CONST WNDCLASSA*))registerClassADetour->getFunction())(lpWndClass);
    registerClassADetour->enable();

    std::cout << "Register classA" << std::endl;

    return retVal;
}

//struct WindowClassProcHook
//{
//    explicit WindowClassProcHook(WNDPROC proc, WNDPROC detour)
//    : proc(proc)
//    , detour(detour)
//    {
//
//    }
//
//    WNDPROC proc;
//    WNDPROC detour;
//
//};


BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc)
{
    WRITE_LOG();

    makeCurrentDetour->disable();
    BOOL retVal = ((WINGDIAPI BOOL (WINAPI *)(HDC, HGLRC))makeCurrentDetour->getFunction())(hdc, hglrc);
    makeCurrentDetour->enable();
    using namespace GraphicsMonitor;

//    HWND window = WindowFromDC(hdc);
//    std::cout << "window: " << (int)shm->window << std::endl;
//    std::cout << "make current " << (int)window << " " << (int)hdc << " " << (int)hglrc << std::endl;

    if(!areExtendedOpenGlFunctionsHooked && hdc && hglrc)
    {
        compTexImgDetour = new FunctionDetour((void*)getProc("glCompressedTexImage2D"), (void*)compTexImg);
        bindFramebufferDetour = new FunctionDetour((void*)getProc("glBindFramebuffer"), (void*)bindFramebuffer);
        subCompTexImgDetour = new FunctionDetour((void*)getProc("glCompressedTexSubImage2D"), (void*)subCompTexImg);
        bufDatDetour = new FunctionDetour((void*)getProc("glBufferData"), (void*)bufDat);
        bufSubDatDetour = new FunctionDetour((void*)getProc("glBufferSubData"), (void*)bufSubDat);
        subBufDat = (void APIENTRY (*)(GLenum, GLintptr, GLsizeiptr, const GLvoid*))getProc("glBufferSubData");
        getSubBufDat = (void APIENTRY (*)(GLenum, GLintptr, GLsizeiptr, GLvoid*))getProc("glGetBufferSubData");

        framebufferTexDetour = new FunctionDetour((void*)getProc("glFramebufferTexture2D"), (void*)framebufferTex);
        activeTextureDetour = new FunctionDetour((void*)getProc("glActiveTexture"), (void*)activeTexture);
        vertexAttribPointerDetour = new FunctionDetour((void*)getProc("glVertexAttribPointer"), (void*)vertexAttribPointer);
        bindBufferDetour = new FunctionDetour((void*)getProc("glBindBuffer"), (void*)bindBuffer);
        bindBufferBaseDetour = new FunctionDetour((void*)getProc("glBindBufferBase"), (void*)bindBufferBase);
        bindBufferRangeDetour = new FunctionDetour((void*)getProc("glBindBufferRange"), (void*)bindBufferRange);
        bindVertexArrayDetour = new FunctionDetour((void*)getProc("glBindVertexArray"), (void*)bindVertexArray);
        bindVertexBufferDetour = new FunctionDetour((void*)getProc("glBindVertexBuffer"), (void*)bindVertexBuffer);
        vertexArrayVertexBufferDetour = new FunctionDetour((void*)getProc("glVertexArrayVertexBuffer"), (void*)vertexArrayVertexBuffer);

//        framebufferRenderbufferDetour = new FunctionDetour((void*)getProc("glFramebufferRenderbuffer"), (void*)framebufferRenderbuffer);
        useProgramDetour = new FunctionDetour((void*)getProc("glUseProgram"), (void*)useProgram);

        createProgramDetour = new FunctionDetour((void*)getProc("glCreateProgram"), (void*)createProgram);
        shaderSourceDetour = new FunctionDetour((void*)getProc("glShaderSource"), (void*)shaderSource);
        attachShaderDetour = new FunctionDetour((void*)getProc("glAttachShader"), (void*)attachShader);

        uniform3fvDetour = new FunctionDetour((void*)getProc("glUniform3fv"), (void*)uniform3fv);
        uniform4fvDetour = new FunctionDetour((void*)getProc("glUniform4fv"), (void*)uniform4fv);
        uniform3uivDetour = new FunctionDetour((void*)getProc("glUniform3uiv"), (void*)uniform3uiv);
        uniform4uivDetour = new FunctionDetour((void*)getProc("glUniform4uiv"), (void*)uniform4uiv);
        uniformMatrix3fvDetour = new FunctionDetour((void*)getProc("glUniformMatrix3fv"), (void*)uniformMatrix3fv);
        uniformMatrix4fvDetour = new FunctionDetour((void*)getProc("glUniformMatrix4fv"), (void*)uniformMatrix4fv);

        enableiDetour = new FunctionDetour((void*)getProc("glEnablei"), (void*)enablei);

        blendColorDetour = new FunctionDetour((void*)getProc("glBlendColor"), (void*)blendColor);

        blitNamedFramebuffer = (void APIENTRY (*)
        (
            GLuint readFramebuffer,
            GLuint drawFramebuffer,
            GLint srcX0,
            GLint srcY0,
            GLint srcX1,
            GLint srcY1,
            GLint dstX0,
            GLint dstY0,
            GLint dstX1,
            GLint dstY1,
            GLbitfield mask,
            GLenum filter
        ))getProc("glBlitNamedFramebuffer");
        assert(blitNamedFramebuffer != NULL);


        blitFramebuffer = (void APIENTRY (*)
        (
            GLint srcX0,
            GLint srcY0,
            GLint srcX1,
            GLint srcY1,
            GLint dstX0,
            GLint dstY0,
            GLint dstX1,
            GLint dstY1,
            GLbitfield mask,
            GLenum filter
        ))getProc("glBlitFramebuffer");
        assert(blitFramebuffer != NULL);


        genFramebuffers = (void APIENTRY (*)(GLsizei, GLuint*))getProc("glGenFramebuffers");
        assert(genFramebuffers != NULL);

        genRenderbuffers = (void APIENTRY (*)(GLsizei, GLuint*))getProc("glGenRenderbuffers");
        assert(genRenderbuffers != NULL);

        bindRenderbuffer = (void APIENTRY (*)(GLenum, GLuint))getProc("glBindRenderbuffer");
        assert(bindRenderbuffer != NULL);

        renderbufferStorage = (void APIENTRY (*)(GLenum, GLenum, GLsizei, GLsizei))getProc("glRenderbufferStorage");
        assert(renderbufferStorage != NULL);

        framebufferRenderbuffer = (void APIENTRY (*)(GLenum, GLenum, GLenum, GLuint))getProc("glFramebufferRenderbuffer");
        assert(framebufferRenderbuffer != NULL);

        checkFramebufferStatus = (GLenum APIENTRY (*)(GLenum))getProc("glCheckFramebufferStatus");
        assert(checkFramebufferStatus != NULL);

        drawBuffers = (void APIENTRY (*)(GLsizei, const GLenum*))getProc("glDrawBuffers");
        assert(drawBuffers != NULL);

        areExtendedOpenGlFunctionsHooked = true;
    }

    if(shm->window != 0 && WindowFromDC(hdc) == shm->window)
    {
        if(rescaleFramebuffer == 0)
        {

            genFramebuffers(1, &rescaleFramebuffer);
            CHECK_GL_ERROR();
            bindFramebuffer(GL_DRAW_FRAMEBUFFER, rescaleFramebuffer);
            CHECK_GL_ERROR();
        //            genRenderbuffers(1, &rescaleRenderbufferColor);
            CHECK_GL_ERROR();

            genTexFunc(1, &rescaleRenderbufferColor);
            bindTex(GL_TEXTURE_2D, rescaleRenderbufferColor);
            CHECK_GL_ERROR();

        //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            //NULL means reserve texture memory, but texels are undefined

            texImgDetour->disable();
            ((void WINAPI (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))
                texImgDetour->getFunction())(GL_TEXTURE_2D, 0, GL_RGBA32F, 4096, 4096, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            texImgDetour->enable();

            std::cout << __LINE__ << std::endl;
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            std::cout << __LINE__ << std::endl;
            texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            std::cout << __LINE__ << std::endl;
            CHECK_GL_ERROR();

            //Attach 2D texture to this FBO

            framebufferTexDetour->disable();
            ((void WINAPI (*)(GLenum, GLenum, GLenum, GLuint, GLint))framebufferTexDetour->getFunction())
            (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rescaleRenderbufferColor, 0);
            framebufferTexDetour->enable();
            CHECK_GL_ERROR();

            bindTex(GL_TEXTURE_2D, 0);
            CHECK_GL_ERROR();

        //            bindRenderbuffer(GL_RENDERBUFFER, rescaleRenderbufferColor);
        //            CHECK_GL_ERROR();
        //            renderbufferStorage(GL_RENDERBUFFER, GL_RGB8, 2048, 2048);
        //            CHECK_GL_ERROR();
        //            framebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rescaleRenderbufferColor);
            CHECK_GL_ERROR();
            genRenderbuffers(1, &rescaleRenderbufferDepth);
            CHECK_GL_ERROR();
            bindRenderbuffer(GL_RENDERBUFFER, rescaleRenderbufferDepth);
            CHECK_GL_ERROR();

            renderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
            CHECK_GL_ERROR();
            framebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rescaleRenderbufferDepth);
            CHECK_GL_ERROR();

            GLenum status = checkFramebufferStatus(GL_DRAW_FRAMEBUFFER);
            if(status != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "Framebuffer incomplete: " << status << std::endl;
                assert(false);
            }

        //            bindRenderbuffer(GL_RENDERBUFFER, 0);
            bindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
    }

    dataBuffer.clear();
    createNewFrame();
//    pixelStorei(GL_PACK_ALIGNMENT, 1);

    return retVal;
}
HGLRC WINAPI createContext(HDC hdc)
{
    WRITE_LOG();
    createContextDetour->disable();
    HGLRC retVal = ((WINGDIAPI HGLRC (WINAPI *)(HDC))createContextDetour->getFunction())(hdc);
    createContextDetour->enable();

    return retVal;
}

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
//    std::cout << "create file: ";

//    std::cout << name << std::endl;


//    for(LPCSTR c = lpFileName; *c != '\0'; c += 2)
//    {
//        std::cout << *c;
//    }
//    std::cout << std::endl;
//    Sleep(100);
    createFileDetour->disable();
    HANDLE retVal = ((WINBASEAPI HANDLE (WINAPI *)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE))createFileDetour->getFunction())
    (lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    createFileDetour->enable();

    char name[512];
    size_t bytesRead = wcstombs(name, (LPCWSTR)lpFileName, 512);
    GraphicsMonitor::fileHandleToPath[retVal] = std::string(name, bytesRead);

    return retVal;
}

void insertFileIo(HANDLE hFile, SharedMemoryProtocol::FileIo::Type type)
{
    auto pathIt = GraphicsMonitor::fileHandleToPath.find(hFile);
    if(pathIt == GraphicsMonitor::fileHandleToPath.end())
        return;

    using namespace SharedMemoryProtocol;
    FileIo io;
    io.messageType = Message::MessageType::FILE_IO;
    io.type = type;

    const std::string& path = pathIt->second;
    io.pathSize = path.size();

    using namespace GraphicsMonitor;
    waitForIoBufferLock();
    ioBuffer.insert(ioBuffer.end(), (char*)&io, (char*)&io + sizeof(io));
    ioBuffer.insert(ioBuffer.end(), path.c_str(), path.c_str() + io.pathSize);
    signalIoBufferLock();

//    appendToDataBuffer(io);
//    appendToDataBuffer(, io.pathSize);
}


BOOL WINAPI readFile(HANDLE hFile, PVOID lpBuffer, DWORD nNumberOfBytesToRead, PDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
//    std::cout << "read file: "  << std::endl;
//    Sleep(100);
    readFileDetour->disable();
    BOOL retVal = ((WINBASEAPI BOOL (WINAPI *)(HANDLE,PVOID,DWORD,PDWORD,LPOVERLAPPED))readFileDetour->getFunction())
        (hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    readFileDetour->enable();

    insertFileIo(hFile, SharedMemoryProtocol::FileIo::Type::READ);

    return retVal;
}

BOOL WINAPI readFileEx(HANDLE hFile, PVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
//    std::cout << "read file ex" << std::endl;
//    Sleep(100);

    readFileExDetour->disable();
    BOOL retVal = ((WINBASEAPI BOOL (WINAPI *)(HANDLE,PVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE))readFileExDetour->getFunction())
        (hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);
    readFileExDetour->enable();

    insertFileIo(hFile, SharedMemoryProtocol::FileIo::Type::READ);

    return retVal;
}

WINBOOL WINAPI writeFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
//    std::cout << "write file" << std::endl;

    writeFileDetour->disable();
    BOOL retVal = ((WINBASEAPI BOOL (WINAPI *)(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED))writeFileDetour->getFunction())
        (hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    writeFileDetour->enable();

    insertFileIo(hFile, SharedMemoryProtocol::FileIo::Type::WRITE);
    return retVal;
}

WINBOOL WINAPI writeFileEx(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    writeFileExDetour->disable();
    BOOL retVal = ((WINBASEAPI BOOL (WINAPI *)(HANDLE,LPCVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE))writeFileExDetour->getFunction())
        (hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
    writeFileExDetour->enable();

    insertFileIo(hFile, SharedMemoryProtocol::FileIo::Type::WRITE);

    return retVal;
}

WINBOOL WINAPI virtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    std::cout << "virtualProtect: " << (int)lpAddress << ", " << flNewProtect << std::endl;
    virtualProtectDetour->disable();
    WINBOOL retVal = ((WINBOOL (WINAPI *)(LPVOID, SIZE_T, DWORD, PDWORD))virtualProtectDetour->getFunction())
        (lpAddress, dwSize, flNewProtect, lpflOldProtect);
    virtualProtectDetour->enable();
}

WINBOOL WINAPI virtualProtectEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    std::cout << "virtualProtectEx: " << (int)lpAddress << ", " << flNewProtect << std::endl;
    virtualProtectExDetour->disable();
    WINBOOL retVal = ((WINBOOL (WINAPI *)(HANDLE, LPVOID, SIZE_T, DWORD, PDWORD))virtualProtectExDetour->getFunction())
        (hProcess, lpAddress, dwSize, flNewProtect, lpflOldProtect);
    virtualProtectExDetour->enable();
}

LPVOID WINAPI virtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    virtualAllocDetour->disable();
    LPVOID retVal = ((LPVOID (WINAPI *)(LPVOID, SIZE_T, DWORD, DWORD))virtualAllocDetour->getFunction())
        (lpAddress, dwSize, flAllocationType, flProtect);
    std::cout << "virtual alloc: " << (int)retVal << ", " << flProtect << std::endl;
    virtualAllocDetour->enable();
}

LPVOID WINAPI virtualAllocEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    virtualAllocExDetour->disable();
    LPVOID retVal = ((LPVOID (WINAPI *)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD))virtualAllocExDetour->getFunction())
        (hProcess, lpAddress, dwSize, flAllocationType, flProtect);
    std::cout << "virtual alloc ex: " << (int)retVal << ", " << flProtect << std::endl;
    virtualAllocExDetour->enable();
}

HINSTANCE WINAPI detourFunc(LPCSTR moduleName)
{
    WRITE_LOG();
    loadLibDetour->disable();
    HINSTANCE retVal = ((WINBASEAPI HINSTANCE WINAPI (*)(LPCSTR))loadLibDetour->getFunction())(moduleName);
    loadLibDetour->enable();

//    std::cout << "module: " << moduleName << std::endl;

    if(strcmp(moduleName, "OPENGL32.DLL") == 0 || strcmp(moduleName, "opengl32.dll") == 0)
    {
        if(genTexDetour)
        {
            delete pixelStoreiDetour;
            delete genTexDetour;
            delete swapBufDetour;
            delete delTexDetour;
            delete texImgDetour;
            delete subTexImgDetour;
            delete bindTexDetour;
            delete drawArraysDetour;
            delete drawElementsDetour;
            delete makeCurrentDetour;
            delete createContextDetour;
            delete viewportDetour;
            delete drawBufferDetour;

            delete disableDetour;
            delete enableDetour;

            delete copyTexImageDetour;
            delete copyTexSubImageDetour;
            delete readBufferDetour;
            delete vertexPointerDetour;
            delete enableClientStateDetour;
            delete scissorDetour;
        }

        getError = (GLenum APIENTRY (*)())GetProcAddress(retVal, "glGetError");
        getIntegerv = (void APIENTRY (*)(GLenum, GLint*))GetProcAddress(retVal, "glGetIntegerv");
        getTexImage = (void APIENTRY (*)(GLenum, GLint, GLenum, GLenum, void*))GetProcAddress(retVal, "glGetTexImage");
        getTexLevelParameteriv = (void APIENTRY (*)(GLenum, GLint, GLenum, GLint*))GetProcAddress(retVal, "glGetTexLevelParameteriv");
//        pixelStorei = (void APIENTRY (*)(GLenum, GLint))GetProcAddress(retVal, "glPixelStorei");


        pixelStoreiDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glPixelStorei"), (void*)pixelStorei);
        pixelStorefDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glPixelStoref"), (void*)pixelStoref);
        genTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glGenTextures"), (void*)genTexFunc);
        swapBufDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("gdi32.dll"), "SwapBuffers"), (void*)swapBuf);
        delTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDeleteTextures"), (void*)delTex);
        texImgDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glTexImage2D"), (void*)texImg);
        subTexImgDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glTexSubImage2D"), (void*)subTexImg);
        bindTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glBindTexture"), (void*)bindTex);
        drawArraysDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawArrays"), (void*)drawArrays);
        drawElementsDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawElements"), (void*)drawElements);

        getProc = (WINGDIAPI PROC (WINAPI *)(LPCSTR))GetProcAddress(retVal, "wglGetProcAddress");
        createContextDetour = new FunctionDetour((void*)GetProcAddress(retVal, "wglCreateContext"), (void*)createContext);
        makeCurrentDetour = new FunctionDetour((void*)GetProcAddress(retVal, "wglMakeCurrent"), (void*)makeCurrent);

        viewportDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glViewport"), (void*)viewport);
//        drawBufferDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawBuffer"), (void*)drawBuffer);

        disableDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDisable"), (void*)disable);
        enableDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glEnable"), (void*)enable);

        copyTexImageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glCopyTexImage2D"), (void*)copyTexImage);
        copyTexSubImageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glCopyTexSubImage2D"), (void*)copyTexSubImage);

        readBufferDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glReadBuffer"), (void*)readBuffer);

        vertexPointerDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glVertexPointer"), (void*)vertexPointer);
        enableClientStateDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glEnableClientState"), (void*)enableClientState);

        texParameteri = (void APIENTRY (*)(GLenum, GLenum, GLint))GetProcAddress(retVal, "glTexParameteri");
        assert(texParameteri != NULL);

        clear = (void APIENTRY (*)(GLbitfield))GetProcAddress(retVal, "glClear");
        assert(clear != NULL);

        scissorDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glScissor"), (void*)scissor);

    }
    else if(strcmp(moduleName, "USER32.DLL") == 0 || strcmp(moduleName, "user32.dll") == 0)
    {
        if(createWindowExWDetour)
        {
            delete createWindowExWDetour;
            delete createWindowExADetour;
            delete createWindowADetour;
            delete createWindowWDetour;

            delete peekMessageDetour;
            delete getCursorPosDetour;
//            delete getMessageDetour;
//            delete getKeyStateDetour;
//            delete getAsyncKeyStateDetour;
//            delete getKeyboardStateDetour;
//            delete translateMessageDetour;
//            delete translateAcceleratorDetour;
//            delete getQueueStatusDetour;
//            delete getInputStateDetour;

//            delete getClientRectDetour;
//            delete getWindowRectDetour;
//            delete screenToClientDetour;
//            delete isWindowVisibleDetour;
//            delete getFocusDetour;

//            delete registerClassWDetour;
//            delete registerClassADetour;
            delete registerClassExWDetour;
            delete registerClassExADetour;
        }

        createWindowExWDetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowExW"), (void*)createWindowExW);
        createWindowExADetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowExA"), (void*)createWindowExA);
//        createWindowADetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowA"), (void*)createWindowA);
//        createWindowWDetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowW"), (void*)createWindowW);
        peekMessageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "PeekMessageW"), (void*)peekMessage);
//        getMessageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetMessageA"), (void*)getMessage);
        getCursorPosDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetCursorPos"), (void*)getCursorPos);
//        getKeyStateDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetKeyState"), (void*)getKeyState);
//        getAsyncKeyStateDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetAsyncKeyState"), (void*)getAsyncKeyState);
//        getKeyboardStateDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetKeyboardState"), (void*)getKeyboardState);
//        translateMessageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "TranslateMessage"), (void*)translateMessage);
//        translateAcceleratorDetour = new FunctionDetour((void*)GetProcAddress(retVal, "TranslateAcceleratorA"), (void*)translateAccelerator);
//        getQueueStatusDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetQueueStatus"), (void*)getQueueStatus);
//        getInputStateDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetInputState"), (void*)getInputState);
//        getClientRectDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetClientRect"), (void*)getClientRect);
//        getWindowRectDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetWindowRect"), (void*)getWindowRect);
//        screenToClientDetour = new FunctionDetour((void*)GetProcAddress(retVal, "ScreenToClient"), (void*)screenToClient);

//        isWindowVisibleDetour = new FunctionDetour((void*)GetProcAddress(retVal, "IsWindowVisible"), (void*)isWindowVisible);
//        getFocusDetour = new FunctionDetour((void*)GetProcAddress(retVal, "GetFocus"), (void*)getFocus);
//        registerClassWDetour = new FunctionDetour((void*)GetProcAddress(retVal, "RegisterClassW"), (void*)registerClassW);
//        registerClassADetour = new FunctionDetour((void*)GetProcAddress(retVal, "RegisterClassA"), (void*)registerClassA);
        registerClassExWDetour = new FunctionDetour((void*)GetProcAddress(retVal, "RegisterClassExW"), (void*)registerClassExW);
        registerClassExADetour = new FunctionDetour((void*)GetProcAddress(retVal, "RegisterClassExA"), (void*)registerClassExA);
    }
    else if(strcmp(moduleName, "KERNEL32.DLL") == 0 || strcmp(moduleName, "kernel32.dll") == 0)
    {
        std::cout << "KERNEL32 " << std::endl;
        if(readFileDetour)
        {
            delete createFileDetour;
            delete readFileDetour;
            delete readFileExDetour;
            delete writeFileDetour;
            delete writeFileExDetour;
//            delete virtualProtectDetour;
        }

//        virtualProtectDetour = new FunctionDetour((void*)GetProcAddress(retVal, "VirtualProtect"), (void*)virtualProtect);

//        createFileDetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateFileW"), (void*)createFile);
//        readFileDetour = new FunctionDetour((void*)GetProcAddress(retVal, "ReadFile"), (void*)readFile);
//        readFileExDetour = new FunctionDetour((void*)GetProcAddress(retVal, "ReadFileEx"), (void*)readFileEx);
//        writeFileDetour = new FunctionDetour((void*)GetProcAddress(retVal, "WriteFile"), (void*)writeFile);
//        writeFileExDetour = new FunctionDetour((void*)GetProcAddress(retVal, "WriteFileEx"), (void*)writeFileEx);
    }

    return retVal;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            std::cout << "Hallo!" << std::endl;
            GraphicsMonitor::log.open("sb-error.log");

//            setPointSizeDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("Qt5Gui.dll"), "?setPointSize@QFont@@QAEXH@Z"), (void*)(QFont::setPointSize));
//            setPointSizeFDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("Qt5Gui.dll"), "?setPointSizeF@QFont@@QAEXN@Z"), (void*)(QFont::setPointSizeF));
//            void (QFont::*qFontFunc)(const QString &family, int pointSize, int weight, bool italic);
//            qFontFunc = QFont::QFont;
//            qFontDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("Qt5Gui.dll"), "??0QFont@@QAE@ABVQString@@HH_N@Z"), (void*)QFont::QFont);
//            createFileDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateFileW"), (void*)createFile);
//            createFileDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernelbase.dll"), "CreateFileW"), (void*)createFile);

            {
                char szName[SharedMemoryProtocol::SHARED_MEMORY_NAME_LENGTH + 1];
                GetEnvironmentVariable(SharedMemoryProtocol::SHARED_MEMORY_ENV_VAR_NAME, szName, sizeof(szName));
                SetEnvironmentVariable(SharedMemoryProtocol::SHARED_MEMORY_ENV_VAR_NAME, NULL);
                HANDLE hMapFile = OpenFileMapping
                (
                    FILE_MAP_ALL_ACCESS,   // read/write access
                    FALSE,                 // do not inherit the name
                    szName      // name of mapping object
                );

                if (hMapFile == NULL)
                {
                    std::stringstream sstream;
                    sstream << "Could not create shared memory object (" << GetLastError() << ").";
                    std::cout << sstream.str();
                    throw std::runtime_error(sstream.str());
                }

                GraphicsMonitor::shm = (SharedMemoryProtocol::SharedMemorySegment*) MapViewOfFile
                (
                    hMapFile, // handle to map object
                    FILE_MAP_ALL_ACCESS,  // read/write permission
                    0,
                    0,
                    SharedMemoryProtocol::NUM_BYTES
                );

                if (GraphicsMonitor::shm == NULL)
                {
                    std::stringstream sstream;
                    sstream << "Could not map shared memory (" << GetLastError() << ").";
                    std::cout << sstream.str();
                    throw std::runtime_error(sstream.str());
                }

                GraphicsMonitor::ioBufferLock = CreateMutex(NULL, FALSE, NULL);

//                std::cout << GraphicsMonitor::shm->numDrawCalls << std::endl;

//                UnmapViewOfFile(pBuf);
//
//                CloseHandle(hMapFile);

                void* loadLib = (void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");


                if(loadLib == NULL)
                    std::cout << "Cannot get loadLib address" << std::endl;

                loadLibDetour = new FunctionDetour(loadLib, (void*)detourFunc);
//                virtualProtectDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualProtect"), (void*)virtualProtect);
//                virtualProtectExDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualProtectEx"), (void*)virtualProtectEx);
//                virtualAllocDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualAlloc"), (void*)virtualAlloc);
//                virtualAllocExDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualAllocEx"), (void*)virtualAllocEx);
            }
            break;

        default:
            break;
    }
    return TRUE;
}
