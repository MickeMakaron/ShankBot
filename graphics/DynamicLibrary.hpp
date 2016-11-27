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
#ifndef GRAPHICS_LAYER_DYNAMIC_LIBRARY_HPP
#define GRAPHICS_LAYER_DYNAMIC_LIBRARY_HPP

#include <windows.h>
#include <stdexcept>

#include <vector>
#include <string>

#include "DynamicClass.hpp"

namespace GraphicsLayer
{
class DynamicLibrary
{
    public:
        ~DynamicLibrary()
        {
            for(HINSTANCE lib : mLoadedLibraries)
                FreeLibrary(lib);
        };

        template<typename... DynamicClassesT>
        void loadDynamicClasses(const std::vector<std::string>& libPaths)
        {
            for(std::string path : libPaths)
            {
                mLoadedLibraries.push_back(LoadLibrary(path.c_str()));
                if(mLoadedLibraries.back() == NULL)
                    throw std::runtime_error("Failed to load library at '" + path + "'.");
            }

            loadDynamicClasses<DynamicClassesT...>();
        }

    private:
        template<typename DynamicClassT>
        void loadDynamicClass()
        {
            GraphicsLayer::DynamicMethod* const METHODS = DynamicClassT::getMethods();
            const size_t NUM_METHODS = DynamicClassT::getNumMethods();
            const HMODULE MODULE = GetModuleHandle(DynamicClassT::getLib());
            assert(MODULE != NULL);
            for(size_t i = 0; i < NUM_METHODS; i++)
            {
                void* const ADDRESS = (void*)GetProcAddress(MODULE, METHODS[i].symbol.c_str());
                assert(ADDRESS != nullptr);
                METHODS[i].address = ADDRESS;
            }
        }


        template<typename... DynamicClassesT>
        typename std::enable_if<sizeof...(DynamicClassesT) == 0>::type loadDynamicClasses()
        {
        }

        template<typename DynamicClassT, typename... DynamicClassesT>
        void loadDynamicClasses()
        {
            loadDynamicClass<DynamicClassT>();
            loadDynamicClasses<DynamicClassesT...>();
        }

    private:
        std::vector<HINSTANCE> mLoadedLibraries;
};

}
#endif // GRAPHICS_LAYER_DYNAMIC_LIBRARY_HPP
