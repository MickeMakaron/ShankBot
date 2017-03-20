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
#include "injection/TmpFuncs.hpp"
#include "injection/TextureUnitHolder.hpp"
#include "utility/FunctionDetour.hpp"
#include "injection/SharedMemoryProtocol.hpp"
#include "injection/Injection.hpp"
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
namespace GraphicsMonitor
{

    GLuint boundTexture = 0;
    GLuint boundFramebuffer = 0;
    GLuint boundBuffer = 0;
    GLuint boundProgram = 0;
    Color currentBlendColor;

    std::map<GLuint, GLuint> framebufferTextureAttachment;
    TextureUnitHolder textureUnits;

    std::vector<char> dataBuffer;
    size_t currentFrameIndex = 0;


    std::map<GLuint, VertexBufferWrite> vertexBuffers;

    GLuint tileSheetTextureId = 0;

    enum TextureBuffers : unsigned char
    {
        SHADED,
        UNSHADED,
        SHADING_MAP,
        NUM_BUFFERS
    };
    std::vector<GLuint> textureBuffers;

    SharedMemorySegment* shm = nullptr;

    float peakDataOccupancy = 0.f;


    GLsizei currentViewportWidth = 0;
    GLsizei currentViewportHeight = 0;
    GLenum currentUnpackAlignment = 4;

    std::map<GLuint, GLuint> vaoToVbo;
    GLuint boundVertexArray = 0;

    std::map<std::string, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)> classNameToWndProc;
    std::map<HWND, LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)> hwndToWndProc;
}

using namespace GraphicsMonitor;

#define THROW_ASSERT(v) if(!(v)) {std::cout << __LINE__ << std::endl; throw 1;}

Frame* getCurrentFrame()
{
    return (Frame*)((char*)dataBuffer.data() + currentFrameIndex);
}

void createNewFrame()
{
    Frame f;
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

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    THROW_ASSERT(x == 0 && y == 0);
    currentViewportWidth = width;
    currentViewportHeight = height;
}


void setPixelStore(GLenum pname, GLint param)
{
    if(pname == GL_UNPACK_ALIGNMENT)
    {
        currentUnpackAlignment = param;
    }
}

void setProgram(GLuint program)
{
    boundProgram = program;
}

void appendVec4ToDataBuffer(GLint location, GLsizei count, const GLfloat* vec4)
{
    THROW_ASSERT(count == 1);

    Uniform4f u;
    u.messageType = Message::MessageType::UNIFORM_4_F;
    u.programId = boundProgram;
    u.location = location;
    memcpy(u.values, vec4, sizeof(u.values));
    appendToDataBuffer(u);
}

void appendTransformationMatrixToDataBuffer(GLsizei count, GLboolean doTranspose, const GLfloat* matrix)
{
    THROW_ASSERT(count == 1);
    THROW_ASSERT(!doTranspose);

    TransformationMatrix m;
    m.messageType = Message::MessageType::TRANSFORMATION_MATRIX;
    memcpy(m.matrix[0], matrix, sizeof(m.matrix));
    m.programId = boundProgram;
    appendToDataBuffer(m);
}


void setBlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    currentBlendColor = Color(r * 255, g * 255, b * 255, a * 255);
}


void setTexture(GLenum target, GLuint texture)
{
    textureUnits.setTexture(target, texture);
}


void appendDrawArraysToDataBuffer(GLenum mode, GLint indicesOffset, GLsizei count)
{
    THROW_ASSERT(mode == GL_TRIANGLE_FAN);

    DrawCall drawCall;
    drawCall.bufferId = (boundVertexArray == 0 ? boundBuffer : vaoToVbo[boundVertexArray]);
    drawCall.programId = boundProgram;
    drawCall.sourceTextureId = textureUnits.getTexture(GL_TEXTURE_2D);
    drawCall.blendColor = currentBlendColor;
    drawCall.indicesOffset = indicesOffset;
    drawCall.numIndices = count;
    drawCall.type = DrawCall::PrimitiveType::TRIANGLE_FAN;

    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        THROW_ASSERT(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

    drawCall.messageType = Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);
}


