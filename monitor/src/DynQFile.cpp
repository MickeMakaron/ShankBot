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
#include "DynQFile.hpp"

DEFINE_DYNAMIC_CLASS_PROPERTIES
(
    QFile,
    "Qt5Core.dll",
    "??1QFile@@UAE@XZ",

    QFile* (*ctor1)(const QString&) = nullptr;
    std::string ctorSymbol1 = "??0QFile@@QAE@ABVQString@@@Z";
    bool (*open)(QFlags<QIODevice::OpenModeFlag>) = nullptr;
    std::string openSym = "?open@QFile@@UAE_NV?$QFlags@W4OpenModeFlag@QIODevice@@@@@Z";
    QByteArray (*readAll)() = nullptr;
    std::string readAllSym = "?readAll@QIODevice@@QAE?AVQByteArray@@XZ";
    void (*close)() = nullptr;
    std::string closeSym = "?close@QFileDevice@@UAEXXZ";
);

using namespace GraphicsLayer;
DynQFile::DynQFile(const DynQString& str)
{
    auto& i = str.getInstance();
    CALL_DYNAMIC_CLASS_CTOR(mInstance, QFile, DYNM.ctor1(i));
}




bool DynQFile::open(QFlags<QIODevice::OpenModeFlag> flag)
{
    CALL_DYNAMIC_CLASS_METHOD(mInstance, return DYNM.open(flag));
}

DynQByteArray DynQFile::readAll()
{
    CALL_DYNAMIC_CLASS_METHOD(mInstance, return DynQByteArray(DYNM.readAll()));
}

void DynQFile::close()
{
    CALL_DYNAMIC_CLASS_METHOD(mInstance, DYNM.close());

}
