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

#if defined(_WIN32)
#define WINVER _WIN32_WINNT_WS03
#define _WIN32_WINNT _WIN32_WINNT_WS03
#include <windows.h>
#else
#include <sys/mman.h>
#endif // defined

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/TibiaClient.hpp"
#include "injection/SharedMemoryProtocol.hpp"
#include "utility/utility.hpp"
#include "monitor/Constants.hpp"

#include "monitor/TextBuilder.hpp"
#include "monitor/OutfitResolver.hpp"
#include "messaging/GoRequest.hpp"
#include "messaging/Response.hpp"
#include "messaging/AttackRequest.hpp"
#include "messaging/FrameRequest.hpp"
#include "messaging/FrameResponse.hpp"
#include "messaging/LoginRequest.hpp"
#include "messaging/LoginResponse.hpp"
#include "messaging/ObjectResponse.hpp"
using namespace GraphicsLayer;
using namespace SharedMemoryProtocol;
using namespace sb::tibiaassets;
///////////////////////////////////





///////////////////////////////////
// Windows
#ifndef PROCESS_QUERY_LIMITED_INFORMATION
    #define PROCESS_QUERY_LIMITED_INFORMATION (0x1000)
#endif
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <sstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <chrono>
///////////////////////////////////

///////////////////////////////////
// STD C
#include <fcntl.h>
#include <unistd.h>
///////////////////////////////////

TibiaClient::TibiaClient(std::string clientDirectory, const TibiaContext& context)
: mContext(context)
, mFrameParser(context)
, mScene(context)
, mGui(context)
, mOutfitResolver(context)
{


    std::string sharedMemoryName;
    prepareSharedMemory(sharedMemoryName);
    mGraphicsMonitorReader.reset(new GraphicsMonitorReader(*this, context, mShm));

    char** tibiaEnv = prepareEnvironment();
    launchClient(tibiaEnv, clientDirectory, sharedMemoryName);
    deleteEnvironment(tibiaEnv);
    waitForWindow();
    initializeInput();

    mMiniMap.reset(new MiniMap(*mInput, *mGraphicsMonitorReader));

    mConnectionManager.addConnection(false);

    mShm->dataFilters = DataFilter::Standard | DataFilter::ScreenPixels;
}

TibiaClient::~TibiaClient()
{
    close();
}

void TibiaClient::close()
{
    if(mClientProcessHandle != NULL)
    {
        if(isAlive())
        {
            if(TerminateProcess(mClientProcessHandle, 0) == 0)
            {
                std::stringstream sstream;
                sstream << "Failed to terminate Tibia client. Error code: " << GetLastError() << std::endl;
                THROW_RUNTIME_ERROR(sstream.str());
            }
        }

        CloseHandle(mClientProcessHandle);
        mClientProcessHandle = NULL;
    }

    if(mShmHandle != NULL)
    {
        CloseHandle(mShm->window);
        CloseHandle(mShm->parentProcessHandle);
        CloseHandle(mShm->semRead);
        CloseHandle(mShm->semWrite);

        UnmapViewOfFile(mShm);
        CloseHandle(mShmHandle);

        mShm = nullptr;
        mShmHandle = NULL;
    }
}

void TibiaClient::waitForWindow() const
{
    std::cout << "Waiting for window to open... ";
    while(mShm->window == NULL)
        Sleep(50);

    std::cout << "OK" << std::endl;
}

void TibiaClient::initializeInput()
{
    mInput = std::make_unique<Input>(mShm->window);
}

bool TibiaClient::isAlive() const
{
    unsigned long code;
    if(GetExitCodeProcess(mClientProcessHandle, &code) == 0)
    {
        std::stringstream sstream;
        sstream << "Could not get Tibia process status. Error code: " << GetLastError() << std::endl;
        THROW_RUNTIME_ERROR(sstream.str());
    }

    return (code == STILL_ACTIVE);
}

bool doStart = false;
//WalkState* moveState = nullptr;
bool isWalking = false;
size_t updateCount = 0;
bool isWaitingForGreenText = false;

std::string loginStrings[2] = {"FrixaRael", "choochoo1"};
size_t loginStringIndex = 0;
bool isLoggingIn = false;
bool doDrag = true;






bool doGetContextMenu = true;

bool bubbabubba = true;
size_t clearTextCount = 0;
bool doGameStartup = true;
#include "utility/file.hpp"


std::shared_ptr<sb::messaging::Message> TibiaClient::handleGoRequest(const char* data, size_t size)
{
    using namespace sb::messaging;
    GoRequest g;
    size_t numBytesRead = g.fromBinary(data, size);
    if(numBytesRead == 0)
    {
        std::cout << "Failed to handle go request. " << std::endl;
        return std::make_shared<Response>();
    }

    if(mGui.getState() != Gui::State::GAME)
    {
        std::cout << "Cannot go. Not in game." << std::endl;
        return std::make_shared<Response>();
    }

    std::cout << "Handling go request: " << g.getX() << "x" << g.getY() << std::endl;
    auto response = std::make_shared<Response>(sb::RequestResult::SUCCESS);

    Frame frame = mGraphicsMonitorReader->getNewFrame();
    mMiniMap->update(frame);

    unsigned int startX = mMiniMap->getX();
    unsigned int startY = mMiniMap->getY();

    unsigned int destinationX = startX + g.getX();
    unsigned int destinationY = startY + g.getY();
    unsigned short screenX;
    unsigned short screenY;
    mMiniMap->goTo(destinationX, destinationY);
    std::cout << "Going to " << destinationX << "x" << destinationY << std::endl;

    size_t numFramesWithoutMovement = 0;
    unsigned int prevX = mMiniMap->getX();
    unsigned int prevY = mMiniMap->getY();
    while(mMiniMap->getX() != destinationX || mMiniMap->getY() != destinationY)
    {
        frame = mGraphicsMonitorReader->getNewFrame();
        for(const TextDraw& d : *frame.textDraws)
        {
            TextBuilder builder(d, frame.width, frame.height);
            typedef Text::Type T;
            if(builder.getTextType() == T::INFO_TEXT)
            {
                for(const Text& t : builder.getText())
                {
                    if(t.string == "Sorry, not possible." || t.string == "There is no way." || t.string == "Destination is out of range.")
                    {
                        std::cout << "Cannot go to target. Aborting." << std::endl;
                        return response;
                    }
                }
            }
        }
        mMiniMap->update(frame);
        if(mMiniMap->getX() == prevX && mMiniMap->getY() == prevY)
            numFramesWithoutMovement++;
        else
            numFramesWithoutMovement = 0;

        prevX = mMiniMap->getX();
        prevY = mMiniMap->getY();

        if(numFramesWithoutMovement > 20)
        {
            std::cout << "Not moving for some reason. Aborting." << std::endl;
            return response;
        }
        std::cout << "Current delta: " << int(destinationX - mMiniMap->getX()) << "x" << int(destinationY - mMiniMap->getY()) << std::endl;
    }

//    mInput->sendKey(VK_LEFT);
//
//    do
//    {
//        frame = mGraphicsMonitorReader->getNewFrame();
//        mMiniMap->update(frame);
//
////    } while(map.getX() - startX != g.getX() || map.getY() - startY != g.getY());
//    } while(mMiniMap->getX() == startX);

    std::cout << "Done walkin'!" << std::endl;



    return response;
}

