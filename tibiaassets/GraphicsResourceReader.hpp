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
#ifndef SB_TIBIAASSETS_GRAPHICS_RESOURCE_READER_HPP
#define SB_TIBIAASSETS_GRAPHICS_RESOURCE_READER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <string>
#include <map>
///////////////////////////////////

namespace sb
{
namespace tibiaassets
{
    class SHANK_BOT_TIBIAASSETS_DECLSPEC GraphicsResourceReader
    {
        public:
            struct GraphicsResource
            {
                std::string name;
                unsigned char* pixels;
                unsigned int width;
                unsigned int height;
            };

        public:
            explicit GraphicsResourceReader(std::string path);
            ~GraphicsResourceReader();

            const std::vector<GraphicsResource>& getGraphicsResources() const;

        private:
            void readResources(std::string path);
            void printResources(std::string path) const; // DEBUGGING PURPOSES

        private:
            std::vector<GraphicsResource> mGraphicsResources;

            struct SpriteSheet
            {
                size_t spriteWidth = 0;
                size_t spriteHeight = 0;
                std::vector<std::string> names;
            };
            static const std::map<std::string, SpriteSheet> SPRITE_SHEETS;
    };
}
}

#endif // SB_TIBIAASSETS_GRAPHICS_RESOURCE_READER_HPP
