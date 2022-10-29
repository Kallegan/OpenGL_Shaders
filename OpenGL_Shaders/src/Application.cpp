#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//scales viewport buffer to new size of window.
void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH) 
{
    glViewport(0, 0, fbW, fbH);
}


int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;
    int framebufferWidth = 0;
    int framebufferHeight = 0;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGLWindow", NULL, NULL);

    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);  

    if (!window)
    {
        glfwTerminate();
        return -1;
    }
        
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //vsync / refreshrate sync.

    if (glewInit() != GLEW_OK) //has to be init after contextcurrent.
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    { //scope to close app.        

        float positions[] = //float array of positions.
        {
             600.f, 100.0f, 0.0f, 0.0f,
             1200.f, 100.0f, 1.0f, 0.0f,              
             1200.f, 200.0f, 1.0f, 1.0f,              
             600.f, 200.0f, 0.0f, 1.0f
        };        

        unsigned int indices[] = //index buffer
        {
            0, 1, 2,
            3, 2, 0
        };

        GLCall(glEnable(GL_BLEND));

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));       

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2); 
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        

        IndexBuffer ib(indices, 6);       
        
        glm::mat4 proj = glm::ortho(0.f, 1260.f, 0.f, 370.f, -1.0f, 1.0f);                 
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-300, 50, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 0)); 

        glm::mat4 mvp = proj * view * model;

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        //shader.SetUniform4f("u_Color", 0.9f, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", mvp);       
        
        va.Unbind();
        vb.Unbind();
        ib.Unbind();  
        shader.Unbind();

        Renderer renderer;        
        
        while (!glfwWindowShouldClose(window))
        {   
            //update input
            GLCall(glfwPollEvents());

            //update        
            shader.Bind();
            shader.SetUniformMat4f("u_MVP", mvp);
            Texture texture("res/textures/container.jpg");
            //Texture texture("res/textures/1.png");
            texture.Bind();
            shader.SetUniform1i("u_Texture", 0);

            //clear
            renderer.Clear();

            //draw
            renderer.Draw(va, ib, shader);
            
            //end draw
            GLCall(glfwSwapBuffers(window)); //prevents screan tearing.
            GLCall(glFlush());            
        }

       
    }

    glfwTerminate();
    return 0;
}