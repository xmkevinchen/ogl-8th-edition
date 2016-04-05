//
//  gouraud.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 4/4/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include <iostream>
using namespace std;

#include "CKApp.hpp"
#include "shader.hpp"
using namespace Loader;

class GouraudApplication: public CKApplication {
    
public:
    using CKApplication::CKApplication;
    
    virtual void initialize() {
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        
        struct vertex_data {
            GLubyte color[4];
            GLfloat position[4];
        };
        
        vertex_data vertices[6] = {
            {{255,   0,   0, 255}, {-0.90, -0.90}},
            {{  0, 255,   0, 255}, { 0.85, -0.90}},
            {{  0,   0, 255, 255}, {-0.90,  0.85}},
            {{ 10,  10,  10, 255}, { 0.90, -0.85}},
            {{100, 100, 100, 255}, { 0.90,  0.90}},
            {{255, 255, 255, 255}, {-0.85,  0.90}}
        };
        
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        shader = new Shader("gouraud.vertex", "gouraud.fragment");
        shader->use();
        
        GLint vColor = glGetAttribLocation(shader->program, "vColor");
        GLint vPosition = glGetAttribLocation(shader->program, "vPosition");
        
        glVertexAttribPointer(vColor,
                              4,
                              GL_UNSIGNED_BYTE,
                              GL_TRUE,
                              sizeof(vertex_data),
                              (GLvoid *)0);
        glVertexAttribPointer(vPosition,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_data),
                              (GLvoid *)(sizeof(vertices[0].color)));
        glEnableVertexAttribArray(vColor);
        glEnableVertexAttribArray(vPosition);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
    }
    
    virtual void display() {
        
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
    }
    
    virtual void terminate() {
        glUseProgram(0);
        glDeleteProgram(shader->program);
        glDeleteVertexArrays(1, &vertex_array);
        glDeleteBuffers(1, &vertex_buffer);
    }
    
    virtual void resize(int width, int height) {}
    
private:
    GLuint vertex_array;
    GLuint vertex_buffer;
    Shader *shader;
    
};


int main(int argc, const char *argv[]) {
    
    GouraudApplication app = GouraudApplication("Red Book - Chapter 04 Gourand");
    app.run();
    
    return 0;
    
}

