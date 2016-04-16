//
//  ch06_volumn_texture.cpp
//  ogl-8th-edition
//
//  Created by Chen Kevin on 4/16/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "CKApp.hpp"
#include "shader.hpp"
#include "vermilion.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Loader;

class VolumnTextureApplication: public CKApplication {
    
public:
    using CKApplication::CKApplication;
    
    ~VolumnTextureApplication() {
        if (shader != NULL) {
            delete shader;
            shader = NULL;
        }
    }
    
    void initialize() {
        shader = new Shader("volumn_texture.vertex", "volumn_texture.fragment");
        shader->use();
        
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
        
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
        
        glGenVertexArrays(1, &vertex);
        glBindVertexArray(vertex);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(8 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        rotate = glGetUniformLocation(shader->program, "tc_rotate");
        
        vglImageData image;
        tex = vglLoadTexture("cloud.dds", 0, &image);
        glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        vglUnloadImage(&image);
        
    }
    
    void display() {
        static const uint64_t start_time = glfwGetTimerValue();
        float t = float(glfwGetTimerValue() - start_time) / float(0x3FFF);
        
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_CULL_FACE);
        
        static glm::vec3 X(1.0, 0.0, 0.0);
        static glm::vec3 Y(0.0, 1.0, 0.0);
        static glm::vec3 Z(0.0, 0.0, 1.0);
        
        glm::mat4 tc_rotate = glm::rotate(glm::mat4(1.0f), (t * 170.0f) * 180.0f / float(M_PI), X)
                            * glm::rotate(glm::mat4(1.0f), (t * 137.0f) * 180.0f / float(M_PI), Y)
                            * glm::rotate(glm::mat4(1.0f), (t *  93.0f) * 180.0f / float(M_PI), Z);
        glUniformMatrix4fv(rotate, 1, GL_FALSE, glm::value_ptr(tc_rotate));
        
        glBindVertexArray(vertex);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    void terminate() {
        glUseProgram(0);
        glDeleteProgram(shader->program);
        glDeleteBuffers(1, &buffer);
        glDeleteVertexArrays(1, &vertex);
        glDeleteTextures(1, &tex);
    }
    
    void resize(int width, int height) {}
    
private:
    Shader *shader;
    GLuint vertex;
    GLuint buffer;
    GLuint rotate;
    GLuint tex;
};

int main(const int argc, const char *argv[]) {
    
    VolumnTextureApplication app = VolumnTextureApplication("Red Book - Chapter 06 Volumn Texture");
    app.run();
    
    return 0;
}
