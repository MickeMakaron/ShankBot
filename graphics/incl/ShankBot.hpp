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
#ifndef GRAPHICS_LAYER_SHANK_BOT_HPP
#define GRAPHICS_LAYER_SHANK_BOT_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "TibiaClient.hpp"
#include "TibiaContext.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    class ShankBot
    {
        public:
            explicit ShankBot(std::string clientDir, std::string versionControlDir);

            void run();

        private:
            void initializeData(std::string clientDir, std::string versionControlDir);

        private:
            std::unique_ptr<TibiaClient> mTibiaClient;
            std::unique_ptr<TibiaContext> mTibiaContext;
    };
}


#endif // GRAPHICS_LAYER_SHANK_BOT_HPP
