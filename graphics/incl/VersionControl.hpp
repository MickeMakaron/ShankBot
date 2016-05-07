#ifndef GRAPHICS_LAYER_VERSION_CONTROL
#define GRAPHICS_LAYER_VERSION_CONTROL

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    class VersionControl
    {
        public:
            static bool hasNewVersion(std::string clientDir, std::string versionControlDir);
            static std::string getPath(std::string versionControlDir);

        private:
            static void prepareFiles(std::string clientDir, std::string versionControlDir);
            static void writeVersionFile(std::string path, unsigned int spr, unsigned int dat, unsigned int pic);
            static void writeVersionFile(std::string clientDir, std::string path);
            static bool updateVersion(std::string clientDir, std::string versionControlDir);
            static void getVersion(std::string path, unsigned int& spr, unsigned int& dat, unsigned int& pic);
            static void getTibiaVersion(std::string clientDir, unsigned int& spr, unsigned int& dat, unsigned int& pic);
            static void backup(std::string oldVersionsDir, std::string currentVersionDir);

        private:
            static std::string OLD_PATH;
            static std::string CURRENT_PATH;
            static std::string CURRENT_VERSION_PATH;
    };
}


#endif // GRAPHICS_LAYER_VERSION_CONTROL
