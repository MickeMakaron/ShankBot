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
#include "utility/FunctionDetour.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <windows.h>
#include <string>
#include <stdexcept>
#include <iostream>
///////////////////////////////////

std::map<unsigned long, FunctionDetour::PageOccupancy> FunctionDetour::mPageOccupancyMap;

FunctionDetour::FunctionDetour(void* func, void* detour)
: mFunc(func)
, mDetour(detour)
{
    if(!func || !detour)
    {
        std::string errorMsg = "Null pointer given.";
        std::cout << errorMsg << std::endl;
        throw std::runtime_error(errorMsg);
    }

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    mPageIndex = ((unsigned long)mFunc) / info.dwPageSize;

    createDetour();
}

FunctionDetour::~FunctionDetour()
{
    destroyDetour();
}

void FunctionDetour::createDetour()
{

    // Can a function span over several pages? If so, this could do a Murphy.
    PageOccupancy& po = mPageOccupancyMap[mPageIndex];
    if(po.numAddresses == 0)
        VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &po.originalProtect);
    po.numAddresses++;

    memcpy(mFuncBackup, mFunc, JMP_INSTRUCTION_SIZE);

    unsigned long jmpSize = ((unsigned long)mDetour - (unsigned long)mFunc - 5);
    memcpy(&mJmpInstruction[1], &jmpSize, 4);
    memcpy(mFunc, mJmpInstruction, JMP_INSTRUCTION_SIZE);
}

void FunctionDetour::enable() const
{
    memcpy(mFunc, mJmpInstruction, JMP_INSTRUCTION_SIZE);
}

void FunctionDetour::disable() const
{
    memcpy(mFunc, mFuncBackup, JMP_INSTRUCTION_SIZE);
}

void* FunctionDetour::getFunction() const
{
    return mFunc;
}

void FunctionDetour::destroyDetour()
{
    disable();

    PageOccupancy& po = mPageOccupancyMap[mPageIndex];
    if(po.numAddresses == 1)
    {
        unsigned long p;
        VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, po.originalProtect, &p);
    }
    po.numAddresses--;
}

