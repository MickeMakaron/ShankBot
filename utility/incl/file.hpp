// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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
#ifndef SB_UTILITY_FILE_HPP
#define SB_UTILITY_FILE_HPP


///////////////////////////////////
// STD C++
#include <string>
#include <functional>
#include <list>
///////////////////////////////////

namespace sb
{
namespace utility
{
namespace file
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
    uint64_t getLatestModifiedFileTime(const std::string& directory);
    std::list<std::pair<uint64_t, std::string>> getLatestModifiedFiles(const std::string& directory, size_t numFiles);
    uint64_t getFileModifiedTime(const std::string& file);
}
}
}


#endif // SB_UTILITY_FILE_HPP

