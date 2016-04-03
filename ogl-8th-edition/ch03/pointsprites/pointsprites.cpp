//
//  pointsprites.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/31/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "pointsprites.hpp"
#include "tga.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

#define POINT_COUNT 6

static inline float random_float()
{
    float res;
    unsigned int tmp;
    static unsigned int seed = 0x13371337;
    
    seed *= 16807;
    tmp = seed ^ (seed >> 4) ^ (seed << 15);
    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;
    return (res - 1.0f);
}

void PointSpritesApplication::initialize() {
    
    
    glGenTextures(1, &sprite_texture);
    glBindTexture(GL_TEXTURE_2D, sprite_texture);
    
    TGALoader::loadTGA(&tga, "sprite.tga");
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 tga.width,
                 tga.height,
                 0,
                 tga.format,
                 GL_UNSIGNED_BYTE,
                 tga.image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
        
    program_id = LoadShaders("pointsprites.vertex", "pointsprites.fragment");
    glUseProgram(program_id);
    
    vertex_model_matrix = glGetUniformLocation(program_id, "model_matrix");
    vertex_projection_matrix = glGetUniformLocation(program_id, "projection_matrix");
    
    if (tga.image_data != NULL) {
        free(tga.image_data);
    }
    
    // A single triangle
    static vec4 *vertex_positions;
    
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, POINT_COUNT * sizeof(vec4), NULL, GL_STATIC_DRAW);
    
    vertex_positions = (vec4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    
    for (int i = 0; i < POINT_COUNT; i++) {
        vertex_positions[i] = vec4(random_float() * 2.0f - 1.0f,
                                   random_float() * 2.0f - 1.0f,
                                   random_float() * 2.0f - 1.0f,
                                   1.0f);
    }
    
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
        
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(0));
    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(sizeof(vertex_positions)));
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
}

void PointSpritesApplication::display() {
    
    float t = random_float() * 2.0f - 1.0f;
//    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    
    mat4 project_matrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 8.0f));
    glUniformMatrix4fv(vertex_projection_matrix, 1, GL_FALSE, value_ptr(project_matrix));
    
    mat4 model_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.0f))
    * rotate(mat4(1.0f), t * float(M_PI) * 2.0f, Y)
    * rotate(mat4(1.0f), t * float(M_PI) * 4.0f, Z);
    
    glUniformMatrix4fv(vertex_model_matrix, 4, GL_FALSE, value_ptr(model_matrix));
    
//    glActiveTexture(0);
//    GLuint texture = glGetUniformLocation(program_id, "sprite_texture");
//    glUniform1i(texture, 0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(32.0f);
    
    glDrawArrays(GL_POINTS, 0, POINT_COUNT);
    
    
}

void PointSpritesApplication::terminate() {
    
    glDeleteProgram(program_id);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteTextures(1, &sprite_texture);
}

void PointSpritesApplication::resize(int width, int height) {
    
}

int main(int argc, char *argv[]) {
    PointSpritesApplication *application = new PointSpritesApplication("Red Book Ch03 Point Sprites");
    application->run();
    
    return 0;
}