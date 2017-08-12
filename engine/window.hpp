#ifndef WINDOW_HPP
#define WINDOW_HPP

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


namespace window
{
    // define width/height aspect ratio for a window
    typedef enum {
        ASPECT_RATIO_16_9,  // HD video
        ASPECT_RATIO_4_3,   // standard monitor
        ASPECT_RATIO_3_2,   // classic film
        ASPECT_RATIO_21_9,  // cinemascope
        ASPECT_RATIO_1_1    // quadratic window
    } as_ratio_t;

    int get_aspect_ratio_height(int width, as_ratio_t aspect)
    {
        int height;

        switch(aspect) {
        case ASPECT_RATIO_16_9:
            height = width / 16 * 9;
            break;
        case ASPECT_RATIO_21_9:
            height = width / 21 * 9;
            break;
        case ASPECT_RATIO_3_2:
            height = width / 3 * 2;
            break;
        case ASPECT_RATIO_4_3:
            height = width / 4 * 3;
            break;
        case ASPECT_RATIO_1_1:
            height = width;
            break;
        default:
            std::cout << "Error - undefined aspect ratio!" << std::endl;
            height = 0;
        }

        return height;
    }

    class BaseWindow
    {
    protected:
        GLFWwindow* window;
        bool isMinimized;
        as_ratio_t as_ratio;
    public:
        int width, height;

        // universal destructor, de-allocating the window resources
        virtual ~BaseWindow()
        {
            glfwDestroyWindow(window);
        }

        // accessing the window
        GLFWwindow* Window()
        {
            return window;
        }

        // this function may only be called from the main thread
        void SetAspectRatio(as_ratio_t new_as_ratio)
        {
            height = get_aspect_ratio_height(width, new_as_ratio);
            glfwSetWindowSize(window, width, height);
        }
    };

    class WindowedWindow : public BaseWindow
    {
    private:
    public:
        std::string title;

        WindowedWindow(GLFWwindow* window)
        {
            this->window = window;
        }
        ~WindowedWindow() {}
    };

    class FullscreenWindow : public BaseWindow
    {
    private:
    public:
        FullscreenWindow(GLFWwindow* window)
        {
            this->window = window;
        }
        ~FullscreenWindow() {}

        GLFWwindow* Window()
        {
            return window;
        }
    };

    void init_GLFW(void)
    {
        if(glfwInit() == GL_FALSE)
        {
            std::cout << "Could not initialize GLFW!" << std::endl;
        }
    }

    void init_GLEW(void)
    {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cout << "Failed to initialize GLEW" << std::endl;
        }
    }

    void set_window_hints(void)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
    }

    WindowedWindow* create_window(std::string title, int width, as_ratio_t aspect)
    {
        init_GLFW();
        set_window_hints();

        // height according to specified aspect ratio
        int height = get_aspect_ratio_height(width, aspect);

        // parameters: (height, width, title, monitor, share)
        GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if(window == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return NULL;
        }
        // activating the window within the current thread
        glfwMakeContextCurrent(window);

        // create the wrapper object
        WindowedWindow* win = new WindowedWindow(window);

        // tell OpenGL the size of the rendering window
        // this information is retrieved from GLFW
        glfwGetFramebufferSize(win->Window(), &win->width, &win->height);

        // parameters: bottom-left corner and the dimensions of the rendering window
        glViewport(0, 0, win->width, win->height);
        win->title = title;

        init_GLEW();
        return win;
    }

    FullscreenWindow* create_window_fullscreen(as_ratio_t as_ratio)
    {
        init_GLFW();
        set_window_hints();

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // height according to specified aspect ratio
        int height = get_aspect_ratio_height(mode->width, as_ratio);

        // parameters: (height, width, title, monitor, share)
        GLFWwindow* window = glfwCreateWindow(mode->width, height,
                                              "Fullscreen Window", monitor, NULL);
        if(window == NULL) {
            std::cerr << "Failed to create full screen GLFW window" << std::endl;
            glfwTerminate();
            return NULL;
        }
        // activating the window within the current thread
        glfwMakeContextCurrent(window);

        // create the wrapper object
        FullscreenWindow* win = new FullscreenWindow(window);

        // tell OpenGL the size of the rendering window
        // this information is retrieved from GLFW
        glfwGetFramebufferSize(win->Window(), &win->width, &win->height);

        // parameters: bottom-left corner and the dimensions of the rendering window
        glViewport(0, 0, win->width, win->height);

        init_GLEW();
        return win;
    }

    // WARNING - DO NOT USE:
    #ifdef WINDOW_UNSAFE
    // this function may only be called from the main thread
    void change_window_resolution(GLFWwindow* window, int width, as_ratio_t aspect)
    {
        int height = get_aspect_ratio_height(width, aspect);
        glfwSetWindowSize(window, width, height);
    }
    #endif
}

#endif // WINDOW_HPP