std::shared_ptr<sb::messaging::Message> TibiaClient::handleLoginRequest(const char* data, size_t size)
{
    using namespace sb::messaging;

    LoginRequest l;
    size_t numBytesRead = l.fromBinary(data, size);
    if(numBytesRead == 0)
    {
        std::cout << "Failed to handle login request." << std::endl;
        return std::make_shared<LoginResponse>();
    }



    std::cout << "Handling login request: " << std::endl;
    std::cout << "\tAccount name: " << l.getAccountName() << std::endl;
    std::cout << "\tPassword: " << l.getPassword() << std::endl;

    mGui.update(mGraphicsMonitorReader->getNewFrame());
    if(mGui.getState() != Gui::State::MAIN_MENU)
    {
        std::cout << "Cannot login. Not in main menu." << std::endl;
        return std::make_shared<LoginResponse>();
    }

    for(const Gui::Button& b : mGui.getButtons())
    {
        if(b.text == "Login")
        {
            mInput->sendString(l.getAccountName());
            mInput->sendKey(VK_TAB);
            mGraphicsMonitorReader->getNewFrame();
            mGraphicsMonitorReader->getNewFrame();
            mInput->sendString(l.getPassword());
            mInput->sendKey(VK_RETURN);

            OVERLAPPED* overlapped = new OVERLAPPED();
            *overlapped = {0};
            for(size_t i = 0; i < 10; i++)
            {
                Frame frame = mGraphicsMonitorReader->getNewFrame();
                for(auto it = frame.textDraws->begin(); it != frame.textDraws->end(); it++)
                {
                    const TextDraw& text = *it;
                    TextBuilder builder(text, frame.width, frame.height);
                    for(const Text& t : builder.getText())
                    {
                        if(t.string == "Select Character")
                        {
                            auto response = std::make_shared<LoginResponse>(sb::RequestResult::SUCCESS);

                            it++;
                            while(it != frame.textDraws->end())
                            {
                                std::list<Text> character = TextBuilder(*it, frame.width, frame.height).getText();
                                if(character.size() != 2)
                                    break;
                                if(character.front().string == "Account Status:")
                                    break;

                                response->addCharacter(character.front().string, character.back().string);

                                it++;
                            }
                            return response;
                        }
                    }
                }
            }

            return std::make_shared<LoginResponse>();
        }
    }

    std::cout << "Failed to login. Login screen not visible." << std::endl;
    return std::make_shared<LoginResponse>();
}


std::shared_ptr<sb::messaging::Message> TibiaClient::handleFrameRequest(const char* data, size_t size)
{
    using namespace sb::messaging;

    FrameRequest request;
    size_t numBytesRead = request.fromBinary(data, size);
    if(numBytesRead == 0 )
    {
        std::cout << "Failed to handle frame request." << std::endl;
        return std::make_shared<FrameResponse>();
    }

    std::cout << "Handling frame request." << std::endl;

    GraphicsLayer::Frame frame = mGraphicsMonitorReader->getNewFrame();
    using namespace sb::messaging;;
    sb::Frame f;

    mGui.update(frame);
    if(mGui.getState() != Gui::State::GAME)
    {
        std::cout << "Cannot get frame. Not in game." << std::endl;
        return std::make_shared<FrameResponse>();
    }
    mScene.update(frame);
    mMiniMap->update(frame);
    f.miniMap.x = mMiniMap->getX();
    f.miniMap.y = mMiniMap->getY();
    f.miniMap.level = mMiniMap->getLevel();

    const unsigned int GLOBAL_X_START = f.miniMap.x - sb::Scene::WIDTH / 2;
    const unsigned int GLOBAL_Y_START = f.miniMap.y - sb::Scene::HEIGHT / 2;
    for(size_t x = 0; x < sb::Scene::WIDTH; x++)
    {
        for(size_t y = 0; y < sb::Scene::HEIGHT; y++)
        {
            f.miniMap.colors[x][y] = mMiniMap->getColor(GLOBAL_X_START + x, GLOBAL_Y_START + y);

            const Scene::Tile& t = mScene.getTile(x, y);
            char currLayer = -1;
            std::vector<unsigned int>& objects = f.scene.objects[x][y];
            for(const Scene::Object& o : t.knownLayerObjects)
            {
                if(currLayer != o.layer)
                {
                    currLayer = o.layer;
//                    sb::Object obj;
//                    obj.type = o.object->type;
//                    obj.id = o.object->id;
//                    objects.push_back(obj);
                    objects.push_back(o.object);
                }
            }
        }
    }

    mOutfitResolver.resolve(mScene, frame, *mGraphicsMonitorReader);

    f.scene.npcs.reserve(mOutfitResolver.getNpcs().size());
    for(const OutfitResolver::Npc& npc : mOutfitResolver.getNpcs())
    {
        f.scene.npcs.emplace_back();
        sb::Npc& n = f.scene.npcs.back();
        n.id = npc.object != -1 ? mContext.getObjects()[npc.object].id : 0;
        n.name = npc.name;
        n.x = npc.x;
        n.y = npc.y;
    }
    f.scene.creatures.reserve(mOutfitResolver.getCreatures().size());
    for(const OutfitResolver::Creature& creature : mOutfitResolver.getCreatures())
    {
        f.scene.creatures.emplace_back();
        sb::Creature& c = f.scene.creatures.back();
        c.id = creature.object != -1 ? mContext.getObjects()[creature.object].id : 0;
        c.name = creature.name;
        c.x = creature.x;
        c.y = creature.y;
        c.hp = creature.hp;
    }
    f.scene.players.reserve(mOutfitResolver.getPlayers().size());
    for(const OutfitResolver::Player& player : mOutfitResolver.getPlayers())
    {
        f.scene.players.emplace_back();
        sb::Player& p = f.scene.players.back();
        p.name = player.name;
        p.x = player.x;
        p.y = player.y;
        p.hp = player.hp;
    }

    std::cout << "Frame request handled successfully." << std::endl;
    auto response = std::make_shared<FrameResponse>(sb::RequestResult::SUCCESS);
    response->set(f);
    return response;
}


