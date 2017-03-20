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
#ifndef GRAPHICS_LAYER_TMP_FUNCS_HPP
#define GRAPHICS_LAYER_TMP_FUNCS_HPP

///////////////////////////////////
// Internal ShankBot headers
namespace GraphicsLayer
{
    class FunctionDetour;
    class Injection;
}
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
///////////////////////////////////

namespace GraphicsLayer
{
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
    void setClassWindowProc(CONST WNDCLASSEXA* windowClass);
    void setWindowProc(HWND window, LPCSTR windowClass);
    void setWindowProc(HWND window, LPCWSTR windowClass);
    void createWindow(HWND window, LPCWSTR windowName);
    void handleWindowMessage(LPMSG msg);
    void hookExtendedOpenGlFunctions(Injection& injection, const std::string& module, HDC hdc, HGLRC hglrc);
    void clearDataBuffer();
    void hookModuleExports(Injection& injection, HINSTANCE module, LPCSTR moduleName);
    void initializeInjection();
}

#endif // GRAPHICS_LAYER_TMP_FUNCS_HPP
