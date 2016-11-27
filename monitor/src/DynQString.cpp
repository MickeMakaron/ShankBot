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
#include "DynQString.hpp"
#include "QtCore/QByteArray"

DEFINE_DYNAMIC_CLASS_PROPERTIES
(
    QString,
    "Qt5Core.dll",
    "??1QString@@QAE@XZ",

    QString& (*append)(const char*) = nullptr;
    std::string appendSymbol = "?append@QString@@QAEAAV1@PBD@Z";
    QString* (*qStringCtor1)() = nullptr;
    std::string qStringCtorSymbol1 = "??0QString@@QAE@XZ";
    QString* (*qStringCtor2)(const QString&) = nullptr;
    std::string qStringCtorSymbol2 = "??0QString@@QAE@ABV0@@Z";
    int (*size)() = nullptr;
    std::string qSizeSymbol = "?size@QString@@QBEHXZ";
    QString (*fromStdString)(const std::string&) = nullptr;
    std::string fromStdStringSymbol = "?fromStdString@QString@@SA?AV1@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
    QString& (*assignByteArray)(const QByteArray&) = nullptr;
    std::string assignByteArraySym = "??4QString@@QAEAAV0@ABVQByteArray@@@Z";
    QByteArray (*toUtf8)() = nullptr;
    std::string toUtf8Sym = "?toUtf8@QString@@QBE?AVQByteArray@@XZ";
);

using namespace GraphicsLayer;
DynQString::DynQString()
{
    CALL_DYNAMIC_CLASS_CTOR(mInstance, QString, DYNM.qStringCtor1());
}

DynQString::DynQString(const QString& str)
{
    CALL_DYNAMIC_CLASS_CTOR(mInstance, QString, DYNM.qStringCtor2(str));
}

QString& DynQString::append(const char* str)
{
    CALL_DYNAMIC_CLASS_METHOD(mInstance, return DYNM.append(str));
}

DynQString DynQString::fromStdString(const std::string& str)
{
    return DynQString(DYNM.fromStdString(str));
}

DynQString& DynQString::operator=(const DynQByteArray& other)
{
    auto& i = other.getInstance();
    CALL_DYNAMIC_CLASS_METHOD(mInstance, *mInstance = DYNM.assignByteArray(i));
    return *this;
}

QByteArray DynQString::toUtf8()
{
    CALL_DYNAMIC_CLASS_METHOD(mInstance, return DYNM.toUtf8());
}
