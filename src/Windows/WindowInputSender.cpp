////////////////////////////////////////////////
// ShankBot internal headers
#include "Windows/WindowInputSender.hpp"
#include "Windows/utility.hpp"
using namespace Windows;
////////////////////////////////////////////////


WindowInputSender::WindowInputSender(std::string title)
: mWindow(FindWindow(NULL, title.c_str()))
{
    if(!mWindow)
        throwLastErrorMessage("Could not find '" + title + "' window.");
}

//////////////////////////////////////

void WindowInputSender::sendKey(char key) const
{
    if(!PostMessage(mWindow, WM_CHAR, (WPARAM)key, 0))
        throwLastErrorMessage("Could not send event to window.");
}

//////////////////////////////////////

void WindowInputSender::sendString(std::string str, unsigned int delayAsMs) const
{
   for(char c : str)
   {
       sendKey(c);
       Sleep(delayAsMs);
   }
}

//////////////////////////////////////

void WindowInputSender::sendMouseButton(bool left) const
{
    // Unimplemented
}
