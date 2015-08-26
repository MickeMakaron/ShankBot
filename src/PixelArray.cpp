////////////////////////////////////////////////
// ShankBot internal headers
#include "PixelArray.hpp"
////////////////////////////////////////////////

////////////////////////////////////////////////
// STD - C++ Standard Library
#include <stdexcept>
////////////////////////////////////////////////

PixelArray::PixelArray(char* pixels, unsigned int size)
: mPixels(pixels)
, mSize(size)
{
}

//////////////////////////////////////

PixelArray::~PixelArray()
{
    delete[] mPixels;
}

//////////////////////////////////////

const char& PixelArray::operator[](unsigned int i) const
{
    if(i < mSize)
        return mPixels[i];
    else
        throw std::out_of_range("Index out of range.");
}

//////////////////////////////////////

char* PixelArray::getPixels()
{
    return mPixels;
}

//////////////////////////////////////

unsigned int PixelArray::getSize() const
{
    return mSize;
}
