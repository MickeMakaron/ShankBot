
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glx.h"

#include <iostream>
#include <map>
#include <cstring>


#include <vector>

#include <unistd.h>

#include <dlfcn.h>
#include "PipeProtocol.hpp"

namespace GraphicsMonitor
{
    struct Texture
    {
        GLsizei width;
        GLsizei height;
        unsigned char* data;
    };

    std::map<GLuint, Texture> textures;

    size_t numGlEndCalls = 0;
    size_t numGlEndCallsNotRect = 0;
    size_t numGlEndCallsNonTexRect = 0;
    size_t numLetters = 0;

    struct Vector2f
    {
        float x;
        float y;
    };

    struct Pixel
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    std::vector<Vector2f> vertices;
    std::vector<Vector2f> texCoords;
    std::vector<Pixel> colors;

    bool isPerformingPrimitiveDrawSession = false;

    GLuint current2dTexture = 0;
    GLuint currentFramebuffer = 0;
    size_t numTextures = 0;


    size_t swapCount = 0;


    Atom tibiaRunningAtom;

    int pipeWriteFd = strtol(getenv("SHANKBOT_PIPE_WRITE"), NULL, 10);
}

///////////////////////////////////

void glGenTextures( GLsizei n, GLuint *textures )
{
    void (*originalFunc)(GLsizei, GLuint*) = (void (*)(GLsizei, GLuint*))dlsym(RTLD_NEXT, "glGenTextures");
    originalFunc(n, textures);

    GraphicsMonitor::numTextures += n;
    std::cout << "NUM TEXTURES CHANGED TO: " << GraphicsMonitor::numTextures << std::endl;
}

///////////////////////////////////

void glDeleteTextures( GLsizei n, const GLuint *textures)
{
    void (*originalFunc)(GLsizei, const GLuint*) = (void (*)(GLsizei, const GLuint*))dlsym(RTLD_NEXT, "glDeleteTextures");
    originalFunc(n, textures);

    GraphicsMonitor::numTextures -= n;
    std::cout << "NUM TEXTURES CHANGED TO: " << GraphicsMonitor::numTextures << std::endl;
}

///////////////////////////////////

void glXSwapBuffers(Display* dpy, GLXDrawable drawable)
{
    void (*originalFunc)(Display*, GLXDrawable) = (void (*)(Display*, GLXDrawable))dlsym(RTLD_NEXT, "glXSwapBuffers");

    originalFunc(dpy, drawable);
    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glXSwapBuffers 1"<< std::endl;

    std::cout << "swapping" << GraphicsMonitor::swapCount++ << std::endl;

    GraphicsMonitor::numGlEndCallsNotRect = 0;
    GraphicsMonitor::numLetters = 0;
    GraphicsMonitor::numGlEndCallsNonTexRect = 0;
    GraphicsMonitor::numGlEndCalls = 0;

    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glXSwapBuffers 2" << std::endl;
}

///////////////////////////////////

void glBindTexture(GLenum target, GLuint texture)
{
    void (*originalFunc)(GLenum, GLuint) = (void (*)(GLenum, GLuint))dlsym(RTLD_NEXT, "glBindTexture");
    originalFunc(target, texture);

    if(target != GL_TEXTURE_2D)
        std::cout << "TEXTURE NOT BOUND AS GL_TEXTURE_2D" << std::endl;

    GraphicsMonitor::current2dTexture = texture;
    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glBindTexture" << std::endl;
}

///////////////////////////////////

void glBegin(GLenum mode)
{
    if(GraphicsMonitor::isPerformingPrimitiveDrawSession)
        return;

    void (*originalFunc)(GLenum) = (void (*)(GLenum))dlsym(RTLD_NEXT, "glBegin");

    GraphicsMonitor::vertices.clear();
    GraphicsMonitor::texCoords.clear();
    GraphicsMonitor::colors.clear();
    if(mode == GL_QUADS)
    {

        GraphicsMonitor::isPerformingPrimitiveDrawSession = true;
        originalFunc(mode);
    }
}

///////////////////////////////////

