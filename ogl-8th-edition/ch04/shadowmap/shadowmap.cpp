//
//  shadowmap.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 4/5/16.
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

class ShadowMapApplication: public CKApplication {
public:
    using CKApplication::CKApplication;
    
    void initialize() {
        
        glfwGetWindowSize(window, &current_width, &current_heigth);
        
        light = new Shader("shadowmap_light.vertex", "shadowmap_light.fragment");
        light->use();
        render_light_uniforms.model_view_projection_matrix = glGetUniformLocation(light->program, "model_view_projection_matrix");
        
        scene = new Shader("shadowmap_scene.vertex", "shadowmap_scene.fragment");
        scene->use();
        render_scene_uniforms.model_matrix = glGetUniformLocation(scene->program, "model_matrix");
        render_scene_uniforms.view_matrix = glGetUniformLocation(scene->program, "view_matrix");
        render_scene_uniforms.projection_matrix = glGetUniformLocation(scene->program, "projection_matrix");
        render_scene_uniforms.shadow_matrix = glGetUniformLocation(scene->program, "shadow_matrix");
        render_scene_uniforms.light_position = glGetUniformLocation(scene->program, "light_position");
        render_scene_uniforms.material_ambient = glGetUniformLocation(scene->program, "material_ambient");
        render_scene_uniforms.material_diffuse = glGetUniformLocation(scene->program, "material_diffuse");
        render_scene_uniforms.material_specular = glGetUniformLocation(scene->program, "material_specular");
        render_scene_uniforms.material_specular_power = glGetUniformLocation(scene->program, "material_specular_power");
        
        // Set the depth texture uniform to unit 0
        glUniform1i(glGetUniformLocation(scene->program, "depth_texture"), 0);
        
        // Create a depth texture
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Create FBO to render depth into
        glGenFramebuffers(1, &depth_frame_buffer_object);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer_object);
//        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_texture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
        glDrawBuffer(GL_NONE);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // Upload geometry for the ground plane
        static const float ground_vertices[] = {
            -500.0f, -50.0f, -500.0f, 1.0f,
            -500.0f, -50.0f,  500.0f, 1.0f,
             500.0f, -50.0f,  500.0f, 1.0f,
             500.0f, -50.0f, -500.0f, 1.0f
        };
        
