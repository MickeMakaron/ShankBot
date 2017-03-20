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
#include "utility/FunctionDetour.hpp"
#include "injection/DetourHolder.hpp"
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
#include <vector>
///////////////////////////////////

namespace GraphicsLayer
{
    void APIENTRY viewport(GLint x, GLint y, GLsizei width, GLsizei height);

    void APIENTRY pixelStorei(GLenum pname, GLint param);
    void APIENTRY useProgram(GLuint program);
    void APIENTRY uniform4fv(GLint location, GLsizei count, const GLfloat* value);
    void APIENTRY uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    void APIENTRY blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void WINAPI bindTex(GLenum target, GLuint texture);
    void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count);
    void WINAPI bindVertexArray(GLuint array);
    void WINAPI bindBuffer(GLenum target, GLuint buffer);
    void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
    void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
    void WINAPI bufDat
    (
        GLenum target,
        GLsizei size,
        const GLvoid* data,
        GLenum usage
    );
    void WINAPI copyTexImage(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    void WINAPI copyTexSubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
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
    );
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
    );
    void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer);
    void WINAPI activeTexture(GLenum texture);
    void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    BOOL WINAPI swapBuf(HDC hdc);
    ATOM WINAPI registerClassExA(CONST WNDCLASSEXA* lpWndClass);
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
    );
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
    );
    WINBOOL WINAPI peekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
    BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc);
    HINSTANCE WINAPI detourFunc(LPCSTR moduleName);



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
    void hookExtendedOpenGlFunctions(HDC hdc, HGLRC hglrc);
    void clearDataBuffer();
    void hookModuleExports(HINSTANCE module, LPCSTR moduleName);
    void initializeInjection();
    DetourHolder& getDetour(const void* detourFunc);
    template<typename CFunction>
    DetourHolder& getDetour(const CFunction& detourFunc)
    {
        return getDetour((const void*)detourFunc);
    }


    extern FunctionDetour* loadLibDetour;
    extern FunctionDetour* swapBufDetour;
    extern FunctionDetour* texImgDetour;
    extern FunctionDetour* subTexImgDetour;
    extern FunctionDetour* bufDatDetour;
    extern FunctionDetour* bindTexDetour;
    extern FunctionDetour* drawArraysDetour;
    extern FunctionDetour* drawElementsDetour;
    extern FunctionDetour* bindFramebufferDetour;
    extern FunctionDetour* framebufferTexDetour;
    extern FunctionDetour* activeTextureDetour;
    extern FunctionDetour* vertexAttribPointerDetour;
    extern FunctionDetour* bindBufferDetour;
    extern FunctionDetour* makeCurrentDetour;
    extern FunctionDetour* viewportDetour;
    extern FunctionDetour* useProgramDetour;
    extern FunctionDetour* uniform4fvDetour;
    extern FunctionDetour* uniformMatrix4fvDetour;
    extern FunctionDetour* copyTexImageDetour;
    extern FunctionDetour* copyTexSubImageDetour;
    extern FunctionDetour* bindVertexArrayDetour;
    extern FunctionDetour* blendColorDetour;
    extern FunctionDetour* pixelStoreiDetour;

    extern std::vector<DetourHolder> user32Detours;
    extern std::vector<DetourHolder> opengl32Detours;

    extern FunctionDetour* createWindowExWDetour;
    extern FunctionDetour* createWindowExADetour;
    extern FunctionDetour* peekMessageDetour;
    extern FunctionDetour* registerClassExADetour;

    extern PROC (WINAPI *getProc)(LPCSTR);

}

#endif // GRAPHICS_LAYER_TMP_FUNCS_HPP
