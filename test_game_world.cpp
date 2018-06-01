#include "./game_world.hpp"
#include <string>
#include <cassert>

typedef struct vec3 {
    vec3(int x, int y, int z) : x(x), y(y), z(z) {}
    int x, y, z;
} vec3;
static int errors = 0;

void print_error_world(vec3 world, std::string error)
{
    std::cerr << "Error in world (" << world.x << "," << world.y << "," << world.z
              << "): " << error << std::endl;
    errors++;
}

void test_111()
{
    vec3 v(1,1,1);
    GameWorld w(v.x, v.y, v.z);


}

int _GetArrayPos(vec3 pos, vec3 dim)
{
    if(pos.x < 0 || pos.x >= dim.x) { return -1; }
    if(pos.y < 0 || pos.y >= dim.y) { return -1; }
    if(pos.z < 0 || pos.z >= dim.z) { return -1; }
    return (pos.z * dim.y * dim.x) + (pos.y * dim.x) + pos.x;
}

void test_get_array_position()
{
    vec3 v1(1,1,1); // world size
    assert(_GetArrayPos(vec3(0,0,0), v1) == 0);

    vec3 v2(2,1,1);
    assert(_GetArrayPos(vec3(0,0,0), v2) == 0);
    assert(_GetArrayPos(vec3(1,0,0), v2) == 1);

    vec3 v3(1,2,1);
    assert(_GetArrayPos(vec3(0,0,0), v3) == 0);
    //assert(_GetArrayPos(vec3(0,1,0), v3) == 1);
    std::cout << _GetArrayPos(vec3(0,1,0), v3) << std::endl;
}

int main()
{
    test_get_array_position();
    return 0;
}
