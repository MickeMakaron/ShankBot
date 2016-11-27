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
#include "DynQByteArray.hpp"

DEFINE_DYNAMIC_CLASS_PROPERTIES
(
    QByteArray,
    "Qt5Core.dll",
    "??1QByteArray@@QAE@XZ",

    QByteArray* (*ctor1)(const QByteArray&) = nullptr;
    std::string ctorSymbol1 = "??0QByteArray@@QAE@ABV0@@Z";
);

using namespace GraphicsLayer;
DynQByteArray::DynQByteArray(const DynQByteArray& arr)
{
    auto& i = arr.getInstance();
    CALL_DYNAMIC_CLASS_CTOR(mInstance, QByteArray, DYNM.ctor1(i));
}

DynQByteArray::DynQByteArray(const QByteArray& arr)
{
    CALL_DYNAMIC_CLASS_CTOR(mInstance, QByteArray, DYNM.ctor1(arr));
}
