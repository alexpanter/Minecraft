
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

// CUSTOM
#include "./circular_queue_struct.hpp"

// STANDARD
#include <iostream> // std::cerr
#include <utility> // std::pair


#define SURFACE_MESH_TOP     1
#define SURFACE_MESH_BACK    2
#define SURFACE_MESH_LEFT    4
#define SURFACE_MESH_RIGHT   8
#define SURFACE_MESH_FRONT  16
#define SURFACE_MESH_BOTTOM 32


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
    _block_t() : type(BLOCK_TYPE_NONE), surface_mesh(0) {}
    _block_t(_block_type_t type) : type(type)
    {
        surface_mesh =
            SURFACE_MESH_TOP   | SURFACE_MESH_BACK   |
            SURFACE_MESH_LEFT  | SURFACE_MESH_RIGHT  |
            SURFACE_MESH_FRONT | SURFACE_MESH_BOTTOM ;
    }
    _block_t(_block_type_t type, int surface_mesh)
        : type(type), surface_mesh(surface_mesh) {}

    _block_type_t type;
    int surface_mesh;
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
        // after this initialization all blocks will be marked as `BLOCK_TYPE_NONE'
        _blocks = new _block_t[_height * _width * _depth];

        BufferVertexData();
        GenerateSurfaceMesh();
    }

    ~GameWorld()
    {
        delete _blocks;
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    // return false if there is already a block at the desired entry
    bool InsertBlock(int x, int y, int z, _block_type_t type)
    {
        int index = get_array_position(x, y, z);
        if(_blocks[index].type != BLOCK_TYPE_NONE)
        {
            return false;
        }

        // TODO: calculate surface mesh compared to neighbor blocks
        _blocks[index] = _block_t(type, 0);
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
        _blocks[index].type = BLOCK_TYPE_NONE;
        _blocks[index].surface_mesh = 0;
        return true;
    }

    // generate surface mesh from the specified starting position
    void GenerateSurfaceMesh(int x = 0, int y = 0, int z = 0)
    {
        DataStructures::CircularQueueStruct<std::pair<_block_t, int> >
            queue(_width*_height*_depth);
        int position = get_array_position(x, y, z);
        queue.Enqueue(std::make_pair(_blocks[position], position));

        while(!(queue.IsEmpty()))
        {
            std::pair<_block_t, int> block = queue.Dequeue();

            // check all 6 neighbor blocks:
            // top
            if(_blocks[block.second].type == BLOCK_TYPE_NONE)
            {}

            // back

            // left

            // right

            // front

            // bottom

        }
    }

    // VERY naive approach, but good enough for simple demonstration
    void DrawBlocks(GLuint shader, int size)
    {
        glBindVertexArray(_VAO);
        GLint model_loc = glGetUniformLocation(shader, "model");
        GLint s_mesh_loc = glGetUniformLocation(shader, "surfaceMesh");

        for(int i = 0; i < _width; i++)
        {
            for(int j = 0; j < _height; j++)
            {
                for(int k = 0; k < _width; k++)
                {
                    _block_t* index = &_blocks[get_array_position(i, j, k)];
                    if(index->type != BLOCK_TYPE_NONE)
                    {
                        glm::mat4 model;
                        model = glm::translate(model, glm::vec3(i*size,
                                                                j*size,
                                                                k*size));
                        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
                        glUniform1i(s_mesh_loc, index->surface_mesh);

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