std::shared_ptr<sb::messaging::Message> TibiaClient::handleAttackRequest(const char* data, size_t size)
{
    using namespace sb::messaging;

    AttackRequest request;
    size_t numBytesRead = request.fromBinary(data, size);
    if(numBytesRead == 0)
    {
        std::cout << "Failed to handle attack request." << std::endl;
        return std::make_shared<Response>(sb::RequestResult::FAIL);
    }

    std::cout << "Handling attack request." << std::endl;

    mScene.update(mGraphicsMonitorReader->getNewFrame());

    size_t minD = -1;
    const Scene::Object* closestOutfit = nullptr;
    mScene.forEach([&](const Scene::Object& o)
    {
        int dX = request.getX() - o.screenX;
        int dY = request.getY() - o.screenY;
        size_t d = dX * dX + dY * dY;
        if(d < minD)
        {
            minD = d;
            closestOutfit = &o;
        }
    });

    std::shared_ptr<Response> response;
    if(closestOutfit)
    {
        mInput->sendMouseClick(VK_LBUTTON, closestOutfit->screenX + mScene.getTileWidth() / 2.f, closestOutfit->screenY + mScene.getTileHeight() / 2.f);
        response.reset(new Response(sb::RequestResult::SUCCESS));
    }
    else
        response.reset(new Response(sb::RequestResult::FAIL));

    return response;
}

std::shared_ptr<sb::messaging::Message> TibiaClient::handleObjectRequest(const char* data, size_t size)
{
    using namespace sb::messaging;
    std::cout << "Handling object request." << std::endl;
    auto r = std::make_shared<ObjectResponse>(sb::RequestResult::SUCCESS);
    r->set(mContext.getObjects());
    return r;
}