void setVertexArray(GLuint array)
{
    boundVertexArray = array;
}


void setBuffer(GLenum target, GLuint buffer)
{
    THROW_ASSERT(target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER);
    boundBuffer = buffer;
}


void appendVertexAttribPointerToDataBuffer(GLuint index, GLsizei stride, const GLvoid* offset)
{
    if(boundVertexArray != 0)
    {
        vaoToVbo[boundVertexArray] = boundBuffer;
    }

    VertexAttribPointer p;
    p.bufferId = boundBuffer;
    p.index = index;
    p.stride = stride;
    p.offset = (unsigned int)offset;
    p.messageType = Message::MessageType::VERTEX_ATTRIB_POINTER;
    appendToDataBuffer(p);
}


void appendDrawElementsToDataBuffer(GLenum mode, const GLvoid* indicesOffset, GLsizei count)
{
    THROW_ASSERT(boundBuffer != 1);

    DrawCall drawCall;
    drawCall.bufferId = boundBuffer;
    drawCall.programId = boundProgram;
    drawCall.sourceTextureId = textureUnits.getTexture(GL_TEXTURE_2D);
    drawCall.blendColor = currentBlendColor;
    drawCall.indicesOffset = (unsigned int)indicesOffset;
    drawCall.numIndices = count;
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

    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        THROW_ASSERT(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }

    drawCall.messageType = Message::MessageType::DRAW_CALL;
    appendToDataBuffer(drawCall);
}


void appendVertexBufferWriteToDataBuffer(const GLvoid* data, GLsizei size)
{
    VertexBufferWrite& buffer = vertexBuffers[boundBuffer];
    buffer.bufferId = boundBuffer;
    buffer.numBytes = size;

    buffer.messageType = Message::MessageType::VERTEX_BUFFER_WRITE;
    appendToDataBuffer(buffer);
    appendToDataBuffer((const char*)data, size);
}


void insertTextureData(GLsizei width, GLsizei height, GLenum target)
{
    GLuint textureId = textureUnits.getTexture(target);
    if(textureId == 0)
    {
        return;
    }

    if(width == 480 && height == 352)
    {
        textureBuffers.push_back(textureId);
        THROW_ASSERT(textureBuffers.size() <= TextureBuffers::NUM_BUFFERS);
    }
    else if(width == 4096 && height == 4096)
    {
        tileSheetTextureId = textureId;
    }

    TextureData data;
    data.width = width;
    data.height = height;
    data.id = textureId;
    data.messageType = Message::MessageType::TEXTURE_DATA;
    appendToDataBuffer(data);
}


