

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
#include <fstream>

const size_t IMAGE_WIDTH = 32;
const size_t IMAGE_HEIGHT = 32;
const size_t IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;
const size_t IMAGE_NUM_BYTES = IMAGE_SIZE * 4;

const size_t HISTOGRAM_SIZE = 256 * 4;

struct Image
{
    std::string path;
    unsigned char* pixels;
    unsigned short histogram[HISTOGRAM_SIZE];
};



typedef std::unique_ptr<Image> ImagePtr;




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

std::string basename(std::string path)
{
    return path.substr(path.find_last_of('/') + 1);
}

int main()
{
    std::string haystack = "/home/vendrii/Desktop/pythonSpr/out/";
    std::string inDir = "/home/vendrii/Desktop/ShankBot/crawler/out/";
    std::string outDir = "/home/vendrii/Desktop/compare/out/";

    std::list<std::string> files;
    getFileNames(haystack, files);

    std::cout << "Loading haystack..." << std::endl;

    std::list<ImagePtr> haystackImages;
    for(const std::string& file : files)
        loadImage(file, haystackImages);



    std::cout << "Loaded " << haystackImages.size() << " images." << std::endl;


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

    std::cout << "Loading input sprites..." << std::endl;

    std::list<ImagePtr> inSprites;
    for(const std::string& file : files)
        loadImage(file, inSprites);



    std::cout << "Loaded " << inSprites.size() << " images." << std::endl;

    const size_t MAX_HISTOGRAM_SUM = IMAGE_WIDTH * IMAGE_HEIGHT * 4;
    const size_t MAX_DIFFERENCE_SUM = 104448;
    for(const ImagePtr& inSprite : inSprites)
    {
        float minDelta = 100.f;
        Image* needle = nullptr;

        for(const ImagePtr& img : haystackImages)
        {
            float difference = computeDifference(inSprite->pixels, img->pixels, IMAGE_NUM_BYTES);
            float histogramDifference = computeDifference(inSprite->histogram, inSprite->histogram, HISTOGRAM_SIZE);

            difference /= MAX_DIFFERENCE_SUM;
            histogramDifference /= MAX_HISTOGRAM_SUM;

            float delta = difference + histogramDifference;
            if(delta < minDelta)
            {
                minDelta = delta;
                needle = img.get();
                std::cout << "Current delta: " << minDelta << " - " << needle->path << std::endl;
                std::cout << "Pairing " << inSprite->path<< "..." << std::endl;
            }

        }
        assert(needle != nullptr);

        std::string needleBasename = basename(needle->path);
        std::string inSpriteBasename = basename(inSprite->path);


        std::ifstream src(needle->path, std::ios::binary);
        std::ofstream dst(outDir + "/" + inSpriteBasename + "(" + needleBasename + ")",   std::ios::binary);
        dst << src.rdbuf();

        src.close();
        src.open(inSprite->path, std::ios::binary);

        dst.close();
        dst.open(outDir + "/" + inSpriteBasename, std::ios::binary);

        dst << src.rdbuf();


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
