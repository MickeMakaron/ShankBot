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
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cstdio>
#include "utility/FunctionDetour.hpp"
#include "utility/Vertex.hpp"
#include "injection/SharedMemoryProtocol.hpp"


#include "GL/glew.h"

#include <fstream>
#include <algorithm>
#include <list>

#include <sstream>

using namespace GraphicsLayer;

FunctionDetour* loadLibDetour = nullptr;
FunctionDetour* swapBufDetour = nullptr;
FunctionDetour* delTexDetour = nullptr;
FunctionDetour* texImgDetour = nullptr;
FunctionDetour* subTexImgDetour = nullptr;
FunctionDetour* bufDatDetour = nullptr;
FunctionDetour* bindTexDetour = nullptr;
FunctionDetour* drawArraysDetour = nullptr;
FunctionDetour* drawElementsDetour = nullptr;
FunctionDetour* bindFramebufferDetour = nullptr;
FunctionDetour* framebufferTexDetour = nullptr;
FunctionDetour* activeTextureDetour = nullptr;
FunctionDetour* vertexAttribPointerDetour = nullptr;
FunctionDetour* bindBufferDetour = nullptr;
FunctionDetour* makeCurrentDetour = nullptr;
FunctionDetour* viewportDetour = nullptr;
FunctionDetour* useProgramDetour = nullptr;
FunctionDetour* uniform4fvDetour = nullptr;
FunctionDetour* uniformMatrix4fvDetour = nullptr;
FunctionDetour* copyTexImageDetour = nullptr;
FunctionDetour* copyTexSubImageDetour = nullptr;
FunctionDetour* bindVertexArrayDetour = nullptr;
FunctionDetour* blendColorDetour = nullptr;
FunctionDetour* pixelStoreiDetour = nullptr;

FunctionDetour* createWindowExWDetour = nullptr;
FunctionDetour* createWindowExADetour = nullptr;
FunctionDetour* peekMessageDetour = nullptr;
FunctionDetour* registerClassExADetour = nullptr;

PROC (WINAPI *getProc)(LPCSTR) = nullptr;


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

    std::map<GLuint, Texture> textures;

    GLuint boundTexture = 0;
    GLuint boundFramebuffer = 0;
    GLuint boundBuffer = 0;
    GLuint boundRenderbuffer = 0;
    GLuint boundProgram = 0;
    Color currentBlendColor;


    struct TextureUnit
    {
        TextureUnit() : targets(), id(0){}

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
    : mTextureUnits()
    , mActiveTextureUnit(nullptr)
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

    float peakDataOccupancy = 0.f;


    unsigned short currentViewportWidth = 0;
    unsigned short currentViewportHeight = 0;
    unsigned int currentUnpackAlignment = 4;

    std::map<GLuint, GLuint> vaoToVbo;
    GLuint boundVertexArray = 0;

    std::ofstream log;



    std::map<std::string, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)> classNameToWndProc;
    std::map<HWND, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)> hwndToWndProc;

}

#ifdef NDEBUG
#define WRITE_LOG() ((void)0)
#define WRITE_LOG_STEP() ((void)0)
#else
#define WRITE_LOG() GraphicsMonitor::log << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")" << std::endl
#define WRITE_LOG_STEP() GraphicsMonitor::log << "\t" << __LINE__ << std::endl
#endif

#define THROW_ASSERT(v) if(!(v)) {std::cout << __LINE__ << std::endl; throw 1;}

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
    using namespace GraphicsMonitor;
    THROW_ASSERT(x == 0 && y == 0);
    currentViewportWidth = width;
    currentViewportHeight = height;

    viewportDetour->disable();
    ((void APIENTRY (*)(GLint, GLint, GLsizei, GLsizei))viewportDetour->getFunction())(x, y, width, height);
    viewportDetour->enable();
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
    GraphicsMonitor::boundProgram = program;

    useProgramDetour->disable();
    ((void APIENTRY (*)(GLuint))useProgramDetour->getFunction())(program);
    useProgramDetour->enable();
}


void APIENTRY uniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;
    THROW_ASSERT(count == 1);

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

void APIENTRY uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    THROW_ASSERT(count == 1);
    THROW_ASSERT(!transpose);

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

void APIENTRY blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GraphicsMonitor::currentBlendColor = Color(red * 255, green * 255, blue * 255, alpha * 255);

    blendColorDetour->disable();
    ((void APIENTRY (*)(GLfloat, GLfloat, GLfloat, GLfloat))blendColorDetour->getFunction())(red, green, blue, alpha);
    blendColorDetour->enable();
}

