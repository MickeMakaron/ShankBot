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
#ifndef GRAPHICS_LAYER_SIDE_BAR_WINDOW_DIFF_HPP
#define GRAPHICS_LAYER_SIDE_BAR_WINDOW_DIFF_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/SideBarWindowAssembler.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
///////////////////////////////////

namespace GraphicsLayer
{
    class SideBarWindowDiff
    {
        public:
            struct Event
            {
                enum class Type : unsigned char
                {
                    OPEN,
                    CLOSE,
                    MIN,
                    MAX,
                    RESIZE,
                    SCROLL,
                    MOVE,

                    NUM_TYPES,
                    INVALID,
                };

                Type type = Type::INVALID;
                SideBarWindow::Type windowType = SideBarWindow::Type::INVALID;
                const SideBarWindow* oldWindow = nullptr;
                const SideBarWindow* newWindow = nullptr;
            };

        public:
            SideBarWindowDiff(const SideBarWindowAssembler::Data& oldData, const SideBarWindowAssembler::Data& newData);

            const std::vector<Event>& getEvents() const;

        private:
            void parse(const SideBarWindow* oldW, const SideBarWindow* newW, SideBarWindow::Type type);
            void parseContainers(const std::vector<ContainerWindow>& oldData, const std::vector<ContainerWindow>& newData);


            static bool isContainerEqual(const ContainerWindow& w1, const ContainerWindow& w2);
            static bool isSpriteDrawEqual(const SpriteDraw& d1, const SpriteDraw& d2);
            static bool isItemEqual(const ContainerWindow::Item& i1, const ContainerWindow::Item& i2);
            static bool isContainerContentsEqual(const ContainerWindow& w1, const ContainerWindow& w2);
            static bool isContainerContentsMoved(const ContainerWindow& oldW, const ContainerWindow& newW);

        private:
            std::vector<Event> mEvents;
    };
}

#endif // GRAPHICS_LAYER_SIDE_BAR_WINDOW_DIFF_HPP
