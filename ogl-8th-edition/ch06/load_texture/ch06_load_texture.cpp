//
//  ch06_load_texture.cpp
//  ogl-8th-edition
//
//  Created by Chen Kevin on 4/14/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "vermilion.h"
#include "CKApp.hpp"
#include "shader.hpp"
using namespace Loader;

class LoadTextureApplication: public CKApplication {
    
public:
    using CKApplication::CKApplication;
    
    void initialize() {
        shader = new Shader("static_texture.vertex", "static_texture.fragment");
        shader->use();
        
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        
        static const GLfloat quad_data[] = {
             1.0f, -1.0f,
            -1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f,
            
             0.0f,  0.0f,
             1.0f,  0.0f,
             1.0f,  1.0f,
             0.0f,  1.0f
        };
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
        
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(8 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        vglImageData image;
        
        texture = vglLoadTexture("test.dds", 0, &image);
        glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        vglUnloadImage(&image);
    }
    
    void display() {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_CULL_FACE);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    void terminate() {
        glUseProgram(0);
        glDeleteProgram(shader->program);
        glDeleteTextures(1, &texture);
        glDeleteBuffers(1, &vertex_buffer);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteVertexArrays(1, &vertex_array);
        
    }
    
    void resize(int width, int height) {}
    
private:
    Shader *shader;
    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint texture;
};

int main(const int argc, const char *argv[]) {
    
    LoadTextureApplication app = LoadTextureApplication("Red Book - Chapter 06 Load Texture");
    app.run();
    return 0;
}