#include "QtGui/QImage"
void TibiaClient::update()
{
    using namespace sb::messaging;

    typedef ConnectionManager::Event E;
    typedef ConnectionManager::ConnectionId C;

//    static const std::chrono::milliseconds MS_PER_FRAME(500);
    static const std::chrono::milliseconds MS_PER_FRAME(25);
    static std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();

    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::milliseconds timeSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);

    if(timeSinceLastFrame > MS_PER_FRAME)
    {
        Frame frame = mGraphicsMonitorReader->getNewFrame();
        if(frame.screenPixels != nullptr)
        {
            static size_t screenPixelsCount = 0;

            const RawImage& i = *frame.screenPixels;
            QImage img(i.pixels.data(), i.width, i.height, QImage::Format_RGB888);
            img = img.mirrored(false, true);
            img.save(QString::fromStdString("frameDumps/p" + std::to_string(screenPixelsCount) + ".png"));

            screenPixelsCount++;
        }
        mMiniMap->update(frame);
        lastFrameTime = std::chrono::steady_clock::now();

        mGui.update(frame);
        if(mGui.getState() == Gui::State::GAME)
        {
            mScene.update(frame);
            mOutfitResolver.resolve(mScene, frame, *mGraphicsMonitorReader);

            std::cout << "Cap: " << mGui.getCap() << std::endl;
            std::cout << "Soul: " << mGui.getSoul() << std::endl;
            std::cout << "Hp: " << mGui.getHp() << std::endl;
            std::cout << "Mana: " << mGui.getMana() << std::endl;
            std::cout << "Hp percent: " << mGui.getHpLevel() * 100.f << "%" << std::endl;
            std::cout << "Mana percent: " << mGui.getManaLevel() * 100.f << "%" << std::endl;
            std::cout << "Level: " << mGui.getLevel() << std::endl;
            std::cout << "Exp: " << mGui.getExperience() << std::endl;
            std::cout << "Xp gain rate: " << mGui.getXpGainRate() << std::endl;
            std::cout << "Speed: " << mGui.getSpeed() << std::endl;
            std::cout << "Food: " << mGui.getFoodMinutes() << std::endl;
            std::cout << "Stamina: " << mGui.getStaminaMinutes() << std::endl;
            std::cout << "Offline training: " << mGui.getOfflineTrainingMinutes() << std::endl;
            std::cout << "Magic lvl: " << mGui.getMagicLevel() << std::endl;
            std::cout << "Fist: " << mGui.getFistFighting() << std::endl;
            std::cout << "Club: " << mGui.getClubFighting() << std::endl;
            std::cout << "Sword: " << mGui.getSwordFighting() << std::endl;
            std::cout << "Axe: " << mGui.getAxeFighting() << std::endl;
            std::cout << "Distance: " << mGui.getDistanceFighting() << std::endl;
            std::cout << "Shielding: " << mGui.getShielding() << std::endl;
            std::cout << "Fishing: " << mGui.getFishing() << std::endl;
            std::cout << "Crit chance: " << mGui.getCritChance() << "%" << std::endl;
            std::cout << "Crit dmg: " << mGui.getCritDamage() << std::endl;
            std::cout << "Hp leech chance: " << mGui.getHpLeechChance() << "%" << std::endl;
            std::cout << "Hp leech amount: " << mGui.getHpLeechAmount() << std::endl;
            std::cout << "Mana leech chance: " << mGui.getManaLeechChance() << "%" << std::endl;
            std::cout << "Mana leech amoumt: " << mGui.getManaLeechAmount() << std::endl;


            std::cout << "EQUIPMENT: " << std::endl;
            for(const auto& pair : mGui.getEquipment())
            {
                const Object& o = mContext.getObjects()[pair.second];
                std::cout << "\t" << (int)pair.first << ": " << o.id;
                if(o.itemInfo.hasMarketInfo)
                    std::cout << " (" << o.itemInfo.marketInfo.name << ")";
                std::cout << std::endl;
            }

            std::cout << "CONTAINERS: " << std::endl;
            for(const Gui::Container& c : mGui.getContainers())
            {
                std::cout << "\t" << c.items.size() << " items: " << std::endl;
                for(const auto& pair : c.items)
                {
                    const Object& o = mContext.getObjects()[*pair.second.begin()];
                    std::cout << "\t" << (int)pair.first << ": " << o.id;
                    if(o.itemInfo.hasMarketInfo)
                        std::cout << " (" << o.itemInfo.marketInfo.name << ")";
                    std::cout << std::endl;
                }
            }

        }
        return;
    }

    std::chrono::milliseconds timeRemaining = MS_PER_FRAME - timeSinceLastFrame;

    E e;
    C c = mConnectionManager.poll(e, timeRemaining.count());
    switch(e)
    {
        case E::CONNECT:
            mConnectionManager.getConnection(c).read();
            mConnectionManager.addConnection(false);
            std::cout << "Connected" << std::endl;
            break;

        case E::DROP:
            mConnectionManager.removeConnection(c);
            std::cout << "Dropped" << std::endl;
            break;

        case E::READ:
        {
            Connection& connection = mConnectionManager.getConnection(c);
            std::list<std::vector<char>> messages = connection.getMessages();
            std::cout << "Got " << messages.size() << " message(s)." << std::endl;
            assert(!messages.empty());
            std::list<std::shared_ptr<sb::messaging::Message>> responses;
            for(const std::vector<char>& message : messages)
            {
                sb::messaging::Message::Type t = sb::messaging::Message::readMessageType(message.data(), message.size());
                typedef sb::messaging::Message::Type T;
                std::shared_ptr<sb::messaging::Message> response;
                switch(t)
                {
                    case T::LOGIN_REQUEST:
                        response = handleLoginRequest(message.data(), message.size());
                        break;

                    case T::GO_REQUEST:
                        response = handleGoRequest(message.data(), message.size());
                        break;

                    case T::FRAME_REQUEST:
                        response = handleFrameRequest(message.data(), message.size());
                        break;

                    case T::ATTACK_REQUEST:
                        response = handleAttackRequest(message.data(), message.size());
                        break;

                    case T::OBJECT_REQUEST:
                        response = handleObjectRequest(message.data(), message.size());
                        break;

                    default:
                        std::cout << "Invalid message type: " << (int)t << std::endl;
                }
                if(response != nullptr)
                    responses.push_back(response);
            }

            if(responses.empty())
                connection.read();
            else
            {
                Frame frame = mGraphicsMonitorReader->getNewFrame();
                mMiniMap->update(frame);
                lastFrameTime = std::chrono::steady_clock::now();
                connection.sendMessages(responses);
            }
            break;
        }

        case E::WRITE:
        {
            std::cout << "Sent message." << std::endl;
            Connection& connection = mConnectionManager.getConnection(c);
            connection.read();
            break;
        }

        case E::INVALID_STATE:
            THROW_RUNTIME_ERROR("Connection is in an invalid state.");

        case E::TIME_OUT:
            break;

        default:
            THROW_RUNTIME_ERROR(sb::utility::stringify("Unimplemented event: ", (int)e));
    }

    return;

//    mConnectionManager.poll(500);
//    DWORD result = WaitForMultipleObjects(mEvents.size(), mEvents.data(), FALSE, 500);
//
//    if(result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + mEvents.size())
//    {
//        size_t i = result - WAIT_OBJECT_0;
//        assert(i < mEvents.size());
//        assert(i < mConnections.size());
//
//        Connection& connection = *mConnections[i];
//        typedef Connection::State S;
//        S previousState = connection.getState();
//        connection.update();
//
//
//        if(connection.getState() == S::DROPPED)
//        {
//            std::cout << "Connection is dropped." << std::endl;
//            mConnections.erase(mConnections.begin() + i);
//            mEvents.erase(mEvents.begin() + i);
//        }
//        else
//        {
//            assert(connection.getState() == S::IDLE);
//            switch(previousState)
//            {
//                case S::READING:
//                {
//                    std::list<std::vector<char>> messages = connection.getMessages();
//                    assert(!messages.empty());
//                    std::cout << "Got " << messages.size() << " message(s)." << std::endl;
//                    std::list<std::unique_ptr<Message>> responses;
//                    for(const std::vector<char>& message : messages)
//                    {
//                        Message::Type t = Message::readMessageType(message.data(), message.size());
//                        typedef Message::Type T;
//                        switch(t)
//                        {
//                            case T::LOGIN_REQUEST:
////                                handleLoginRequest(message.data(), message.size());
////                                break;
//
//                            case T::GO_REQUEST:
////                                handleGoRequest(message.data(), message.size());
////                                break;
//
//                            case T::FRAME_REQUEST:
////                                handleFrameRequest(message.data(), message.size());
////                                break;
//
//                            case T::ATTACK_REQUEST:
////                                handleAttackRequest(message.data(), message.size());
//                                responses.push_back(std::make_unique<Response>(sb::RequestResult::SUCCESS));
//                                break;
//
//                            default:
//                                std::cout << "Invalid message type: " << (int)t << std::endl;
//                        }
//                    }
//
//                    if(responses.empty())
//                        connection.read();
//                    else
//                        connection.sendMessages(responses);
//                    break;
//                }
//
//
//                case S::WRITING:
//                {
//                    std::cout << "Sent message." << std::endl;
//                    connection.read();
//                    break;
//                }
//
//
//                case S::CONNECTING:
//                    std::cout << "Connected." << std::endl;
//                    createNewConnection();
//                    connection.read();
//                    break;
//
//                case S::IDLE:
//                    std::cout << "This shouldn't happen?" << std::endl;
//                    break;
//
//                case S::DROPPED:
//                    std::cout << "Connection is dropped." << std::endl;
//                    mConnections.erase(mConnections.begin() + i);
//                    mEvents.erase(mEvents.begin() + i);
//                    break;
//
//                default:
//                    mConnections.erase(mConnections.begin() + i);
//                    mEvents.erase(mEvents.begin() + i);
//                    connection.drop();
//                    break;
//            }
//        }
//    }
//    else if(result == WAIT_TIMEOUT)
//    {
//
//    }
//    else
//        THROW_RUNTIME_ERROR(stringify("Unexpected result when waiting for message connection events: ", result, ". Error: ", GetLastError()));