void glEnd()
{
    if(!GraphicsMonitor::isPerformingPrimitiveDrawSession)
        return;

    GraphicsMonitor::isPerformingPrimitiveDrawSession = false;

    void (*originalFunc)() = (void (*)())dlsym(RTLD_NEXT, "glEnd");
    originalFunc();

    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glEnd" << std::endl;

    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[GraphicsMonitor::current2dTexture];
    const auto& vertices = GraphicsMonitor::vertices;
    const auto& texCoords = GraphicsMonitor::texCoords;

    if(vertices.size() != 4)
    {
        GraphicsMonitor::numGlEndCallsNotRect++;
    }
    else if(texCoords.size() == 4)
    {
        GraphicsMonitor::Vector2f texCoordMax;
        GraphicsMonitor::Vector2f texCoordMin;

        if(texCoords[2].x > texCoords[0].x)
        {
            texCoordMax.x = texCoords[2].x;
            texCoordMin.x = texCoords[0].x;
        }
        else
        {
            texCoordMax.x = texCoords[0].x;
            texCoordMin.x = texCoords[2].x;
        }

        if(texCoords[2].y > texCoords[0].y)
        {
            texCoordMax.y = texCoords[2].y;
            texCoordMin.y = texCoords[0].y;
        }
        else
        {
            texCoordMax.y = texCoords[0].y;
            texCoordMin.y = texCoords[2].y;
        }

        GraphicsMonitor::Vector2f vertexMax;
        GraphicsMonitor::Vector2f vertexMin;

        if(vertices[2].x > vertices[0].x)
        {
            vertexMax.x = vertices[2].x;
            vertexMin.x = vertices[0].x;
        }
        else
        {
            vertexMax.x = vertices[0].x;
            vertexMin.x = vertices[2].x;
        }

        if(vertices[2].y > vertices[0].y)
        {
            vertexMax.y = vertices[2].y;
            vertexMin.y = vertices[0].y;
        }
        else
        {
            vertexMax.y = vertices[0].y;
            vertexMin.y = vertices[2].y;
        }


        size_t width = (texCoordMax.x - texCoordMin.x) * texture.width;
        size_t height = (texCoordMax.y - texCoordMin.y) * texture.height;

        using namespace GraphicsLayer::PipeProtocol;
        Header header;
        header.packetCount = 1;
        header.type = Type::DRAW_CALL;

        DrawCall packet;
        packet.textureId = GraphicsMonitor::current2dTexture;

        if(texCoordMin.x > 0.99f || texCoordMin.y > 0.99f)
        {
            std::cout << "SOMETHING'S WRONG: " << texCoords[1].y << std::endl;

            std::cout   << "Drawing quad" << std::endl;

            std::cout << "\tVERTICES: ";
            for(GraphicsMonitor::Vector2f v : GraphicsMonitor::vertices)
            {
                std::cout << "{" << v.x << ", " << v.y << "} ";
            }

            std::cout << "\tTEXCOORDS: ";
            for(GraphicsMonitor::Vector2f v : GraphicsMonitor::texCoords)
            {
                std::cout << "{" << (int)(v.x * texture.width) << ", " << (int)(v.y * texture.height) << "} ";
            }
        }

        packet.texX = texCoordMin.x * texture.width;
        packet.texY = texCoordMin.y * texture.height;
        packet.screenX = vertexMin.x;
        packet.screenY = vertexMin.y;
        packet.width = width;
        packet.height = height;


        void* p = malloc(sizeof(Header) + sizeof(DrawCall) * header.packetCount);
        memcpy(p, &header, sizeof(Header));
        memcpy(p + sizeof(Header), &packet, sizeof(DrawCall) * header.packetCount);
        write(GraphicsMonitor::pipeWriteFd, p, sizeof(Header) + sizeof(DrawCall) * header.packetCount);

        delete p;

        if
        (
            (width == 8 && height == 8) ||
            (width == 8 && height == 10) ||
            (width == 8 && height == 16) ||
            (width == 8 && height == 14) ||
            (width == 16 && height == 16)
        )
        {
            GraphicsMonitor::numLetters++;
        }
    }
    else
    {
        GraphicsMonitor::numGlEndCallsNonTexRect++;
    }


    GraphicsMonitor::numGlEndCalls++;
}

///////////////////////////////////

void glVertex2i(GLint x,  GLint y)
{
    void (*originalFunc)(GLint, GLint) = (void (*)(GLint, GLint))dlsym(RTLD_NEXT, "glVertex2i");
    originalFunc(x, y);

    GraphicsMonitor::vertices.push_back({x, y});
}

///////////////////////////////////

