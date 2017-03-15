# ShankBot

## Build
### 1. Install dependencies:
ShankBot has the following dependencies:
* Qt
* GLEW

#### Install Qt
The easiest way to install Qt is to use their installer. However, only the QtCore and QtGui libraries are actually used by ShankBot, so you could just build those two libraries from source. Here's how to install the necessary libraries and headers using Qt's installer:
1. Download the Qt Online Installer for your platform [here](https://www.qt.io/download-open-source/#section-2).
2. Run the installer and specify the installation directory. When you get to select components to install, the only thing you need is the prebuilt component for your compiler toolchain. So, for example, if you only want the headers and libraries for MinGW 5.3.0 32 bit for Qt 5.8, uncheck all boxes that can be unchecked, expand Qt 5.8 and check the box that says "MinGW 5.3.0 32 bit". 


#### Install GLEW
Only the headers are needed. Download them at http://glew.sourceforge.net/.

### 2. Download source
> git clone https://github.com/MickeMakaron/ShankBot

### 3. Build with CMake 
Set the "Source" directory to the directory that contains a "CMakeLists.txt" file. You will be prompted to tell CMake where you installed the dependencies. 


## System Maps
![](http://i.imgur.com/xS3nv35.jpg)
![](http://i.imgur.com/Erdu6bU.jpg)
![](http://i.imgur.com/Ph5YocH.jpg)