//    handleMessages();

    GraphicsLayer::Frame frame = mGraphicsMonitorReader->getNewFrame();
//
//    for(const TextDraw& d : *frame.textDraws)
//    {
//        TextBuilder builder(d, frame.width, frame.height);
//        std::cout << (int)builder.getTextType() << std::endl;
//        for(const Text& t : builder.getText())
//        {
//            std::cout << "\t\"" << t.string << "\"" << "\t" << t.localX << "x" << t.localY << " ->" << t.x << "x" << t.y << std::endl;
//        }
//    }


    std::cout << "new frame" << std::endl;
    mGui.update(frame);

    if(mGui.getState() == Gui::State::GAME)
    {
        bool isCombatOffensiveDown = false;
        for(const GuiDraw& d : *frame.guiDraws)
        {
//            if(d.name.find("automap-crosshair-") != d.name.npos)
//            {
//                std::cout << "Crosshair: " << d.topLeft.x << "x" << d.topLeft.y << std::endl;
//            }
            if(d.name.find("button-combat-offensive-down.png") != d.name.npos)
            {
                isCombatOffensiveDown = true;
                break;
            }
        }
//        mScene.update(frame);
//        mOutfitResolver.resolve(mScene, frame, *mGraphicsMonitorReader);

//
//        std::cout << "---------" << std::endl;
//        std::cout << "Players : " << std::endl;
//        for(const OutfitResolver::Player& p : mOutfitResolver.getPlayers())
//            std::cout << "\t" << p.name << ": " << p.x << "x" << p.y << " (" << p.hp * 100.f << "%)" << std::endl;
//        std::cout << "---------" << std::endl;
//
//        std::cout << "Creatures : " << std::endl;
//        for(const OutfitResolver::Creature& p : mOutfitResolver.getCreatures())
//            std::cout << "\t" << p.name << "(" << "): " << p.x << "x" << p.y << " (" << p.hp * 100.f << "%)" << std::endl;
//        std::cout << "---------" << std::endl;
//
//        std::cout << "Npcs : " << std::endl;
//        for(const OutfitResolver::Npc& p : mOutfitResolver.getNpcs())
//            std::cout << "\t" << p.name << ": " << p.x << "x" << p.y << std::endl;
//        std::cout << "---------" << std::endl;




        unsigned int prevX = mMiniMap->getX();
        unsigned int prevY = mMiniMap->getY();
        mMiniMap->update(frame);
//        std::cout << "Coords: " << mMiniMap->getX() << "x" << mMiniMap->getY() << std::endl;
//        if(mMiniMap->getX() != prevX || mMiniMap->getY() != prevY)
        if(false)
//        if(isCombatOffensiveDown)
        {
            std::cout << "------------------" << std::endl;
            unsigned int currX = mMiniMap->getX();
            unsigned int currY = mMiniMap->getY();
            for(size_t y = currY - Constants::NUM_TILES_VIEW_Y / 2; y <= currY + Constants::NUM_TILES_VIEW_Y / 2; y++)
            {
                for(size_t x = currX - Constants::NUM_TILES_VIEW_X / 2; x <= currX + Constants::NUM_TILES_VIEW_X / 2; x++)
                {
                    std::cout << (int)(mMiniMap->getColor(x, y) == sb::tibiaassets::Object::MiniMapColor::YELLOW) << " ";
//                    mMiniMap->isFloorChange(x, y);
//                    std::cout << " ";
                }
                std::cout << std::endl;
            }

//            for(size_t y = 0; y < Constants::NUM_TILES_VIEW_Y; y++)
//            {
//                for(size_t x = 0; x < Constants::NUM_TILES_VIEW_X; x++)
//                {
//                    size_t numGroundTiles = 0;
//                    const Scene::Tile& tile = mScene.getTile(x, y);
//                    char currentLayer = -1;
//                    char groundLayer = -1;
//                    bool isTileBlocked = false;
//                    for(const Scene::Object& so : tile.knownLayerObjects)
//                    {
////                        if(currentLayer != so.layer)
////                        {
////                            currentLayer = so.layer;
////                            if(so.object->itemInfo.isGround)
////                                numGroundTiles++;
////                        }
//
////                        if(so.object->itemInfo.isGround)
////                        if(so.object->itemInfo.hasMinimapColor && so.object->itemInfo.minimapColor == 210)
////                        {
////                            groundLayer = so.layer;
////                            break;
////                        }
//
//                        if(so.object->itemInfo.isBlocking && !so.object->itemInfo.isMovable)
//                        {
//                            isTileBlocked = true;
//                        }
//                        else if(so.object->itemInfo.isGround)
//                            isTileBlocked = false;
//                    }
//
////                    std::cout << numGroundTiles << " ";
////                    std::cout << (int)groundLayer << " ";
//                    std::cout << (int)isTileBlocked << " ";
//                }
//
//                std::cout << std::endl;
//            }
        }

    }

    return;

    clearTextCount++;


    mGui.update(frame);
    if(mGui.getState() == Gui::State::MAIN_MENU)
    {
        for(const Gui::Button& b : mGui.getButtons())
        {
            if(b.text == "Login")
            {
                if(loginStringIndex == 0)
                {
                    mInput->sendString(loginStrings[loginStringIndex++]);
                    mInput->sendKey(VK_TAB);
                    mGraphicsMonitorReader->getNewFrame();
                    mInput->sendString(loginStrings[loginStringIndex++]);
                    mInput->sendKey(VK_RETURN);

                }

                break;
            }
        }

        if(!isLoggingIn)
        {
            for(const TextDraw& text : *frame.textDraws)
            {
                TextBuilder builder(text, frame.width, frame.height);
                bool doBreak = false;
                for(const Text& t : builder.getText())
                {
                    if(t.string == "Select Character")
                    {
                        mInput->sendFocus();
                        mInput->sendKey(VK_RETURN);
                        isLoggingIn = true;
                        mGraphicsMonitorReader->getNewFrame();
                        mGraphicsMonitorReader->getNewFrame();
                        doBreak = true;
                        break;
                    }
                }
                if(doBreak)
                    break;
            }
        }
    }
    else if(mGui.getState() == Gui::State::GAME)
    {
        mScene.update(frame);
        mGui.getButtons();


        static MiniMap miniMap(*mInput, *mGraphicsMonitorReader);
        miniMap.update(frame);
        std::cout << miniMap.getX() << "x" << miniMap.getY() << "\t" << (int)miniMap.getLevel() << std::endl;


//
//        const std::list<Gui::Container>& containers = mGui.getContainers();
//        static bool isDone = false;
//        if(!isDone && containers.size() == 2)
//        {
//            const Gui::Container& c1 = containers.front();
//            const Gui::Container& c2 = containers.back();
//
//            Gui::Rect c1Resizer = c1.window->resizer;
//            Vertex c1Point;
//            Vertex c2Point;
////            c1Point.x = c1Resizer.pos.x + c1Resizer.size.x;
////            c1Point.y = c1Resizer.pos.y;
//
//            Gui::Rect ca1 = c1.window->clientArea;
//            Gui::Rect ca2 = c2.window->clientArea;
////            mInput->sendScroll(ca1.pos.x - 1, ca1.pos.y, -120 * 10);
////            mInput->sendScroll(ca2.pos.x, ca2.pos.y - 1, -120 * 10);
////            mInput->sendScroll(ca1.pos.x + ca1.size.x - 1, ca1.pos.y + ca1.size.y, -120 * 10);
////            mInput->sendScroll(ca2.pos.x + ca2.size.x, ca2.pos.y + ca2.size.y - 1, -120 * 10);
////            mInput->sendScroll(ca1.pos.x + ca1.size.x - 1, ca1.pos.y + ca1.size.y - 1, -120 * 10);
////            mInput->sendScroll(ca2.pos.x + ca2.size.x, ca2.pos.y + ca2.size.y, -120 * 10);
//
//            std::cout << "Visible items: " << std::endl;
//            std::vector<std::pair<size_t, Gui::Rect>> visibleItems1 = c1.getVisibleItems();
//            for(const auto& pair : visibleItems1)
//            {
//                const auto& item = c1.items[pair.first];
//                std::cout << "\t" << item.first << ": ";
//                for(const Object* object : item.second)
//                    std::cout << object->id << " ";
//                std::cout << std::endl;
//            }
//
//            assert(visibleItems1.size() == 9);
//            Gui::Rect srcRect = visibleItems1.back().second;
//            Gui::Rect tarRect = visibleItems1[1].second;
//            mInput->sendMouseDrag(srcRect.pos.x, srcRect.pos.y, tarRect.pos.x, tarRect.pos.y);
//
////            c1Point.x = c1.window->exitButton.pos.x - 5;
////            c1Point.y = c1.window->exitButton.pos.y + c1.window->exitButton.size.y + 5;
////
////            c2Point.x = c2.window->exitButton.pos.x - 5;
////            c2Point.y = c2.window->exitButton.pos.y + c2.window->exitButton.size.y + 5;
////
////            mInput->sendMouseDrag(c2Point.x, c2Point.y, c1Point.x, c1Point.y);
//
//            isDone = true;
//        }


//        std::cout << "----" << std::endl;
//        mScene.forEach([](const Scene::Object& object)
//        {
//            if(object.object->type == Object::Type::OUTFIT)
//                std::cout << "\t" << object.object->id << "\t\t" << (int)object.tileX << "x" << (int)object.tileY << std::endl;
//        });

//        std::cout << "Cap: " << mGui.getCap() << std::endl;
//        std::cout << "Soul: " << mGui.getSoul() << std::endl;
//        std::cout << "Hp: " << mGui.getHp() << std::endl;
//        std::cout << "Mana: " << mGui.getMana() << std::endl;
//        std::cout << "Hp percent: " << mGui.getHpLevel() * 100.f << "%" << std::endl;
//        std::cout << "Mana percent: " << mGui.getManaLevel() * 100.f << "%" << std::endl;
//        std::cout << "Level: " << mGui.getLevel() << std::endl;
//        std::cout << "Exp: " << mGui.getExperience() << std::endl;
//        std::cout << "Xp gain rate: " << mGui.getXpGainRate() << std::endl;
//        std::cout << "Speed: " << mGui.getSpeed() << std::endl;
//        std::cout << "Food: " << mGui.getFoodMinutes() << std::endl;
//        std::cout << "Stamina: " << mGui.getStaminaMinutes() << std::endl;
//        std::cout << "Offline training: " << mGui.getOfflineTrainingMinutes() << std::endl;
//        std::cout << "Magic lvl: " << mGui.getMagicLevel() << std::endl;
//        std::cout << "Fist: " << mGui.getFistFighting() << std::endl;
//        std::cout << "Club: " << mGui.getClubFighting() << std::endl;
//        std::cout << "Sword: " << mGui.getSwordFighting() << std::endl;
//        std::cout << "Axe: " << mGui.getAxeFighting() << std::endl;
//        std::cout << "Distance: " << mGui.getDistanceFighting() << std::endl;
//        std::cout << "Shielding: " << mGui.getShielding() << std::endl;
//        std::cout << "Fishing: " << mGui.getFishing() << std::endl;
//        std::cout << "Crit chance: " << mGui.getCritChance() << "%" << std::endl;
//        std::cout << "Crit dmg: " << mGui.getCritDamage() << std::endl;
//        std::cout << "Hp leech chance: " << mGui.getHpLeechChance() << "%" << std::endl;
//        std::cout << "Hp leech amount: " << mGui.getHpLeechAmount() << std::endl;
//        std::cout << "Mana leech chance: " << mGui.getManaLeechChance() << "%" << std::endl;
//        std::cout << "Mana leech amoumt: " << mGui.getManaLeechAmount() << std::endl;

//
//        std::cout << "EQUIPMENT: " << std::endl;
//        for(const auto& pair : mGui.getEquipment())
//        {
//            std::cout << "\t" << (int)pair.first << ": " << pair.second->id;
//            if(pair.second->itemInfo.hasMarketInfo)
//                std::cout << " (" << pair.second->itemInfo.marketInfo.name << ")";
//            std::cout << std::endl;
//        }
//
//        std::cout << "CONTAINERS: " << std::endl;
//        for(const Gui::Container& c : mGui.getContainers())
//        {
//            std::cout << "\t" << c.items.size() << " items: " << std::endl;
//            for(const auto& pair : c.items)
//            {
//                std::cout << "\t" << (int)pair.first << ": " << pair.second->id;
//                if(pair.second->itemInfo.hasMarketInfo)
//                    std::cout << " (" << pair.second->itemInfo.marketInfo.name << ")";
//                std::cout << std::endl;
//            }
//        }


//        mScene.forEach([](const Scene::Object& object)
//        {
//            if(object.object->itemInfo.isGround)
//                std::cout << object.object->id << " ";
//        });

//        if(doGetContextMenu)
//        {
//            std::vector<std::string> contextMenu = getContextMenu(frame, 7, 5);
//            for(auto str : contextMenu)
//                std::cout << str << std::endl;
//
////            std::string greenText = getGreenText(frame, rand() % Constants::NUM_TILES_VIEW_X, rand() % Constants::NUM_TILES_VIEW_Y);
////            std::cout << greenText << std::endl;
//            doGetContextMenu = false;
//            return;
//        }

//        mOutfitResolver.resolve(mScene, frame, *mGraphicsMonitorReader);
//        std::cout << "NPCS: " << std::endl;
//        for(const OutfitResolver::Npc& npc : mOutfitResolver.getNpcs())
//            std::cout << npc.name << " " << npc.x << "x" << npc.y << " (" << (npc.object ? npc.object->id : 0) << ")" << std::endl;
//        std::cout << "PLAYERS: " << std::endl;
//        for(const OutfitResolver::Player& player : mOutfitResolver.getPlayers())
//            std::cout << player.name << " " << player.x << "x" << player.y << " (" << player.hp << "%" << ")" << std::endl;
//        std::cout << "CREATURES: " << std::endl;
//        for(const OutfitResolver::Creature& creature : mOutfitResolver.getCreatures())
//            std::cout << creature.name << " " << creature.x << "x" << creature.y << " (" << creature.hp << "%" << ")" << " (" << (creature.object ? creature.object->id : 0) << ")" <<  std::endl;



//        bool hasGreenText = false;
//        for(const TextDraw& text : *frame.textDraws)
//        {
//            TextBuilder builder(text, frame.width, frame.height);
//            if(builder.getTextType() == Text::Type::GREEN_TEXT)
//            {
////                std::cout << text.string << "->" << text.x << "x" << text.y << " (" << (int)text.r << " " << (int)text.g << " " << (int)text.b << ")" << std::endl;
//                hasGreenText = true;
//                isWaitingForGreenText = false;
//            }
//        }

//        if(!hasGreenText && !isWaitingForGreenText)
//        {
//            unsigned char tileX = rand() % Constants::NUM_TILES_VIEW_X;
//            unsigned char tileY = rand() % Constants::NUM_TILES_VIEW_Y;
////            tileX = 7;
////            tileY = 5;
//            float tileWidth = ((float)frame.viewWidth) / ((float)Constants::NUM_TILES_VIEW_X);
//            float tileHeight = ((float)frame.viewHeight) / ((float)Constants::NUM_TILES_VIEW_Y);
//            unsigned short screenX = frame.viewX + (tileX + 0.5f) * tileWidth;
//            unsigned short screenY = frame.viewY + (tileY + 0.5f) * tileHeight;
//            mInput->sendMouseClick(MK_LBUTTON, screenX, screenY, MK_SHIFT);
////            mInput->sendMouseClick(MK_LBUTTON, screenX, screenY, screenX + tileWidth, screenY + tileHeight, MK_SHIFT);
//
//            std::cout << "Getting green text from: " << (int)tileX << "x" << (int)tileY << std::endl;
//            isWaitingForGreenText = true;
//        }
    }

    char movementX = mScene.resetMovementX();
    char movementY = mScene.resetMovementY();
}

