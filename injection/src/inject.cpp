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
#include "injection/Injection.hpp"
///////////////////////////////////

///////////////////////////////////
// GLEW
#include "GL/glew.h"
///////////////////////////////////

///////////////////////////////////
// Windows
#include <windows.h>
///////////////////////////////////

namespace GraphicsLayer
{

Injection* injection = nullptr;

void APIENTRY viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    static DetourHolder& detour = injection->getDetour(viewport);
    setViewport(x, y, width, height);
    detour.callAs(viewport, x, y, width, height);
}


void APIENTRY pixelStorei(GLenum pname, GLint param)
{
    static DetourHolder& detour = injection->getDetour(pixelStorei);
    setPixelStore(pname, param);
    detour.callAs(pixelStorei, pname, param);
}

void APIENTRY useProgram(GLuint program)
{
    static DetourHolder& detour = injection->getDetour(useProgram);
    setProgram(program);
    detour.callAs(useProgram, program);
}


void APIENTRY uniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
    static DetourHolder& detour = injection->getDetour(uniform4fv);
    appendVec4ToDataBuffer(location, count, value);
    detour.callAs(uniform4fv, location, count, value);
}

void APIENTRY uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    static DetourHolder& detour = injection->getDetour(uniformMatrix4fv);
   appendTransformationMatrixToDataBuffer(count, transpose, value);
   detour.callAs(uniformMatrix4fv, location, count, transpose, value);
}

void APIENTRY blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    static DetourHolder& detour = injection->getDetour(blendColor);
    setBlendColor(red, green, blue, alpha);
    detour.callAs(blendColor, red, green, blue, alpha);
}

void WINAPI bindTex(GLenum target, GLuint texture)
{
    static DetourHolder& detour = injection->getDetour(bindTex);
    setTexture(target, texture);
    detour.callAs(bindTex, target, texture);
}

void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count)
{
    static DetourHolder& detour = injection->getDetour(drawArrays);
    appendDrawArraysToDataBuffer(mode, first, count);
    detour.callAs(drawArrays, mode, first, count);
}

void WINAPI bindVertexArray(GLuint array)
{
    static DetourHolder& detour = injection->getDetour(bindVertexArray);
    setVertexArray(array);
    detour.callAs(bindVertexArray, array);
}


void WINAPI bindBuffer(GLenum target, GLuint buffer)
{
    static DetourHolder& detour = injection->getDetour(bindBuffer);
    setBuffer(target, buffer);
    detour.callAs(bindBuffer, target, buffer);
}

void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    static DetourHolder& detour = injection->getDetour(vertexAttribPointer);
    appendVertexAttribPointerToDataBuffer(index, stride, pointer);
    detour.callAs(vertexAttribPointer, index, size, type, normalized, stride, pointer);
}

void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    static DetourHolder& detour = injection->getDetour(drawElements);
    appendDrawElementsToDataBuffer(mode, indices, count);
    detour.callAs(drawElements, mode, count, type, indices);
}

void WINAPI bufDat(GLenum target, GLsizei size, const GLvoid* data, GLenum usage)
{
    static DetourHolder& detour = injection->getDetour(bufDat);
    appendVertexBufferWriteToDataBuffer(data, size);
    detour.callAs(bufDat, target, size, data, usage);
}

void WINAPI copyTexImage(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    static DetourHolder& detour = injection->getDetour(copyTexImage);
    appendCopyTextureToDataBuffer(target, level, 0, 0, x, y, width, height);
    detour.callAs(copyTexImage, target, level, internalformat, x, y, width, height, border);
}

void WINAPI copyTexSubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    static DetourHolder& detour = injection->getDetour(copyTexSubImage);
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
    static DetourHolder& detour = injection->getDetour(texImg);
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
    static DetourHolder& detour = injection->getDetour(subTexImg);
    insertPixelData(target, level, xoffset, yoffset, width, height, format, type, pixels);
    detour.callAs(subTexImg, target, level, xoffset, yoffset, width, height, format, type, pixels);
}


void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer)
{
    static DetourHolder& detour = injection->getDetour(bindFramebuffer);
    setFramebuffer(framebuffer);
    detour.callAs(bindFramebuffer, target, framebuffer);
}

void WINAPI activeTexture(GLenum texture)
{
    static DetourHolder& detour = injection->getDetour(activeTexture);
    setActiveTexture(texture);
    detour.callAs(activeTexture, texture);
}


