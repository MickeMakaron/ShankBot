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
#include "injection/Injection.hpp"
#include "injection/utility.hpp"
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
#include <iostream>
#include <algorithm>
///////////////////////////////////

namespace GraphicsLayer
{

Injection* injection = nullptr;
Monitor* monitor = nullptr;
void hookExtendedOpenGlFunctions(const std::string& module, HDC hdc, HGLRC hglrc);
void hookModuleExports(HINSTANCE module, LPCSTR moduleName);


void APIENTRY attachShader(GLuint program, GLuint shader)
{
    static DetourHolder& detour = injection->getDetour(attachShader);
    detour.callAs(attachShader, program, shader);

    std::cout << "Attach shader to program: " << shader << "->" << program << std::endl;
}

void APIENTRY shaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
{
    static DetourHolder& detour = injection->getDetour(shaderSource);
    detour.callAs(shaderSource, shader, count, string, length);

    std::cout << "Shader source: " << shader << std::endl;
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
}

void APIENTRY enable(GLenum cap)
{
    static DetourHolder& detour = injection->getDetour(enable);
    if(cap == GL_DEPTH_TEST)
    {
        monitor->setDepthTest(true);
    }
    detour.callAs(enable, cap);
}

void APIENTRY disable(GLenum cap)
{
    static DetourHolder& detour = injection->getDetour(disable);
    if(cap == GL_DEPTH_TEST)
    {
        monitor->setDepthTest(false);
    }
    detour.callAs(disable, cap);
}

void APIENTRY enableVertexAttribArray(GLuint index)
{
    static DetourHolder& detour = injection->getDetour(enableVertexAttribArray);
    detour.callAs(enableVertexAttribArray, index);

    monitor->toggleVao(index, true);
}


void APIENTRY disableVertexAttribArray(GLuint index)
{
    static DetourHolder& detour = injection->getDetour(disableVertexAttribArray);
    detour.callAs(disableVertexAttribArray, index);
    monitor->toggleVao(index, false);
}

void APIENTRY depthFunc(GLenum func)
{
    static DetourHolder& detour = injection->getDetour(depthFunc);
    monitor->setDepthFunc(func);
    detour.callAs(depthFunc, func);
}

void APIENTRY depthMask(GLboolean flag)
{
    static DetourHolder& detour = injection->getDetour(depthMask);
    monitor->setDepthMask(flag);
    detour.callAs(depthMask, flag);
}

void APIENTRY depthRange(GLclampd zNear, GLclampd zFar)
{
    static DetourHolder& detour = injection->getDetour(depthRange);
    monitor->setDepthRange(zNear, zFar);
    detour.callAs(depthRange, zNear, zFar);
}




void APIENTRY viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    static DetourHolder& detour = injection->getDetour(viewport);
    monitor->setViewport(x, y, width, height);
    detour.callAs(viewport, x, y, width, height);
}


void APIENTRY pixelStorei(GLenum pname, GLint param)
{
    static DetourHolder& detour = injection->getDetour(pixelStorei);
    monitor->setPixelStore(pname, param);
    detour.callAs(pixelStorei, pname, param);
}

void APIENTRY useProgram(GLuint program)
{
    static DetourHolder& detour = injection->getDetour(useProgram);
    monitor->setProgram(program);
    detour.callAs(useProgram, program);
}


void APIENTRY uniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
    static DetourHolder& detour = injection->getDetour(uniform4fv);
    monitor->appendVec4ToDataBuffer(location, count, value);
    detour.callAs(uniform4fv, location, count, value);
}

void APIENTRY uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    static DetourHolder& detour = injection->getDetour(uniformMatrix4fv);
    monitor->appendTransformationMatrixToDataBuffer(count, transpose, value);
    detour.callAs(uniformMatrix4fv, location, count, transpose, value);
}

void APIENTRY blendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    static DetourHolder& detour = injection->getDetour(blendColor);
    monitor->setBlendColor(red, green, blue, alpha);
    detour.callAs(blendColor, red, green, blue, alpha);
}

void WINAPI bindTex(GLenum target, GLuint texture)
{
    static DetourHolder& detour = injection->getDetour(bindTex);
    monitor->setTexture(target, texture);
    detour.callAs(bindTex, target, texture);
}

void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count)
{
    static DetourHolder& detour = injection->getDetour(drawArrays);
    monitor->appendDrawArraysToDataBuffer(mode, first, count);
    detour.callAs(drawArrays, mode, first, count);
}

void WINAPI bindVertexArray(GLuint array)
{
    static DetourHolder& detour = injection->getDetour(bindVertexArray);
    monitor->setVertexArray(array);
    detour.callAs(bindVertexArray, array);
}


void WINAPI bindBuffer(GLenum target, GLuint buffer)
{
    static DetourHolder& detour = injection->getDetour(bindBuffer);
    monitor->setBuffer(target, buffer);
    detour.callAs(bindBuffer, target, buffer);
}

void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    static DetourHolder& detour = injection->getDetour(vertexAttribPointer);
    monitor->appendVertexAttribPointerToDataBuffer(index, stride, pointer);
    detour.callAs(vertexAttribPointer, index, size, type, normalized, stride, pointer);
}

void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    static DetourHolder& detour = injection->getDetour(drawElements);
    monitor->appendDrawElementsToDataBuffer(mode, indices, count);
    detour.callAs(drawElements, mode, count, type, indices);
}

