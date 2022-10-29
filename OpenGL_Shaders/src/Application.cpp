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

void updateImput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
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
             -50.f,  -50.f,  0.0f, 0.0f,
              300.f, -50.f,  1.0f, 0.0f,              
              300.f,  300.f, 1.0f, 1.0f,
             -50.f,   300.f, 0.0f, 1.0f
        };        

        unsigned int indices[] = //index buffer
        {
            0, 1, 2,
            3, 2, 0
        };
        //GL OPTIONS
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        GLCall(glEnable(GL_DEPTH_TEST)); //checks pixel distance.
        //GLCall(glEnable(GL_CULL_FACE)); //doesnt draw abstructed vertesies. 
        GLCall(glCullFace(GL_BACK));
        GLCall(glFrontFace(GL_CCW));

        GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));        

        
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2); 
        layout.Push<float>(2);
        VertexArray va;
        va.AddBuffer(vb, layout);        

        IndexBuffer ib(indices, 6);     
                

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
       

        Texture texture("res/textures/container.jpg");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        
        va.Unbind();
        vb.Unbind();
        ib.Unbind();  
        shader.Unbind();

        Renderer renderer;  

        glm::mat4 modelMatrix(1.f);
        glm::vec3 translationA(300, 200, 0);        
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.f, 0.f, 1.f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f));


        glm::mat4 proj = glm::ortho(0.f, 900.f, 0.f, 700.f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 mvp = proj * view * modelMatrix;
                
        while (!glfwWindowShouldClose(window))
        {   
            GLCall(glfwPollEvents());
            renderer.Clear();
            updateImput(window);                      

            shader.Bind();
            shader.SetUniformMat4f("u_MVP", mvp);

            renderer.Draw(va, ib, shader);

            //end draw
            GLCall(glfwSwapBuffers(window)); //prevents screan tearing.
            GLCall(glFlush());            
        }

       
    }

    glfwTerminate();
    return 0;
}