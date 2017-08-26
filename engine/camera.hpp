#pragma once

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


namespace camera
{
    // basic first-person camera
    class BasicFPSCamera
    {
    private:
    protected:
        GLFWwindow* window;
        GLfloat window_width, window_height;

        glm::mat4 view, projection;

        glm::vec3 pos;
        glm::vec3 front;
        glm::vec3 up;

        GLfloat fov, max_fov, min_fov;

        GLfloat lastX;
        GLfloat lastY;

        GLfloat yaw;
        GLfloat pitch;

        GLfloat view_distance;

    public:
        ~BasicFPSCamera() {}
        BasicFPSCamera(GLFWwindow* win, GLfloat width, GLfloat height)
        {
            window = win;
            window_width = width;
            window_height = height;

            pos = glm::vec3(0.0f, 0.0f, 10.0f);
            front = glm::vec3(0.0f, 0.0f, -1.0f);
            up = glm::vec3(0.0f, 1.0f, 0.0f);

            fov = 45.0f;
            max_fov = 45.0f;
            min_fov = 44.3f;

            lastX = width / 2.0f;
            lastY = height / 2.0f;

            yaw = -90.f;
            pitch = 0.0f;

            view_distance = 400.0f;
        }

        const glm::mat4* ViewMatrix() { return &view; }
        const glm::mat4* ProjectionMatrix() { return &projection; }

        void SetInitialPosition(GLfloat x, GLfloat y, GLfloat z)
        {
            pos = glm::vec3(x, y, z);
        }

        void SetInitialDirection(GLfloat x, GLfloat y, GLfloat z)
        {
        }

        void SetViewDistance(GLfloat dist)
        {
            view_distance = dist;
        }

        void MouseCallback(GLFWwindow* window, double xpos, double ypos)
        {
            GLfloat xoffset = xpos - lastX;
            GLfloat yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            GLfloat sensitivity = 0.05;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw   += xoffset;
            pitch += yoffset;

            if(pitch > 89.0f)
                pitch = 89.0f;
            if(pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 new_front;
            new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            new_front.y = sin(glm::radians(pitch));
            new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(new_front);
        }

        void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            if (fov >= min_fov && fov <= max_fov) { fov -= yoffset * 0.1f; }
            if (fov <= min_fov)                   { fov = min_fov; }
            if (fov >= max_fov)                   { fov = max_fov; }
        }

        void StrafeLeft(GLfloat cameraSpeed)
        {
            pos -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
        }

        void StrafeRight(GLfloat cameraSpeed)
        {
            pos += glm::normalize(glm::cross(front, up)) * cameraSpeed;
        }

        void MoveForwards(GLfloat cameraSpeed)
        {
            pos += cameraSpeed * front;
        }

        void MoveBackwards(GLfloat cameraSpeed)
        {
            pos -= cameraSpeed * front;
        }

        // update the camera's view and projection matrices
        // according to any changes to the position vectors
        //
        // This function should be called at each iteration
        // of the game loop.
        void CalculatePosition()
        {
            view = glm::lookAt(pos, pos + front, up);
            projection = glm::perspective(fov, window_width / window_height,
                                          0.1f, view_distance);
        }
    }; // BasicFPSCamera

} // namespace camera
