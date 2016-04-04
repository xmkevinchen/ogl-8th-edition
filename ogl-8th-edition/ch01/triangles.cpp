//
//  triangles.cpp
//  triangles
//
//  Created by Kevin Chen on 3/20/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

#define GL_BUFFER_OFFSET(offset) ((void *)(offset))

int main(int argc, const char * argv[]) {
    
    GLFWwindow *window;
    
    /* Initialize the library */
    
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    /* Create a window mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "Triangles", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Initialize GLEW */
    if (GLEW_OK != glewInit()) {
        glfwTerminate();
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glewExperimental = true;
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    Loader::Shader *shader = new Loader::Shader("triangles.vertex", "triangles.fragment");
    
    static const GLfloat g_vertext_buffer_data[] = {
         -0.90, -0.90,  // Triangle 1
          0.85, -0.90,
         -0.90,  0.85,
          0.90, -0.85,  // Triangle 2
          0.90,  0.90,
         -0.85,  0.90
    };
    
    
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertext_buffer_data), g_vertext_buffer_data, GL_STATIC_DRAW);
    
    
    
    glVertexAttribPointer(
                          0,                //
                          2,                // size
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          GL_BUFFER_OFFSET(0)
                          );
    glEnableVertexAttribArray(0);
    
    /* Loop until the user close the window */
    while (!glfwWindowShouldClose(window)
           && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* Use shader */
        glUseProgram(shader->program);
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
//        glDisableVertexAttribArray(0);
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(shader->program);
    
    glfwTerminate();
    
    return 0;
}

