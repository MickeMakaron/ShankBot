///////////////////////////////////
// Internal ShankBot headers
#include "TileBuffer.hpp"

///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstring>
#include <sstream>
///////////////////////////////////

using namespace GraphicsLayer;

///////////////////////////////////

TileBuffer::TileBuffer()
{
    memset(mPixels, 0, NUM_BYTES);
}

///////////////////////////////////

void TileBuffer::setTile(PipeProtocol::PixelData pixels)
{
    size_t x = pixels.texX * TileBuffer::BYTES_PER_PIXEL;
    size_t y = pixels.texY * TileBuffer::BYTES_PER_PIXEL;
    for(size_t iSrc = 0, iDest = x + y * TileBuffer::WIDTH; iSrc < 32 * 32 * TileBuffer::BYTES_PER_PIXEL; iSrc += 32 * TileBuffer::BYTES_PER_PIXEL, iDest += TileBuffer::WIDTH * TileBuffer::BYTES_PER_PIXEL)
        memcpy(&mPixels[iDest], &pixels.pixels[iSrc], 32 * TileBuffer::BYTES_PER_PIXEL);
}

unsigned char* TileBuffer::getTile(size_t x, size_t y) const
{
    return getPixels(x, y, 32, 32);
}

///////////////////////////////////

unsigned char* TileBuffer::getPixels(size_t x, size_t y, size_t width, size_t height) const
{
    if(x + width > TileBuffer::WIDTH || y + height > TileBuffer::HEIGHT)
    {
        std::stringstream sstream;
        sstream << "Pixel area out of range. An area at position " << x << "x" << y << " with dimensions " << width << "x" << height << " was requested.";
        throw std::runtime_error(sstream.str());
    }


    const size_t NUM_BYTES = width * height * TileBuffer::BYTES_PER_PIXEL;

    unsigned char* pixels = new unsigned char[NUM_BYTES];
    x *= TileBuffer::BYTES_PER_PIXEL;
    y *= TileBuffer::BYTES_PER_PIXEL;

    for(size_t iDest = 0, iSrc = x + y * TileBuffer::WIDTH; iDest < NUM_BYTES; iDest += width * TileBuffer::BYTES_PER_PIXEL, iSrc += TileBuffer::WIDTH * TileBuffer::BYTES_PER_PIXEL)
        memcpy(&pixels[iDest], &mPixels[iSrc], width * TileBuffer::BYTES_PER_PIXEL);

    return pixels;
}

///////////////////////////////////

const unsigned char* TileBuffer::getPixels() const
{
    return mPixels;
}

///////////////////////////////////

size_t TileBuffer::coordsToIndex(size_t x, size_t y)
{
    return x + y * TileBuffer::WIDTH;
}
