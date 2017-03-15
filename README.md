# ShankBot
ShankBot is an automation tool for the MMORPG Tibia. 

It works by monitoring what Tibia renders to the screen by hooking OpenGL calls. 

# Build
## 1. Install dependencies:
ShankBot has the following dependencies:
* Qt
* GLEW

### Install Qt
Qt is a big collection of tools and libaries, and ShankBot only uses the libraries and headers of QtCore and QtGui. There are three ways you could install Qt:
1. Use the Qt installer. This is the easiest way, but will install LOTS of bloatware.
2. Download only QtBase, which contains QtCore and QtGui among other things. It is a bit tricky to find the right file to download, but the upside is that you reduce the bloat a lot, and it will go a lot faster to download and "install" once you've found the right file.
3. Build QtCore and QtGui explicitly from source. Yeah... it's possible, but you're on your own.

#### Qt installer
NOTE: This will install tons of bloat. If you mind installing an extra +2gb of documentation and tools, go to the next section instead.
1. Download the Qt Online Installer for your platform [here](https://www.qt.io/download-open-source/#section-2).
2. Run the installer and specify the installation directory. When you get to select components to install, the only thing you need is the prebuilt component for your compiler toolchain. So, for example, if you only want the headers and libraries for MinGW 5.3.0 32 bit for Qt 5.8, uncheck all boxes that can be unchecked, expand Qt 5.8 and check the box that says "MinGW 5.3.0 32 bit".

#### QtBase only
1. Go to [Qt's SDK browser](http://download.qt.io/online/qtsdkrepository/). You will be presented with a list of platforms. Pick your platform.
2. You will be presented with a list of target devices. Pick your target device.
3. Pick the Qt version you're interested in. For example, if you want to use Qt 5.8.0, pick qt5_58.
4. Pick the compiler you want the Qt binaries to be built with. This list a bit cluttered. Look for list items formatted as "qt.\<version\>.\<compiler\>". For Qt 5.8.0 MinGW 5.3.0 it's "qt.58.win32_mingw53".
5. Finally, download QtBase. On Windows the name has "qtbase" in it, so just do a Ctrl+F for "qtbase". On linux it has "essentials" in it instead.
6. Extract QtBase to wherever you want it.

For example, if you want QtBase for Win32 on desktop with Qt 5.8.0 and MinGW 5.3.0, download
> http://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt5_58/qt.58.win32_mingw53/5.8.0-1qtbase-Windows-Windows_7-Mingw53-Windows-Windows_7-X86.7z

### Install GLEW
Only the headers are needed. Download them at http://glew.sourceforge.net/.

## 2. Download source
> git clone https://github.com/MickeMakaron/ShankBot

## 3. Build with CMake 
Set the "Source" directory to the directory that contains a "CMakeLists.txt" file. It should be at "\<installdir\>/ShankBot/CMakeLists.txt". You will be prompted to tell CMake where you installed the dependencies. 

If you are using precompiled binaries of Qt, remember to make sure that the compiler you are compiling ShankBot with is ABI-compatible with the compiler that the Qt binaries were built with.


# Run
Currently, ShankBot can only run one Tibia client, and its location is hardcoded in the main function at
> monitor/src/main.cpp

So, to run ShankBot:
1. Set the "tibiaDir" string to Tibia's directory. The directory should contain an executable called "Tibia". 
2. Set the "versionControlDir" to any directory you like. It will be used by ShankBot to keep track of Tibia versions. 
3. Run the "ShankBotMonitor" executable.

Now a ShankBot server is running and is attached to a Tibia client. You may connect ShankBot clients to the server. For an example of how to do this, build the messaging example at
> examples/messaging


# System Maps
![](http://i.imgur.com/xS3nv35.jpg)
![](http://i.imgur.com/Erdu6bU.jpg)
![](http://i.imgur.com/Ph5YocH.jpg)
