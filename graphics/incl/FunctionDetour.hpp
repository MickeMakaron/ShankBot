#ifndef GRAPHICS_LAYER_FUNCTION_DETOUR_HPP
#define GRAPHICS_LAYER_FUNCTION_DETOUR_HPP

namespace GraphicsLayer
{
class FunctionDetour
{
    public:
        explicit FunctionDetour(void* func, void* detour);
        ~FunctionDetour();

        void enable() const;
        void disable() const;

        void* getFunction() const;

    private:
        void createDetour();

    private:
        static const int JMP_INSTRUCTION_SIZE = 6;

        unsigned char mFuncBackup[JMP_INSTRUCTION_SIZE] = {0};
        unsigned char mJmpInstruction[JMP_INSTRUCTION_SIZE] = {0xE9, 0x90, 0x90, 0x90, 0x90, 0xC3};
        void* mFunc;
        void* mDetour;
};
}


#endif // GRAPHICS_LAYER_FUNCTION_DETOUR_HPP