void WINAPI bindTex(GLenum target, GLuint texture)
{
    using namespace GraphicsMonitor;

    textureUnits.setTexture(target, texture);
    bindTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))
        bindTexDetour->getFunction())(target, texture);
    bindTexDetour->enable();
}

void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count)
{
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
        THROW_ASSERT(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

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

    drawArraysDetour->disable();
    ((void APIENTRY (*)(GLenum, GLint, GLsizei))drawArraysDetour->getFunction())(mode, first, count);
    drawArraysDetour->enable();
}

void WINAPI bindVertexArray(GLuint array)
{
    using namespace GraphicsMonitor;
    boundVertexArray = array;

    bindVertexArrayDetour->disable();
    ((void WINAPI (*)(GLuint))bindVertexArrayDetour->getFunction())(array);
    bindVertexArrayDetour->enable();
}


void WINAPI bindBuffer(GLenum target, GLuint buffer)
{

    GraphicsMonitor::boundBuffer = buffer;

    THROW_ASSERT(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER);

    bindBufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindBufferDetour->getFunction())(target, buffer);
    bindBufferDetour->enable();
}

void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
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

    vertexAttribPointerDetour->disable();
    ((void WINAPI (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))
     vertexAttribPointerDetour->getFunction())(index, size, type, normalized, stride, pointer);
    vertexAttribPointerDetour->enable();
}

void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    using namespace GraphicsMonitor;

    THROW_ASSERT(boundBuffer != 1);

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
        THROW_ASSERT(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

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
    drawCall.messageType = SharedMemoryProtocol::Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);


    drawElementsDetour->disable();
    ((void APIENTRY (*)(GLenum, GLsizei, GLenum, const GLvoid*))drawElementsDetour->getFunction())(mode, count, type, indices);
    drawElementsDetour->enable();
}

void WINAPI bufDat
(
    GLenum target,
    GLsizei size,
    const GLvoid* data,
    GLenum usage
)
{
    using namespace GraphicsMonitor;

    GLuint buf = boundBuffer;

    SharedMemoryProtocol::VertexBufferWrite& buffer = vertexBuffers[buf];
    buffer.bufferId = buf;
    buffer.numBytes = size;

    buffer.messageType = SharedMemoryProtocol::Message::MessageType::VERTEX_BUFFER_WRITE;
    appendToDataBuffer(buffer);
    appendToDataBuffer((char*)data, size);

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
    if(data == nullptr)
        return;

    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    THROW_ASSERT(format == GL_BGRA || format == GL_RGBA || format == GL_ALPHA);
    THROW_ASSERT(type == GL_UNSIGNED_BYTE);

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
    size_t size = width * height * bytesPerPixel;
    p.messageType = Message::MessageType::PIXEL_DATA;
    appendToDataBuffer(p);
    if(p.format == PixelFormat::ALPHA)
    {
        size_t remainder = width % currentUnpackAlignment;
        if(remainder != 0)
        {
            size_t paddedWidth = width + currentUnpackAlignment - remainder;
            char* unpaddedData = new char[size];
            for(size_t iSrc = 0, iDest = 0; iDest < size; iSrc += paddedWidth, iDest += width)
                memcpy(&unpaddedData[iDest], &data[iSrc], width);
            appendToDataBuffer(unpaddedData, size);
            delete[] unpaddedData;
            return;
        }
    }
    appendToDataBuffer((char*)data, size);
}

void WINAPI copyTexImage(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    THROW_ASSERT(level == 0);
    using namespace GraphicsMonitor;
    unsigned int targetTextureId = GraphicsMonitor::textureUnits.getTexture(target);
    unsigned int sourceTextureId = 0;
    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        THROW_ASSERT(it != framebufferTextureAttachment.end());

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

    copyTexImageDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))
        copyTexImageDetour->getFunction())(target, level, internalformat, x, y, width, height, border);
    copyTexImageDetour->enable();
}

void WINAPI copyTexSubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    using namespace GraphicsMonitor;
    THROW_ASSERT(level == 0);
    unsigned int targetTextureId = GraphicsMonitor::textureUnits.getTexture(target);
    unsigned int sourceTextureId = 0;
    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        THROW_ASSERT(it != framebufferTextureAttachment.end());

        sourceTextureId = it->second;
    }

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
    THROW_ASSERT(level == 0);
    GLuint textureId = GraphicsMonitor::textureUnits.getTexture(target);

    if(target == GL_TEXTURE_2D)
    {
        GraphicsMonitor::textures[textureId] = {width, height, format};
        if(width == 480 && height == 352)
        {
            GraphicsMonitor::textureBuffers.push_back(textureId);
            THROW_ASSERT(GraphicsMonitor::textureBuffers.size() <= GraphicsMonitor::TextureBuffers::NUM_BUFFERS);
        }

        if(width == 4096 && height == 4096)
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
    THROW_ASSERT(level == 0);
    using namespace GraphicsMonitor;
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

    subTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
        subTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, type, pixels);
    subTexImgDetour->enable();
}


