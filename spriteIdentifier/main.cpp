

#include "../graphics/incl/PngImage.hpp"
using namespace GraphicsLayer;

#include <dirent.h>
#include <string>
#include <stdexcept>
#include <cstring>
#include <list>
#include <iostream>
#include <memory>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

const size_t IMAGE_WIDTH = 32;
const size_t IMAGE_HEIGHT = 32;
const size_t IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;
const size_t IMAGE_NUM_BYTES = IMAGE_SIZE * 4;

const size_t HISTOGRAM_SIZE = 256 * 4;


struct Object;
struct Image
{
    std::string path;
    size_t id;
    std::list<Object*> objects;
    unsigned char* pixels;
    unsigned short histogram[HISTOGRAM_SIZE];

    ~Image(){delete[] pixels;};
};



typedef std::unique_ptr<Image> ImagePtr;

struct Object
{
    std::list<std::list<ImagePtr>::iterator> spriteIterators;
};



void createHistogram(Image& img)
{
    const size_t R_BUCKET_OFFSET = 0 * 256;
    const size_t G_BUCKET_OFFSET = 1 * 256;
    const size_t B_BUCKET_OFFSET = 2 * 256;
    const size_t A_BUCKET_OFFSET = 3 * 256;

    memset(img.histogram, 0, sizeof(img.histogram));

    for(size_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT * 4; i += 4)
    {
        img.histogram[img.pixels[i] + R_BUCKET_OFFSET]++;
        img.histogram[img.pixels[i + 1] + G_BUCKET_OFFSET]++;
        img.histogram[img.pixels[i + 2] + B_BUCKET_OFFSET]++;
        img.histogram[img.pixels[i + 3] + A_BUCKET_OFFSET]++;
    }

}


std::string basename(std::string path)
{
    return path.substr(path.find_last_of('/') + 1);
}

std::string basenameNoExt(std::string path)
{
    size_t start = path.find_last_of('/') + 1;
    if(start == path.size())
        start = 0;

    size_t end = path.find_last_of('.');
    if(end <= start)
        end = path.size() - 1;

    return path.substr(start, end - start);
}

void loadImage(std::string path, std::list<ImagePtr>& out)
{
//    std::cout << "Loading... " << path << std::endl;

    Image* img = new Image();

    PngImage png = readPng(path);
    if(png.width != 32 || png.height != 32)
    {
        delete img;
        return;
    }

    img->pixels = png.pixelData;
    img->path =  path;

    std::istringstream sstream(basenameNoExt(path));
    sstream >> img->id;

    createHistogram(*img);

    out.push_back(ImagePtr(img));
}


void getFileNames(std::string directory, std::list<std::string>& fileNames)
{
    DIR* dir;
    dir = opendir(directory.c_str());
    if(dir == nullptr)
        throw std::runtime_error("Cannot load directory. Directory not found: '" + directory + "'.");

    dirent* ent;
    while((ent = readdir(dir)) != nullptr)
    {
        if(ent->d_type == DT_DIR)
        {
            if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
                getFileNames(directory + "/" + std::string(ent->d_name), fileNames);
        }
        else if(ent->d_type == DT_REG)
            fileNames.push_back(directory + "/" + std::string(ent->d_name));
    }

    closedir(dir);
}

template<typename T>
float computeDifference(const T* p1, const T* p2, size_t numElements)
{
    float difference = 0.f;
    for(size_t i = 0; i < numElements; i++)
        difference += std::fabs(p1[i] - p2[i]);

    return difference;
}


typedef std::map<size_t, std::list<size_t>> IdMap;

void getDuplicateBindings(std::string filePath, IdMap& map)
{
    std::ifstream file(filePath);
    if(file.is_open())
    {
        std::string line;
        while(getline(file,line))
        {
            std::istringstream stream(line);
            size_t spriteId;
            stream >> spriteId;
            auto it = map.insert(std::make_pair(spriteId, std::list<size_t>())).first;
            while(stream.good())
            {
                size_t objectId;
                stream >> objectId;
                it->second.push_back(objectId);
            }
        }

        file.close();
    }
    else
        throw std::runtime_error("Could not open duplicate bindings file at '" + filePath + "'.");
}

