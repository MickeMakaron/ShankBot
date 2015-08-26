////////////////////////////////////////////////
// ShankBot internal headers
#include "Windows/utility.hpp"
////////////////////////////////////////////////

////////////////////////////////////////////////
// Windows headers
#include <windows.h>
////////////////////////////////////////////////

////////////////////////////////////////////////
// STD - C++ Standard Library
#include <stdexcept>
////////////////////////////////////////////////

namespace Windows
{
    void throwLastErrorMessage(std::string details)
    {
        wchar_t message[256];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message, 256, NULL);

        std::wstring wStr(message);
        std::string str(wStr.begin(), wStr.end());

        if(details != "")
            str += "\nDetails: " + details;

        throw std::runtime_error(str);
    }
}

