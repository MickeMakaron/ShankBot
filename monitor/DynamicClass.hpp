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
#ifndef GRAPHICS_LAYER_DYNAMIC_CLASS_HPP
#define GRAPHICS_LAYER_DYNAMIC_CLASS_HPP

#include <cassert>
#include <vector>
#include <string>

#define CALL_DYNAMIC_CLASS_METHOD(instance, invocation)     \
__asm__ ("movl %0, %%ecx;" : : "r"(instance) : "%ecx");     \
invocation

#define CALL_DYNAMIC_CLASS_CTOR(out, type, invocation)      \
{                                                           \
    out = (type*)malloc(sizeof(type));                      \
    CALL_DYNAMIC_CLASS_METHOD(out, invocation);             \
}

#define DEFINE_DYNAMIC_CLASS_PROPERTIES(type, libBasename, dtorSymbol, dynamicMethodDefinitions)                        \
const struct                                                                                                            \
{                                                                                                                       \
    void (*dtor)() = nullptr;                                                                                           \
    std::string dtorSymbolName = dtorSymbol;                                                                            \
    dynamicMethodDefinitions                                                                                            \
} DYNM;                                                                                                                 \
                                                                                                                        \
template<>                                                                                                              \
GraphicsLayer::DynamicMethod* const GraphicsLayer::DynamicClass<type>::mMethods = (GraphicsLayer::DynamicMethod*)&DYNM; \
template<>                                                                                                              \
const size_t GraphicsLayer::DynamicClass<type>::mNumMethods = sizeof(DYNM) / sizeof(GraphicsLayer::DynamicMethod);      \
                                                                                                                        \
template<>                                                                                                              \
const char* const GraphicsLayer::DynamicClass<type>::mLib = libBasename;                                                \
                                                                                                                        \
template<>                                                                                                              \
void (*const *GraphicsLayer::DynamicClass<type>::mDestructor)() = &DYNM.dtor;


namespace GraphicsLayer
{
    struct DynamicMethod
    {
        void* address;
        std::string symbol;
    };

    template<typename T>
    class DynamicClass
    {
        public:
            constexpr explicit DynamicClass(T* instance = nullptr)
            : mInstance(instance)
            {
            }

            ~DynamicClass()
            {
                if(mInstance)
                {
                    CALL_DYNAMIC_CLASS_METHOD(mInstance, (*mDestructor)());
                    free(mInstance);
                }
            }

            T& getInstance() const
            {
                assert(mInstance);
                return *mInstance;
            }

            constexpr static DynamicMethod* getMethods()
            {
                return mMethods;
            }

            constexpr static size_t getNumMethods()
            {
                return mNumMethods;
            }

            constexpr static const char* getLib()
            {
                return mLib;
            }


        protected:
            T* mInstance = nullptr;
            static void (*const *mDestructor)();

        private:
            static DynamicMethod* const mMethods;
            static const size_t mNumMethods;
            static const char* const mLib;
    };
}



#endif // GRAPHICS_LAYER_DYNAMIC_CLASS_HPP
