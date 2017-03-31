// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
///////////////////////////////////
// Internal ShankBot headers
#include "utility/file.hpp"
#include "utility/utility.hpp"
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

#ifndef _WIN32
#include <dirent.h>
#endif // _WIN32

#ifndef S_ISDIR
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define	S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define	S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define	S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif // S_ISDIR
///////////////////////////////////

///////////////////////////////////
// Windows
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif // _WIN32
///////////////////////////////////

namespace sb
{
namespace utility
{
namespace file
{
void makeDirIfNotExists(std::string dirPath)
{
    struct stat st = {0};
    if(stat(dirPath.c_str(), &st) == -1)
    {
        #if defined(_WIN32)
        if(CreateDirectory(dirPath.c_str(), NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
        {
        #else
        if(mkdir(dirPath.c_str(), 0700) != 0)
        {
        #endif // defined
            SB_THROW("Could not create directory at '" + dirPath + "'.");
        }
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
    #ifdef _WIN32
    std::string searchPattern = directory + "\\*";
    WIN32_FIND_DATA ent;
    HANDLE find = FindFirstFile(searchPattern.c_str(), &ent);
    if(find == INVALID_HANDLE_VALUE)
    {
        if(GetLastError() == ERROR_FILE_NOT_FOUND)
            return;

        SB_THROW("Cannot open directory at '" + directory + "'.");
    }

    #else
    DIR* dir;
    dirent* ent;
    dir = opendir(directory.c_str());
    if(dir == nullptr)
        SB_THROW("Cannot open directory at '" + directory + "'.");

    #endif // _WIN32

    if(bottomUp)
    {
        #ifdef _WIN32
        do
        #else
        while((ent = readdir(dir)) != nullptr)
        #endif // _WIN32
        {
            struct stat s;
            std::string fileName;
            #ifdef _WIN32
            fileName.assign(ent.cFileName);
            #else
            fileName.assign(ent->d_name);
            #endif // _WIN32

            std::string path = directory + "/" + fileName;
            int result = stat(path.c_str(), &s);
			if (result != 0)
			{
				unsigned int errorCode;
				#ifdef _WIN32
				errorCode = GetLastError();
				#else
				errorCode = errno;
				#endif // _WIN32
                SB_THROW("Coult not stat file at '", path, "'. Error code: ", errorCode);

			}

            if (S_ISDIR(s.st_mode))
//            if(ent->d_type == DT_DIR)
            {
                if(fileName != ".." && fileName != ".")
                {
                    forEachFile(path, function);
                    function(path);
                }
            }
            else if(S_ISREG(s.st_mode))
//            else if(ent->d_type == DT_REG)
            {
                function(path);
            }
        #ifdef _WIN32
        } while(FindNextFile(find, &ent) != 0);
        #else
        }
        #endif // _WIN32

    }
    else // top down
    {
        std::list<std::string> directories;
        #ifdef _WIN32
        do
        #else
        while((ent = readdir(dir)) != nullptr)
        #endif // _WIN32
        {
            struct stat s;

            std::string fileName;
            #ifdef _WIN32
            fileName.assign(ent.cFileName);
            #else
            fileName.assign(ent->d_name);
            #endif // _WIN32

            std::string path = directory + "/" + fileName;
            stat(path.c_str(), &s);
            if (S_ISDIR(s.st_mode))
            {
                if(fileName != ".." && fileName != ".")
                {
                    directories.push_back(path);
                    function(path);
                }
            }
            else if(S_ISREG(s.st_mode))
            {
                function(path);
            }
        #ifdef _WIN32
        } while(FindNextFile(find, &ent) != 0);
        #else
        }
        #endif // _WIN32

        for(std::string d : directories)
            forEachFile(d, function, bottomUp);
    }

    #ifdef _WIN32
    FindClose(find);
    #else
    closedir(dir);
    #endif // _WIN32

    /*
    #else


    if(bottomUp)
    {
        while((ent = readdir(dir)) != nullptr)
        {
            struct stat s;
            std::stringstream fileNameStream;
            fileNameStream << directory << "/" << ent->d_name;
            int result = stat(fileNameStream.str().c_str(), &s);
            if(result != 0)
            {
                std::stringstream sstream;
                sstream << "Could not stat file at '" << directory + "/" << ent->d_name << "'. Error code: " << errno << std::endl;
                SB_THROW(sstream.str());
            }
            if (S_ISDIR(s.st_mode))
//            if(ent->d_type == DT_DIR)
            {
                if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
                {
                    std::stringstream sstream;
                    sstream << directory << "/" << ent->d_name;
                    forEachFile(sstream.str(), function);
                    function(sstream.str());
                }
            }
            else if(S_ISREG(s.st_mode))
//            else if(ent->d_type == DT_REG)
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
            struct stat s;
            stat(ent->d_name, &s);
            if (S_ISDIR(s.st_mode))
            {
                if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
                {
                    std::stringstream sstream;
                    sstream << directory << "/" << ent->d_name;
                    directories.push_back(sstream.str());
                    function(sstream.str());
                }
            }
            else if(S_ISREG(s.st_mode))
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

    #endif // _WIN32
    */
}

bool isDir(std::string path)
{
    struct stat st = {0};
    if(stat(path.c_str(), &st) == -1)
        SB_THROW("Could not open file at '" + path + "'.");

    return S_ISDIR(st.st_mode);
}

bool isFile(std::string path)
{
    struct stat st = {0};
    if(stat(path.c_str(), &st) == -1)
        SB_THROW("Could not open file at '" + path + "'.");

    return S_ISREG(st.st_mode);
}

void copyFile(std::string src, std::string dest)
{
    if(isDir(src))
        makeDirIfNotExists(dest);
    else if(isFile(src))
    {
        if(fileExists(dest) && isDir(dest))
            dest += "/" + basename(src);
        std::ifstream in(src, std::ios::binary);
        std::ofstream out(dest, std::ios::binary);

        out << in.rdbuf();

        in.close();
        out.close();
    }
}

std::string getPath(std::string path)
{
    size_t i = path.find_last_of('/');
    if(i == path.npos)
        i = path.find_last_of('\\');

    return path.substr(0,  i);
}

std::string basename(std::string path)
{
    size_t i = path.find_last_of('/');
    if(i == path.npos)
        i = path.find_last_of('\\');
    return path.substr(i + 1);
}

std::string basenameNoExt(std::string path)
{
    size_t i = path.find_last_of('/');
    if(i == path.npos)
        i = path.find_last_of('\\');
    size_t start = i + 1;
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


uint64_t getFileModifiedTime(const std::string& file)
{
    HANDLE hFile = CreateFile(file.c_str(),               // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                       NULL);

    if(hFile == INVALID_HANDLE_VALUE)
        SB_THROW("Failed to open file.");

    uint64_t modified;
    if(!GetFileTime(hFile, NULL, NULL, (FILETIME*)&modified))
        SB_THROW("Failed to get file time.");

    CloseHandle(hFile);

    return modified;
}


std::list<std::pair<uint64_t, std::string>> getLatestModifiedFiles(const std::string& directory, size_t numFiles)
{
    std::list<std::pair<uint64_t, std::string>> files(numFiles);
    forEachFile(directory, [&](const std::string& file)
    {
        uint64_t modified = getFileModifiedTime(file);

        auto it = files.begin();

        while(it != files.end())
        {
            if(modified <= it->first)
                break;

            it++;
        }

        if(it != files.begin())
        {
            files.insert(it, std::make_pair(modified, file));
            files.pop_front();
        }
    });

    std::list<std::pair<uint64_t, std::string>> out;
    for(const auto& pair : files)
        if(pair.first != 0)
            out.push_front(pair);

    return out;
}

uint64_t getLatestModifiedFileTime(const std::string& directory)
{
    uint64_t latest = 0;
    forEachFile(directory, [&latest](const std::string& file)
    {
        latest = std::max(getFileModifiedTime(file), latest);
    });

    return latest;
}

}
}
}