void insertPixelData
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
    p.targetTextureId = textureUnits.getTexture(target);
    if(p.targetTextureId == 0)
    {
        return;
    }
    THROW_ASSERT(target == GL_TEXTURE_2D);
    p.height = height;
    p.width = width;
    p.texX = x;
    p.texY = y;
    using PixelFormat = PixelData::PixelFormat;
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

        size_t remainder = width % currentUnpackAlignment;
        if(remainder != 0)
        {
            size_t paddedWidth = width + currentUnpackAlignment - remainder;
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


void appendCopyTextureToDataBuffer(GLenum target, GLint level, GLint srcX, GLint srcY, GLint x, GLint y, GLsizei width, GLsizei height)
{
    THROW_ASSERT(level == 0);

    GLuint targetTextureId = textureUnits.getTexture(target);
    GLuint sourceTextureId = 0;
    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        THROW_ASSERT(it != framebufferTextureAttachment.end());

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

void setFramebuffer(GLuint framebuffer)
{
    boundFramebuffer = framebuffer;
}


void setActiveTexture(GLenum texture)
{
    textureUnits.setActiveTextureUnit(texture);
}

void setFramebufferTexture(GLuint texture, GLint level)
{
    THROW_ASSERT(level == 0);
    framebufferTextureAttachment[boundFramebuffer] = texture;
}



void updateSharedMemory()
{
    if(dataBuffer.size() < sizeof(Frame))
    {
        return;
    }

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
    {
        insertionSize = dataBuffer.size();
    }

    memcpy(shm->data, dataBuffer.data(), insertionSize);
    shm->size = insertionSize;
    if(insertionSize == dataBuffer.size())
    {
        dataBuffer.clear();
    }
    else
    {
        dataBuffer.erase(dataBuffer.begin(), dataBuffer.begin() + insertionSize);
    }
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
    if(GetExitCodeProcess(shm->parentProcessHandle, &code) == 0)
    {
        std::cout << "Could not get parent process status. Error code: " << GetLastError() << std::endl;
        throw std::runtime_error("");
    }

    return (code == STILL_ACTIVE);
}

void postFrame()
{
    if(ReleaseSemaphore(shm->semRead, 1, NULL) == 0)
    {
        std::cout << "Could not release shared memory read semaphore. Error code: " << GetLastError() << std::endl;
        throw std::runtime_error("");
    }
}


void waitForFrameRequest()
{
    switch(WaitForSingleObject(shm->semWrite, 500))
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


void submitFrameData()
{
    THROW_ASSERT(getCurrentFrame()->size <= DATA_BUFFER_SIZE);

    getCurrentFrame()->width = currentViewportWidth;
    getCurrentFrame()->height = currentViewportHeight;

    updateSharedMemory();
    createNewFrame();
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

std::string toString(LPCSTR str)
{
    return std::string(str);
}

std::string toString(LPCWSTR str)
{
    char mbStr[512];
    wcstombs(mbStr, str, 512);
    return std::string(mbStr);
}


void setClassWindowProc(CONST WNDCLASSEXA* windowClass)
{
    classNameToWndProc[toString(windowClass->lpszClassName)] = windowClass->lpfnWndProc;
    ((WNDCLASSEXA*)windowClass)->lpfnWndProc = windowClassProcDetour;
}

void setWindowProc(HWND window, const std::string& windowClass)
{
    hwndToWndProc[window] = classNameToWndProc[windowClass];
}

void setWindowProc(HWND window, LPCSTR windowClass)
{
    setWindowProc(window, toString(windowClass));
}


void setWindowProc(HWND window, LPCWSTR windowClass)
{
    setWindowProc(window, toString(windowClass));
}

void createWindow(HWND window, LPCWSTR windowName)
{
    std::string name = toString(windowName);
    if(name == "Tibia")
    {
        shm->window = window;
        waitForFrameRequest();
    }
}

void handleWindowMessage(LPMSG msg)
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

void hookExtendedOpenGlFunctions(Injection& injection, const std::string& module, HDC hdc, HGLRC hglrc)
{
    if(hdc && hglrc)
    {
        PROC (WINAPI *getProc)(LPCSTR) = (PROC (WINAPI *)(LPCSTR))GetProcAddress(GetModuleHandle("opengl32.dll"), "wglGetProcAddress");
        injection.hookModule(module, [getProc](const std::string& symbol)
        {
            return (void*)getProc(symbol.c_str());
        });
    }
}

void clearDataBuffer()
{
    dataBuffer.clear();
    createNewFrame();
}

void hookModuleExports(Injection& injection, HINSTANCE module, LPCSTR moduleName)
{
    std::string name = toString(moduleName);
    std::transform(name.begin(), name.end(), name.begin(), tolower);

    injection.hookModule(name, [module](const std::string& symbol)
    {
        return (void*)GetProcAddress(module, symbol.c_str());
    });
}

void initializeInjection()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    std::cout << "Hallo!" << std::endl;

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

    shm = (SharedMemorySegment*) MapViewOfFile
    (
        hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        NUM_BYTES
    );

    if (shm == NULL)
    {
        throw std::runtime_error("Could not map shared memory (" + std::to_string(GetLastError()) + ").");
    }

    shm->isClientAttached = true;
}
}


