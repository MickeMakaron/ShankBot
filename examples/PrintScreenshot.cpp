// Example of printing a screenshot from a window and writing it to file.
// Open the window to work with before running the program.
// Note: If the window is minimized, the resulting screenshot will be blank.

#include "Windows/WindowPrinter.hpp"
using namespace Windows;

int main()
{
    // Assign printer to a window with the title "Window".
    WindowPrinter printer("Window");

    // Print a screenshot of the window and write it to the file "pic.bmp" in the current directory.
    printer.printToFile("pic.bmp");
}
