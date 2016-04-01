//
//  CKApp.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/30/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "CKApp.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;


void CKApplication::run() {
    
    application = this;
    
    if (!glfwInit())
    {
        exit(-1);
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    /* Create a window mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glewExperimental = true;
    
    /* Initialize GLEW */
    if (GLEW_OK != glewInit())
    {
        glfwTerminate();
        exit(-1);
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    glfwSetWindowSizeCallback(window, application->windowSizeDidChange);
    
    initialize();
    
    while (!glfwWindowShouldClose(window)
           && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        
        display();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    
    terminate();
    glfwTerminate();
}

void CKApplication::windowSizeDidChange(GLFWwindow *window, int width, int height) {
    
    glViewport(0, 0, width, height);
    application->resize(width, height);
    
}

