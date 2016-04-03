//
//  CKApp.hpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/30/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#ifndef CKApp_hpp
#define CKApp_hpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class CKApplication {
    
public:
    
    CKApplication(const char *title = "Red Book - OpenGL Application", int width = 512, int height = 512);
    
    virtual void initialize() = 0;
    virtual void display() = 0;
    virtual void terminate() = 0;
    virtual void resize(int width, int height) = 0;
    
    void run();
    
protected:
    GLFWwindow *window;
    const char *title;
    float aspect;
    int width;
    int height;
    
    static void windowSizeDidChange(GLFWwindow *window, int width, int height);
    
    
};

static CKApplication *application;

#endif /* CKApp_hpp */
