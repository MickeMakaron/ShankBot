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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/SequenceTree.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <sstream>
///////////////////////////////////
#include <iostream>

SequenceTree::SequenceTree(const std::vector<std::vector<size_t>>& sequences, const std::vector<unsigned int>& ids)
: mRoot(new Node(0))
{
    if(sequences.size() != ids.size())
        SB_THROW("Element count does not match ID count.");

    for(size_t i = 0; i < sequences.size(); i++)
    {
        if(!sequences[i].empty())
            insert(mRoot, Element(&sequences[i], ids[i]));
    }

    removeEmptyNodes();
//    computeIgnoredValues();
    forEachNode(mRoot, [](NodePtr& node)
    {
        for(Element& element : node->elements)
        {
            element.numValuesIgnored = element.sequence->size() - node->level;
//            std::cout << "values ignored " << element.id << ": " << element.numValuesIgnored << std::endl;
        }
    });
}

void SequenceTree::removeEmptyNodes()
{
    forEachNode(mRoot, [](NodePtr& node)
    {
        for(auto& pair : node->children)
        {
            NodePtr& child = pair.second;
            while(child->children.size() == 1 && child->elements.empty())
            {
                Node* pointerToGrandChild = child->children.begin()->second.release();
                child.reset(pointerToGrandChild);
            }
        }
    });
}

void SequenceTree::forEachNode(NodePtr& node, std::function<void(NodePtr& node)> function)
{
    function(node);
    for(auto& child : node->children)
        forEachNode(child.second, function);
}

SequenceTree::SequenceTree(std::string filePath)
: mRoot(new Node(0))
{
    loadFromBinaryFile(filePath);
}

void SequenceTree::loadFromBinaryFile(std::string filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    loadFromBinaryFile(mRoot, file);

    file.close();
}

void SequenceTree::loadFromBinaryFile(NodePtr& node, std::istream& file)
{
    unsigned int numElements;
    file.read((char*)&numElements, sizeof(numElements));
    if(numElements > 0)
    {
        unsigned int* elements = new unsigned int[numElements];
        int* numValuesIgnored = new int[numElements];
        file.read((char*)elements, sizeof(unsigned int) * numElements);
        file.read((char*)numValuesIgnored, sizeof(int) * numElements);
        for(size_t i = 0; i < numElements; i++)
        {
            node->elements.push_back(Element(nullptr, elements[i]));
            node->elements.back().numValuesIgnored = numValuesIgnored[i];
        }


        delete[] elements;
        delete[] numValuesIgnored;
    }

    unsigned int level;
    file.read((char*)&level, sizeof(level));
    node->level = level;

    unsigned short numChildren;
    file.read((char*)&numChildren, sizeof(numChildren));

    if(numChildren > 0)
    {
        size_t* children = new size_t[numChildren];
        file.read((char*)children, sizeof(size_t) * numChildren);
        for(size_t i = 0; i < numChildren; i++)
        {
            auto it = node->children.insert(std::make_pair(children[i], NodePtr(new Node(node->level + 1))));
            loadFromBinaryFile(it.first->second, file);
        }

        delete[] children;
    }
}


void SequenceTree::writeToBinaryFile(const NodePtr& node, std::ostream& file) const
{
    unsigned int* elements = new unsigned int[node->elements.size()];
    int* numValuesIgnored = new int[node->elements.size()];
    size_t i = 0;
    for(const Element& element : node->elements)
    {
        elements[i] = element.id;
        numValuesIgnored[i] = element.numValuesIgnored;

        i++;
    }

    unsigned int numElements = node->elements.size();


    file.write((char*)&numElements, sizeof(numElements));
    file.write((char*)elements, sizeof(unsigned int) * node->elements.size());
    file.write((char*)numValuesIgnored, sizeof(int) * node->elements.size());
    delete[] elements;
    delete[] numValuesIgnored;

    unsigned int level = node->level;
    file.write((char*)&level, sizeof(level));

    unsigned short numChildren = node->children.size();
    file.write((char*)&numChildren, sizeof(numChildren));

    size_t* children = new size_t[node->children.size()];
    i = 0;
    for(const auto& pair : node->children)
        children[i++] = pair.first;

    file.write((char*)children, sizeof(size_t) * node->children.size());
    delete[] children;

    for(const auto& pair : node->children)
        writeToBinaryFile(pair.second, file);
}



void SequenceTree::insert(NodePtr& node, Element element)
{
    if(node->level >= element.sequence->size())
    {
        node->elements.push_back(element);
        return;
    }


    if(!node->elements.empty())
    {
        node->elements.push_back(element);

        for(auto it = node->elements.begin(); it != node->elements.end();)
        {
            if(node->level < it->sequence->size())
            {
                if(node->level > 16000)
                    std::cout << "Sequence tree getting deep... Level at " << node->level << ". Did you insert identical sequences?" << std::endl;

                auto child = node->children.insert(std::make_pair(it->sequence->operator[](node->level), NodePtr(new Node(node->level + 1))));
                insert(child.first->second, *it);

                node->elements.erase(it++);
            }
            else
                it++;
        }
    }
    else
    {
        if(node->children.empty())
        {
            node->elements.push_back(element);
            return;
        }
        else
        {
            auto it = node->children.insert(std::make_pair(element.sequence->operator[](node->level), NodePtr(new Node(node->level + 1))));
            insert(it.first->second, element);
        }
    }
}

size_t SequenceTree::getSize() const
{
    return getSize(mRoot);
}

size_t SequenceTree::getSize(const NodePtr& node) const
{
    size_t size = 0;
    for(const auto& pair : node->children)
        size += getSize(pair.second);

    size += node->elements.size();

    return size;
}

bool SequenceTree::find(const std::vector<size_t>& sequence, std::list<unsigned int>& ids) const
{
    Node* node = mRoot.get();
    for(size_t i = node->level; i < sequence.size(); i = node->level)
    {
        auto it = node->children.find(sequence[i]);

        if(it == node->children.end())
        {
            break;
        }

        node = it->second.get();
    }

    bool foundMatch = false;
    for(const Element& element : node->elements)
    {
        const int numValuesIgnored = sequence.size() - node->level;

        if(element.numValuesIgnored == numValuesIgnored)
        {
            ids.push_back(element.id);
            foundMatch = true;
        }
    }

    if(!foundMatch)
        return false;

    return true;
}


void SequenceTree::writeToBinaryFile(std::string filePath) const
{
    std::ofstream file(filePath, std::ios::binary);
    writeToBinaryFile(mRoot, file);
    file.close();
}

std::list<size_t> SequenceTree::trace(unsigned int id) const
{
    std::list<size_t> breadcrumbs;
    bool isFound = false;
    trace(mRoot, id, isFound, breadcrumbs);

    return breadcrumbs;
}

void SequenceTree::trace(const NodePtr& node, unsigned int id, bool& isFound, std::list<size_t>& breadcrumbs) const
{
    for(const Element& element : node->elements)
        if(element.id == id)
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
