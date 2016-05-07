#ifndef GRAPHICS_LAYER_FILE_UTILITY
#define GRAPHICS_LAYER_FILE_UTILITY


///////////////////////////////////
// STD C++
#include <string>
#include <functional>
///////////////////////////////////


namespace GraphicsLayer
{
    void makeDirIfNotExists(std::string dirPath);
    bool fileExists(std::string path);
    void forEachFile(std::string directory, std::function<void(const std::string& file)> function, bool bottomUp = true);
    bool isDir(std::string path);
    bool isFile(std::string path);
    void copyFile(std::string src, std::string dest);
    std::string getPath(std::string path);
    std::string basename(std::string path);
    std::string basenameNoExt(std::string path);
    void recursiveRemove(std::string directory, bool removeRoot = true);
    std::string recursiveCopy(std::string srcDir, std::string destDir);
    void renameFile(std::string path, std::string newName);
}

#endif // GRAPHICS_LAYER_FILE_UTILITY

