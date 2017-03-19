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
#ifndef GRAPHICS_LAYER_DETOUR_HOLDER_HPP
#define GRAPHICS_LAYER_DETOUR_HOLDER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "utility/FunctionDetour.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    class DetourHolder
    {
        public:
            explicit DetourHolder(const std::string& targetSymbol, void* detour);
            template<typename CFunction>
            explicit DetourHolder(const std::string& targetSymbol, const CFunction& detour)
            : mDetour(nullptr)
            , mTargetSymbol(targetSymbol)
            , mDetourFunc((void*)detour)
            {

            }

            void renew(const std::function<void*(const std::string&)>& getTargetAddress);

            const std::string& getTargetSymbol() const;
            const void* getDetourFunc() const;
            FunctionDetour* getDetour();


        private:
            std::shared_ptr<FunctionDetour> mDetour;
            const std::string mTargetSymbol;
            void* const mDetourFunc;
    };
}



#endif // GRAPHICS_LAYER_DETOUR_HOLDER_HPP
