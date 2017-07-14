#ifndef TIMER_HPP
#define TIMER_HPP

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

namespace timer
{
    // class for a main timer for a game loop.
    // Proper usage:
    //
    // timer::MainTimer timer;
    // while(gameisrunning) {
    //     timer.MeasureTime();
    //
    //     while(timer.ShouldUpdate()) {
    //         ... (update game logic) ...
    //         timer.UpdateTimer();
    //     }
    //
    //     ... (call render functions) ...
    //
    //     if(timer.ShouldReset()) {
    //         ... (one second has passed, might want to
    //              do something with this information) ...
    //     }
    // }

    class MainTimer
    {
    private:
        GLfloat timeLimit;
        GLfloat lastTime, timer;
        GLfloat deltaTime, nowTime;
        int frames, updates;

    public:
        MainTimer(int FPS)
        {
            timeLimit = 1.0f / (float)FPS;
            lastTime = glfwGetTime();
            timer = lastTime;
            deltaTime = 0;
            nowTime = 0;
            frames = 0; // count how many updates we can do per second
            updates = 0; // how many times `update()` gets called per second
        }
        ~MainTimer() {}

        // needs to be called every iteration of the game loop
        void MeasureTime()
        {
            nowTime = glfwGetTime();
            deltaTime += (nowTime - lastTime) / timeLimit;
            lastTime = nowTime;
            frames++;
        }

        bool ShouldUpdate()
        {
            return deltaTime >= 1.0f;
        }
        void UpdateTimer()
        {
            updates++;
            deltaTime--;
        }
        bool ShouldReset()
        {
            bool res = glfwGetTime() - timer > 1.0f;
            if(res)
            {
                timer++;
                updates = 0;
                frames = 0;
            }

            return res;
        }
        std::string GetTimeTitle()
        {
            std::string title;
            title += "FPS " + std::to_string(frames);
            title += ", UPS " + std::to_string(updates);
            return title;
        }
    };
}

#endif // TIMER_HPP
