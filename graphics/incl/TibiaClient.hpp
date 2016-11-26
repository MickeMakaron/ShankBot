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
#ifndef GRAPHICS_LAYER_TIBIA_CLIENT_HPP
#define GRAPHICS_LAYER_TIBIA_CLIENT_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "TibiaContext.hpp"
#include "Scene.hpp"
#include "Input.hpp"
#include "Gui.hpp"
#include "OutfitResolver.hpp"
#include "GraphicsMonitorReader.hpp"
#include "Message.hpp"
#include "MiniMap.hpp"
#include "ConnectionManager.hpp"

namespace GraphicsLayer
{
namespace SharedMemoryProtocol
{
    struct SharedMemorySegment;
}
}
///////////////////////////////////

///////////////////////////////////
// Xlib
typedef struct _XDisplay Display;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <vector>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    class TibiaClient
    {
        public:
            explicit TibiaClient(std::string clientDirectory, const TibiaContext& context);
            ~TibiaClient();

            void update();
            bool isAlive() const;
            void close();

            const Input& getInput() const;

            SharedMemoryProtocol::SharedMemorySegment* getSharedMemory() const; // Should not be needed unless for debugging.

        private:
            void walkLeft() const;// TEMPORARY

            void initializeInput();
            void prepareSharedMemory(std::string& sharedMemoryName);
            void mapSharedMemory();
            char** prepareEnvironment() const;
            void deleteEnvironment(char** environment) const;
            void launchClient(char** environment, std::string clientDirectory, std::string sharedMemoryName);
            void waitForWindow() const;
            void launchMessageListener();
            void handleMessages();
            size_t handleLoginRequest(const char* data, size_t size);
            size_t handleGoRequest(const char* data, size_t size);
            size_t handleFrameRequest(const char* data, size_t size);
            void sendMessage(const sb::messaging::Message& m) const;
            size_t handleAttackRequest(const char* data, size_t size);

            void registerMessageCallbacks();



        private:
            const TibiaContext& mContext;
            HANDLE mShmHandle = NULL;
            SharedMemoryProtocol::SharedMemorySegment* mShm = nullptr;
            FrameParser mFrameParser;
            Display* mXDisplay;
            Scene mScene;
            std::unique_ptr<Input> mInput;
            Gui mGui;
            HANDLE mClientProcessHandle = NULL;
            OutfitResolver mOutfitResolver;
            std::unique_ptr<GraphicsMonitorReader> mGraphicsMonitorReader;
//            MessageProtocol::Queue mMessageQueue;
            HANDLE mMessageListenerThread = NULL;
            std::unique_ptr<MiniMap> mMiniMap;
//            std::vector<std::unique_ptr<Connection>> mConnections;
//            std::vector<HANDLE> mEvents;
            sb::messaging::ConnectionManager mConnectionManager;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_CLIENT_HPP
