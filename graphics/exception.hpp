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
#ifndef GRAPHICS_LAYER_EXCEPTION_HPP
#define GRAPHICS_LAYER_EXCEPTION_HPP

///////////////////////////////////
// STD C++
#include <sstream>
#include <stdexcept>
///////////////////////////////////


namespace GraphicsLayer
{
    template<typename ExceptionType>
    void throwException(size_t line, std::string file, std::string function, std::string message)
    {
        std::stringstream sstream;
        sstream << std::endl
                << "File: " << file << std::endl
                << "Function (line nr): " << function << " (" << line << ")" << std::endl
                << "Message: " << message << std::endl;
        throw ExceptionType(sstream.str());
    }

    #define THROW_RUNTIME_ERROR(message) GraphicsLayer::throwException<std::runtime_error>(__LINE__, __FILE__, __PRETTY_FUNCTION__, message);

}

#endif // GRAPHICS_LAYER_EXCEPTION_HPP
