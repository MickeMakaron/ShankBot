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
#ifndef GRAPHICS_LAYER_VERSION_CONTROL_HPP
#define GRAPHICS_LAYER_VERSION_CONTROL_HPP

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    class VersionControl
    {
        private:
            struct VersionFile
            {
                void toFile(std::string path) const;
                static VersionFile fromFile(std::string);

                std::string version;
            };

        public:
            static bool hasNewVersion(std::string clientDir, std::string versionControlDir);
            static std::string getPath(std::string versionControlDir);
            static void checkout(std::string versionControlDir);

        private:
            static void prepareFiles(std::string versionControlDir);
            static void checkClientFiles(std::string clientDir);
            static bool updateVersion(std::string clientDir, std::string versionControlDir);
            static std::string getTibiaVersion(std::string clientDir);
            static void backup(std::string oldVersionsDir, std::string currentVersionDir);

        private:
            static const std::string OLD_PATH;
            static const std::string CURRENT_PATH;
            static const std::string CURRENT_VERSION_PATH;
            static const std::string CLIENT_MODULE_NAME;
            static const std::string CLIENT_MODULE_PATH;
            static const std::string TIBIA_PACKAGE_PATH;
            static const std::string BIN_PATH;
            static const std::string ASSETS_PATH;
            static const std::string TIBIA_PACKAGE_INFO_PATH;

            static VersionFile mVersionFile;
    };
}


#endif // GRAPHICS_LAYER_VERSION_CONTROL_HPP
