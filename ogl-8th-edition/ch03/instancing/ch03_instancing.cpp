//
//  ch03_instancing.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/30/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vbm.h"
#include "CKApp.hpp"
#include "shader.hpp"

using namespace glm;

#define INSTANCE_COUNT 200

class InstancingApp: public CKApplication {
    
public:
    
    using CKApplication::CKApplication;
    
    virtual void initialize();
    virtual void display();
    virtual void terminate();
    virtual void resize(int width, int height);
    
protected:
    GLuint programID;
    GLuint vertex_model_matrix;
    GLuint vertex_projection_matrix;
    VBObject object;
    
    GLuint weight_vertex_buffer;
    GLuint color_vertex_buffer;
    
};

void InstancingApp::initialize() {
    
    programID = LoadShaders("instancing.vertex", "instancing.fragment");
    glUseProgram(programID);
    
    vertex_model_matrix = glGetUniformLocation(programID, "model_matrix");
    vertex_projection_matrix = glGetUniformLocation(programID, "projection_matrix");
    
    object.LoadFromVBM("armadillo_low.vbm", 0, 1, 2);
    
    object.BindVertexArray();
    
    vec4 colors[INSTANCE_COUNT];
    
    for (int i = 0; i < INSTANCE_COUNT; i++) {
        
        float a = float(i) / 4.0f;
        float b = float(i) / 5.0f;
        float c = float(i) / 6.0f;
        
        colors[i][0] = 0.5 * (sinf(a + 1.0f) + 1.0f);
        colors[i][1] = 0.5 * (sinf(b + 2.0f) + 1.0f);
        colors[i][2] = 0.5 * (sinf(c + 3.0f) + 1.0f);
        colors[i][3] = 1.0f;
        
    }
    
    /* Create and allocate the VBO to hold the weights
     * Notice that we use the 'colors' array as the initial data,
     * but only because we know it's the same size
     */
    
    glGenBuffers(1, &weight_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, weight_vertex_buffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
    
    /* Here is the instanced vertex attribute - set the divisor */
    glVertexAttribDivisor(3, 1);
    // It's otherwise the same as any other vetex attribute - set the pointer and enable it
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void *)(0));
    glEnableVertexAttribArray(3);
    
    glGenBuffers(1, &color_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    
    glVertexAttribDivisor(4, 1);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void *)(0));
    glEnableVertexAttribArray(4);
    
    glBindVertexArray(0);
    

    
}

void InstancingApp::display() {
    
    float t = float(glfwGetTimerValue() & 0x3FFFF) / float(0x3FFFF);
    
    vec4 weights[INSTANCE_COUNT];
    for (int n = 0; n < INSTANCE_COUNT; n++) {
        float a = float(n) / 4.0f;
        float b = float(n) / 5.0f;
        float c = float(n) / 6.0f;
        
        weights[n][0] = 0.5f * (sinf(t * 6.28318531f * 8.0f + a) + 1.0f);
        weights[n][1] = 0.5f * (sinf(t * 6.28318531f * 26.0f + b) + 1.0f);
        weights[n][2] = 0.5f * (sinf(t * 6.28318531f * 21.0f + c) + 1.0f);
        weights[n][3] = 0.5f * (sinf(t * 6.28318531f * 13.0f + a + b) + 1.0f);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, weight_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(weights), weights, GL_DYNAMIC_DRAW);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glUseProgram(programID);
    
    mat4 model_matrix[4];
    for (int n = 0; n < 4; n++) {
        model_matrix[n] = scale(mat4(1.0f), vec3(5.0f))
        * rotate(mat4(1.0f),t * (float)(M_PI*2) * 40.0f + float(n + 1) * 29.0f, vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f),t * (float)(M_PI*2) * 20.0f + float(n + 1) * 35.0f, vec3(0.0f, 0.0f, 1.0f))
        * rotate(mat4(1.0f),t * (float)(M_PI*2) * 30.0f + float(n + 1) * 67.0f, vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), vec3((float)n * 10.0f - 15.0f, 0.0f, 0.0f))
        * scale(mat4(1.0f), vec3(0.01f));
    }
    
    glUniformMatrix4fv(vertex_model_matrix, 4, GL_FALSE, value_ptr(model_matrix[0]));
    
    mat4 projection_matrix = frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f)
    * translate(mat4(1.0f), vec3(0.0f, 0.0f, -100.0f));
    
    glUniformMatrix4fv(vertex_projection_matrix, 1, GL_FALSE, value_ptr(projection_matrix));
    
    object.Render(0, INSTANCE_COUNT);
}

void InstancingApp::terminate() {
    
    glDeleteProgram(programID);
    glDeleteBuffers(1, &weight_vertex_buffer);
    glDeleteBuffers(1, &color_vertex_buffer);
    object.Free();
}

void InstancingApp::resize(int width, int height) {}


int main(int argc, const char* argv[]) {
    
    InstancingApp *app = new InstancingApp("Red Book - Ch03 Instancing", 256, 256);
    app->run();
    
    return 0;
    
}
