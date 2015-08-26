////////////////////////////////////////////////
// ShankBot internal headers
#include "Windows/WindowPrinter.hpp"
#include "Windows/utility.hpp"
using namespace Windows;
////////////////////////////////////////////////

////////////////////////////////////////////////
// STD - C++ Standard Library
#include <stdexcept>
////////////////////////////////////////////////

WindowPrinter::WindowPrinter(std::string title)
: M_WINDOW(FindWindow(NULL, title.c_str()))
{
    if(!M_WINDOW)
        throwLastErrorMessage("Could not find '" + title + "' window.");
}

void WindowPrinter::writeToFile(HBITMAP bitmap, LPTSTR filename) const
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD cClrBits;
    HANDLE hf; // file handle
    BITMAPFILEHEADER hdr; // bitmap file-header
    PBITMAPINFOHEADER pbih; // bitmap info-header
    LPBYTE lpBits; // memory pointer
    DWORD cb; // incremental count of bytes
    BYTE *hp; // byte pointer
    DWORD dwTmp;

    // create the bitmapinfo header information
    if(!GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp))
        throw std::runtime_error("Could not retrieve bitmap info");

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if(cClrBits == 1)
        cClrBits = 1;
    else if(cClrBits <= 4)
        cClrBits = 4;
    else if(cClrBits <= 8)
        cClrBits = 8;
    else if(cClrBits <= 16)
        cClrBits = 16;
    else if(cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure.
    if(cClrBits != 24)
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
    else
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if(cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits;

    // Set biClrImportant to 0, indicating that all of the
    // device colors are important.
    pbmi->bmiHeader.biClrImportant = 0;

    // now open file and save the data
    pbih = (PBITMAPINFOHEADER) pbmi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if(!lpBits)
        throwLastErrorMessage("writeBMP::Could not allocate memory");


    // Retrieve the color table (RGBQUAD array) and the bits
    if(!GetDIBits(GetWindowDC(GetDesktopWindow()), HBITMAP(bitmap), 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS))
        std::runtime_error("writeBMP::GetDIB error");

    // Create the .BMP file.
    hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
    if(hf == INVALID_HANDLE_VALUE)
        throwLastErrorMessage("Could not create file for writing");

    hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if(!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL))
        throwLastErrorMessage("Could not write bitmap file header to file");

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if(!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwTmp, NULL))
        throwLastErrorMessage("Could not write bitmap info header to file");


    // Copy the array of color indices into the .BMP file.
    cb = pbih->biSizeImage;
    hp = lpBits;
    if(!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
        throwLastErrorMessage("Could not write bitmap color indices to file.");

    // Close the .BMP file.
    if(!CloseHandle(hf))
        throwLastErrorMessage("Could not close file");

    // Free memory.
    GlobalFree((HGLOBAL)lpBits);
}

PixelArray WindowPrinter::getPixelArray(HBITMAP BitmapHandle) const
{
    BITMAP Bmp = {0};
    BITMAPINFO Info = {{0}};

    HDC DC = CreateCompatibleDC(NULL);
    HBITMAP OldBitmap = (HBITMAP)SelectObject(DC, BitmapHandle);
    GetObject(BitmapHandle, sizeof(Bmp), &Bmp);

    int height, width;

    Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Info.bmiHeader.biWidth = width = Bmp.bmWidth;
    Info.bmiHeader.biHeight = height = Bmp.bmHeight;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = Bmp.bmBitsPixel;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = ((width * Bmp.bmBitsPixel + 31) / 32) * 4 * height;

    size_t size = Info.bmiHeader.biSizeImage;
    char* pixels = new char[size];
    GetDIBits(DC, BitmapHandle, 0, height, pixels, &Info, DIB_RGB_COLORS);
    SelectObject(DC, OldBitmap);

    DeleteDC(DC);

    return PixelArray(pixels, size);
}

HBITMAP WindowPrinter::print() const
{
    HDC srcDC = GetDC(M_WINDOW);

    RECT rect;
    GetClientRect(M_WINDOW, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    HBITMAP bitmapHandle = CreateCompatibleBitmap(srcDC, width, height);

    HDC destDC = CreateCompatibleDC(srcDC);
    SelectObject(destDC, bitmapHandle);
    BitBlt(destDC, 0, 0, width, height, srcDC, 0, 0, SRCCOPY|CAPTUREBLT);

    ReleaseDC(M_WINDOW, srcDC);
    DeleteDC(destDC);

    return bitmapHandle;
}

PixelArray WindowPrinter::printToPixelArray() const
{
    HBITMAP bitmapHandle = print();
    PixelArray pixels = getPixelArray(bitmapHandle);
    DeleteObject(bitmapHandle);

    return pixels;
}

void WindowPrinter::printToFile(std::string fileName) const
{
    HBITMAP bitmapHandle = print();
    writeToFile(bitmapHandle, (LPTSTR)fileName.data());
    DeleteObject(bitmapHandle);
}