void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer)
{
    GraphicsMonitor::boundFramebuffer = framebuffer;

    bindFramebufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindFramebufferDetour->getFunction())(target, framebuffer);
    bindFramebufferDetour->enable();
}

void WINAPI activeTexture(GLenum texture)
{
    GraphicsMonitor::textureUnits.setActiveTextureUnit(texture);

    activeTextureDetour->disable();
    ((void WINAPI (*)(GLenum))activeTextureDetour->getFunction())(texture);
    activeTextureDetour->enable();
}


void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GraphicsMonitor::framebufferTextureAttachment[GraphicsMonitor::boundFramebuffer] = texture;
    THROW_ASSERT(level == 0);
    framebufferTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLenum, GLenum, GLuint, GLint))framebufferTexDetour->getFunction())(target, attachment, textarget, texture, level);
    framebufferTexDetour->enable();
}


void updateSharedMemory()
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

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
        THROW_ASSERT(insertionSize <= dataBuffer.size());
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



LRESULT CALLBACK windowClassProcDetour(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_SIZE:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        default:
            break;
    }

    using namespace GraphicsMonitor;
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

    THROW_ASSERT(false);
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
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    THROW_ASSERT(getCurrentFrame()->size <= DATA_BUFFER_SIZE);

    getCurrentFrame()->width = currentViewportWidth;
    getCurrentFrame()->height = currentViewportHeight;

    updateSharedMemory();
    createNewFrame();

    swapBufDetour->disable();
    BOOL retVal = ((BOOL WINAPI (*)(HDC))swapBufDetour->getFunction())(hdc);
    swapBufDetour->enable();

    postFrame();
    waitForFrameRequest();
    return retVal;
}

void WINAPI delTex(GLsizei n, const GLuint* textures)
{
    for(size_t i = 0; i < n; i++)
    {
        GraphicsMonitor::textures.erase(textures[i]);
    }

    delTexDetour->disable();
    ((void WINAPI (*)(GLsizei, const GLuint*))delTexDetour->getFunction())(n, textures);
    delTexDetour->enable();
}


ATOM WINAPI registerClassExA(CONST WNDCLASSEXA* lpWndClass)
{
    using namespace GraphicsMonitor;
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
    createWindowExADetour->disable();
    HWND retVal = ((HWND (WINAPI *)(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID))
                   createWindowExADetour->getFunction())
                   (dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);
    createWindowExADetour->enable();

    using namespace GraphicsMonitor;
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
    createWindowExWDetour->disable();
    HWND retVal = ((HWND (WINAPI *)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID))
                   createWindowExWDetour->getFunction())
                   (dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);
    createWindowExWDetour->enable();

    char classN[256];
    wcstombs(classN, className, 256);

    using namespace GraphicsMonitor;
    hwndToWndProc[retVal] = classNameToWndProc[std::string(classN)];

    char name[256];
    wcstombs(name, windowName, 256);
    if(strcmp(name, "Tibia") == 0)
    {
        using namespace GraphicsMonitor;
        shm->window = retVal;
        waitForFrameRequest();
    }

    return retVal;
}

WINBOOL WINAPI peekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    THROW_ASSERT(lpMsg != nullptr);

    using namespace GraphicsMonitor;
    peekMessageDetour->disable();
    BOOL retVal =
    ((WINBOOL (WINAPI *)(LPMSG, HWND, UINT, UINT, UINT))peekMessageDetour->getFunction())(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    peekMessageDetour->enable();

    switch(lpMsg->message)
    {
        case WM_KEYDOWN:
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

        default:
            break;
    }

    return retVal;
}

BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc)
{
    makeCurrentDetour->disable();
    BOOL retVal = ((BOOL (WINAPI *)(HDC, HGLRC))makeCurrentDetour->getFunction())(hdc, hglrc);
    makeCurrentDetour->enable();
    using namespace GraphicsMonitor;

    if(!areExtendedOpenGlFunctionsHooked && hdc && hglrc)
    {
        bindFramebufferDetour = new FunctionDetour((void*)getProc("glBindFramebuffer"), (void*)bindFramebuffer);
        bufDatDetour = new FunctionDetour((void*)getProc("glBufferData"), (void*)bufDat);

        framebufferTexDetour = new FunctionDetour((void*)getProc("glFramebufferTexture2D"), (void*)framebufferTex);
        activeTextureDetour = new FunctionDetour((void*)getProc("glActiveTexture"), (void*)activeTexture);
        vertexAttribPointerDetour = new FunctionDetour((void*)getProc("glVertexAttribPointer"), (void*)vertexAttribPointer);
        bindBufferDetour = new FunctionDetour((void*)getProc("glBindBuffer"), (void*)bindBuffer);
        bindVertexArrayDetour = new FunctionDetour((void*)getProc("glBindVertexArray"), (void*)bindVertexArray);

        useProgramDetour = new FunctionDetour((void*)getProc("glUseProgram"), (void*)useProgram);


        uniform4fvDetour = new FunctionDetour((void*)getProc("glUniform4fv"), (void*)uniform4fv);
        uniformMatrix4fvDetour = new FunctionDetour((void*)getProc("glUniformMatrix4fv"), (void*)uniformMatrix4fv);

        blendColorDetour = new FunctionDetour((void*)getProc("glBlendColor"), (void*)blendColor);

        areExtendedOpenGlFunctionsHooked = true;
    }

    dataBuffer.clear();
    createNewFrame();

    return retVal;
}


HINSTANCE WINAPI detourFunc(LPCSTR moduleName)
{
    loadLibDetour->disable();
    HINSTANCE retVal = ((HINSTANCE WINAPI (*)(LPCSTR))loadLibDetour->getFunction())(moduleName);
    loadLibDetour->enable();

    if(strcmp(moduleName, "OPENGL32.DLL") == 0 || strcmp(moduleName, "opengl32.dll") == 0)
    {
        if(swapBufDetour)
        {
            delete pixelStoreiDetour;
            delete swapBufDetour;
            delete delTexDetour;
            delete texImgDetour;
            delete subTexImgDetour;
            delete bindTexDetour;
            delete drawArraysDetour;
            delete drawElementsDetour;
            delete makeCurrentDetour;
            delete viewportDetour;
            delete copyTexImageDetour;
            delete copyTexSubImageDetour;
        }

        pixelStoreiDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glPixelStorei"), (void*)pixelStorei);
        swapBufDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("gdi32.dll"), "SwapBuffers"), (void*)swapBuf);
        delTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDeleteTextures"), (void*)delTex);
        texImgDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glTexImage2D"), (void*)texImg);
        subTexImgDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glTexSubImage2D"), (void*)subTexImg);
        bindTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glBindTexture"), (void*)bindTex);
        drawArraysDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawArrays"), (void*)drawArrays);
        drawElementsDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawElements"), (void*)drawElements);

        getProc = (PROC (WINAPI *)(LPCSTR))GetProcAddress(retVal, "wglGetProcAddress");
        makeCurrentDetour = new FunctionDetour((void*)GetProcAddress(retVal, "wglMakeCurrent"), (void*)makeCurrent);

        viewportDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glViewport"), (void*)viewport);


        copyTexImageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glCopyTexImage2D"), (void*)copyTexImage);
        copyTexSubImageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glCopyTexSubImage2D"), (void*)copyTexSubImage);
    }
    else if(strcmp(moduleName, "USER32.DLL") == 0 || strcmp(moduleName, "user32.dll") == 0)
    {
        if(createWindowExWDetour)
        {
            delete createWindowExWDetour;
            delete createWindowExADetour;
            delete peekMessageDetour;
            delete registerClassExADetour;
        }

        createWindowExWDetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowExW"), (void*)createWindowExW);
        createWindowExADetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowExA"), (void*)createWindowExA);
        peekMessageDetour = new FunctionDetour((void*)GetProcAddress(retVal, "PeekMessageW"), (void*)peekMessage);
        registerClassExADetour = new FunctionDetour((void*)GetProcAddress(retVal, "RegisterClassExA"), (void*)registerClassExA);
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

                void* loadLib = (void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");


                if(loadLib == NULL)
                    std::cout << "Cannot get loadLib address" << std::endl;

                loadLibDetour = new FunctionDetour(loadLib, (void*)detourFunc);

                GraphicsMonitor::shm->isClientAttached = true;
            }
            break;

        default:
            break;
    }
    return TRUE;
}
