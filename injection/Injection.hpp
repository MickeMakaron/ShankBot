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
#ifndef GRAPHICS_LAYER_INJECTION_HPP
#define GRAPHICS_LAYER_INJECTION_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "injection/DetourHolder.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    class Injection
    {
        public:
            void setModuleDetours(const std::string& module, const std::vector<DetourHolder>& detours);

            void hookModule(const std::string& module, const std::function<void*(const std::string&)>& getTargetFuncAddress);

            DetourHolder& getDetour(const void* detourFunc);
            template<typename CFunction>
            DetourHolder& getDetour(const CFunction& detourFunc)
            {
                return getDetour((const void*)detourFunc);
            }

        private:
            std::map<std::string, std::vector<DetourHolder>> mModuleDetours;
    };
}

#endif // GRAPHICS_LAYER_INJECTION_HPP
