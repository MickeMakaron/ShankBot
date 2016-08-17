#include "FunctionDetour.hpp"

#include <windows.h>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace GraphicsLayer;

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

    createDetour();
}

FunctionDetour::~FunctionDetour()
{
    disable();
}

void FunctionDetour::createDetour()
{
    unsigned long oldProtect;
    VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy(mFuncBackup, mFunc, JMP_INSTRUCTION_SIZE);                            // make backup

    unsigned long jmpSize = ((unsigned long)mDetour - (unsigned long)mFunc - 5);  // calculate jump distance
    memcpy(&mJmpInstruction[1], &jmpSize, 4);                              // fill the nop's with the jump distance (JMP,distance(4bytes),RET)

    memcpy(mFunc, mJmpInstruction, JMP_INSTRUCTION_SIZE);                                 // set jump instruction at the beginning of the original function

    VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, oldProtect, &oldProtect);    // reset protection
}

void FunctionDetour::enable() const
{
    unsigned long oldProtect;
    VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(mFunc, mJmpInstruction, JMP_INSTRUCTION_SIZE);                                 // set the jump instruction again
    VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, oldProtect, &oldProtect);
}

void FunctionDetour::disable() const
{
    unsigned long oldProtect;
    VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(mFunc, mFuncBackup, JMP_INSTRUCTION_SIZE);                            // restore backup
    VirtualProtect(mFunc, JMP_INSTRUCTION_SIZE, oldProtect, &oldProtect);
}

void* FunctionDetour::getFunction() const
{
    return mFunc;
}
