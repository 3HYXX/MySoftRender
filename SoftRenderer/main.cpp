#include<windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FrameBuffer.h"
#include "Shader.h"
#include "Vertex.h"
#include "Draw.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



Draw* dw;
Camera* camera;
int fps = 0;

void ShowFps(GLFWwindow* window) {
    while (1) {
        Sleep(1000);
        //在窗口标题显示
        std::string text = "SoftRender fps:" + std::to_string(fps);
        glfwSetWindowTitle(window, text.c_str());
        fps = 0;
    }
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);



    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SoftRenderer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //
    std::thread t(ShowFps, window);
    t.detach();

    dw = new Draw(SCR_WIDTH, SCR_HEIGHT);
    dw->Init();

    camera = new Camera();

    Texture texture("texture\\container.jpg");
    dw->setTexture(&texture);
    Mesh Box = CreateBox(glm::vec3(0.0, 0.0, 0.0), 0.5);
    dw->setProjectMatrix(GetPerspectiveMatrix(glm::radians(60.0f), (float)SCR_WIDTH / SCR_HEIGHT, -0.3f, -100));
    dw->setViewMatrix(GetViewMatrix(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)));
    float angle = 0.0f;
    
    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        fps++;
        // input
        // -----
        processInput(window);

        dw->setProjectMatrix(camera->PerspectiveMatrix());
        dw->setViewMatrix(camera->ViewMatrix());

        // render
        // ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        dw->ClearBuffer(glm::vec4(0, 0, 0, 0));
        dw->setModelMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0, 1.0, 0.0)));
        dw->DrawMesh(Box);
        dw->Show();
        //angle += 1.0f;


        
        //dw->DrawTriangle(V1, V2, V3);
        //dw->DrawMesh(mesh);
        //dw->Show();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera->RotatePitch(5.0);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera->RotateYaw(5.0);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera->RotateYaw(-5.0);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera->RotatePitch(-5.0);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->MoveForward(0.1);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->MoveForward(-0.1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->MoveRight(-0.1);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->MoveRight(0.1);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->MoveUp(0.1);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->MoveUp(-0.1);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