int numRemovedObjects = 0;
int numRemovedSprites = 0;
int numRemovedFromHaystack = 0;


void decrementSprite(std::list<ImagePtr>& haystack, std::list<ImagePtr>::iterator spriteIterator)
{
    numRemovedFromHaystack++;
    std::cout << "Removing from haystack: '" << (*spriteIterator)->id << "'... ";

    for(Object* object : (*spriteIterator)->objects)
        object->spriteIterators.remove(spriteIterator);

    haystack.erase(spriteIterator);
    std::cout << haystack.size() << " left." << std::endl;
}

//void decrementSprite(std::list<ImagePtr>& haystack, IdMap& map, size_t spriteId)
void decrementSprite(std::list<ImagePtr>& haystack, size_t spriteId)
{
//    haystack.remove_if([&](const ImagePtr& sprite)
//    {
//            if(sprite->id == spriteId)
//            {
//                numRemovedFromHaystack++;
//                std::cout << "Removing from haystack: '" << spriteId << "'." << std::endl;
//
//                sprite->
//            }
//
//            return sprite->id == spriteId;
//    });

    for(auto it = haystack.begin(); it != haystack.end(); it++)
    {
        if((*it)->id == spriteId)
        {
            decrementSprite(haystack, it);
            break;
        }
    }


//    auto found = map.find(spriteId);
//    if(found != map.end())
//    {
//        found->second.pop_back();
//        numRemovedObjects++;
//        std::cout << "Decrementing sprite '" << spriteId << "'." << std::endl;
//
//        if(found->second.empty())
//        {
//            map.erase(found);
//            numRemovedSprites++;
//            std::cout << "Removing sprite '" << spriteId << "'." << std::endl;
//
//            haystack.remove_if([&](const ImagePtr& sprite)
//            {
//                if(sprite->id == spriteId)
//                {
//                    numRemovedFromHaystack++;
//                    std::cout << "Removing from haystack: '" << spriteId << "'." << std::endl;
//                }
//
//                return sprite->id == spriteId;
//            });
//
//        }
//
//    }
//    else
//    {
//        haystack.remove_if([&](const ImagePtr& sprite)
//        {
//                if(sprite->id == spriteId)
//                {
//                    numRemovedFromHaystack++;
//                    std::cout << "Removing from haystack: '" << spriteId << "'." << std::endl;
//                }
//
//                return sprite->id == spriteId;
//        });
//    }
}

void printHelp()
{
    std::cout   << "Usage: " << std::endl
                << "spriteIdentifier <haystack> <bindings> <inDir> <outDir>" << std::endl
                << "<haystack>: The output directory of the extractSprites script." << std::endl
                << "<bindings>: The path to the bindings file created by extractData script." << std::endl
                << "<inDir>: The output directory of the formatSprites script." << std::endl
                << "<outDir>: The directory where this program is to put the identified sprites." << std::endl;
}

