// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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
#ifndef GRAPHICS_LAYER_INPUT_HPP
#define GRAPHICS_LAYER_INPUT_HPP


///////////////////////////////////
// STD C++
#include <string>
#include <vector>
///////////////////////////////////

///////////////////////////////////
// Windows
#include <windows.h>
///////////////////////////////////


namespace GraphicsLayer
{
    class Input
    {
        public:
            enum ModifierKey : unsigned int
            {
                SHIFT = VK_SHIFT,
                CONTROL = VK_CONTROL
            };
            explicit Input(HWND window);
            void sendFocus() const;
            void sendKey(unsigned char key) const;
            void sendChar(char c) const;
            void sendKeyCombination(const std::vector<unsigned char>& keys) const;
            void sendString(std::string str) const;
            void sendMouseClick(unsigned char key, unsigned short x, unsigned short y, unsigned int keysDown = 0) const;
            void sendMouseDrag(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned int keysDown = 0) const;
            void sendScroll(unsigned short x, unsigned short y, short delta, unsigned int keysDown = 0) const;
            void sendPaintMessage() const;

        private:
            bool isKeyExtended(unsigned char key) const;
            bool isNonPrint(char key) const;
            void postMessage(UINT message, WPARAM wParam, LPARAM lParam) const;

        private:
            const HWND M_WINDOW = NULL;
            static const int REPEAT_COUNT = 1;
            static const int KEYUP_TRANSITION_STATE = 1;
            static const int KEYUP_PREVIOUS_KEY_STATE = 1;
            static const unsigned char REPEAT_COUNT_LPARAM_OFFSET = 0;
            static const unsigned char SCAN_CODE_LPARAM_OFFSET = 16;
            static const unsigned char IS_EXTENDED_LPARAM_OFFSET = 24;
            static const unsigned char PREVIOUS_KEY_STATE_LPARAM_OFFSET = 30;
            static const unsigned char TRANSITION_STATE_LPARAM_OFFSET = 31;
//        int repeatCount = (0xffff & l);
//        int scanCode = ((0xff << 16) & l) >> 16;
//        int isExtended = ((1 << 24) & l) >> 24;
//        int reserved = ((0xf << 25) & l) >> 25;
//        int contextCode = ((1 << 29) & l) >> 29;
//        int previousKeyState = ((1 << 30) & l) >> 30;
//        int transitionState = ((1 << 31) & l) >> 31;
            static const int KEYDOWN_LPARAM = (REPEAT_COUNT << REPEAT_COUNT_LPARAM_OFFSET);
            static const int EXTENDED_KEYDOWN_LPARAM = KEYDOWN_LPARAM | (1 << IS_EXTENDED_LPARAM_OFFSET);
            static const int KEYUP_LPARAM = KEYDOWN_LPARAM |
                            (KEYUP_TRANSITION_STATE << TRANSITION_STATE_LPARAM_OFFSET) |
                            (KEYUP_PREVIOUS_KEY_STATE << PREVIOUS_KEY_STATE_LPARAM_OFFSET);
            static const int EXTENDED_KEYUP_LPARAM = EXTENDED_KEYDOWN_LPARAM | KEYUP_LPARAM;
    };
}


#endif // GRAPHICS_LAYER_INPUT_HPP