void WINAPI bufDat(GLenum target, GLsizei size, const GLvoid* data, GLenum usage)
{
    static DetourHolder& detour = injection->getDetour(bufDat);
    monitor->appendVertexBufferWriteToDataBuffer(data, size);
    detour.callAs(bufDat, target, size, data, usage);
}

void WINAPI copyTexImage(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    static DetourHolder& detour = injection->getDetour(copyTexImage);
    monitor->appendCopyTextureToDataBuffer(target, level, 0, 0, x, y, width, height);
    detour.callAs(copyTexImage, target, level, internalformat, x, y, width, height, border);
}

void WINAPI copyTexSubImage(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    static DetourHolder& detour = injection->getDetour(copyTexSubImage);
    monitor->appendCopyTextureToDataBuffer(target, level, xoffset, yoffset, x, y, width, height);
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
    monitor->insertTextureData(width, height, target);
    monitor->insertPixelData(target, level, 0, 0, width, height, format, type, data);
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
    monitor->insertPixelData(target, level, xoffset, yoffset, width, height, format, type, pixels);
    detour.callAs(subTexImg, target, level, xoffset, yoffset, width, height, format, type, pixels);
}


void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer)
{
    static DetourHolder& detour = injection->getDetour(bindFramebuffer);
    monitor->setFramebuffer(framebuffer);
    detour.callAs(bindFramebuffer, target, framebuffer);
}

void WINAPI activeTexture(GLenum texture)
{
    static DetourHolder& detour = injection->getDetour(activeTexture);
    monitor->setActiveTexture(texture);
    detour.callAs(activeTexture, texture);
}


void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    static DetourHolder& detour = injection->getDetour(framebufferTex);
    monitor->setFramebufferTexture(texture, level);
    detour.callAs(framebufferTex, target, attachment, textarget, texture, level);
}

BOOL WINAPI swapBuf(HDC hdc)
{
    static DetourHolder& detour = injection->getDetour(swapBuf);
    monitor->submitFrameData();
    BOOL retVal = detour.callAs(swapBuf, hdc);
    monitor->postFrame();
    monitor->waitForFrameRequest();
    return retVal;
}


ATOM WINAPI registerClassExA(CONST WNDCLASSEXA* lpWndClass)
{
    static DetourHolder& detour = injection->getDetour(registerClassExA);
    monitor->setClassWindowProc(lpWndClass);
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
    monitor->setWindowProc(retVal, className);
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
    monitor->setWindowProc(retVal, className);
    monitor->createWindow(retVal, windowName);
    return retVal;
}

WINBOOL WINAPI peekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    static DetourHolder& detour = injection->getDetour(peekMessage);
    BOOL retVal = detour.callAs(peekMessage,
        lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
    monitor->handleWindowMessage(lpMsg);
    return retVal;
}


BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc)
{
    static DetourHolder& detour = injection->getDetour(makeCurrent);
    BOOL retVal = detour.callAs(makeCurrent, hdc, hglrc);
    hookExtendedOpenGlFunctions("opengl32.dll_EXT", hdc, hglrc);
    monitor->clearDataBuffer();
    return retVal;
}

HINSTANCE WINAPI loadLibrary(LPCSTR moduleName)
{
    static DetourHolder& detour = injection->getDetour(loadLibrary);
    HINSTANCE retVal = detour.callAs(loadLibrary, moduleName);
    hookModuleExports(retVal, moduleName);
    return retVal;
}

void inject()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    std::cout << "Hallo!" << std::endl;

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
        DetourHolder("glEnable", enable),
        DetourHolder("glDisable", disable),
        DetourHolder("glDepthFunc", depthFunc),
        DetourHolder("glDepthMask", depthMask),
        DetourHolder("glDepthRange", depthRange),
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
        DetourHolder("glEnableVertexAttribArray", enableVertexAttribArray),
        DetourHolder("glDisableVertexAttribArray", disableVertexAttribArray),
//        DetourHolder("glAttachShader", attachShader),
//        DetourHolder("glShaderSource", shaderSource),
    });
    injection->setModuleDetours("kernel32.dll",
    {
        DetourHolder("LoadLibraryA", loadLibrary),
    });
    injection->hookModule("kernel32.dll", [](const std::string& symbol)
    {
        return (void*)GetProcAddress(GetModuleHandle("kernel32.dll"), symbol.c_str());
    });

    monitor = new Monitor();
}


void hookExtendedOpenGlFunctions(const std::string& module, HDC hdc, HGLRC hglrc)
{
    if(hdc && hglrc)
    {
        PROC (WINAPI *getProc)(LPCSTR) = (PROC (WINAPI *)(LPCSTR))GetProcAddress(GetModuleHandle("opengl32.dll"), "wglGetProcAddress");
        injection->hookModule(module, [getProc](const std::string& symbol)
        {
            return (void*)getProc(symbol.c_str());
        });
    }
}

void hookModuleExports(HINSTANCE module, LPCSTR moduleName)
{
    std::string name = toString(moduleName);
    std::transform(name.begin(), name.end(), name.begin(), tolower);
    injection->hookModule(name, [module](const std::string& symbol)
    {
        return (void*)GetProcAddress(module, symbol.c_str());
    });
}
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            GraphicsLayer::inject();
            break;

        default:
            break;
    }
    return TRUE;
}
