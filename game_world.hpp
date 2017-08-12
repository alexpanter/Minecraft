// GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/constants.hpp>

// STANDARD
#include <iostream> // std::cerr


typedef enum {
    BLOCK_TYPE_EARTH,
    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_STONE,

    // a block that does not exist, should not be used
    // for collision detection or AI routines, and
    // definitely not should be drawn.
    BLOCK_TYPE_NONE
} _block_type_t;

typedef struct _block_t {
    // default health of a block is 10. When health
    // reaches < 0 it should be destroyed.
    _block_t() : type(BLOCK_TYPE_NONE), health(0) {}
    _block_t(_block_type_t type) : type(type), health(10) {}
    _block_t(_block_type_t type, int health) : type(type), health(health) {}

    _block_type_t type;
    int health;
} _block_t;


class GameWorld
{
    // dimensions of the game world
    int _width;
    int _height;
    int _depth;

    _block_t* _blocks;

    // default vertex buffer data to satisfy OpenGL, for now..
    GLuint _VBO, _VAO;

    GLfloat _vertices[3] = {
        // Positions
        0.0f, 0.0f, 0.0f
    };

    void BufferVertexData()
    {
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        // bind the VAO first, then bind and set vertex buffer(s)
        // and attribute pointer(s)
        glBindVertexArray(_VAO);

        // vertices
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // good practice to unbind the VAO to prevent strange bugs
        // do NOT unbind the EBO, keep it bound to this VAO
        glBindVertexArray(0);
    }

    inline int get_array_position(int x, int y, int z)
    {
        return (z * _height * _width) + (y * _height) + x;
    }


public:
    GameWorld(int width, int height, int depth)
        : _width(width), _height(height), _depth(depth)
    {
        // after this initialization, all blocks will have 10 health
        // and marked as `BLOCK_TYPE_NONE'.
        _blocks = new _block_t[_height * _width * _depth];

        BufferVertexData();
    }

    ~GameWorld()
    {
        delete _blocks;
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    // return false if there is already a block at the desired entry
    //
    // TODO: block health might be set automatically within this method
    // according to block type
    bool InsertBlock(int x, int y, int z, _block_type_t type, int health = 10)
    {
        if(health < 1)
        {
            std::cerr << "Inserting block with health < 1" << std::endl;
            health = 10;
        }

        int index = get_array_position(x, y, z);
        if(_blocks[index].type != BLOCK_TYPE_NONE)
        {
            return false;
        }

        _blocks[index] = _block_t(type, health);
        return true;
    }

    bool DeleteBlock(int x, int y, int z)
    {
        int index = get_array_position(x, y, z);
        if(_blocks[index].type != BLOCK_TYPE_NONE)
        {
            return false;
        }

        // more explicit, could use constructor with empty argument list as well
        _blocks[index] = _block_t(BLOCK_TYPE_NONE, 0);
        return true;
    }

    // TODO: return something player can add to inventory..
    void DecreaseBlockHealth(int x, int y, int z, int health_decrease)
    {
        if(health_decrease < 0)
        {
            std::cerr << "Decreasing block health with a negative amount!"
                      << std::endl;
            health_decrease = 0;
        }
        int index = get_array_position(x, y, z);
        _blocks[index].health -= health_decrease;

        if(_blocks[index].health < 0)
        {
            _blocks[index].type = BLOCK_TYPE_NONE;
        }
    }

    // VERY naive approach, but good enough for simple demonstration
    void DrawBlocks(GLuint shader, int size)
    {
        glBindVertexArray(_VAO);
        GLint model_loc = glGetUniformLocation(shader, "model");

        for(int i = 0; i < _width; i++)
        {
            for(int j = 0; j < _height; j++)
            {
                for(int k = 0; k < _width; k++)
                {
                    if(_blocks[get_array_position(i, j, k)].type != BLOCK_TYPE_NONE)
                    {
                        glm::mat4 model;
                        model = glm::translate(model, glm::vec3(i*size,
                                                                j*size,
                                                                k*size));
                        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

                        glDrawArrays(GL_POINTS, 0, 1);
                    }

                }
            }
        }

        glBindVertexArray(0);
    }

    void DrawSun(GLuint shader, glm::vec3& position)
    {
        glBindVertexArray(_VAO);
        GLint model_loc = glGetUniformLocation(shader, "model");

        glm::mat4 model;
        model = glm::translate(model, position * 3.0f);
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_POINTS, 0, 1);

        glBindVertexArray(0);
    }
};
