//
//  ch06_multiple_texture.cpp
//  ogl-8th-edition
//
//  Created by Chen Kevin on 4/16/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "CKApp.hpp"
#include "vermilion.h"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Loader;

class MultipleTextureApplication: public CKApplication {
public:
    using CKApplication::CKApplication;
    
    void initialize() {
    
        shader = new Shader("multiple_texture.vertex", "multiple_texture.fragment");
        
        static const GLfloat quad_data[] =
        {
             1.0f, -1.0f,
            -1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f,
            
             0.0f,  0.0f,
             1.0f,  0.0f,
             1.0f,  1.0f,
             0.0f,  1.0f
        };
        
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
        
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(8 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        shader->use();
        
        time = glGetUniformLocation(shader->program, "time");
        glUniform1i(glGetUniformLocation(shader->program, "tex1"), 0);
        glUniform1i(glGetUniformLocation(shader->program, "tex2"), 1);
        
        vglImageData image;
        
        tex1 = vglLoadTexture("test.dds", 0, &image);
        glBindTexture(image.target, tex1);
        glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        vglUnloadImage(&image);
        
        tex2 = vglLoadTexture("test3.dds", 0, &image);
        glBindTexture(image.target, tex2);
        vglUnloadImage(&image);
        
    }
    
    void display() {
        float t = float(glfwGetTimerValue() & 0x3FFF) / float(0x3FFF);
        
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_CULL_FACE);
                
        glUniform1f(time, t);
        
        glBindVertexArray(vertex_array);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
    }
    
    void terminate() {
        glUseProgram(0);
        glDeleteProgram(shader->program);
        glDeleteTextures(1, &tex1);
        glDeleteTextures(2, &tex2);
        glDeleteVertexArrays(1, &vertex_array);
        
    }
    
    void resize(int width, int height) {}
    
private:
    Shader *shader;
    GLuint tex1;
    GLuint tex2;
    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint time;
    
};

int main(const int argc, const char *argv[]) {
    MultipleTextureApplication app = MultipleTextureApplication("Red Book - Chapter 06 Multiple Textures");
    app.run();
    return 0;
}
