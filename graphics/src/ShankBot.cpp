///////////////////////////////////
// Internal ShankBot headers
#include "ShankBot.hpp"
#include "VersionControl.hpp"
#include "TibiaSpr.hpp"
#include "ImageTree.hpp"
#include "ImageTrees.hpp"
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
    if(VersionControl::hasNewVersion(clientDir, versionControlDir))
    {
        std::cout << "Has new version" << std::endl;
        TibiaSpr spr(clientDir + "/Tibia.spr");
        ImageTree spriteColor(spr.getSprites(), spr.getSpriteIds());
        spriteColor.writeToBinaryFile(spriteColorTreePath);
//        TibiaDat dat(clientDir + "/Tibia.dat");
    }

    std::cout << "Constructing tree..." << std::endl;
    ImageTrees::initializeSpriteColorTree(spriteColorTreePath);

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
        std::list<std::list<size_t>> drawnSprites = mTibiaClient->getDrawnSprites();
        if(!drawnSprites.empty())
        {
            for(const std::list<size_t> ids : drawnSprites)
                if(ids.size() > 1)
                {
                    std::cout << "Found duplicates:";
                    for(size_t id : ids)
                        std::cout << " " << id;

                    std::cout << std::endl;
                }
        }
//            std::cout << "Found " << drawnSprites.size() << std::endl;
    }
}
