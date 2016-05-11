#ifndef GRAPHICS_LAYER_SHANK_BOT
#define GRAPHICS_LAYER_SHANK_BOT


///////////////////////////////////
// Internal ShankBot headers
#include "TibiaClient.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    class ShankBot
    {
        public:
            explicit ShankBot(std::string clientDir, std::string versionControlDir);

            void run();

        private:
            void initializeData(std::string clientDir, std::string versionControlDir) const;

        private:
            std::unique_ptr<TibiaClient> mTibiaClient;
    };
}


#endif // GRAPHICS_LAYER_SHANK_BOT
