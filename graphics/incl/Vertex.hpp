#ifndef GRAPHICS_LAYER_VERTEX_HPP
#define GRAPHICS_LAYER_VERTEX_HPP

namespace GraphicsLayer
{
    struct Vertex
    {
        float x;
        float y;
    };

    struct TexturedVertex : public Vertex
    {
        float texX;
        float texY;
    };

    struct ColoredVertex : public Vertex
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
}

#endif // GRAPHICS_LAYER_VERTEX_HPP