void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    static DetourHolder& detour = injection->getDetour(framebufferTex);
    setFramebufferTexture(texture, level);
    detour.callAs(framebufferTex, target, attachment, textarget, texture, level);
}

BOOL WINAPI swapBuf(HDC hdc)
{
    static DetourHolder& detour = injection->getDetour(swapBuf);
    submitFrameData();
    BOOL retVal = detour.callAs(swapBuf, hdc);
    postFrame();
    waitForFrameRequest();
    return retVal;
}


ATOM WINAPI registerClassExA(CONST WNDCLASSEXA* lpWndClass)
{
    static DetourHolder& detour = injection->getDetour(registerClassExA);
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
    static DetourHolder& detour = injection->getDetour(createWindowExA);
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
    static DetourHolder& detour = injection->getDetour(createWindowExW);
    HWND retVal = detour.callAs(createWindowExW,
        dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);
    setWindowProc(retVal, className);
    createWindow(retVal, windowName);
    return retVal;
}

WINBOOL WINAPI peekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    static DetourHolder& detour = injection->getDetour(peekMessage);
    BOOL retVal = detour.callAs(peekMessage,
        lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    handleWindowMessage(lpMsg);
    return retVal;
}


BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc)
{
    static DetourHolder& detour = injection->getDetour(makeCurrent);
    BOOL retVal = detour.callAs(makeCurrent, hdc, hglrc);
    hookExtendedOpenGlFunctions(*injection, "opengl32.dll_EXT", hdc, hglrc);
    clearDataBuffer();
    return retVal;
}

HINSTANCE WINAPI loadLibrary(LPCSTR moduleName)
{
    static DetourHolder& detour = injection->getDetour(loadLibrary);
    HINSTANCE retVal = detour.callAs(loadLibrary, moduleName);
    hookModuleExports(*injection, retVal, moduleName);
    return retVal;
}

}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            using namespace GraphicsLayer;
            injection = new Injection();
            injection->setModuleDetours("user32.dll",
            {
                DetourHolder("CreateWindowExW", createWindowExW),
                DetourHolder("CreateWindowExA", createWindowExA),
                DetourHolder("PeekMessageW", peekMessage),
                DetourHolder("RegisterClassExA", registerClassExA),
            });
            injection->setModuleDetours("opengl32.dll",
            {
                DetourHolder("glPixelStorei", pixelStorei),
                DetourHolder("glTexImage2D", texImg),
                DetourHolder("glTexSubImage2D", subTexImg),
                DetourHolder("glBindTexture", bindTex),
                DetourHolder("glDrawArrays", drawArrays),
                DetourHolder("glDrawElements", drawElements),
                DetourHolder("wglMakeCurrent", makeCurrent),
                DetourHolder("glViewport", viewport),
                DetourHolder("glCopyTexImage2D", copyTexImage),
                DetourHolder("glCopyTexSubImage2D", copyTexSubImage),
            });
            injection->setModuleDetours("gdi32.dll",
            {
                DetourHolder("SwapBuffers", swapBuf),
            });
            injection->setModuleDetours("opengl32.dll_EXT",
            {
                DetourHolder("glBindFramebuffer", bindFramebuffer),
                DetourHolder("glBufferData", bufDat),
                DetourHolder("glFramebufferTexture2D", framebufferTex),
                DetourHolder("glActiveTexture", activeTexture),
                DetourHolder("glVertexAttribPointer", vertexAttribPointer),
                DetourHolder("glBindBuffer", bindBuffer),
                DetourHolder("glBindVertexArray", bindVertexArray),
                DetourHolder("glUseProgram", useProgram),
                DetourHolder("glUniform4fv", uniform4fv),
                DetourHolder("glUniformMatrix4fv", uniformMatrix4fv),
                DetourHolder("glBlendColor", blendColor),
            });
            injection->setModuleDetours("kernel32.dll",
            {
                DetourHolder("LoadLibraryA", loadLibrary),
            });
            injection->hookModule("kernel32.dll", [](const std::string& symbol)
            {
                return (void*)GetProcAddress(GetModuleHandle("kernel32.dll"), symbol.c_str());
            });

            initializeInjection();
            break;

        default:
            break;
    }
    return TRUE;
}
