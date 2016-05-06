#include "TibiaClient.hpp"
#include "ImageTree.hpp"

#include <iostream>

using namespace GraphicsLayer;

int main(int argc, char *args[])
{
    std::cout << "Constructing tree..." << std::endl;
    ImageTree tree("tibia/tree.bin");

    TibiaClient client("./tibia", tree);
    while(true)
    {
        client.update();
//
//        for(const std::list<size_t> ids : client.getDrawnSprites())
//        {
//            for(size_t id : ids)
//                std::cout << id << " ";
//            std::cout << std::endl;
//        }
        std::list<std::list<size_t>> drawnSprites = client.getDrawnSprites();
        if(!drawnSprites.empty())
            std::cout << "Found " << drawnSprites.size() << std::endl;
    }
}
