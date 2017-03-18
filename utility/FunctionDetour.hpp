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
#ifndef GRAPHICS_LAYER_FUNCTION_DETOUR_HPP
#define GRAPHICS_LAYER_FUNCTION_DETOUR_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "utility/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
class SHANK_BOT_UTILITY_DECLSPEC FunctionDetour
{
    public:
        explicit FunctionDetour(void* func, void* detour);
        ~FunctionDetour();

        void enable() const;
        void disable() const;

        void* getFunction() const;

    private:
        void createDetour();
        void destroyDetour();

    private:
        struct PageOccupancy
        {
            unsigned long numAddresses = 0;
            unsigned long originalProtect = 0;
        };

    private:
        static const int JMP_INSTRUCTION_SIZE = 6;
        static std::map<unsigned long, PageOccupancy> mPageOccupancyMap;

        unsigned char mFuncBackup[JMP_INSTRUCTION_SIZE] = {0};
        unsigned char mJmpInstruction[JMP_INSTRUCTION_SIZE] = {0xE9, 0x90, 0x90, 0x90, 0x90, 0xC3};

        void* mFunc;
        void* mDetour;

        unsigned long mPageIndex;
};
}


#endif // GRAPHICS_LAYER_FUNCTION_DETOUR_HPP