int main(int argc, char** argv)
{
    Object* objectMap = new Object[24573 + 1];

    if(argc != 5)
    {
         printHelp();
         return 1;
    }

    std::string haystack = argv[1];
    std::string spriteToObjectBindingsPath = argv[2];
    std::string inDir = argv[3];
    std::string outDir = argv[4];

//    std::string haystack = "/home/vendrii/Desktop/pythonSpr/out/";
//    std::string inDir = "/home/vendrii/Desktop/ShankBot/crawler/out/";
//    std::string outDir = "/home/vendrii/Desktop/compare/out/";
//    std::string spriteToObjectBindingsPath = "/home/vendrii/Desktop/pythonDat/out/duplicateBindings.txt";

    struct stat st = {0};
    if(stat(haystack.c_str(), &st) == -1)
        throw std::runtime_error("Could not find haystack directory at '" + haystack + "'.");

    if(stat(spriteToObjectBindingsPath.c_str(), &st) == -1)
        throw std::runtime_error("Could not find sprite-object bindings directory at '" + spriteToObjectBindingsPath + "'.");

    if(stat(inDir.c_str(), &st) == -1)
        throw std::runtime_error("Could not input directory at '" + inDir + "'.");

    if(stat(outDir.c_str(), &st) == -1)
    {
        std::cout << "Output directory does not exist. Creating it at '" + outDir + "'." << std::endl;
        mkdir(outDir.c_str(), 0700);
    }

    std::cout << "Creating range directories." << std::endl;

    std::string delta1Dir = outDir + "/0-0.0001";
    std::string delta2Dir = outDir + "/0.0001f-0.5";
    std::string delta3Dir = outDir + "/0.5-1";
    std::string delta4Dir = outDir + "/1-inf";

    if(stat(delta1Dir.c_str(), &st))
        mkdir(delta1Dir.c_str(), 0700);

    if(stat(delta2Dir.c_str(), &st))
        mkdir(delta2Dir.c_str(), 0700);

    if(stat(delta3Dir.c_str(), &st))
        mkdir(delta3Dir.c_str(), 0700);

    if(stat(delta4Dir.c_str(), &st))
        mkdir(delta4Dir.c_str(), 0700);


    IdMap  spriteToObjectMap;
    getDuplicateBindings(spriteToObjectBindingsPath, spriteToObjectMap);

    std::list<std::string> files;
    getFileNames(haystack, files);

    std::cout << "Loading haystack..." << std::endl;
    std::list<ImagePtr> haystackImages;
    for(const std::string& file : files)
        loadImage(file, haystackImages);
    std::cout << "Loaded " << haystackImages.size() << " files into haystack." << std::endl;

    for(auto it = haystackImages.begin(); it != haystackImages.end(); it++)
    {
        for(size_t objectId : spriteToObjectMap[(*it)->id])
        {
            Object* object = &objectMap[objectId];
            (*it)->objects.push_back(object);
            object->spriteIterators.push_back(it);
        }

    }


    files.clear();
    getFileNames(inDir, files);

    std::list<std::string> existingOutFiles;
    getFileNames(outDir, existingOutFiles);

    for(std::string& str : existingOutFiles)
        str = basename(str);

    files.remove_if([&](const std::string& str)
    {
        return std::find(existingOutFiles.begin(), existingOutFiles.end(), basename(str)) != existingOutFiles.end();
    });

    std::list<size_t> processedSpriteIds;
    for(const std::string& str : existingOutFiles)
    {
        assert(str.size() > 0);
        if(str[str.size() - 1] == ')')
        {
            size_t start = str.find_last_of('(') + 1;
            size_t end = str.find_last_of('.');
            std::string spriteId = str.substr(start, end - start);

            std::istringstream sstream(spriteId);
            size_t spriteIdInt;
            sstream >> spriteIdInt;
            processedSpriteIds.push_back(spriteIdInt);
        }
    }
    std::cout << "Number of processed sprites already processed: " << processedSpriteIds.size() << std::endl;

    for(size_t id : processedSpriteIds)
        decrementSprite(haystackImages, id);
//    std::cout << "numRemovedObjects: " << numRemovedObjects << std::endl;
//    std::cout << "numRemovedSprites: " << numRemovedSprites << std::endl;
    std::cout << "Removed " << numRemovedFromHaystack << " items from haystack since they have already been processed." << std::endl;

    std::cout << "Loading input sprites..." << std::endl;
    std::list<ImagePtr> inSprites;
    for(const std::string& file : files)
        loadImage(file, inSprites);
    std::cout << "Loaded " << inSprites.size() << " images." << std::endl;

    const size_t MAX_HISTOGRAM_SUM = IMAGE_WIDTH * IMAGE_HEIGHT * 4;
    const size_t MAX_DIFFERENCE_SUM = 104448;
    std::list<Object*> relatedObjects;
    for(const ImagePtr& inSprite : inSprites)
    {
        float minDelta = 100.f;
        auto needleIterator = haystackImages.end();

        float definitiveThreshold = 0.0001f;
        float isResultDefinitive = false;
        for(Object* object : relatedObjects)
        {
            std::cout << "Trying our luck at a related object..." << std::endl;
            for(auto imgIterator : object->spriteIterators)
            {
                float difference = computeDifference(inSprite->pixels, (*imgIterator)->pixels, IMAGE_NUM_BYTES);
                float histogramDifference = computeDifference(inSprite->histogram, inSprite->histogram, HISTOGRAM_SIZE);

                difference /= MAX_DIFFERENCE_SUM;
                histogramDifference /= MAX_HISTOGRAM_SUM;

                float delta = difference + histogramDifference;
                if(delta < minDelta)
                {
                    minDelta = delta;
                    needleIterator = imgIterator;
                    std::cout << "Current delta: " << minDelta << " - " << (*needleIterator)->path << std::endl;
                    std::cout << "Pairing " << inSprite->path<< "..." << std::endl;
                }

                if(minDelta < definitiveThreshold)
                {
                    isResultDefinitive = true;
                    std::cout << "Lucky me!" << std::endl;
                    break;
                }

            }

            if(isResultDefinitive)
                break;
        }

        if(!relatedObjects.empty() && !isResultDefinitive)
            std::cout << "No luck..." << std::endl;


        if(!isResultDefinitive)
            for(auto imgIterator = haystackImages.begin(); imgIterator != haystackImages.end(); imgIterator++)
            {


                float difference = computeDifference(inSprite->pixels, (*imgIterator)->pixels, IMAGE_NUM_BYTES);
                float histogramDifference = computeDifference(inSprite->histogram, inSprite->histogram, HISTOGRAM_SIZE);

                difference /= MAX_DIFFERENCE_SUM;
                histogramDifference /= MAX_HISTOGRAM_SUM;

                float delta = difference + histogramDifference;
                if(delta < minDelta)
                {
                    minDelta = delta;
                    needleIterator = imgIterator;
                    std::cout << "Current delta: " << minDelta << " - " << (*needleIterator)->path << std::endl;
                    std::cout << "Pairing " << inSprite->path<< "..." << std::endl;
                }

                if(minDelta < definitiveThreshold)
                    break;

            }

        relatedObjects = (*needleIterator)->objects;

        assert(needleIterator != haystackImages.end()); // Just in case

        std::string needleBasename = basename((*needleIterator)->path);
        std::string inSpriteBasename = basename(inSprite->path);


        std::ifstream src((*needleIterator)->path, std::ios::binary);
        std::stringstream sstream;

        std::string rangeDir;
        if(minDelta < definitiveThreshold)
            rangeDir = delta1Dir;
        else if(minDelta < 0.5f)
            rangeDir = delta2Dir;
        else if(minDelta < 1.f)
            rangeDir = delta3Dir;
        else
            rangeDir = delta4Dir;

        sstream << rangeDir << "/" << inSpriteBasename << "-" << minDelta << "(" << needleBasename << ")";
        std::ofstream dst(sstream.str(), std::ios::binary);
        dst << src.rdbuf();

        src.close();
        src.open(inSprite->path, std::ios::binary);

        dst.close();
        dst.open(rangeDir + "/" + inSpriteBasename, std::ios::binary);

        dst << src.rdbuf();

        decrementSprite(haystackImages, needleIterator);
    }

//path.substr(path.find_last_of('/') + 1);


//
//
//
//    PngImage img = readPng("derp");
//    DIR* dir;
//    dir = opendir(directory.c_str());
//    if(dir == nullptr)
//        throw std::runtime_error("Cannot load images. Directory not found: '" + directory + "'.");
//
//    dirent* ent;
//    size_t numFolders = 0;
//    std::vector<std::string> files;
//    while((ent = readdir(dir)) != nullptr)
//    {
//        if(ent->d_type == DT_DIR)
//        {
//            if(strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0)
//            {
//                numFolders++;
//
//                std::stringstream sstream;
//                sstream << directory << "/" << ent->d_name;
//                loadImages(sstream.str());
//            }
//        }
//        else if(ent->d_type == DT_REG && numFolders == 0)
//            files.push_back(ent->d_name);
//    }
//
//    if(numFolders == 0)
//    {
//        std::string folderName = directory.substr(directory.find_last_of('/') + 1);
//
//        for(std::string file : files)
//            loadImage(directory + "/" + file, folderName);
//    }
//
//    closedir(dir);
}
