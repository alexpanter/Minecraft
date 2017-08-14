
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
#include <cassert>


#define SURFACE_MESH_TOP      1
#define SURFACE_MESH_BACK     2
#define SURFACE_MESH_LEFT     4
#define SURFACE_MESH_RIGHT    8
#define SURFACE_MESH_FRONT   16
#define SURFACE_MESH_BOTTOM  32
#define SURFACE_MESH_CHECKED 64


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
        surface_mesh = 0;
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

        for(int i = 0; i < _height * _width * _depth; i++)
        {
            _blocks[i] = _block_t(BLOCK_TYPE_NONE);
        }

        // testing that the game world is _actually_ initially empty!
        for(int i = 0; i < _width; i++)
        {
            for(int j = 0; j < _height; j++)
            {
                for(int k = 0; k < _depth; k++)
                {
                    assert(_blocks[get_array_position(i, j, k)].type == BLOCK_TYPE_NONE);
                }
            }
        }

        BufferVertexData();
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
        _blocks[index] = _block_t(type);
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

    // get_position_* (source, destination)
    // will copy modified data from source to destination
    inline void get_position_above(const glm::ivec3* pos, glm::ivec3* above)
    {
        above->x = pos->x;
        above->y = pos->y + 1;
        above->z = pos->z;
    }
    inline void get_position_below(const glm::ivec3* pos, glm::ivec3* below)
    {
        below->x = pos->x;
        below->y = pos->y - 1;
        below->z = pos->z;
    }
    inline void get_position_front(const glm::ivec3* pos, glm::ivec3* front)
    {
        front->x = pos->x;
        front->y = pos->y;
        front->z = pos->z - 1;
    }
    inline void get_position_behind(const glm::ivec3* pos, glm::ivec3* behind)
    {
        behind->x = pos->x;
        behind->y = pos->y;
        behind->z = pos->z + 1;
    }
    inline void get_position_left(const glm::ivec3* pos, glm::ivec3* left)
    {
        left->x = pos->x - 1;
        left->y = pos->y;
        left->z = pos->z;
    }
    inline void get_position_right(const glm::ivec3* pos, glm::ivec3* right)
    {
        right->x = pos->x + 1;
        right->y = pos->y;
        right->z = pos->z;
    }

    void print_ivec3(glm::ivec3* vec)
    {
        std::cout << "(" << vec->x << "," << vec->y << ","
                  << vec->z << ")" << std::endl;
    }

    // generate surface mesh from the specified starting position
    void GenerateSurfaceMesh(int x = 0, int y = 0, int z = 0)
    {
        DataStructures::CircularQueueStruct<std::pair<_block_t, glm::ivec3> >
            queue(_width*_height*_depth);
        int position = get_array_position(x, y, z);
        queue.Enqueue(std::make_pair(_blocks[position], glm::ivec3(x,y,z)));

        while(!(queue.IsEmpty()))
        {
            std::cout << std::endl << "queue iteration" << std::endl;
            std::pair<_block_t, glm::ivec3> block = queue.Dequeue();

            int pos = get_array_position(block.second.x, block.second.y, block.second.z);
            std::cout << "pos: "; print_ivec3(&block.second);

            _blocks[pos].surface_mesh |= SURFACE_MESH_CHECKED;
            glm::ivec3 dest(0, 0, 0);

            // check all 6 neighbor blocks:

            // left
            get_position_left(&block.second, &dest);
            int left = get_array_position(dest.x, dest.y, dest.z);
            std::cout << "left: "; print_ivec3(&dest);

            if((dest.x >= 0) && _blocks[left].type != BLOCK_TYPE_NONE)
            {
                std::cout << "found block left of" << std::endl;
                _blocks[pos].surface_mesh &= (~SURFACE_MESH_LEFT);
                if(!(_blocks[left].surface_mesh & SURFACE_MESH_CHECKED))
                {
                    std::cout << "adding left" << std::endl;
                    queue.Enqueue(std::make_pair(_blocks[left], dest));
                }
            }

            // right
            get_position_right(&block.second, &dest);
            int right = get_array_position(dest.x, dest.y, dest.z);
            std::cout << "right: "; print_ivec3(&dest);

            if((dest.x < _width) && _blocks[right].type != BLOCK_TYPE_NONE)
            {
                std::cout << "found block right of" << std::endl;
                _blocks[pos].surface_mesh &= (~SURFACE_MESH_RIGHT);
                if(!(_blocks[right].surface_mesh & SURFACE_MESH_CHECKED))
                {
                    std::cout << "adding right" << std::endl;
                    queue.Enqueue(std::make_pair(_blocks[right], dest));
                }
            }

            // above
            get_position_above(&block.second, &dest);
            int above = get_array_position(dest.x, dest.y, dest.z);
            std::cout << "above: "; print_ivec3(&dest);

            if((dest.y < _height) && _blocks[above].type != BLOCK_TYPE_NONE)
            {
                std::cout << "found block above" << std::endl;
                _blocks[pos].surface_mesh &= (~SURFACE_MESH_TOP);
                if(!(_blocks[above].surface_mesh & SURFACE_MESH_CHECKED))
                {
                    std::cout << "adding above" << std::endl;
                    queue.Enqueue(std::make_pair(_blocks[above], dest));
                }
            }

            // below
            get_position_below(&block.second, &dest);
            int below = get_array_position(dest.x, dest.y, dest.z);
            std::cout << "below: "; print_ivec3(&dest);

            if((dest.y >= 0) && _blocks[below].type != BLOCK_TYPE_NONE)
            {
                std::cout << "found block below" << std::endl;
                _blocks[pos].surface_mesh &= (~SURFACE_MESH_BOTTOM);
                if(!(_blocks[below].surface_mesh & SURFACE_MESH_CHECKED))
                {
                    std::cout << "adding below" << std::endl;
                    queue.Enqueue(std::make_pair(_blocks[below], dest));
                }
            }

            // behind
            get_position_behind(&block.second, &dest);
            int behind = get_array_position(dest.x, dest.y, dest.z);
            std::cout << "behind: "; print_ivec3(&dest);

            if((dest.z < _depth) && _blocks[behind].type != BLOCK_TYPE_NONE)
            {
                std::cout << "found block behind" << std::endl;
                _blocks[pos].surface_mesh &= (~SURFACE_MESH_BACK);
                if(!(_blocks[behind].surface_mesh & SURFACE_MESH_CHECKED))
                {
                    std::cout << "adding behind" << std::endl;
                    queue.Enqueue(std::make_pair(_blocks[behind], dest));
                }
            }

            // front
            get_position_front(&block.second, &dest);
            int front = get_array_position(dest.x, dest.y, dest.z);
            std::cout << "front: "; print_ivec3(&dest);

            if((dest.z >= 0) && _blocks[front].type != BLOCK_TYPE_NONE)
            {
                std::cout << "found block front" << std::endl;
                _blocks[pos].surface_mesh &= (~SURFACE_MESH_FRONT);
                if(!(_blocks[front].surface_mesh & SURFACE_MESH_CHECKED))
                {
                    std::cout << "adding front" << std::endl;
                    queue.Enqueue(std::make_pair(_blocks[front], dest));
                }
            }
        }


        // for testing purposes, outcomment or delete when convinced
        // about program correctness
        // for(int x = 0; x < _width; x++)
        // {
        //     for(int y = 0; y < _height; y++)
        //     {
        //         for(int z = 0; z < _depth; z++)
        //         {
        //             std::cout << "(x,y,z) = (" << x << "," << y << "," << z
        //                       << ")  =  " << _blocks[get_array_position(x, y, z)].type
        //                       << std::endl;
        //         }
        //     }
        // }
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
                for(int k = 0; k < _depth; k++)
                {
                    _block_t* index = &_blocks[get_array_position(i, j, k)];
                    if(index->type != BLOCK_TYPE_NONE)
                    {
                        glm::mat4 model;
                        model = glm::translate(model, glm::vec3(i*size,
                                                                j*size,
                                                                -k*size));
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
