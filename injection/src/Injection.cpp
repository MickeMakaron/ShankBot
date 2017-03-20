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
#include "injection/Injection.hpp"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <algorithm>
///////////////////////////////////

namespace GraphicsLayer
{

    void Injection::setModuleDetours(const std::string& module, const std::vector<DetourHolder>& detours)
    {
        // Force std::vector to use copy constructor
        // instead of assignment operator for copying,
        // since DetourHolder's assignment operator is
        // ill-formed.
        std::vector<DetourHolder>& myDetours = mModuleDetours[module];
        myDetours.clear();
        myDetours.reserve(detours.size());
        for(const DetourHolder& h : detours)
        {
            myDetours.emplace_back(h);
        }
    }

    void Injection::hookModule(const std::string& module, const std::function<void*(const std::string&)>& getTargetFuncAddress)
    {
        auto foundIt = mModuleDetours.find(module);
        if(foundIt == mModuleDetours.end())
        {
            return;
        }

        for(DetourHolder& h : foundIt->second)
        {
            h.renew(getTargetFuncAddress);
        }
    }

    DetourHolder& Injection::getDetour(const void* detourFunc)
    {
        auto findPredicate = [detourFunc](const DetourHolder& h)
        {
            return h.getDetourFunc() == detourFunc;
        };
        for(auto& pair : mModuleDetours)
        {
            auto foundIt = std::find_if(pair.second.begin(), pair.second.end(), findPredicate);
            if(foundIt != pair.second.end())
            {
                return *foundIt;
            }
        }

        throw std::runtime_error("Failed to find detour.");
    }
}

