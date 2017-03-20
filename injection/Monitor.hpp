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
#ifndef GRAPHICS_LAYER_MONITOR_HPP
#define GRAPHICS_LAYER_MONITOR_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "injection/TextureUnitHolder.hpp"
#include "injection/SharedMemoryProtocol.hpp"
///////////////////////////////////

///////////////////////////////////
// GLEW
#include "GL/glew.h"
///////////////////////////////////

///////////////////////////////////
// Windows
#include <windows.h>
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <vector>
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    using WindowProc = LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM);

    class Monitor
    {
        public:
            explicit Monitor();

            void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
            void setPixelStore(GLenum pname, GLint param);
            void setProgram(GLuint program);
            void appendVec4ToDataBuffer(GLint location, GLsizei count, const GLfloat* vec4);
            void appendTransformationMatrixToDataBuffer(GLsizei count, GLboolean doTranspose, const GLfloat* matrix);
            void setBlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
            void setTexture(GLenum target, GLuint texture);
            void appendDrawArraysToDataBuffer(GLenum mode, GLint indicesOffset, GLsizei count);
            void setVertexArray(GLuint array);
            void setBuffer(GLenum target, GLuint buffer);
            void appendVertexAttribPointerToDataBuffer(GLuint index, GLsizei stride, const GLvoid* offset);
            void appendDrawElementsToDataBuffer(GLenum mode, const GLvoid* indicesOffset, GLsizei count);
            void appendVertexBufferWriteToDataBuffer(const GLvoid* data, GLsizei size);
            void insertTextureData(GLsizei width, GLsizei height, GLenum target);
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
                const GLvoid* pixels
            );
            void appendCopyTextureToDataBuffer
            (
                GLenum target,
                GLint level,
                GLint srcX,
                GLint srcY,
                GLint x,
                GLint y,
                GLsizei width,
                GLsizei height
            );

            void setFramebuffer(GLuint framebuffer);
            void setActiveTexture(GLenum texture);
            void setFramebufferTexture(GLuint texture, GLint level);
            void postFrame();
            void waitForFrameRequest();
            void submitFrameData();
            void clearDataBuffer();

            void createWindow(HWND window, LPCWSTR windowName);
            void handleWindowMessage(LPMSG msg);
            static void setClassWindowProc(CONST WNDCLASSEXA* windowClass);
            static void setWindowProc(HWND window, LPCSTR windowClass);
            static void setWindowProc(HWND window, LPCWSTR windowClass);
            static WindowProc getWindowProc(HWND window);
            static WindowProc getWindowProcByClassName(HWND window);
            static void setWindowProc(HWND window, WindowProc proc);

        private:
            SharedMemoryProtocol::Frame* getCurrentFrame();
            void createNewFrame();
            void appendToDataBuffer(const char* data, size_t size);
            template<typename T>
            void appendToDataBuffer(const T& message)
            {
                appendToDataBuffer((char*)&message, sizeof(message));
            }

            void updateSharedMemory();
            void printSharedMemoryBufferDiagnostics();
            bool isParentAlive();

            static void setWindowProc(HWND window, const std::string& windowClass);



        private:
            GLuint boundTexture = 0;
            GLuint boundFramebuffer = 0;
            GLuint boundBuffer = 0;
            GLuint boundProgram = 0;
            Color currentBlendColor;

            std::map<GLuint, GLuint> framebufferTextureAttachment;
            TextureUnitHolder textureUnits;

            std::vector<char> dataBuffer;
            size_t currentFrameIndex = 0;


            std::map<GLuint, SharedMemoryProtocol::VertexBufferWrite> vertexBuffers;

            GLuint tileSheetTextureId = 0;

            enum TextureBuffers : unsigned char
            {
                SHADED,
                UNSHADED,
                SHADING_MAP,
                NUM_BUFFERS
            };
            std::vector<GLuint> textureBuffers;

            SharedMemoryProtocol::SharedMemorySegment* shm = nullptr;

            float peakDataOccupancy = 0.f;


            GLsizei currentViewportWidth = 0;
            GLsizei currentViewportHeight = 0;
            GLenum currentUnpackAlignment = 4;

            std::map<GLuint, GLuint> vaoToVbo;
            GLuint boundVertexArray = 0;

            static std::map<std::string, WindowProc> classNameToWndProc;
            static std::map<HWND, WindowProc> hwndToWndProc;
    };
}

#endif // GRAPHICS_LAYER_MONITOR_HPP
