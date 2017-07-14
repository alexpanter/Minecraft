#ifndef SHADERS_H
#define SHADERS_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "fileIO.hpp"

namespace shaders
{
    // load, compile, and return a shader of the specified `type`
    GLuint loadShader(GLenum type, const char* path)
    {
        // load shader file
        std::string shader_str = fileIO::readFileContents(path);
        const char* shader_src = shader_str.c_str();

        // create shader
        GLuint shader = glCreateShader(type);

        // Compile vertex shader
        switch (type)
        {
        case GL_VERTEX_SHADER:
            std::cout << "Compiling vertex shader" << std::endl;
            break;
        case GL_GEOMETRY_SHADER:
            std::cout << "Compiling geometry shader" << std::endl;
            break;
        case GL_FRAGMENT_SHADER:
            std::cout << "Compiling fragment shader" << std::endl;
            break;
        case GL_COMPUTE_SHADER:
        case GL_TESS_CONTROL_SHADER:
        case GL_TESS_EVALUATION_SHADER:
        default:
            std::cout << "Error: Unrecognized shader type" << std::endl;
            return 0;
        }
        glShaderSource(shader, 1, &shader_src, NULL);
        glCompileShader(shader);

        // check if compilation was successful
        GLint result = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

        // if compilation did not succeed, print the error message
        if(!result)
        {
            int logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> shader_err((logLength > 1) ? logLength : 1);
            glGetShaderInfoLog(shader, logLength, NULL, &shader_err[0]);

            std::cout << &shader_err[0] << std::endl;
        }

        return shader;
    }

    // create shaders, link them together, return the linked program
    GLuint loadShadersVGF(const char* path)
    {
        std::string shader_dir = fileIO::getPlatformPath(path);
        std::string vertex (shader_dir + "vertex.shd");
        std::string geometry (shader_dir + "geometry.shd");
        std::string fragment (shader_dir + "fragment.shd");

        // retrieve all shaders
        GLuint shd_vertex = loadShader(GL_VERTEX_SHADER, vertex.c_str());
        GLuint shd_geometry = loadShader(GL_GEOMETRY_SHADER, geometry.c_str());
        GLuint shd_fragment = loadShader(GL_FRAGMENT_SHADER, fragment.c_str());

        // link the shaders together
        std::cout << "linking shader program" << std::endl;
        GLuint program = glCreateProgram();
        glAttachShader(program, shd_vertex);
        glAttachShader(program, shd_geometry);
        glAttachShader(program, shd_fragment);
        glLinkProgram(program);

        // check if linking was successful
        GLint result = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &result);

        // if linking did not succeed, print the error message
        if(!result)
        {
            int logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> programError( (logLength > 1) ? logLength : 1 );
            glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
            std::cout << &programError[0] << std::endl;
        }

        // perform cleanup
        glDeleteShader(shd_vertex);
        glDeleteShader(shd_geometry);
        glDeleteShader(shd_fragment);

        return program;
    }

    // attach a vertex and a fragment shader, and link them together in a program
    GLuint loadShadersVF(const char* path)
    {
        // retrieve file paths
        std::string shader_dir = fileIO::getPlatformPath(path);
        std::string vertex (shader_dir + "vertex.shd");
        std::string fragment (shader_dir + "fragment.shd");

        // retrieve all shaders
        GLuint shd_vertex = loadShader(GL_VERTEX_SHADER, vertex.c_str());
        GLuint shd_fragment = loadShader(GL_FRAGMENT_SHADER, fragment.c_str());

        // link the shaders together
        std::cout << "linking shader program" << std::endl;
        GLuint program = glCreateProgram();
        glAttachShader(program, shd_vertex);
        glAttachShader(program, shd_fragment);
        glLinkProgram(program);

        // check if linking was successful
        GLint result = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &result);

        // if linking did not succeed, print the error message
        if(!result)
        {
            int logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> programError( (logLength > 1) ? logLength : 1 );
            glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
            std::cout << &programError[0] << std::endl;
        }

        // perform cleanup
        glDeleteShader(shd_vertex);
        glDeleteShader(shd_fragment);

        return program;
    }

    // get location for a uniform variable for a given shader program
    GLuint getUniformLocation(GLuint shader_program, const GLchar* uniform_name)
    {
        GLint result = glGetUniformLocation(shader_program, uniform_name);
        if(result < 0)
        {
            std::cout << "Could not find uniform variable '"
                      << uniform_name << "'" << std::endl;
        }
        return result;
    }
}

#endif // SHADERS_H
