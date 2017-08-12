
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

// CUSTOM
#include "engine/shaders.hpp"
#include "engine/window.hpp"
#include "engine/texture.hpp"
#include "engine/camera.hpp"
#include "game_world.hpp"



// FUNCTION PROTOTYPES:
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement(GLfloat deltaTime);

// VARIABLES
camera::BasicFPSCamera* fps_cam;

// GAME WORLD
#define WIDTH  10
#define HEIGHT 5
#define DEPTH  5
int block_size = 10;

void create_world(GameWorld* world)
{
    // plain field of grass:
    for(int x = 0; x < WIDTH; x++)
    {
        for(int z = 0; z < DEPTH; z++)
        {
            world->InsertBlock(x, 0, z, BLOCK_TYPE_GRASS);
        }
    }
}

// activated keys
bool keys[512]; // perhaps 512 is not sufficient for some keyboards
                // the guide suggests 1024 - might be better?

int main()
{
    // WINDOW
    std::string title = "Minecraft";
    window::as_ratio_t as_ratio = window::ASPECT_RATIO_4_3;
    window::WindowedWindow* win = window::create_window(title, 800, as_ratio);

    // GAME WORLD
    GameWorld* game_world = new GameWorld(WIDTH, HEIGHT, DEPTH);

    create_world(game_world);

    // KEY EVENTS
    glfwSetKeyCallback(win->Window(), key_callback);

    // CAMERA
    fps_cam = new camera::BasicFPSCamera(win->Window(), win->width, win->height);
    fps_cam->SetInitialPosition(0.0f, block_size * 1.0f, block_size * 1.0f);
    fps_cam->SetInitialDirection(0.0f, 0.0f, 0.0f);

    // CURSOR
    glfwSetCursorPosCallback(win->Window(), mouse_callback);
    glfwSetScrollCallback(win->Window(), scroll_callback);

    // HIDE CURSOR
    glfwSetInputMode(win->Window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // SHADERS
    GLuint shader = shaders::loadShadersVGF("shaders|default_block_shader");
    glUseProgram(shader);

    // TEXTURES
    unsigned long tex_options = TEX_GENERATE_MIPMAP | TEX_MIXED_FILTER;
    Texture tex0 = Texture("assets|images|grass|side.png", tex_options);
    Texture tex1 = Texture("assets|images|grass|top.png", tex_options);
    Texture tex2 = Texture("assets|images|grass|bottom.png", tex_options);

    // enable depth testing, by using the GLFW's z-buffer
    glEnable(GL_DEPTH_TEST);

    // enable multisample for MSAA
    glEnable(GL_MULTISAMPLE);

    // TIMER
    GLfloat deltaTime = 0.0f;
    GLfloat lastTime = 0.0f;

    // the 'game loop'
    // forcing GLFW to continuously draw the window
    while(!glfwWindowShouldClose(win->Window()))
    {
        // update the timer
        GLfloat nowTime = glfwGetTime();
        deltaTime = nowTime - lastTime;
        lastTime = nowTime;

        // check incoming events
        glfwPollEvents();

        // change movement logic
        do_movement(deltaTime);

        // render at maximum possible frames:
        // clear the screen to prevent artifacts from the previous iteration
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // calling rendering functions...
        glUseProgram(shader);

        // update camera
        fps_cam->CalculatePosition();

        // model/view/projection uniform matrices
        GLint view_loc = glGetUniformLocation(shader, "view");
        GLint projection_loc = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE,
                           glm::value_ptr(*fps_cam->ViewMatrix()));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE,
                           glm::value_ptr(*fps_cam->ProjectionMatrix()));

        // uniform textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0.GetTexture());
        glUniform1i(glGetUniformLocation(shader, "texture0"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1.GetTexture());
        glUniform1i(glGetUniformLocation(shader, "texture1"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex2.GetTexture());
        glUniform1i(glGetUniformLocation(shader, "texture2"), 2);

        // uniform block size
        glUniform1f(glGetUniformLocation(shader, "sz"), (GLfloat)block_size / 2.0f);

        // drawing calls
        game_world->DrawBlocks(shader, block_size);

        // double-buffering
        glfwSwapBuffers(win->Window());
    }

    // do proper cleanup of any allocated resources
    delete(win);
    delete game_world;
    glfwTerminate();

    // exiting the application
    return 0;
}


void do_movement(GLfloat deltaTime)
{
    GLfloat cameraSpeed = 20.0f * deltaTime;

    if(keys[GLFW_KEY_W]) {
        fps_cam->MoveForwards(cameraSpeed);
    }
    else if(keys[GLFW_KEY_S]) {
        fps_cam->MoveBackwards(cameraSpeed);
    }
    if(keys[GLFW_KEY_A]) {
        fps_cam->StrafeLeft(cameraSpeed);
    }
    else if(keys[GLFW_KEY_D]) {
        fps_cam->StrafeRight(cameraSpeed);
    }
}

// handle user input on the keyboard
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    static unsigned short wireframe = 0;

    if(action == GLFW_PRESS)
    {
        if(key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }
        else {
            keys[key] = true;
        }
    }

    if(action == GLFW_RELEASE)
    {
        if(key == GLFW_KEY_SPACE) {
            switch(wireframe)
            {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                wireframe = 1;
                break;
            default:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                wireframe = 0;
                break;
            }
        }
        else {
            keys[key] = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    fps_cam->MouseCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fps_cam->MouseScrollCallback(window, xoffset, yoffset);
}
