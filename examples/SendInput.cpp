// Example of sending keyboard input to a window.
// Open the window to work with before running the program.

#include "Windows/WindowInputSender.hpp"
using namespace Windows;

int main()
{
    // Assign sender to a window with the title "Window".
    WindowInputSender sender("Window");

    // Send a Tab key press.
    sender.sendKey('\t');

    // Send a sequence of keys. (\r is the Enter key).
    sender.sendString("Hello World!\r");
}