void glTexCoord2d(GLdouble  s,  GLdouble  t)
{
    void (*originalFunc)(GLdouble, GLdouble) = (void (*)(GLdouble, GLdouble))dlsym(RTLD_NEXT, "glTexCoord2d");
    originalFunc(s, t);

    GraphicsMonitor::texCoords.push_back({s, t});

}

///////////////////////////////////

void glTexCoord2f(GLfloat s, GLfloat t)
{
    void (*originalFunc)(GLfloat, GLfloat) = (void (*)(GLfloat, GLfloat))dlsym(RTLD_NEXT, "glTexCoord2f");
    originalFunc(s, t);
}

///////////////////////////////////

void glColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
    void (*originalFunc)(GLubyte, GLubyte, GLubyte) = (void (*)(GLubyte, GLubyte, GLubyte))dlsym(RTLD_NEXT, "glColor3ub");

    GraphicsMonitor::colors.push_back({red, green, blue, 255});
    originalFunc(red, green, blue);
}

///////////////////////////////////

void glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
    void (*originalFunc)(GLubyte, GLubyte, GLubyte, GLubyte) = (void (*)(GLubyte, GLubyte, GLubyte, GLubyte))dlsym(RTLD_NEXT, "glColor4ub");

    GraphicsMonitor::colors.push_back({red, green, blue, alpha});
    originalFunc(red, green, blue, alpha);
}

///////////////////////////////////

void glTexImage2D( 	GLenum target,
  	GLint level,
  	GLint internalFormat,
  	GLsizei width,
  	GLsizei height,
  	GLint border,
  	GLenum format,
  	GLenum type,
  	const GLvoid * data)
{
    GLuint id = GraphicsMonitor::current2dTexture;
    std::cout << "texxing " << id << std::endl;
    GraphicsMonitor::Texture texture = {width, height, (unsigned char*)data};
    GraphicsMonitor::textures[(GLuint)id] = texture;

    void (*originalFunc)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*) = (void (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))dlsym(RTLD_NEXT, "glTexImage2D");

    originalFunc(target, level, internalFormat, width, height, border, format, type, data);
}

///////////////////////////////////

