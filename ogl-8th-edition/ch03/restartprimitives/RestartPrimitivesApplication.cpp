//
//  RestartPrimitivesApplication.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 4/3/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#include "RestartPrimitivesApplication.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define degree_to_radian(x) (float(x) * float(M_PI) / 180.0f)

static const GLfloat cube_positions[] = {
    -1.0f, -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f, 1.0f,
    1.0f, -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 1.0f,
    1.0f,  1.0f, -1.0f, 1.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

static const GLfloat cube_colors[] = {
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f
};

static const GLushort cube_indices[] = {
    0, 1, 2, 3, 6, 7, 4, 5,
    0xFFFF,
    2, 6, 0, 4, 1, 5, 3, 7
};


void RestartPrimitivesApplication::initialize() {
    
    
    program_id = LoadShaders("primitive_restart.vertex", "primitive_restart.fragment");
    glUseProgram(program_id);
    
    shader_model_matrix = glGetUniformLocation(program_id, "model_matrix");
    shader_projection_matrix = glGetUniformLocation(program_id, "projection_matrix");
    
    glGenBuffers(1, &cube_element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &cube_vertex_array);
    glBindVertexArray(cube_vertex_array);
    
    glGenBuffers(1, &cube_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions) + sizeof(cube_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_positions), cube_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_positions), sizeof(cube_colors), cube_colors);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(sizeof(cube_positions)));    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    
}

void RestartPrimitivesApplication::display() {
    
    float t = float(glfwGetTimerValue() & 0x1FFF) / float(0x1FFF);
    
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f))
    * glm::rotate(glm::mat4(1.0f), degree_to_radian(t * 360.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    * glm::rotate(glm::mat4(1.0f), degree_to_radian(t * 720.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    glm::mat4 projection_matrix = glm::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.f);
    
    glUniformMatrix4fv(shader_model_matrix, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(shader_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    
    glBindVertexArray(cube_vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer);
    
#if 1
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFF);
    glDrawElements(GL_TRIANGLE_STRIP, sizeof(cube_indices), GL_UNSIGNED_SHORT, (GLvoid *)(0));
#else
    
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, (const GLvoid *)(0));
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, (const GLvoid *)(9 * sizeof(GLushort)));

    
#endif
    
}

void RestartPrimitivesApplication::terminate() {
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteProgram(program_id);
    glDeleteVertexArrays(1, &cube_vertex_array);
    glDeleteBuffers(1, &cube_element_buffer);
    glDeleteBuffers(1, &cube_vertex_buffer);
    
}

void RestartPrimitivesApplication::resize(int width, int height) {
    
}

int main(int argc, const char *argv[]) {
    
    RestartPrimitivesApplication *app = new RestartPrimitivesApplication("Red Book - Chapter 03 Restarting Primitives");
    app->run();
    
    return 0;
}
