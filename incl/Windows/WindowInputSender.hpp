#ifndef WINDOWS_INPUTSENDER_HPP
#define WINDOWS_INPUTSENDER_HPP

////////////////////////////////////////////////
// Windows headers
#include <windows.h>
////////////////////////////////////////////////

////////////////////////////////////////////////
// STD - C++ Standard Library
#include <string>
////////////////////////////////////////////////

namespace Windows
{
    /// \brief Sends keyboard and mouse input to window handle
    ///
    class WindowInputSender
    {
        public:
            /// \brief Assign sender to window handle by title
            ///
            /// \param title std::string Title of window
            ///
            WindowInputSender(std::string title);

            /// \brief Send a character to window
            ///
            /// \param key char Character to send
            /// \return void
            ///
            ///
            void sendKey(char key) const;

            /// \brief Send string to window
            ///
            /// \param str std::string String to send
            /// \param unsigned int delayAsMs = 0 Time delay in milliseconds between each sent character in string
            /// \return void
            ///
            ///
            void sendString(std::string str, unsigned int delayAsMs = 0) const;

            // Unimplemented
            void sendMouseButton(bool left = true) const;

        private:
            HWND mWindow; ///< Window handle to send input to.
    };
}

#endif // WINDOWS_INPUTSENDER_HPP
