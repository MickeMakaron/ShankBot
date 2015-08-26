#ifndef WINDOWS_UTILITY_HPP
#define WINDOWS_UTILITY_HPP

////////////////////////////////////////////////
// STD - C++ Standard Library
#include <string>
////////////////////////////////////////////////

namespace Windows
{
    /// \brief Throw last error message reported by Windows
    ///
    /// \param "" std::string details= Additional details appended to the end of error message.
    /// \return void
    ///
    ///
    void throwLastErrorMessage(std::string details = "");
}


#endif // WINDOWS_UTILITY_HPP

