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
#ifndef GRAPHICS_LAYER_EQUIPMENT_HPP
#define GRAPHICS_LAYER_EQUIPMENT_HPP

///////////////////////////////////
// Internal ShankBot headers
namespace sb
{
namespace tibiaassets
{
    class Object;
}
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstring>
///////////////////////////////////

namespace GraphicsLayer
{
    class Equipment
    {
        public:
            enum class SlotId : unsigned char
            {
                HIP,
                RIGHT_HAND,
                BACK,
                FEET,
                LEGS,
                TORSO,
                HEAD,
                FINGER,
                LEFT_HAND,
                NECK,
                NUM_SLOTS
            };

        public:
            void setSlot(SlotId slotId, const sb::tibiaassets::Object* object);
            const sb::tibiaassets::Object* getSlot(SlotId slotId) const;

        private:
            const sb::tibiaassets::Object* mSlots[(size_t)SlotId::NUM_SLOTS] = {0};
    };
}


#endif // GRAPHICS_LAYER_EQUIPMENT_HPP
