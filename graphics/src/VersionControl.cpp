///////////////////////////////////
// Internal ShankBot headers
#include "VersionControl.hpp"
#include "fileUtility.hpp"
#include "utility.hpp"
#include "TibiaSpr.hpp"
#include "TibiaDat.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
///////////////////////////////////


using namespace GraphicsLayer;

std::string VersionControl::OLD_PATH = "/old";
std::string VersionControl::CURRENT_PATH = "/current";
std::string VersionControl::CURRENT_VERSION_PATH = VersionControl::CURRENT_PATH + "/version";

std::string VersionControl::getPath(std::string versionControlDir)
{
    return versionControlDir + CURRENT_PATH;
}

bool VersionControl::hasNewVersion(std::string clientDir, std::string versionControlDir)
{
    prepareFiles(clientDir, versionControlDir);
    return updateVersion(clientDir, versionControlDir);
}

void VersionControl::prepareFiles(std::string clientDir, std::string versionControlDir)
{
    if(!fileExists(clientDir))
        throw std::runtime_error("Could not find Tibia client directory '" + clientDir + "'.");

    std::string tibiaExecutable = clientDir + "/Tibia";
    if(!fileExists(tibiaExecutable))
        throw std::runtime_error("Could not find Tibia executable at '" + tibiaExecutable + "'.");

    std::string tibiaSpr = clientDir + "/Tibia.spr";
    if(!fileExists(tibiaSpr))
        throw std::runtime_error("Could not find Tibia.spr at '" + tibiaExecutable + "'.");

    std::string tibiaDat = clientDir + "/Tibia.dat";
    if(!fileExists(tibiaDat))
        throw std::runtime_error("Could not find Tibia.dat at '" + tibiaExecutable + "'.");

    std::string tibiaPic = clientDir + "/Tibia.pic";
    if(!fileExists(tibiaPic))
        throw std::runtime_error("Could not find Tibia.pic at '" + tibiaExecutable + "'.");


    makeDirIfNotExists(versionControlDir);


    std::string oldVersionsDir = versionControlDir + OLD_PATH;
    makeDirIfNotExists(oldVersionsDir);

    std::string currentVersionDir = versionControlDir + CURRENT_PATH;
    makeDirIfNotExists(currentVersionDir);
}


bool VersionControl::updateVersion(std::string clientDir, std::string versionControlDir)
{
    std::string currentVersionPath = versionControlDir + CURRENT_VERSION_PATH;
    if(!fileExists(currentVersionPath))
    {
        writeVersionFile(clientDir, currentVersionPath);
        return true;
    }


    unsigned int sprVersion;
    unsigned int datVersion;
    unsigned int picVersion;
    unsigned int tibiaSprVersion;
    unsigned int tibiaDatVersion;
    unsigned int tibiaPicVersion;

    getVersion(currentVersionPath, sprVersion, datVersion, picVersion);
    getTibiaVersion(clientDir, tibiaSprVersion, tibiaDatVersion, tibiaPicVersion);

    if(sprVersion != tibiaSprVersion || datVersion != tibiaDatVersion || picVersion != tibiaPicVersion)
    {
        std::string currentVersionDir = GraphicsLayer::getPath(currentVersionPath);
        std::string oldVersionsDir = versionControlDir + OLD_PATH;
        backup(oldVersionsDir, currentVersionDir);
        recursiveRemove(currentVersionDir, false);
        writeVersionFile(currentVersionPath, tibiaSprVersion, tibiaDatVersion, tibiaPicVersion);

        return true;
    }
    else
        return false;
}

void VersionControl::backup(std::string oldVersionsDir, std::string currentVersionDir)
{
    std::string backupPath = recursiveCopy(currentVersionDir, oldVersionsDir);
    renameFile(backupPath, getDateTime());
}

void VersionControl::getVersion(std::string path, unsigned int& spr, unsigned int& dat, unsigned int& pic)
{
    std::ifstream file(path);
    file    >> spr
            >> dat
            >> pic;

    file.close();
}

void VersionControl::getTibiaVersion(std::string clientDir, unsigned int& spr, unsigned int& dat, unsigned int& pic)
{
    spr = TibiaSpr::getVersion(clientDir + "/Tibia.spr");
    dat = TibiaDat::getVersion(clientDir + "/Tibia.dat");
    pic = 0;
}

void VersionControl::writeVersionFile(std::string path, unsigned int spr, unsigned int dat, unsigned int pic)
{
    std::ofstream file(path);
    file    << spr << std::endl
            << dat << std::endl
            << pic << std::endl;

    file.close();
}

void VersionControl::writeVersionFile(std::string clientDir, std::string path)
{
    unsigned int sprVersion = TibiaSpr::getVersion(clientDir + "/Tibia.spr");
    unsigned int datVersion = TibiaDat::getVersion(clientDir + "/Tibia.dat");
    unsigned int picVersion = 0;

    std::ofstream file(path);
    file    << sprVersion << std::endl
            << datVersion << std::endl
            << picVersion << std::endl;

    file.close();
}
