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
#include "monitor/Input.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
#include <cassert>
///////////////////////////////////

///////////////////////////////////
// Windows
#ifndef MAPVK_VK_TO_VSC
    #define MAPVK_VK_TO_VSC (0)
#endif
///////////////////////////////////

Input::Input(HWND window)
: M_WINDOW(window)
{
}

void Input::postMessage(UINT message, WPARAM wParam, LPARAM lParam) const
{
    if(!PostMessage(M_WINDOW, message, wParam, lParam))
    {
        std::stringstream sstream;
        sstream     << "Failed to post message: " << std::endl
                    << "\tHWND: " << M_WINDOW << std::endl
                    << "\tMessage: " << message << std::endl;
        SB_THROW(sstream.str());
    }
}


void Input::sendFocus() const
{
    postMessage(WM_SETFOCUS, NULL, NULL);
}
#include <iostream>

void Input::sendKey(unsigned char key) const
{
//    if(isNonPrint(key))
    {
        int scanCode = (MapVirtualKey(key, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        int lParamDown = scanCode;
        int lParamUp = scanCode;
        if(isKeyExtended(key))
        {
            lParamDown |= EXTENDED_KEYDOWN_LPARAM;
            lParamUp |= EXTENDED_KEYUP_LPARAM;
        }
        else
        {
            lParamDown |= KEYDOWN_LPARAM;
            lParamUp |= KEYUP_LPARAM;
        }
        postMessage(WM_KEYDOWN, key, lParamDown);
        postMessage(WM_KEYUP, key, lParamUp);
    }
//    else
//    {
//        postMessage(WM_CHAR, key, 0);
//    }
}

bool Input::isNonPrint(char key) const
{
    return key < 0x20;
}

bool Input::isKeyExtended(unsigned char key) const
{
//    if(!isNonPrint(key))
//        return false;

    return
    (
        key == VK_LEFT ||
        key == VK_RIGHT ||
        key == VK_DOWN ||
        key == VK_UP ||
        key == VK_INSERT ||
        key == VK_DELETE ||
        key == VK_HOME ||
        key == VK_END ||
        key == VK_PRIOR ||
        key == VK_NEXT ||
        key == VK_NUMLOCK ||
        key == VK_PRINT ||
        key == VK_DIVIDE ||
        key == VK_RMENU ||
        key == VK_RCONTROL
    );
}

void Input::sendKeyCombination(const std::vector<unsigned char>& keys) const
{

    for(size_t i = 0; i < keys.size(); i++)
    {
        char key = keys[i];
        int lParam = (MapVirtualKey(key, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        if(isKeyExtended(key))
            lParam |= EXTENDED_KEYDOWN_LPARAM;
        else
            lParam |= KEYDOWN_LPARAM;
        postMessage(WM_KEYDOWN, key, lParam);
    }
    for(int i = keys.size() - 1; i >= 0; i--)
    {
        char key = keys[i];
        int lParam = (MapVirtualKey(key, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        if(isKeyExtended(key))
            lParam |= EXTENDED_KEYUP_LPARAM;
        else
            lParam |= KEYUP_LPARAM;
        postMessage(WM_KEYUP, key, lParam);
    }
}

void Input::sendChar(char c) const
{
    postMessage(WM_CHAR, c, 0);
}


void Input::sendString(std::string str) const
{
    sendFocus();
    for(const char& c : str)
    {
        Sleep(30);
//        sendKey(c);
        sendChar(c);
    }
}

void Input::sendMouseDrag(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned int keysDown) const
{
    unsigned int lParam1 = ((0 | y1) << 16) | x1;
    unsigned int lParam2 = ((0 | y2) << 16) | x2;

    postMessage(WM_LBUTTONDOWN, keysDown | MK_LBUTTON, lParam1);
    postMessage(WM_LBUTTONUP, keysDown, lParam2);
}

void Input::sendMouseClick(unsigned char key, unsigned short x, unsigned short y, unsigned int keysDown) const
{
    if(keysDown & MK_CONTROL)
    {
        int lParam = KEYDOWN_LPARAM | (MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        postMessage(WM_KEYDOWN, VK_CONTROL, lParam);
    }
    if(keysDown & MK_SHIFT)
    {
        int lParam = KEYDOWN_LPARAM | (MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        postMessage(WM_KEYDOWN, VK_SHIFT, lParam);
    }

    unsigned int downMsg;
    unsigned int upMsg;
    switch(key)
    {
        case MK_LBUTTON:
            downMsg = WM_LBUTTONDOWN;
            upMsg = WM_LBUTTONUP;
            break;

        case MK_RBUTTON:
            downMsg = WM_RBUTTONDOWN;
            upMsg = WM_RBUTTONUP;
            break;

        default:
            std::stringstream sstream;
            sstream << "Unimplemented key: " << key << "." << std::endl;
            SB_THROW(sstream.str());
    }

    unsigned int lParamMouse = ((0 | y) << 16) | x;
    postMessage(downMsg, keysDown | key, lParamMouse);
    postMessage(upMsg, keysDown, lParamMouse);

    if(keysDown & MK_CONTROL)
    {
        int lParam = KEYUP_LPARAM | (MapVirtualKey(VK_CONTROL, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        postMessage(WM_KEYUP, VK_CONTROL, lParam);
    }
    if(keysDown & MK_SHIFT)
    {
        int lParam = KEYUP_LPARAM | (MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC) << SCAN_CODE_LPARAM_OFFSET);
        postMessage(WM_KEYUP, VK_SHIFT, lParam);
    }
}

void Input::sendScroll(unsigned short x, unsigned short y, short delta, unsigned int keysDown) const
{
    POINT point;
    point.x = x;
    point.y = y;
    if(!ClientToScreen(M_WINDOW, &point))
        SB_THROW("Failed to get screen coords.");

    unsigned int lParamMouse = ((0 | point.y) << 16) | point.x;
    unsigned int wParamMouse = ((0 | delta) << 16) | keysDown;
    postMessage(WM_MOUSEWHEEL, wParamMouse, lParamMouse);
}

void Input::sendPaintMessage() const
{
    static const RECT R = {0, 0, 1, 1};
    InvalidateRect(M_WINDOW, &R, FALSE);
    postMessage(WM_PAINT, NULL, NULL);
//    postMessage(WM_ERASEBKGND, (WPARAM)GetDC(M_WINDOW), NULL);
//    postMessage(WM_SHOWWINDOW, TRUE, SW_OTHERUNZOOM);


//    postMessage(WM_SIZE, SIZE_MAXIMIZED, 0);
//    postMessage(WM_ACTIVATE, (1 << 16) | WA_ACTIVE, NULL);
//    postMessage(WM_ACTIVATEAPP, TRUE, NULL);
}