void glTexSubImage2D( 	GLenum target,
  	GLint level,
  	GLint xoffset,
  	GLint yoffset,
  	GLsizei width,
  	GLsizei height,
  	GLenum format,
  	GLenum type,
  	const GLvoid * pixels)
{
    if(width == 32 && height == 32)
    {
        using namespace GraphicsMonitor;
        using namespace GraphicsLayer::PipeProtocol;

        PixelData packet;
        memcpy(packet.pixels, pixels, 32 * 32 * 4);
        packet.textureId = GraphicsMonitor::current2dTexture;
        packet.texX = xoffset;
        packet.texY = yoffset;

        Header header;
        header.packetCount = 1;
        header.type = Type::PIXEL_DATA;

        void* p = malloc(sizeof(Header) + sizeof(PixelData) * header.packetCount);
        memcpy(p, &header, sizeof(Header));
        memcpy(p + sizeof(Header), &packet, sizeof(PixelData) * header.packetCount);

        write(GraphicsMonitor::pipeWriteFd, p, sizeof(Header) + sizeof(PixelData) * header.packetCount);
        delete p;

    }

    void (*originalFunc)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*)
    = (void (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))dlsym(RTLD_NEXT, "glTexSubImage2D");
    originalFunc(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

///////////////////////////////////

void glBindFramebufferEXT(GLenum target, GLuint framebuffer)
{
    void (*originalFunc)(GLenum, GLuint) = (void(*)(GLenum, GLuint))dlsym(RTLD_NEXT, "glBindFramebufferEXT");
    originalFunc(target, framebuffer);

    GraphicsMonitor::currentFramebuffer = framebuffer;

    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glBindFramebufferEXT" << std::endl;
}

///////////////////////////////////

Window XGetSelectionOwner(Display* d, Atom selection)
{
    if(selection == GraphicsMonitor::tibiaRunningAtom)
        return None;

    Window (*originalFunc)(Display*, Atom) = (Window (*)(Display*, Atom))dlsym(RTLD_NEXT, "XGetSelectionOwner");
    return originalFunc(d, selection);
}

///////////////////////////////////

Atom XInternAtom(Display* d, const char* name, int ifExists)
{
    Atom (*originalFunc)(Display*, const char*, int) = (Atom (*)(Display*, const char*, int))dlsym(RTLD_NEXT, "XInternAtom");

    Atom atom = originalFunc(d, name, ifExists);

    if(strcmp(name, "TIBIARUNNING") == 0)
        GraphicsMonitor::tibiaRunningAtom = atom;

    return atom;

}


////
////char* strcpy(char* __dest, const char* __src)
////{
////    char* (*originalFunc)(char*, const char*) = (char*(*)(char*, const char*))dlsym(RTLD_NEXT, "strcpy");
////
////    char* result = originalFunc(__dest, __src);
////
////    if(strcmp(__src, "TIBIARUNNING") == 0)
////    {
////        printf("::::::::::::::::::::::::::::::::::::::::::::::::");
////        return nullptr;
//////        char* src = (char*)__src;
//////        src[0] = 'A';
//////        __dest[0] = '\0';
////    }
////
//////    GraphicsMonitor::catStrings.push_back(__dest);
////
////    return result;
////}
////
////char* strncpy (char * d, const char* s, size_t n)
////{
////    char* (*originalFunc)(char*, const char*, size_t) = (char*(*)(char*, const char*, size_t))dlsym(RTLD_NEXT, "strncpy");
////
////    char* result = originalFunc(d, s, n);
////
////    const char* spr = "./Tibia.spr";
////    bool isEqual = false;
////
////    if(n < 10)
////        isEqual = false;
////    else if(s[0] == '.' && s[1] == '/' && s[2] == 'T' && s[3] == 'i' && s[7] == '.' && s[8] == 's')
////        isEqual = true;
////    else if(s[0] == '.' && s[1] == '/' && s[2] == 'T' && s[3] == 'i' && s[7] == '.' && s[8] == 'p')
////        isEqual = true;
////
////    if(!isEqual)
////        GraphicsMonitor::catStrings.push_back(d);
////
////    return result;
////}
////

//
//long int strtol(const char* c, char** endptr, int base)
//{
//    long int (*originalFunc)(const char*, char**, int) = (long int (*)(const char*, char**, int))dlsym(RTLD_NEXT, "strtol");
//
//    long int result = originalFunc(c, endptr, base);
////    if(GraphicsMonitor::strtolConversions.size() == 4 && GraphicsMonitor::strtolConversions[0] == 1 && GraphicsMonitor::strtolConversions[1] == 3 && GraphicsMonitor::strtolConversions[2] == 2 && GraphicsMonitor::strtolConversions[3] == 1)
////    if(GraphicsMonitor::strtolConversions.size() == 308)
////    if(GraphicsMonitor::strtolConversions.size() == 1)
////        result = 0;
//
//    GraphicsMonitor::strtolConversions.push_back(result);
//
//    return result;
//}




///////////////////////////////////////

//
//void writeBoundTextureToFile()
//{
//    GLint id;
//    glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
//
//    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[id];
//    unsigned char* pixels = new unsigned char[texture.width * texture.height * 3];
//
//    std::stringstream sstream;
//    sstream << "texture" << id;
//
//    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
//    writePixelsToFile(sstream.str(), pixels, texture.width, texture.height);
//
//    delete[] pixels;
//}
//
//
//// COORDINATES IN OPENGL COORDINATES
//void writeBoundTextureAreaToFile(std::string fileName, float left, float bottom, float right, float top)
//{
//    if(left > right || bottom > top)
//        std::cout << "LEFT AND BOTTOM CANNOT BE GREATER THAN RIGHT AND TOP RESPECTIVELY" << std::endl;
//
//
//    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[GraphicsMonitor::current2dTexture];
//    int width = (right - left) * texture.width;
//    int height = (top - bottom) * texture.height;
//
//    float tmp = top;
//    top = 1.f - bottom;
//    bottom = 1.f - tmp;
//
//    unsigned char* pixels = new unsigned char[width * height * 3];
//
//
//    unsigned char* textureData = new unsigned char[texture.width * texture.height * 3];
//    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
//
//    for(size_t y = bottom * texture.height, i = 0; y < top * texture.height; y++, i++)
//    {
//        memcpy(pixels + i * width * 3, textureData + y * texture.width * 3 + (int)(right * texture.width) * 3, width * 3);
//    }
//
//
//    writePixelsToFile(fileName, pixels, width, height);
//
//    delete[] pixels;
//    delete[] textureData;
//}



