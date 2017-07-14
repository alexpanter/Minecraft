
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
#include "shaders.hpp"
#include "window.hpp"
#include "texture.hpp"
#include "camera.hpp"

// STANDARD
#include <iostream>
#include <string.h>
#include <string>
#include <functional>


// FUNCTION PROTOTYPES:
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement(GLfloat deltaTime);

// VARIABLES
GLfloat size = 10.0f;
camera::BasicFPSCamera* fps_cam;

// VERTEX DATA
GLfloat vertices[] = {
    // Positions
    0.0f, 0.0f, 0.0f
};
glm::vec3 blockPositions[] = {
    glm::vec3( 0.0f, 0.0f, -size),
    glm::vec3(-size, 0.0f, -size),
    glm::vec3( size, 0.0f, -size),
    glm::vec3( 0.0f, 0.0f,  0.0f),
    glm::vec3(-size, 0.0f,  0.0f),
    glm::vec3( size, 0.0f,  0.0f),
    glm::vec3( 0.0f, 0.0f,  size),
    glm::vec3(-size, 0.0f,  size),
    glm::vec3( size, 0.0f,  size),

    glm::vec3(-size, size, -size),
    glm::vec3( 0.0f, size, -size),
    glm::vec3(-size, size,  0.0f),

    glm::vec3(-size, 2*size, -size)
};

// activated keys
bool keys[512]; // perhaps 512 is not sufficient for some keyboards
                // the guide suggests 1024 - might be better?

int main()
{
    // WINDOW
    std::string title = "Minecraft";
    window::as_ratio_t as_ratio = window::ASPECT_RATIO_4_3;
    window::WindowedWindow* win = window::create_window(title, 800, as_ratio);

    // KEY EVENTS
    glfwSetKeyCallback(win->Window(), key_callback);

    // CAMERA
    fps_cam = new camera::BasicFPSCamera(win->Window(), win->width, win->height);

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
    Texture tex0 = Texture("assets|images|block_dirt_1.png", tex_options);

    // BUFFER OBJECTS
    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the VAO first, then bind and set vertex buffer(s)
    // and attribute pointer(s)
    glBindVertexArray(VAO);

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // good practice to unbind the VAO to prevent strange bugs
    // do NOT unbind the EBO, keep it bound to this VAO
    glBindVertexArray(0);

    // enable depth testing, by using the GLFW's z-buffer
    glEnable(GL_DEPTH_TEST);

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
        GLint model_loc = glGetUniformLocation(shader, "model");
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

        // uniform block size
        glUniform1f(glGetUniformLocation(shader, "sz"), size / 2.0f);

        // drawing calls
        glBindVertexArray(VAO);
        for(GLuint i = 0; i < 13; i++) {
            glm::mat4 model;
            model = glm::translate(model, blockPositions[i]);
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_POINTS, 0, 1);
        }
        glBindVertexArray(0);

        // double-buffering
        glfwSwapBuffers(win->Window());
    }

    // do proper cleanup of any allocated resources
    delete(win);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
