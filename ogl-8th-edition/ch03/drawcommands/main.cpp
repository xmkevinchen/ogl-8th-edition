//
//  main.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/28/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"

#define GL_BUFFER_OFFSET(offset) ((void *)(offset))


static float aspect = 1.0f;

void reshape(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    aspect = float(height) / float(width);
}

int main(int argc, const char* argv[]) {
    
    GLFWwindow *window;
    
    /* Initialize the library */
    
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    /* Create a window mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "Red Book - Chapter 3 Draw Commands", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
        
    glfwSetWindowSizeCallback(window, reshape);
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    
    glewExperimental = true;
    /* Initialize GLEW */
    if (GLEW_OK != glewInit()) {
        glfwTerminate();
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    
    
    
    GLuint programID = LoadShaders("primitive_restart.vertex", "primitive_restart.fragment");
    glUseProgram(programID);
    GLuint render_model_matrix = glGetUniformLocation(programID, "model_matrix");
    GLuint render_projection_matrix = glGetUniformLocation(programID, "projection_matrix");
    
        // 8 corners of a cube, side length 2, centered on the origin
    static const GLfloat g_vertext_positions[] =
    {
        -1.0f, -1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
    };
    
        // Color for each vertex
    static const GLfloat g_vertex_colors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f
    };
    
        // Indices for the triangle strips
    static const GLushort g_vertex_indices[] =
    {
        0, 1, 2
    };
    
    /* set up element array buffer */
    GLuint element_buffer;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_vertex_indices), g_vertex_indices, GL_STATIC_DRAW);
    
    /* setup the vertex attributes */
    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    
    
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertext_positions) + sizeof(g_vertex_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_vertext_positions), g_vertext_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(g_vertext_positions), sizeof(g_vertex_colors), g_vertex_colors);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(vertex_array);
    
    glVertexAttribPointer(0,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          GL_BUFFER_OFFSET(0));
    
    glVertexAttribPointer(1,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          GL_BUFFER_OFFSET(sizeof(g_vertext_positions)));
    
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    /* Loop until the user close the window */
    while (!glfwWindowShouldClose(window)
           && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        
        glm::mat4 model_matrix;
        
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);
        
//        glm::mat4 model_matrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -0.5))
//                               * glm::rotate(glm::mat4(1.0f), t * 360.0f, Y)
//                               * glm::rotate(glm::mat4(1.0f), t * 720.0f, Z));
        
        
        glm::mat4 projection_matrix(glm::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));
        glUniformMatrix4fv(render_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));
        

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(0xFFFFFFFF);
        glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, GL_BUFFER_OFFSET(0));
        
            // Draw Arrays...
        model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-3.0f, 0.0f, -5.0f));
        glUniformMatrix4fv(render_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
            // DrawElements
        model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-1.0f, 0.0f, -5.0f));
        glUniformMatrix4fv(render_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);
        
            // DrawElementsBaseVertex
        model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f, 0.0f, -5.0f));
        glUniformMatrix4fv(render_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);
        
            // DrawArraysInstanced
        model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(3.0f, 0.0f, -5.0f));
        glUniformMatrix4fv(render_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &element_buffer);
    
    glfwTerminate();
    
    return 0;
}



