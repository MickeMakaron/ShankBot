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
#include "injection/Monitor.hpp"
#include "injection/utility.hpp"
using namespace GraphicsLayer::SharedMemoryProtocol;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
///////////////////////////////////

namespace GraphicsLayer
{
#define THROW_ASSERT(v) if(!(v)) {std::cout << __LINE__ << std::endl; throw 1;}



std::map<std::string, WindowProc> Monitor::mClassNameToWndProc;
std::map<HWND, WindowProc> Monitor::mHwndToWndProc;

Monitor::Monitor()
{
    char szName[SHARED_MEMORY_NAME_LENGTH + 1];
    GetEnvironmentVariable(SHARED_MEMORY_ENV_VAR_NAME, szName, sizeof(szName));
    SetEnvironmentVariable(SHARED_MEMORY_ENV_VAR_NAME, NULL);
    HANDLE hMapFile = OpenFileMapping
    (
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        szName      // name of mapping object
    );

    if (hMapFile == NULL)
    {
        throw std::runtime_error("Could not create shared memory object (" + std::to_string(GetLastError()) + ").");
    }

    mShm = (SharedMemorySegment*) MapViewOfFile
    (
        hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        NUM_BYTES
    );

    if (mShm == NULL)
    {
        throw std::runtime_error("Could not map shared memory (" + std::to_string(GetLastError()) + ").");
    }

    mShm->isClientAttached = true;
}


Frame* Monitor::getCurrentFrame() const
{
    return (Frame*)((char*)mDataBuffer.data() + mCurrentFrameIndex);
}

void Monitor::createNewFrame()
{
    Frame f;
    mCurrentFrameIndex = mDataBuffer.size();
    mDataBuffer.insert(mDataBuffer.end(), (char*)&f, (char*)&f + sizeof(f));
}

void Monitor::appendToDataBuffer(const char* data, size_t size)
{
    getCurrentFrame()->size += size;
    mDataBuffer.insert(mDataBuffer.end(), data, data + size);
}

void Monitor::setDepthTest(bool doEnable)
{
    mIsDepthTestEnabled = doEnable;
}

void Monitor::setDepthFunc(GLenum func)
{
    THROW_ASSERT(func == GL_LESS);
}

void Monitor::setDepthMask(bool doEnable)
{
    mIsDepthWriteEnabled = doEnable;
}

void Monitor::setDepthRange(double near, double far)
{
    THROW_ASSERT(false);
}

void Monitor::toggleVao(GLuint index, bool doEnable)
{
    THROW_ASSERT(index < 8);
    if(doEnable)
    {
        mVaoEnableData |= 1 << index;
    }
    else
    {
        mVaoEnableData &= ~(1 << index);
    }
}

void Monitor::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    THROW_ASSERT(x == 0 && y == 0);
    mCurrentViewportWidth = width;
    mCurrentViewportHeight = height;
}

void Monitor::setPixelStore(GLenum pname, GLint param)
{
    if(pname == GL_UNPACK_ALIGNMENT)
    {
        mCurrentUnpackAlignment = param;
    }
}

void Monitor::setProgram(GLuint program)
{
    mBoundProgram = program;
}

void Monitor::appendVec4ToDataBuffer(GLint location, GLsizei count, const GLfloat* vec4)
{
    THROW_ASSERT(count == 1);

    Uniform4f u;
    u.messageType = Message::MessageType::UNIFORM_4_F;
    u.programId = mBoundProgram;
    u.location = location;
    memcpy(u.values, vec4, sizeof(u.values));
    appendToDataBuffer(u);
}

void Monitor::appendTransformationMatrixToDataBuffer(GLsizei count, GLboolean doTranspose, const GLfloat* matrix)
{
    THROW_ASSERT(count == 1);
    THROW_ASSERT(!doTranspose);

    TransformationMatrix m;
    m.messageType = Message::MessageType::TRANSFORMATION_MATRIX;
    memcpy(m.matrix[0], matrix, sizeof(m.matrix));
    m.programId = mBoundProgram;
    appendToDataBuffer(m);
}


void Monitor::setBlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    mCurrentBlendColor = Color(r * 255, g * 255, b * 255, a * 255);
}


void Monitor::setTexture(GLenum target, GLuint texture)
{
    mTextureUnits.setTexture(target, texture);
}


