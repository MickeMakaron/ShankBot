///////////////////////////////////
// Internal ShankBot headers
#include "ImageTree.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <sstream>
///////////////////////////////////
#include <iostream>

ImageTree::ImageTree(const std::vector<std::vector<unsigned char>>& sprites, const std::vector<size_t>& ids)
: mRoot(new Node(0))
{
    if(sprites.size() != ids.size())
        throw std::runtime_error("Sprite count does not match ID count.");

    for(size_t i = 0; i < sprites.size(); i++)
    {
        if(!sprites[i].empty())
            insert(mRoot, Sprite(&sprites[i], ids[i]));
    }

    removeEmptyNodes();
//    computeIgnoredValues();
    forEachNode(mRoot, [](NodePtr& node)
    {
        for(Sprite& sprite : node->sprites)
        {
            sprite.numValuesIgnored = sprite.pixels->size() - node->level;
            std::cout << "values ignored " << sprite.id << ": " << sprite.numValuesIgnored << std::endl;
        }
    });
}

void ImageTree::removeEmptyNodes()
{
    forEachNode(mRoot, [](NodePtr& node)
    {
        for(auto& pair : node->children)
        {
            NodePtr& child = pair.second;
            while(child->children.size() == 1 && child->sprites.empty())
            {
                Node* pointerToGrandChild = child->children.begin()->second.release();
                child.reset(pointerToGrandChild);
            }
        }
    });
}

void ImageTree::forEachNode(NodePtr& node, std::function<void(NodePtr& node)> function)
{
    function(node);
    for(auto& child : node->children)
        forEachNode(child.second, function);
}

ImageTree::ImageTree(std::string filePath)
: mRoot(new Node(0))
{
    loadFromBinaryFile(filePath);
}

void ImageTree::loadFromBinaryFile(std::string filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    loadFromBinaryFile(mRoot, file);

    file.close();
}

void ImageTree::loadFromBinaryFile(NodePtr& node, std::istream& file)
{
    unsigned int numSprites;
    file.read((char*)&numSprites, sizeof(numSprites));
    if(numSprites > 0)
    {
        unsigned int* sprites = new unsigned int[numSprites];
        int* numValuesIgnored = new int[numSprites];
        file.read((char*)sprites, sizeof(unsigned int) * numSprites);
        file.read((char*)numValuesIgnored, sizeof(int) * numSprites);
        for(size_t i = 0; i < numSprites; i++)
        {
            node->sprites.push_back(Sprite(nullptr, sprites[i]));
            node->sprites.back().numValuesIgnored = numValuesIgnored[i];
        }


        delete[] sprites;
        delete[] numValuesIgnored;
    }

    unsigned int level;
    file.read((char*)&level, sizeof(level));
    node->level = level;

    unsigned short numChildren;
    file.read((char*)&numChildren, sizeof(numChildren));

    if(numChildren > 0)
    {
        unsigned char* children = new unsigned char[numChildren];
        file.read((char*)children, sizeof(unsigned char) * numChildren);
        for(size_t i = 0; i < numChildren; i++)
        {
            auto it = node->children.insert(std::make_pair(children[i], NodePtr(new Node(node->level + 1))));
            loadFromBinaryFile(it.first->second, file);
        }

        delete[] children;
    }
}


void ImageTree::writeToBinaryFile(const NodePtr& node, std::ostream& file) const
{
    unsigned int* sprites = new unsigned int[node->sprites.size()];
    int* numValuesIgnored = new int[node->sprites.size()];
    size_t i = 0;
    for(const Sprite& sprite : node->sprites)
    {
        sprites[i] = sprite.id;
        numValuesIgnored[i] = sprite.numValuesIgnored;

        i++;
    }

    unsigned int numSprites = node->sprites.size();


    file.write((char*)&numSprites, sizeof(numSprites));
    file.write((char*)sprites, sizeof(unsigned int) * node->sprites.size());
    file.write((char*)numValuesIgnored, sizeof(int) * node->sprites.size());
    delete[] sprites;
    delete[] numValuesIgnored;

    unsigned int level = node->level;
    file.write((char*)&level, sizeof(level));

    unsigned short numChildren = node->children.size();
    file.write((char*)&numChildren, sizeof(numChildren));

    unsigned char* children = new unsigned char[node->children.size()];
    i = 0;
    for(const auto& pair : node->children)
        children[i++] = pair.first;

    file.write((char*)children, sizeof(unsigned char) * node->children.size());
    delete[] children;

    for(const auto& pair : node->children)
        writeToBinaryFile(pair.second, file);
}



void ImageTree::insert(NodePtr& node, Sprite sprite)
{
    if(node->level >= sprite.pixels->size())
    {
        node->sprites.push_back(sprite);
        return;
    }


    if(!node->sprites.empty())
    {
        node->sprites.push_back(sprite);

        for(auto it = node->sprites.begin(); it != node->sprites.end();)
        {
            if(node->level < it->pixels->size())
            {
                auto child = node->children.insert(std::make_pair(it->pixels->operator[](node->level), NodePtr(new Node(node->level + 1))));
                insert(child.first->second, *it);

                node->sprites.erase(it++);
            }
            else
                it++;
        }
    }
    else
    {
        if(node->children.empty())
        {
            node->sprites.push_back(sprite);
            return;
        }
        else
        {
            auto it = node->children.insert(std::make_pair(sprite.pixels->operator[](node->level), NodePtr(new Node(node->level + 1))));
            insert(it.first->second, sprite);
        }
    }
}

size_t ImageTree::getSize() const
{
    return getSize(mRoot);
}

size_t ImageTree::getSize(const NodePtr& node) const
{
    size_t size = 0;
    for(const auto& pair : node->children)
        size += getSize(pair.second);

    size += node->sprites.size();

    return size;
}

bool ImageTree::find(const std::vector<unsigned char>& pixels, std::list<size_t>& ids) const
{
    Node* node = mRoot.get();
    for(size_t i = node->level; i < pixels.size(); i = node->level)
    {
        auto it = node->children.find(pixels[i]);

        if(it == node->children.end())
        {
            break;
        }

        node = it->second.get();
    }

    bool foundMatch = false;
    for(const Sprite& sprite : node->sprites)
    {
        const int numValuesIgnored = pixels.size() - node->level;

        if(sprite.numValuesIgnored == numValuesIgnored)
        {
            ids.push_back(sprite.id);
            foundMatch = true;
        }
    }

    if(!foundMatch)
        return false;

    return true;
}


void ImageTree::writeToBinaryFile(std::string filePath) const
{
    std::ofstream file(filePath, std::ios::binary);
    writeToBinaryFile(mRoot, file);
    file.close();
}

std::list<unsigned char> ImageTree::trace(size_t id) const
{
    std::list<unsigned char> breadcrumbs;
    bool isFound = false;
    trace(mRoot, id, isFound, breadcrumbs);

    return breadcrumbs;
}

void ImageTree::trace(const NodePtr& node, size_t id, bool& isFound, std::list<unsigned char>& breadcrumbs) const
{
    for(const Sprite& sprite : node->sprites)
        if(sprite.id == id)
        {
            isFound = true;
            return;
        }

    for(const auto& pair : node->children)
    {
        trace(pair.second, id, isFound, breadcrumbs);
        if(isFound)
        {
            breadcrumbs.push_front(pair.first);
            return;
        }
    }
}
