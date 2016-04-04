//
//  ch03_instancing2.cpp
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

using namespace glm;
using namespace Loader;

#define INSTANCE_COUNT 200

class Instancing2Application: public CKApplication {
    
private:
    Shader *shader;
    GLuint vertex_view_matrix;
    GLuint vertex_projection_matrix;
    VBObject object;
    GLuint vertex_color_buffer;
    GLuint vertex_model_matrix_buffer;
    
public:
    
    using CKApplication::CKApplication;
    
    virtual void initialize() {
        shader = new Shader("instancing2.vertex", "instancing2.fragment");
        shader->use();
        
        vertex_view_matrix = glGetUniformLocation(shader->program, "view_matrix");
        vertex_projection_matrix = glGetUniformLocation(shader->program, "projection_matrix");
        
        // Load the object
        object.LoadFromVBM("armadillo_low.vbm", 0, 1, 2);
        
        // Bind its vertex array object so that we can append the instanced attributes
        object.BindVertexArray();
        
        /** 
         Get the locations of the vertex attributes in 'program',
         which is the (linked) program object that we're going to be rendering with.
         Note that this sin't really necessary because we specified locations for 
         all the attributes in our vertex shader.
         This code could be made more concise by assuming the vertex attributes are
         where we asked the compiler to put them.
         */
        
//        GLuint position     = glGetAttribLocation(shader->program, "position");
//        GLuint normal       = glGetAttribLocation(shader->program, "normal");
        GLuint color        = glGetAttribLocation(shader->program, "color");
        GLuint model_matrix = glGetAttribLocation(shader->program, "model_matrix");
        
        vec4 colors[INSTANCE_COUNT];
        
        for (int i = 0; i < INSTANCE_COUNT; i++) {
            float a = float(i) / 4.0f;
            float b = float(i) / 5.0f;
            float c = float(i) / 6.0f;
            
            colors[i][0] = 0.5f + 0.25f * (sinf(a + 1.0f) + 1.0f);
            colors[i][1] = 0.5f + 0.25f * (sinf(b + 2.0f) + 1.0f);
            colors[i][2] = 0.5f + 0.25f * (sinf(c + 3.0f) + 1.0f);
            colors[i][3] = 1.0f;
        }
        
        glGenBuffers(1, &vertex_color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
        
        /**
         Now we set up the color array. 
         We want each instance of our geometry to assume a dfferent color,
         so we'll just pack colors into a buffer object and make an instanced vertex 
         attribute out of it.
         */
        glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);
        glEnableVertexAttribArray(color);
        
        /**
         *  This is the important bit... set the divisor for the color array to 1 
         *  to get OpenGL to give us a new value of 'color' per-instance rather than per-vertex
         */
        glVertexAttribDivisor(color, 1);
        
        /**
         *  Likewise, we can do the same with model matrix.
         *  Note that a matrix input to the vertex shader consumes N consecutive input locations.
         *  where N is the number of columns in the matrix.
         *  So...we have four vertex attributes to set up.
         */
        glGenBuffers(1, &vertex_model_matrix_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_model_matrix_buffer);
        glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
        
        // Loop over each column of the matrix
        for (int i = 0; i < 4; i++) {
            // Set up the vertex attribute
            glVertexAttribPointer(model_matrix + i,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(mat4),
                                  (const GLvoid *)(sizeof(vec4) * i));
            // Enable it
            glEnableVertexAttribArray(model_matrix + i);
            // Make it instanced
            glVertexAttribDivisor(model_matrix + i, 1);
        }
        
        
        // Done (unbind the object's VAO);
        glBindVertexArray(0);
        
        
    };
    
    virtual void display() {
        
        float t = float(glfwGetTimerValue() & 0x3FFF) / float(0x3FFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        glBindBuffer(GL_ARRAY_BUFFER, vertex_model_matrix_buffer);
        
        // set model matrices for each instance
        mat4 *matrice = (mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        for (int i = 0; i < INSTANCE_COUNT; i++) {
            float a = 50.0f * float(i) / 4.0f;
            float b = 50.0f * float(i) / 5.0f;
            float c = 50.0f * float(i) / 6.0f;
            
            matrice[i] = rotate(mat4(1.0f), a + t * float(M_PI) * 2.0f, vec3(1.0f, 0.0f, 0.0f))
                       * rotate(mat4(1.0f), b + t * float(M_PI) * 2.0f, vec3(0.0f, 1.0f, 0.0f))
                       * rotate(mat4(1.0f), c + t * float(M_PI) * 2.0f, vec3(0.0f, 0.0f, 1.0f))
                       * translate(mat4(1.0f), vec3(10.0f + a, 40.0f + b, 50.0f + c));
        }
        
        glUnmapBuffer(GL_ARRAY_BUFFER);
        
        shader->use();
        
        mat4 view_matrix(translate(mat4(1.0f), vec3(0.0f, 0.0f, -1500.0f))
                         * rotate(mat4(1.0f), t * float(M_PI) * 2.0f * 2.0f, vec3(0.0f, 1.0f, 0.0f))
                         );
        mat4 projection_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f));
        
        glUniformMatrix4fv(vertex_view_matrix, 1, GL_FALSE, value_ptr(view_matrix));
        glUniformMatrix4fv(vertex_projection_matrix, 1, GL_FALSE, value_ptr(projection_matrix));
        
        // Render objects
        object.Render(0, INSTANCE_COUNT);
        
        lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        
    };
    
    virtual void terminate() {
        glUseProgram(0);
        glDeleteProgram(shader->program);
        glDeleteBuffers(1, &vertex_color_buffer);
        glDeleteBuffers(1, &vertex_model_matrix_buffer);
    }
    
    virtual void resize(int width, int height) {}
    
};

int main(int argc, const char *argv[]) {
    
    Instancing2Application app = Instancing2Application("Red Book - Chapter 03 Instancing 2");
    app.run();
    return 0;
}