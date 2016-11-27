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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/VersionControl.hpp"
#include "utility/file.hpp"
#include "utility/utility.hpp"
using namespace sb::utility::file;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtCore/QtCore"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
///////////////////////////////////




using namespace GraphicsLayer;

const std::string VersionControl::OLD_PATH = "/old";
const std::string VersionControl::CURRENT_PATH = "/current";
const std::string VersionControl::CURRENT_VERSION_PATH = CURRENT_PATH + "/version";
#if defined(_WIN32)
const std::string VersionControl::CLIENT_MODULE_NAME = "/client.exe";
#else
const std::string VersionControl::CLIENT_MODULE_NAME = "/client";
#endif // defined
const std::string VersionControl::TIBIA_PACKAGE_PATH = "/packages/Tibia";
const std::string VersionControl::BIN_PATH = TIBIA_PACKAGE_PATH + "/bin";
const std::string VersionControl::ASSETS_PATH = TIBIA_PACKAGE_PATH + "/assets";
const std::string VersionControl::TIBIA_PACKAGE_INFO_PATH = TIBIA_PACKAGE_PATH + "/package.json";

const std::string VersionControl::CLIENT_MODULE_PATH = BIN_PATH + CLIENT_MODULE_NAME;

VersionControl::VersionFile VersionControl::mVersionFile;

std::string VersionControl::getPath(std::string versionControlDir)
{
    return versionControlDir + CURRENT_PATH;
}

bool VersionControl::hasNewVersion(std::string clientDir, std::string versionControlDir)
{
    checkClientFiles(clientDir);
    prepareFiles(versionControlDir);
    return updateVersion(clientDir, versionControlDir);
}

void VersionControl::prepareFiles(std::string versionControlDir)
{
    makeDirIfNotExists(versionControlDir);

    std::string oldVersionsDir = versionControlDir + OLD_PATH;
    makeDirIfNotExists(oldVersionsDir);

    std::string currentVersionDir = versionControlDir + CURRENT_PATH;
    makeDirIfNotExists(currentVersionDir);
}

void VersionControl::checkClientFiles(std::string clientDir)
{
    if(!fileExists(clientDir))
        THROW_RUNTIME_ERROR("Could not find Tibia client directory '" + clientDir + "'.");

    std::string tibiaExecutable = clientDir + CLIENT_MODULE_PATH;
    if(!fileExists(tibiaExecutable))
        THROW_RUNTIME_ERROR("Could not find Tibia executable at '" + tibiaExecutable + "'.");

    std::string tibiaPackageInfo = clientDir + TIBIA_PACKAGE_INFO_PATH;
    if(!fileExists(tibiaPackageInfo))
        THROW_RUNTIME_ERROR("Could not find Tibia package info at '" + tibiaPackageInfo + "'.");
}


void VersionControl::checkout(std::string versionControlDir)
{
    prepareFiles(versionControlDir);
    std::string currentVersionPath = versionControlDir + CURRENT_VERSION_PATH;

    if(fileExists(currentVersionPath))
        THROW_RUNTIME_ERROR("Already checked out.");

    mVersionFile.toFile(currentVersionPath);
}


bool VersionControl::updateVersion(std::string clientDir, std::string versionControlDir)
{
    std::string currentVersionPath = versionControlDir + CURRENT_VERSION_PATH;
    if(!fileExists(currentVersionPath))
    {
        mVersionFile.version = getTibiaVersion(clientDir);
        return true;
    }

    VersionFile versionFile = VersionFile::fromFile(currentVersionPath);
    std::string tibiaVersion = getTibiaVersion(clientDir);
    if(versionFile.version == tibiaVersion)
        return false;

    mVersionFile.version = tibiaVersion;

    std::string currentVersionDir = sb::utility::file::getPath(currentVersionPath);
    std::string oldVersionsDir = versionControlDir + OLD_PATH;
    backup(oldVersionsDir, currentVersionDir);
    recursiveRemove(currentVersionDir, false);
    return true;
}

void VersionControl::backup(std::string oldVersionsDir, std::string currentVersionDir)
{
    std::string backupPath = recursiveCopy(currentVersionDir, oldVersionsDir);
    renameFile(backupPath, sb::utility::getDateTime());
}

std::string VersionControl::getTibiaVersion(std::string clientDir)
{
    std::string packageInfoFilePath = clientDir + TIBIA_PACKAGE_INFO_PATH;

    QFile file(QString::fromStdString(packageInfoFilePath));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = file.readAll();
    file.close();
    QJsonDocument package = QJsonDocument::fromJson(content.toUtf8());

    if(package.isNull())
        THROW_RUNTIME_ERROR("Failed to read Tibia package at '" + packageInfoFilePath + "'.");

    QJsonObject obj = package.object();
    return obj["version"].toString().toStdString();
}

void VersionControl::VersionFile::toFile(std::string path) const
{
    std::ofstream file(path);
    file    << version << std::endl;
    file.close();
}

VersionControl::VersionFile VersionControl::VersionFile::fromFile(std::string path)
{
    VersionFile versionFile;
    std::ifstream file(path);
    file >> versionFile.version;
    return versionFile;
}
