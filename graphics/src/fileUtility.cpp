///////////////////////////////////
// Internal ShankBot headers
#include "fileUtility.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstring>
#include <sstream>
#include <list>
#include <fstream>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <sys/stat.h>
#include <dirent.h>
///////////////////////////////////

namespace GraphicsLayer
{

void makeDirIfNotExists(std::string dirPath)
{
    struct stat st = {0};
    if(stat(dirPath.c_str(), &st) == -1)
    {
        if(mkdir(dirPath.c_str(), 0700) != 0)
            throw std::runtime_error("Could not create directory at '" + dirPath + "'.");
    }
}

bool fileExists(std::string path)
{
    struct stat st = {0};
    if(stat(path.c_str(), &st) == -1)
        return false;
    else
        return true;
}

void forEachFile(std::string directory, std::function<void(const std::string& file)> function, bool bottomUp)
{
    DIR* dir;
    dir = opendir(directory.c_str());
    if(dir == nullptr)
        throw std::runtime_error("Cannot open directory at '" + directory + "'.");

    dirent* ent;
    if(bottomUp)
    {
        while((ent = readdir(dir)) != nullptr)
        {
            if(ent->d_type == DT_DIR)
            {
                if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
                {
                    std::stringstream sstream;
                    sstream << directory << "/" << ent->d_name;
                    forEachFile(sstream.str(), function);
                    function(sstream.str());
                }
            }
            else if(ent->d_type == DT_REG)
            {
                std::stringstream sstream;
                sstream << directory << "/" << ent->d_name;
                function(sstream.str());
            }
        }
    }
    else // top down
    {
        std::list<std::string> directories;
        while((ent = readdir(dir)) != nullptr)
        {
            if(ent->d_type == DT_DIR)
            {
                if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
                {
                    std::stringstream sstream;
                    sstream << directory << "/" << ent->d_name;
                    directories.push_back(sstream.str());
                    function(sstream.str());
                }
            }
            else if(ent->d_type == DT_REG)
            {
                std::stringstream sstream;
                sstream << directory << "/" << ent->d_name;
                function(sstream.str());
            }
        }

        for(std::string d : directories)
            forEachFile(d, function, bottomUp);
    }


    closedir(dir);
}

bool isDir(std::string path)
{
    struct stat st = {0};
    if(stat(path.c_str(), &st) == -1)
        throw std::runtime_error("Could not open file at '" + path + "'.");

    return S_ISDIR(st.st_mode);
}

bool isFile(std::string path)
{
    struct stat st = {0};
    if(stat(path.c_str(), &st) == -1)
        throw std::runtime_error("Could not open file at '" + path + "'.");

    return S_ISREG(st.st_mode);
}

void copyFile(std::string src, std::string dest)
{
    if(isDir(src))
        makeDirIfNotExists(dest);
    else if(isFile(src))
    {
        std::ifstream in(src, std::ios::binary);
        std::ofstream out(dest, std::ios::binary);

        out << in.rdbuf();

        in.close();
        out.close();
    }
}

std::string getPath(std::string path)
{
    return path.substr(0, path.find_last_of('/'));
}

std::string basename(std::string path)
{
    return path.substr(path.find_last_of('/') + 1);
}

std::string basenameNoExt(std::string path)
{
    size_t start = path.find_last_of('/') + 1;
    if(start == path.size())
        start = 0;

    size_t end = path.find_last_of('.');
    if(end <= start)
        end = path.size() - 1;

    return path.substr(start, end - start);
}

void recursiveRemove(std::string directory, bool removeRoot)
{
    forEachFile(directory, [](const std::string& file)
    {
        remove(file.c_str());
    });

    if(removeRoot)
        remove(directory.c_str());
}

std::string recursiveCopy(std::string srcDir, std::string destDir)
{
    makeDirIfNotExists(destDir);
    destDir = destDir + "/" + basename(srcDir);
    copyFile(srcDir, destDir);

    forEachFile(srcDir, [&](const std::string& file)
    {
        std::string localPath = file.substr(srcDir.size());
        std::string destPath = destDir + localPath;

        copyFile(file, destPath);

    }, false);

    return destDir;
}

void renameFile(std::string path, std::string newName)
{
    std::string destPath = getPath(path) + "/" + newName;
    rename(path.c_str(), destPath.c_str());
}

}
