//
//  ch03_instancing3.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 4/3/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "CKApp.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vbm.h"

using namespace Loader;
using namespace glm;

#define INSTANCE_COUNT 100

class Instancing3Application: public CKApplication {
    
public:
    
    using CKApplication::CKApplication;
    
    virtual void initialize() {
        shader = new Shader("instancing3.vertex", "instancing3.fragment");
        shader->use();
        
        view_matrix_location = glGetUniformLocation(shader->program, "view_matrix");
        projection_matrix_location = glGetUniformLocation(shader->program, "projection_matrix");
        
        // Set up the TBO samplers
        GLint color_tbo_location = glGetUniformLocation(shader->program, "color_texture_buffer_objects");
        GLint model_matrix_tbo_location = glGetUniformLocation(shader->program, "model_matrix_texture_buffer_objects");
        
        // Set them to the right texture unit indices
        glUniform1i(color_tbo_location, 0);
        glUniform1i(model_matrix_tbo_location, 1);
        
        // Load the object
        object.LoadFromVBM("armadillo_low.vbm", 0, 1, 2);
        
        /**
         *  Now we set up the TBOs for the instance colors and the model matrices
         *
         *  First, create the TBO to store colors, bind a buffer to it and initialize its format.
         *  The buffer has previously been created ans sized to store one vec4 per-instance.
         */
        glGenTextures(1, &color_texture_buffer_objects);
        glBindTexture(GL_TEXTURE_BUFFER, color_texture_buffer_objects);
        
        vec4 colors[INSTANCE_COUNT];
        for (int n = 0; n < INSTANCE_COUNT; n++) {
            float a = float(n) / 4.0f;
            float b = float(n) / 5.0f;
            float c = float(n) / 6.0f;
            
            colors[n][0] = 0.5f + 0.25f * (sinf(a + 1.0f) + 1.0f);
            colors[n][1] = 0.5f + 0.25f * (sinf(b + 2.0f) + 1.0f);
            colors[n][2] = 0.5f + 0.25f * (sinf(c + 3.0f) + 1.0f);
            colors[n][3] = 1.0f;
        }
        
        // Create the buffer, initialize it and attach it to the buffer texture
        glGenBuffers(1, &color_buffer);
        glBindBuffer(GL_TEXTURE_BUFFER, color_buffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, color_buffer);
        
        /**
         *  Now do the same thing with a TBO for the model matrices.
         *  The buffer object has been created and sized to store one mat4 per-instance.
         */
        glGenTextures(1, &model_matrix_texture_buffer_objects);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, model_matrix_texture_buffer_objects);
        
        glGenBuffers(1, &model_matrix_buffer);
        glBindBuffer(GL_TEXTURE_BUFFER, model_matrix_buffer);
        glBufferData(GL_TEXTURE_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, model_matrix_buffer);
        glActiveTexture(GL_TEXTURE0);
    }
    
    virtual void display() {
        float t = float(glfwGetTimerValue() & 0x3FFF) / float(0x3FFF);
        mat4 matrices[INSTANCE_COUNT];
        
        for (int i = 0; i < INSTANCE_COUNT; i++) {
            float a = 50.0f * float(i) / 4.0f;
            float b = 50.0f * float(i) / 5.0f;
            float c = 50.0f * float(i) / 6.0f;
            
            matrices[i] = rotate(mat4(1.0f), a + t * 360.0f * float(M_PI) / 180.0f, vec3(1.0f, 0.0f, 0.0f))
            * rotate(mat4(1.0f), b + t * 360.0f * float(M_PI) / 180.0f, vec3(0.0f, 1.0f, 0.0f))
            * rotate(mat4(1.0f), c + t * 360.0f * float(M_PI) / 180.0f, vec3(0.0f, 0.0f, 1.0f))
            * translate(mat4(1.0f), vec3(10.f + a, 40.0f + b, 50.0f + c));
        }
        
        glBindBuffer(GL_TEXTURE_BUFFER, model_matrix_buffer);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(matrices), matrices, GL_DYNAMIC_DRAW);
        
        // Clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Setup
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        shader->use();
        
        mat4 view_matrix(translate(mat4(1.0f), vec3(0.0f, 0.0f, -1500.0f)) * rotate(mat4(1.0f), t * 360.0f * float(M_PI) / 180.f, vec3(0.0f, 1.0f, 0.0f)));
        mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.f));
        
        glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, value_ptr(view_matrix));
        glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, value_ptr(projection_matrix));
        
        object.Render(0, INSTANCE_COUNT);
        
    }
    
    virtual void terminate() {
        glUseProgram(0);
        glDeleteProgram(shader->program);
        glDeleteBuffers(1, &color_buffer);
        glDeleteBuffers(1, &model_matrix_buffer);
    }
    virtual void resize(int width, int height) {}
    
private:
    Shader *shader;
    GLuint view_matrix_location;
    GLuint projection_matrix_location;
    GLuint color_buffer;
    GLuint model_matrix_buffer;
    GLuint color_texture_buffer_objects;
    GLuint model_matrix_texture_buffer_objects;
    
    VBObject object;
};


int main(int argc, const char *argv[]) {
    Instancing3Application app = Instancing3Application("Red Book - Chapter 03 gl_InstanceID Example");
    app.run();
    
    return 0;
}