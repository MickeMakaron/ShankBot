///////////////////////////////////
// Internal ShankBot headers
#include "ShankBot.hpp"
#include "VersionControl.hpp"
#include "TibiaSpr.hpp"
#include "ImageTree.hpp"
#include "ImageTrees.hpp"
#include "SpriteObjectBindings.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
///////////////////////////////////

///////////////////////////////////
// STD C
///////////////////////////////////

ShankBot::ShankBot(std::string clientDir, std::string versionControlDir)
{
    std::string storagePath = VersionControl::getPath(versionControlDir);
    std::string spriteColorTreePath = storagePath + "/tree-sprite-color";
    std::string spriteTransparencyTreePath = storagePath + "/tree-sprite-transparency";
    std::string spriteObjectBindingsPath = storagePath + "/sprite-object-bindings";
    if(VersionControl::hasNewVersion(clientDir, versionControlDir))
    {
        std::cout << "Has new version" << std::endl;
        {
            TibiaSpr spr(clientDir + "/Tibia.spr");
            std::vector<std::vector<unsigned char>> sprites;
            for(auto pixels : spr.getSprites())
            {
                std::vector<unsigned char> sprite;
                for(size_t i = 0; i < pixels.size(); i += 4)
                {
                    if(pixels[i + 3] != 0)
                    {
                        sprite.push_back(pixels[i]);
                        sprite.push_back(pixels[i + 1]);
                        sprite.push_back(pixels[i + 2]);
                    }
                }
                sprites.push_back(sprite);
            }

            {
                ImageTree spriteColor(sprites, spr.getSpriteIds());
                spriteColor.writeToBinaryFile(spriteColorTreePath);
            }

            sprites.clear();
            for(auto pixels : spr.getSprites())
            {
                std::vector<unsigned char> sprite;
                for(size_t i = 0; i < pixels.size(); i += 4)
                {
                    if(pixels[i + 3] != 0)
                    {
                        unsigned char x = i % 32;
                        unsigned char y = i / 32;

                        sprite.push_back(x);
                        sprite.push_back(y);
                    }
                }

                sprites.push_back(sprite);
            }

            {
                ImageTree spriteTransparency(sprites, spr.getSpriteIds());
                spriteTransparency.writeToBinaryFile(spriteTransparencyTreePath);
            }
        }

        {
            TibiaDat dat(clientDir + "/Tibia.dat");
            SpriteObjectBindings::initialize(dat);
        }

        SpriteObjectBindings::writeToBinaryFile(spriteObjectBindingsPath);
    }
    else
    {
        SpriteObjectBindings::initialize(spriteObjectBindingsPath);
    }

    std::cout << "Constructing tree..." << std::endl;
    ImageTrees::initializeSpriteColorTree(spriteColorTreePath);
    ImageTrees::initializeSpriteTransparencyTree(spriteTransparencyTreePath);

    std::cout << "Starting client" << std::endl;
    mTibiaClient = std::unique_ptr<TibiaClient>(new TibiaClient(clientDir));
}

void ShankBot::run()
{
    while(true)
    {
        mTibiaClient->update();
//
//        for(const std::list<size_t> ids : client.getDrawnSprites())
//        {
//            for(size_t id : ids)
//                std::cout << id << " ";
//            std::cout << std::endl;
//        }
//        std::list<std::list<size_t>> drawnSprites = mTibiaClient->getDrawnSprites();
//        if(!drawnSprites.empty())
//        {
//            for(const std::list<size_t> ids : drawnSprites)
//                if(ids.size() > 1)
//                {
//                    std::cout << "Found duplicates:";
//                    for(size_t id : ids)
//                        std::cout << " " << id;
//
//                    std::cout << std::endl;
//                }
//        }
//            std::cout << "Found " << drawnSprites.size() << std::endl;
    }
}
