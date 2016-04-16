//
//  ch06_texture_wrapping.cpp
//  ogl-8th-edition
//
//  Created by Chen Kevin on 4/13/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "CKApp.hpp"
#include "shader.hpp"

using namespace Loader;

class TextureWrappingApplication: public CKApplication {
    
public:
    using CKApplication::CKApplication;
    
    void initialize() {
        shader = new Shader("static_texture.vertex", "static_texture.fragment");
        shader->use();
        
        static const GLfloat quad_data[] = {
            -0.75f, -0.75f,
             0.75f, -0.75f,
             0.75f,  0.75f,
            -0.75f,  0.75f,
            
             0.00f,  0.00f,
             4.00f,  0.00f,
             4.00f,  4.00f,
             0.00f,  4.00f
        };

        
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
        
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(8 * sizeof(float)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, 8, 8);
        
        static const unsigned char texture_data[] = {
            0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
            0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
            0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
            0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
            0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
        };
        
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0, 0,
                        8, 8,
                        GL_RED, GL_UNSIGNED_BYTE,
                        texture_data);
        
        static const GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
    }
    
    void display() {
        
        GLFWuint64 ticks = glfwGetTimerValue();
//        GLFWuint64 ticks = glfwGetTimerFrequency();
        
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_CULL_FACE);
        
        static const GLenum wrap_modes[] = {
            GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_MIRRORED_REPEAT
        };
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_modes[(ticks >> 11) & 0x3]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_modes[(ticks >> 11) & 0x3]);
        
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

int main(int argc, const char* argv[]) {
    
    TextureWrappingApplication app = TextureWrappingApplication("Red Book - Chapter 06 Texture Wrapping");
    app.run();    
    return 0;
}