void TibiaClient::prepareSharedMemory(std::string& sharedMemoryName)
{
    #if defined(_WIN32)

    std::string sharedMemoryNamespace = "Local\\";
    sharedMemoryName = sharedMemoryNamespace + sb::utility::randStr(SHARED_MEMORY_NAME_LENGTH - sharedMemoryNamespace.size());
    mShmHandle = CreateFileMapping(
             INVALID_HANDLE_VALUE,    // use paging file
             NULL,                    // default security
             PAGE_READWRITE,          // read/write access
             0,                       // maximum object size (high-order DWORD)
             NUM_BYTES,                // maximum object size (low-order DWORD)
             sharedMemoryName.c_str());                 // name of mapping object

    if (mShmHandle == NULL)
    {
        std::stringstream sstream;
        sstream << "Could not create shared memory object (" << GetLastError() << ").";
        THROW_RUNTIME_ERROR(sstream.str());
    }

    #else
    int shmFd = shm_open("shared_memory", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    int flags = fcntl(shmFd, F_GETFD);
    flags &= ~FD_CLOEXEC;
    fcntl(shmFd, F_SETFD, flags);

    if(shmFd == -1)
        THROW_RUNTIME_ERROR("Could not open shared memory.");

    if(ftruncate(shmFd, NUM_BYTES) == -1)
        THROW_RUNTIME_ERROR("Could not set shared memory size.");

    mShmHandle = shmFd;
    #endif // defined

    mapSharedMemory();
}

void TibiaClient::mapSharedMemory()
{
    #if defined(_WIN32)
    mShm = (SharedMemorySegment*)MapViewOfFile
    (
        mShmHandle,   // handle to map object
        FILE_MAP_ALL_ACCESS, // read/write permission
        0,
        0,
        NUM_BYTES
    );

    if(mShm == NULL)
    {
        std::stringstream sstream;
        sstream << "Could not map shared memory (" << GetLastError() << ").";
        THROW_RUNTIME_ERROR(sstream.str());
    }
    #else
    mShm = (SharedMemorySegment*)mmap(nullptr, NUM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, mShmHandle, 0);
    if(mShm == MAP_FAILED)
        THROW_RUNTIME_ERROR("Could not map shared memory.");
    #endif // defined

}



void TibiaClient::launchClient(char** environment, std::string clientDirectory, std::string sharedMemoryName)
{
    #if defined(_WIN32)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    const std::string MODULE_PATH = clientDirectory + "/packages/Tibia/bin/client.exe";
    const std::string STARTING_DIR = clientDirectory + "/packages/Tibia/bin";

    std::vector<char> envBuf(1024 * 10);
    GetModuleFileName(NULL, (char*)envBuf.data(), envBuf.size());
    std::string executablePath(envBuf.data());
    executablePath = sb::utility::file::getPath(executablePath);
    SetDllDirectory(executablePath.c_str());

    SetEnvironmentVariable(SHARED_MEMORY_ENV_VAR_NAME, sharedMemoryName.c_str());
    SECURITY_ATTRIBUTES parentSyncSecAttr;
    parentSyncSecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    parentSyncSecAttr.lpSecurityDescriptor = NULL;
    parentSyncSecAttr.bInheritHandle = TRUE;
    mShm->semRead = CreateSemaphore(&parentSyncSecAttr, 0, 1, NULL);
    mShm->semWrite = CreateSemaphore(&parentSyncSecAttr, 0, 1, NULL);
    mShm->parentProcessHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, TRUE, GetCurrentProcessId());
    if(mShm->parentProcessHandle == NULL)
    {
        std::stringstream sstream;
        sstream << "Could not open process handle to self. Error code: " << GetLastError() << std::endl;
        THROW_RUNTIME_ERROR(sstream.str());
    }
    if(!CreateProcess
       (
            MODULE_PATH.c_str(),   // No module name (use command line)
            NULL,        // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            TRUE,          // Set handle inheritance to FALSE
            CREATE_SUSPENDED,
//            NULL,
            NULL,
//            environment,           // environment block
            STARTING_DIR.c_str(),           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi             // Pointer to PROCESS_INFORMATION structure
        ))
    {
        std::stringstream sstream;
        sstream << "Failed to start client. " << GetLastError() << std::endl;
        THROW_RUNTIME_ERROR(sstream.str());
    }
    SetDllDirectory(NULL);
    SetEnvironmentVariable(SHARED_MEMORY_ENV_VAR_NAME, NULL);

    mClientProcessHandle = pi.hProcess;

    LPVOID RemoteString, LoadLibAddy;

    LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    const char* DLL_NAME = "libShankBotMonitorInjection.dll";
    RemoteString = (LPVOID)VirtualAllocEx(mClientProcessHandle, NULL, strlen(DLL_NAME), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(mClientProcessHandle, (LPVOID)RemoteString, DLL_NAME,strlen(DLL_NAME), NULL);
    HANDLE injectionThread = CreateRemoteThread(mClientProcessHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);


    std::cout << "Injecting... ";
    DWORD result = WaitForSingleObject(injectionThread, 5000);

    if(result == WAIT_OBJECT_0 && mShm->isClientAttached)
    {
        std::cout << "OK" << std::endl;
        ResumeThread(pi.hThread);
    }
    else
    {
        std::cout << "FAIL" << std::endl;
        std::stringstream sstream;
        sstream << "Failed to inject DLL at '" << DLL_NAME << "' into process of module at '" << MODULE_PATH << "'." << std::endl;
        THROW_RUNTIME_ERROR(sstream.str());
    }

    CloseHandle(pi.hThread);



    #else
    char* argv[] = {(char*)"./Tibia", 0};

    pid_t pid = fork();
    if(pid == 0)
    {
        chdir(clientDirectory.c_str());
        execve(argv[0], &argv[0], environment);
    }
    else if(pid < 0)
        THROW_RUNTIME_ERROR("Failed to fork.");
    #endif // defined

}

char** TibiaClient::prepareEnvironment() const
{
    #if defined(_WIN32)
//    extern const char** environ = (const char**)GetEnvironmentStrings();
    return nullptr;
    #endif // defined
//
//    size_t envSize = 0;
//    while(environ[envSize] != nullptr)
//        envSize++;
//
//    #if defined(_WIN32)
//    char** tibiaEnv = new char*[envSize + 1];
//    tibiaEnv[envSize] = nullptr;
//    return tibiaEnv;
//    #else
//    size_t tibiaEnvSize = envSize + 2;
//    char** tibiaEnv = new char*[tibiaEnvSize + 1];
//    tibiaEnv[tibiaEnvSize] = nullptr;
//
//    for(size_t i = 0; i < envSize; i++)
//    {
//        if(strncmp(environ[i], "HOME=", 5) == 0)
//            tibiaEnv[i] = strdup("HOME=./");
//        else
//            tibiaEnv[i] = strdup(environ[i]);
//    }
//
//    tibiaEnv[envSize] = strdup("LD_PRELOAD=../bin/Release/libGraphicsMonitor.so");
//
//    std::stringstream sstream;
//    sstream << "SHANKBOT_SHARED_MEMORY_FD=" << mShmHandle;
//    tibiaEnv[envSize + 1] = strdup(sstream.str().c_str());
//
//    return tibiaEnv;
//    #endif // defined
}

void TibiaClient::deleteEnvironment(char** environment) const
{
    #if defined(_WIN32)
    return;
    #else
    for(size_t i = 0; environment[i] != nullptr; i++)
        delete environment[i];

    delete[] environment;
    #endif // defined
}


SharedMemorySegment* TibiaClient::getSharedMemory() const
{
    return mShm;
}

const Input& TibiaClient::getInput() const
{
    return *mInput;
}