        static const float ground_normals[] = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };
        
        glGenVertexArrays(1, &ground_vertex_array_object);
        glBindVertexArray(ground_vertex_array_object);
        
        glGenBuffers(1, &ground_vertex_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, ground_vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_normals), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_normals), ground_normals);
        
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(ground_vertices));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        object.LoadFromVBM("armadillo_low.vbm", 0, 1, 2);
        
    }
    
    void display() {
    
        float t = float(glfwGetTimerValue() & 0xFFFF) / float(0xFFFF);
//        float t = float(glfwGetTimerFrequency());
        vec3 light_position = vec3(sin(t * 6.0f * float(M_PI)) * 300.0f,
                                   200.f,
                                   cos(t * 4.0f * float(M_PI)) * 100.0f + 250.0f);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        static const vec3 X(1.0f, 0.0f, 0.0f);
        static const vec3 Y(0.0f, 1.0f, 0.0f);
        static const vec3 Z(0.0f, 0.0f, 1.0f);
        
        // Matrices for rendering the scene
        mat4 scene_model_matrix = rotate(mat4(1.0f), t * 720.0f, Y);
        mat4 scene_view_matrix = translate(mat4(1.0), vec3(0.0f, 0.0f, -300.0f));
        mat4 scene_projection_matrix = frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 800.0f);
        const mat4 scale_bias_matrix = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
                                            vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                            vec4(0.0f, 0.0f, 0.5f, 0.0f),
                                            vec4(0.5f, 0.5f, 0.5f, 1.0f));
        
        // Matrices used when rendering from the light's position
        mat4 light_view_matrix = lookAt(light_position, vec3(0.0f), Y);
        mat4 light_projection_matrix = frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 800.0f);
        
        // Now we render from the light's position into the depth buffer.
        // Select the appropriate program
        light->use();
        glUniformMatrix4fv(render_light_uniforms.model_view_projection_matrix,
                           1,
                           GL_FALSE,
                           value_ptr(light_projection_matrix * light_view_matrix * scene_model_matrix));
        
        // Bind the 'depth only' FBO and set the viewport to the size of the depth texture
        glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer_object);
        glViewport(0, 0, 2048, 2048);
        
        // Clear
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // Enable polygon offset to resolve depth-fighting issues
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 4.0f);
        // Draw from the light's point of view
        drawScene(true);
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        // Restore the default framebuffer and field of view
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, current_width, current_heigth);
        
        // Now render from the viewer's position
        scene->use();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        // Set up all the matrices
        glUniformMatrix4fv(render_scene_uniforms.model_matrix, 1, GL_FALSE, value_ptr(scene_model_matrix));
        glUniformMatrix4fv(render_scene_uniforms.view_matrix, 1, GL_FALSE, value_ptr(scene_view_matrix));
        glUniformMatrix4fv(render_scene_uniforms.projection_matrix, 1, GL_FALSE, value_ptr(scene_projection_matrix));
        glUniformMatrix4fv(render_scene_uniforms.shadow_matrix, 1, GL_FALSE, value_ptr(scale_bias_matrix * light_projection_matrix * light_view_matrix));
        glUniformMatrix4fv(render_scene_uniforms.light_position, 1, GL_FALSE, value_ptr(light_position));
        
        // Bind the depth texture
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Draw
        drawScene(false);
        
    }
    
    void drawScene(bool depth_only) {
        // Set material properties for the object
        if (!depth_only)
        {
            glUniform3fv(render_scene_uniforms.material_ambient, 1, value_ptr(vec3(0.1f, 0.0f, 0.2f)));
            glUniform3fv(render_scene_uniforms.material_diffuse, 1, value_ptr(vec3(0.3f, 0.2f, 0.8f)));
            glUniform3fv(render_scene_uniforms.material_specular, 1, value_ptr(vec3(1.0f, 1.0f, 1.0f)));
            glUniform1f(render_scene_uniforms.material_specular_power, 25.0f);
        }
        
        // Draw the object
        object.Render();
        
        // Set material properties for the ground
        if (!depth_only)
        {
            glUniform3fv(render_scene_uniforms.material_ambient, 1, value_ptr(vec3(0.1f, 0.1f, 0.1f)));
            glUniform3fv(render_scene_uniforms.material_diffuse, 1, value_ptr(vec3(0.1f, 0.5f, 0.1f)));
            glUniform3fv(render_scene_uniforms.material_specular, 1, value_ptr(vec3(0.1f, 0.1f, 0.1f)));
            glUniform1f(render_scene_uniforms.material_specular_power, 3.0f);
        }
        
        // Draw the ground
        glBindVertexArray(ground_vertex_array_object);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);
    }
    
    void terminate() {
        glUseProgram(0);
        glDeleteProgram(light->program);
        glDeleteProgram(scene->program);
        glDeleteBuffers(1, &ground_vertex_buffer_object);
        glDeleteVertexArrays(1, &ground_vertex_array_object);
        object.Free();
    }
    
    void resize(int width, int height) {
        current_width = width;
        current_heigth = height;
    }
    
private:
    Shader *light;
    
    struct {
        GLint model_view_projection_matrix;
    } render_light_uniforms;
    
    GLuint depth_frame_buffer_object;
    GLuint depth_texture;
    
    Shader *scene;
    struct {
        GLint model_matrix;
        GLint view_matrix;
        GLint projection_matrix;
        GLint shadow_matrix;
        GLint light_position;
        GLint material_ambient;
        GLint material_diffuse;
        GLint material_specular;
        GLint material_specular_power;
    } render_scene_uniforms;
    
    GLuint ground_vertex_buffer_object;
    GLuint ground_vertex_array_object;
    
    VBObject object;
    
    GLint current_width;
    GLint current_heigth;
};

int main(int argc, const char *argv[]) {
    
    ShadowMapApplication app = ShadowMapApplication("Red Book - Chapter 04 Shadow Mapping", 512, 512);
    
    app.run();
    
    return 0;
}