#ifndef GRAPHICS_LAYER_SHANK_BOT
#define GRAPHICS_LAYER_SHANK_BOT


///////////////////////////////////
// Internal ShankBot headers
#include "TibiaClient.hpp"
#include "TibiaContext.hpp"
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
            void initializeData(std::string clientDir, std::string versionControlDir);

        private:
            std::unique_ptr<TibiaClient> mTibiaClient;
            std::unique_ptr<TibiaContext> mTibiaContext;
    };
}


#endif // GRAPHICS_LAYER_SHANK_BOT