void Monitor::appendDrawArraysToDataBuffer(GLenum mode, GLint indicesOffset, GLsizei count)
{
    THROW_ASSERT(mode == GL_TRIANGLE_FAN);

    DrawCall drawCall;
    drawCall.bufferId = (mBoundVertexArray == 0 ? mBoundBuffer : mVaoToVbo[mBoundVertexArray]);
    drawCall.programId = mBoundProgram;
    drawCall.sourceTextureId = mTextureUnits.getTexture(GL_TEXTURE_2D);
    drawCall.blendColor = mCurrentBlendColor;
    drawCall.indicesOffset = indicesOffset;
    drawCall.numIndices = count;
    drawCall.type = DrawCall::PrimitiveType::TRIANGLE_FAN;
    drawCall.isDepthTestEnabled = mIsDepthTestEnabled;
    drawCall.isDepthWriteEnabled = mIsDepthWriteEnabled;
    drawCall.enabledVaos = mVaoEnableData;

    if(mBoundFramebuffer != 0)
    {
        auto it = mFramebufferTextureAttachment.find(mBoundFramebuffer);
        THROW_ASSERT(it != mFramebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

    drawCall.messageType = Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);
}


void Monitor::setVertexArray(GLuint array)
{
    mBoundVertexArray = array;
}


void Monitor::setBuffer(GLenum target, GLuint buffer)
{
    THROW_ASSERT(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER);
    mBoundBuffer = buffer;
}


void Monitor::appendVertexAttribPointerToDataBuffer(GLuint index, GLsizei stride, const GLvoid* offset)
{
    if(mBoundVertexArray != 0)
    {
        mVaoToVbo[mBoundVertexArray] = mBoundBuffer;
    }

    VertexAttribPointer p;
    p.bufferId = mBoundBuffer;
    p.index = index;
    p.stride = stride;
    p.offset = (unsigned int)offset;
    p.messageType = Message::MessageType::VERTEX_ATTRIB_POINTER;
    appendToDataBuffer(p);
}


void Monitor::appendDrawElementsToDataBuffer(GLenum mode, const GLvoid* indicesOffset, GLsizei count)
{
    THROW_ASSERT(mBoundBuffer != 1);

    DrawCall drawCall;
    drawCall.bufferId = mBoundBuffer;
    drawCall.programId = mBoundProgram;
    drawCall.sourceTextureId = mTextureUnits.getTexture(GL_TEXTURE_2D);
    drawCall.blendColor = mCurrentBlendColor;
    drawCall.indicesOffset = (unsigned int)indicesOffset;
    drawCall.numIndices = count;
    drawCall.isDepthTestEnabled = mIsDepthTestEnabled;
    drawCall.isDepthWriteEnabled = mIsDepthWriteEnabled;
    drawCall.enabledVaos = mVaoEnableData;
    switch(mode)
    {
        case GL_TRIANGLES:
            drawCall.type = DrawCall::PrimitiveType::TRIANGLE;
            break;

        case GL_TRIANGLE_STRIP:
            drawCall.type = DrawCall::PrimitiveType::TRIANGLE_STRIP;
            break;

        default:
            throw std::runtime_error("Unsupported primitive type: " + std::to_string(mode) + ".");
            break;
    }

    if(mBoundFramebuffer != 0)
    {
        auto it = mFramebufferTextureAttachment.find(mBoundFramebuffer);
        THROW_ASSERT(it != mFramebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

    drawCall.messageType = Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);
}


void Monitor::appendVertexBufferWriteToDataBuffer(const GLvoid* data, GLsizei size)
{
    VertexBufferWrite& buffer = mVertexBuffers[mBoundBuffer];
    buffer.bufferId = mBoundBuffer;
    buffer.numBytes = size;

    buffer.messageType = Message::MessageType::VERTEX_BUFFER_WRITE;
    appendToDataBuffer(buffer);
    appendToDataBuffer((const char*)data, size);
}


void Monitor::insertTextureData(GLsizei width, GLsizei height, GLenum target)
{
    GLuint textureId = mTextureUnits.getTexture(target);
    if(textureId == 0)
    {
        return;
    }

    if(width == 480 && height == 352)
    {
        mTextureBuffers.push_back(textureId);
        THROW_ASSERT(mTextureBuffers.size() <= TextureBuffers::NUM_BUFFERS);
    }
    else if(width == 4096 && height == 4096)
    {
        mTileSheetTextureId = textureId;
    }

    TextureData data;
    data.width = width;
    data.height = height;
    data.id = textureId;
    data.messageType = Message::MessageType::TEXTURE_DATA;
    appendToDataBuffer(data);
}


void Monitor::insertPixelData
(
    GLenum target,
  	GLint level,
  	GLint x,
  	GLint y,
  	GLsizei width,
  	GLsizei height,
  	GLenum format,
  	GLenum type,
  	const GLvoid* data
)
{
    if(data == nullptr)
    {
        return;
    }

    THROW_ASSERT(level == 0);
    THROW_ASSERT(type == GL_UNSIGNED_BYTE);



    PixelData p;
    p.targetTextureId = mTextureUnits.getTexture(target);
    if(p.targetTextureId == 0)
    {
        return;
    }
    THROW_ASSERT(target == GL_TEXTURE_2D);
    p.height = height;
    p.width = width;
    p.texX = x;
    p.texY = y;
    using PixelFormat = sb::utility::PixelFormat;
    switch(format)
    {
        case GL_RGBA:
            p.format = PixelFormat::RGBA;
            break;

        case GL_BGRA:
            p.format = PixelFormat::BGRA;
            break;

        case GL_ALPHA:
            p.format = PixelFormat::ALPHA;
            break;

        default:
            throw std::runtime_error("Unsupported pixel format: " + std::to_string(format) + ".");
            break;
    }

    p.messageType = Message::MessageType::PIXEL_DATA;
    appendToDataBuffer(p);

    size_t size;
    if(p.format == PixelFormat::ALPHA)
    {
        size = width * height * 1;

        size_t remainder = width % mCurrentUnpackAlignment;
        if(remainder != 0)
        {
            size_t paddedWidth = width + mCurrentUnpackAlignment - remainder;
            char* unpaddedData = new char[size];
            for(size_t iSrc = 0, iDest = 0; iDest < size; iSrc += paddedWidth, iDest += width)
                memcpy(&unpaddedData[iDest], &((const char*)data)[iSrc], width);
            appendToDataBuffer(unpaddedData, size);
            delete[] unpaddedData;
            return;
        }
    }
    else
    {
        size = width * height * 4;
    }

    appendToDataBuffer((char*)data, size);
}


void Monitor::appendCopyTextureToDataBuffer(GLenum target, GLint level, GLint srcX, GLint srcY, GLint x, GLint y, GLsizei width, GLsizei height)
{
    THROW_ASSERT(level == 0);

    GLuint targetTextureId = mTextureUnits.getTexture(target);
    GLuint sourceTextureId = 0;
    if(mBoundFramebuffer != 0)
    {
        auto it = mFramebufferTextureAttachment.find(mBoundFramebuffer);
        THROW_ASSERT(it != mFramebufferTextureAttachment.end());

        sourceTextureId = it->second;
    }

    CopyTexture copy;
    copy.srcX = srcX;
    copy.srcY = srcY;
    copy.targetX = x;
    copy.targetY = y;
    copy.width = width;
    copy.height = height;
    copy.sourceTextureId = sourceTextureId;
    copy.targetTextureId = targetTextureId;
    copy.messageType = Message::MessageType::COPY_TEXTURE;
    appendToDataBuffer(copy);
}

void Monitor::setFramebuffer(GLuint framebuffer)
{
    mBoundFramebuffer = framebuffer;
}


void Monitor::setActiveTexture(GLenum texture)
{
    mTextureUnits.setActiveTextureUnit(texture);
}

void Monitor::setFramebufferTexture(GLuint texture, GLint level)
{
    THROW_ASSERT(level == 0);
    mFramebufferTextureAttachment[mBoundFramebuffer] = texture;
}



void Monitor::updateSharedMemory()
{
    if(mDataBuffer.size() < sizeof(Frame))
    {
        return;
    }

    size_t insertionSize = 0;
    if(mDataBuffer.size() > DATA_BUFFER_SIZE)
    {
        while(true)
        {
            const Frame& f = *(Frame*)((char*)mDataBuffer.data() + insertionSize);
            size_t newInsertionSize = insertionSize + f.size;
            if(newInsertionSize > DATA_BUFFER_SIZE || newInsertionSize > mDataBuffer.size())
                break;
            insertionSize = newInsertionSize;
        }
        THROW_ASSERT(insertionSize <= mDataBuffer.size());
    }
    else
    {
        insertionSize = mDataBuffer.size();
    }

    memcpy(mShm->data, mDataBuffer.data(), insertionSize);
    mShm->size = insertionSize;
    if(insertionSize == mDataBuffer.size())
    {
        mDataBuffer.clear();
    }
    else
    {
        mDataBuffer.erase(mDataBuffer.begin(), mDataBuffer.begin() + insertionSize);
    }
}

void Monitor::printSharedMemoryBufferDiagnostics()
{
    float dataOccupancy = ((float)mDataBuffer.size()) / ((float)DATA_BUFFER_SIZE) * 100.f;
    mPeakDataOccupancy = std::max(mPeakDataOccupancy, dataOccupancy);
    std::cout   << "Shared memory buffer occupancy: " << mDataBuffer.size() << " / " << DATA_BUFFER_SIZE
                << "  (" << dataOccupancy << "%)"
                << " peak: " << mPeakDataOccupancy << "%" <<  std::endl;
}

bool Monitor::isParentAlive() const
{
    unsigned long code;
    if(GetExitCodeProcess(mShm->parentProcessHandle, &code) == 0)
    {
        std::cout << "Could not get parent process status. Error code: " << GetLastError() << std::endl;
        throw std::runtime_error("");
    }

    return (code == STILL_ACTIVE);
}

void Monitor::postFrame() const
{
    if(ReleaseSemaphore(mShm->semRead, 1, NULL) == 0)
    {
        std::cout << "Could not release shared memory read semaphore. Error code: " << GetLastError() << std::endl;
        throw std::runtime_error("");
    }
}


void Monitor::waitForFrameRequest() const
{
    switch(WaitForSingleObject(mShm->semWrite, 500))
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


void Monitor::submitFrameData()
{
    THROW_ASSERT(getCurrentFrame()->size <= DATA_BUFFER_SIZE);

    getCurrentFrame()->width = mCurrentViewportWidth;
    getCurrentFrame()->height = mCurrentViewportHeight;

    if(mShm->dataFilters & DataFilter::ScreenPixels)
    {
        appendScreenPixelsToDataBuffer();
    }

    updateSharedMemory();
    createNewFrame();
}

void Monitor::appendScreenPixelsToDataBuffer()
{
    size_t size = mCurrentViewportWidth * mCurrentViewportHeight * 4;
    char* pixels = new char[size];

    void APIENTRY (*readPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
    readPixels = (decltype(readPixels))GetProcAddress(GetModuleHandle("opengl32.dll"), "glReadPixels");
    THROW_ASSERT(readPixels != nullptr);

    readPixels(0, 0, mCurrentViewportWidth, mCurrentViewportHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    PixelData p;
    p.format = sb::utility::PixelFormat::RGBA;
    p.width = mCurrentViewportWidth;
    p.height = mCurrentViewportHeight;
    p.messageType = Message::MessageType::SCREEN_PIXELS;
    appendToDataBuffer(p);
    appendToDataBuffer(pixels, size);

    delete[] pixels;
}

WindowProc Monitor::getWindowProc(HWND window)
{
    auto foundIt = mHwndToWndProc.find(window);
    if(foundIt == mHwndToWndProc.end())
    {
        return nullptr;
    }

    return foundIt->second;
}

WindowProc Monitor::getWindowProcByClassName(HWND window)
{
    char className[512];
    GetClassNameA(window, className, 512);
    auto foundIt = mClassNameToWndProc.find(std::string(className));
    if(foundIt == mClassNameToWndProc.end())
    {
        return nullptr;
    }

    return foundIt->second;
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

    WindowProc windowProc = Monitor::getWindowProc(hwnd);
    if(windowProc)
    {
        return windowProc(hwnd, uMsg, wParam, lParam);
    }

    windowProc = Monitor::getWindowProcByClassName(hwnd);
    if(windowProc)
    {
        Monitor::setWindowProc(hwnd, windowProc);
        return windowProc(hwnd, uMsg, wParam, lParam);
    }

    THROW_ASSERT(false);
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

void Monitor::setClassWindowProc(CONST WNDCLASSEXA* windowClass)
{
    mClassNameToWndProc[toString(windowClass->lpszClassName)] = windowClass->lpfnWndProc;
    ((WNDCLASSEXA*)windowClass)->lpfnWndProc = windowClassProcDetour;
}

void Monitor::setWindowProc(HWND window, const std::string& windowClass)
{
    mHwndToWndProc[window] = mClassNameToWndProc[windowClass];
}

void Monitor::setWindowProc(HWND window, LPCSTR windowClass)
{
    setWindowProc(window, toString(windowClass));
}


void Monitor::setWindowProc(HWND window, LPCWSTR windowClass)
{
    setWindowProc(window, toString(windowClass));
}

void Monitor::setWindowProc(HWND window, WindowProc proc)
{
    mHwndToWndProc[window] = proc;
}

void Monitor::createWindow(HWND window, LPCWSTR windowName)
{
    std::string name = toString(windowName);
    if(name == "Tibia")
    {
        mShm->window = window;
        waitForFrameRequest();
    }
}

void Monitor::handleWindowMessage(LPMSG msg)
{
    THROW_ASSERT(msg != nullptr);
    switch(msg->message)
    {
        case WM_KEYDOWN:
            switch(msg->wParam)
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
            switch(msg->wParam)
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
}


void Monitor::clearDataBuffer()
{
    mDataBuffer.clear();
    createNewFrame();
}
}


