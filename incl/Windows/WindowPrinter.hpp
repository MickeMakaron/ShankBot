#ifndef WINDOWS_WINDOWPRINTER_HPP
#define WINDOWS_WINDOWPRINTER_HPP


////////////////////////////////////////////////
// ShankBot internal headers
#include "PixelArray.hpp"
////////////////////////////////////////////////

////////////////////////////////////////////////
// Windows headers
#include <windows.h>
////////////////////////////////////////////////

////////////////////////////////////////////////
// STD - C++ Standard Library
#include <string>
////////////////////////////////////////////////


/// \brief Print screenshots from window
///
class WindowPrinter
{
    public:
        /// \brief Assign printer to window by title
        ///
        /// \param title std::string Title of window
        ///
        ///
        WindowPrinter(std::string title);

        /// \brief Print a screenshot to pixel array object.
        ///
        /// \return PixelArray Pixel array object containing pixel data of screenshot.
        ///
        ///
        PixelArray printToPixelArray() const;

        /// \brief Print a screenshot to file.
        ///
        /// \param fileName std::string Complete or relative filepath of resulting file.
        /// \return void
        ///
        ///
        void printToFile(std::string fileName) const;

    private:
        /// \brief Write bitmap handle to file.
        ///
        /// \param bitmap HBITMAP Bitmap handle to write.
        /// \param filename LPTSTR Name of written file.
        /// \return void
        ///
        ///
        void writeToFile(HBITMAP bitmap, LPTSTR filename) const;

        /// \brief Get pixel array object from bitmap handle.
        ///
        /// \param BitmapHandle HBITMAP Bitmap handle to extract pixel data from
        /// \return PixelArray Pixel array object containing pixel data.
        ///
        ///
        PixelArray getPixelArray(HBITMAP BitmapHandle) const;

        /// \brief Print a screenshot into bitmap handle.
        ///
        /// \return HBITMAP Bitmap handle to resulting screenshot
        ///
        ///
        HBITMAP print() const;

    private:
        const HWND M_WINDOW; ///< Window handle to print from
};

#endif // WINDOWS_WINDOWPRINTER_HPP
