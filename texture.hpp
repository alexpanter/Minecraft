#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h> // glGenerateMipmap
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>
#include <string>
#include "fileIO.hpp"

#define TEX_GENERATE_MIPMAP 0x1
#define TEX_REPEAT          0x2
#define TEX_MIRRORED_REPEAT 0x4
#define TEX_CLAMP_TO_EDGE   0x8
#define TEX_CLAMP_TO_BORDER 0x16
#define TEX_NEAREST_FILTER  0x32
#define TEX_LINEAR_FILTER   0x64
#define TEX_MIXED_FILTER    0x128

#define HAS_FLAG(mask, flag) (mask & flag)

class Texture
{
private:
    GLuint texture;
    int width, height;
public:
    // simple delegating constructor
    Texture(const char* path) : Texture(path, 0) {}

    // advanced constructor - use bit mask to set texture options
    Texture(const char* path, unsigned long mask)
    {
        // create texture object
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // load the image
        std::string fullpath = fileIO::getPlatformFilePath(path);
        std::string extension = fileIO::getFileExtension(fullpath);
        unsigned char* image;
        if(extension == "jpg") {
            // fourth argument specifies the number of channels in the image:
            image = SOIL_load_image(fullpath.c_str(), &width, &height, 0,
                                    SOIL_LOAD_RGB); // can also use `SOIL_LOAD_AUTO`

            // second argument is desired mipmap level - ´0´ is base image size
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, image);
        }
        else if(extension == "png") {
            image = SOIL_load_image(fullpath.c_str(), &width, &height, 0,
                                    SOIL_LOAD_RGBA);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, image);
        }
        else {
            std::cerr << "Unknown texture file format: '" << fullpath
                      << "'" << std::endl;
            return;
        }
        if(image == NULL) {
            std::cerr << "Could not read image '" << fullpath << "'" << std::endl;
        }

        // set options
        if(HAS_FLAG(mask, TEX_GENERATE_MIPMAP)) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        if(HAS_FLAG(mask, TEX_REPEAT)) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else if(HAS_FLAG(mask, TEX_MIRRORED_REPEAT)) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        }
        else if(HAS_FLAG(mask, TEX_CLAMP_TO_BORDER)) {
            // WARNING
            // for some reason, this functionality is currently broken
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        if(HAS_FLAG(mask, TEX_NEAREST_FILTER)) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else if(HAS_FLAG(mask, TEX_LINEAR_FILTER)) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else if(HAS_FLAG(mask, TEX_MIXED_FILTER)) {
            // will use nearest neighbor filtering when scaled down
            // (more pixelated look), and use linear filtering when scaling
            // up (more realistic/blurry look)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        // cleanup
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // destructor
    ~Texture() {}

    GLuint GetTexture()
    {
        return texture;
    }
};


#endif // TEXTURE_HPP
