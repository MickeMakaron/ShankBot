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

#include "injection/TmpFuncs.hpp"


namespace GraphicsLayer
{

FunctionDetour* loadLibDetour = nullptr;
FunctionDetour* swapBufDetour = nullptr;
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


void APIENTRY viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    static DetourHolder& detour = getDetour(viewport);

    setViewport(x, y, width, height);

    detour.callAs(viewport, x, y, width, height);
}


void APIENTRY pixelStorei(GLenum pname, GLint param)
{
    static DetourHolder& detour = getDetour(pixelStorei);

    setPixelStore(pname, param);

    detour.callAs(pixelStorei, pname, param);
}

void APIENTRY useProgram(GLuint program)
{
    setProgram(program);

    useProgramDetour->disable();
    ((void APIENTRY (*)(GLuint))useProgramDetour->getFunction())(program);
    useProgramDetour->enable();
}


void APIENTRY uniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
    appendVec4ToDataBuffer(location, count, value);

    uniform4fvDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, const GLfloat*))uniform4fvDetour->getFunction())(location, count, value);
    uniform4fvDetour->enable();
}

void APIENTRY uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
   appendTransformationMatrixToDataBuffer(count, transpose, value);

    uniformMatrix4fvDetour->disable();
    ((void APIENTRY (*)(GLint, GLsizei, GLboolean, const GLfloat*))uniformMatrix4fvDetour->getFunction())(location, count, transpose, value);
    uniformMatrix4fvDetour->enable();
}

void APIENTRY blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    setBlendColor(red, green, blue, alpha);

    blendColorDetour->disable();
    ((void APIENTRY (*)(GLfloat, GLfloat, GLfloat, GLfloat))blendColorDetour->getFunction())(red, green, blue, alpha);
    blendColorDetour->enable();
}

void WINAPI bindTex(GLenum target, GLuint texture)
{
    static DetourHolder& detour = getDetour(bindTex);

    setTexture(target, texture);

    detour.callAs(bindTex, target, texture);
}

void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count)
{
    static DetourHolder& detour = getDetour(drawArrays);

    appendDrawArraysToDataBuffer(mode, first, count);

    detour.callAs(drawArrays, mode, first, count);
}

void WINAPI bindVertexArray(GLuint array)
{
    setVertexArray(array);

    bindVertexArrayDetour->disable();
    ((void WINAPI (*)(GLuint))bindVertexArrayDetour->getFunction())(array);
    bindVertexArrayDetour->enable();
}


void WINAPI bindBuffer(GLenum target, GLuint buffer)
{
    setBuffer(target, buffer);

    bindBufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindBufferDetour->getFunction())(target, buffer);
    bindBufferDetour->enable();
}

void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    appendVertexAttribPointerToDataBuffer(index, stride, pointer);

    vertexAttribPointerDetour->disable();
    ((void WINAPI (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))
     vertexAttribPointerDetour->getFunction())(index, size, type, normalized, stride, pointer);
    vertexAttribPointerDetour->enable();
}

void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    static DetourHolder& detour = getDetour(drawElements);

    appendDrawElementsToDataBuffer(mode, indices, count);

    detour.callAs(drawElements, mode, count, type, indices);
}

void WINAPI bufDat
(
    GLenum target,
    GLsizei size,
    const GLvoid* data,
    GLenum usage
)
{
    appendVertexBufferWriteToDataBuffer(data, size);

    bufDatDetour->disable();
    ((void WINAPI (*)(GLenum, GLsizei, const GLvoid*, GLenum))
        bufDatDetour->getFunction())(target, size, data, usage);
    bufDatDetour->enable();
}

void WINAPI copyTexImage(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    static DetourHolder& detour = getDetour(copyTexImage);

    appendCopyTextureToDataBuffer(target, level, 0, 0, x, y, width, height);

    detour.callAs(copyTexImage, target, level, internalformat, x, y, width, height, border);
}

void WINAPI copyTexSubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    static DetourHolder& detour = getDetour(copyTexSubImage);

    appendCopyTextureToDataBuffer(target, level, xoffset, yoffset, x, y, width, height);

    detour.callAs(copyTexSubImage, target, level, xoffset, yoffset, x, y, width, height);
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
    static DetourHolder& detour = getDetour(texImg);


    insertTextureData(width, height, target);
    insertPixelData(target, level, 0, 0, width, height, format, type, data);

    detour.callAs(texImg, target, level, internalFormat, width, height, border, format, type, data);
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
    static DetourHolder& detour = getDetour(subTexImg);

    insertPixelData(target, level, xoffset, yoffset, width, height, format, type, pixels);

    detour.callAs(subTexImg, target, level, xoffset, yoffset, width, height, format, type, pixels);
}


void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer)
{
    setFramebuffer(framebuffer);

    bindFramebufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindFramebufferDetour->getFunction())(target, framebuffer);
    bindFramebufferDetour->enable();
}

void WINAPI activeTexture(GLenum texture)
{
    setActiveTexture(texture);

    activeTextureDetour->disable();
    ((void WINAPI (*)(GLenum))activeTextureDetour->getFunction())(texture);
    activeTextureDetour->enable();
}


void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    setFramebufferTexture(texture, level);

    framebufferTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLenum, GLenum, GLuint, GLint))framebufferTexDetour->getFunction())(target, attachment, textarget, texture, level);
    framebufferTexDetour->enable();
}

BOOL WINAPI swapBuf(HDC hdc)
{
    submitFrameData();

    swapBufDetour->disable();
    BOOL retVal = ((BOOL WINAPI (*)(HDC))swapBufDetour->getFunction())(hdc);
    swapBufDetour->enable();

    postFrame();
    waitForFrameRequest();
    return retVal;
}


ATOM WINAPI registerClassExA(CONST WNDCLASSEXA* lpWndClass)
{
    static DetourHolder& detour = getDetour(registerClassExA);

    setClassWindowProc(lpWndClass);

    return detour.callAs(registerClassExA, lpWndClass);
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
    static DetourHolder& detour = getDetour(createWindowExA);

    HWND retVal = detour.callAs(createWindowExA,
        dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);

    setWindowProc(retVal, className);
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
    static DetourHolder& detour = getDetour(createWindowExW);

    HWND retVal = detour.callAs(createWindowExW,
        dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);

    setWindowProc(retVal, className);
    createWindow(retVal, windowName);

    return retVal;
}

WINBOOL WINAPI peekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    static DetourHolder& detour = getDetour(peekMessage);

    BOOL retVal = detour.callAs(peekMessage,
        lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    handleWindowMessage(lpMsg);

    return retVal;
}


BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc)
{
    static DetourHolder& detour = getDetour(makeCurrent);

    BOOL retVal = detour.callAs(makeCurrent, hdc, hglrc);

    hookExtendedOpenGlFunctions(hdc, hglrc);
    clearDataBuffer();

    return retVal;
}

HINSTANCE WINAPI detourFunc(LPCSTR moduleName)
{
    loadLibDetour->disable();
    HINSTANCE retVal = ((HINSTANCE WINAPI (*)(LPCSTR))loadLibDetour->getFunction())(moduleName);
    loadLibDetour->enable();

    hookModuleExports(retVal, moduleName);

    return retVal;
}

}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            GraphicsLayer::initializeInjection();
            break;

        default:
            break;
    }
    return TRUE;
}
