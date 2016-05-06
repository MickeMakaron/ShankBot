///////////////////////////////////
// Internal ShankBot headers
#include "PngImage.hpp"
///////////////////////////////////

///////////////////////////////////
// libPNG
#include <png.h>
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <stdexcept>
///////////////////////////////////


namespace GraphicsLayer
{

///////////////////////////////////

PngImage readPng(std::string filePath)
{
    FILE* image = fopen(filePath.c_str(), "rb");

    if(image == nullptr)
        throw std::runtime_error("Error reading file: '" + filePath + "'.");


    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(png == nullptr)
        throw std::runtime_error("Could not create PNG read struct.");

    png_infop info = png_create_info_struct(png);
    if(info == nullptr)
        throw std::runtime_error("Could not create PNG info struct.");


    png_infop end_info = png_create_info_struct(png);

    if(setjmp(png_jmpbuf(png)) != 0)
        throw std::runtime_error("Welp.");

    png_init_io(png, image);
    png_read_info(png, info);

    png_byte color_type = png_get_color_type(png, info);
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    png_read_update_info(png, info);

    PngImage img;
    img.width = png_get_image_width(png, info);
    img.height = png_get_image_height(png, info);
    img.bytesPerPixel = png_get_rowbytes(png,info) / img.width;
    img.pixelData = new unsigned char[img.width * img.height * img.bytesPerPixel];
    unsigned char** rows = new unsigned char*[img.height];

    for(size_t y = 0; y < img.height; y++)
        rows[y] = &img.pixelData[y * img.width * img.bytesPerPixel];

    png_read_image(png, rows);
    fclose(image);

    delete[] rows;
    png_destroy_read_struct(&png, &info, &end_info);
    png_destroy_info_struct(png, &info);
    png_destroy_info_struct(png, &end_info);
    return img;
}

///////////////////////////////////

void writePng(std::string filePath, unsigned char* pixels, size_t width, size_t height)
{
    FILE* file = fopen(filePath.c_str(), "wb");

    if(file == nullptr)
        throw std::runtime_error("Could not create file: '" + filePath + "'.");

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(png == nullptr)
        throw std::runtime_error("Could not create PNG write struct.");

    png_infop info = png_create_info_struct(png);
    if(info == nullptr)
        throw std::runtime_error("Could not create PNG info struct.");

    if(setjmp(png_jmpbuf(png)) != 0)
        throw std::runtime_error("Halp!");

    png_init_io(png, file);

    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    unsigned char** rows = new unsigned char*[height];

    for(size_t y = 0; y < height; y++)
        rows[y] = &pixels[y * width * 4];

    png_write_image(png, rows);
    png_write_end(png, nullptr);

    delete[] rows;
    png_destroy_write_struct(&png, &info);
    png_destroy_info_struct(png, &info);

    fclose(file);
}

}
