//
//  shader.cpp
//  triangles
//
//  Created by Kevin Chen on 3/22/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

#include <string.h>

#include "shader.hpp"

namespace Loader {
    
    Shader::Shader(const GLchar *vertex_file_path, const GLchar *fragment_file_path) {
        
        // 1. Retrieve the vertex / fragment source code from file path.
        string vertex_code;
        string fragment_code;
        ifstream v_shader_file;
        ifstream f_shader_file;
        
        v_shader_file.exceptions(ifstream::badbit);
        f_shader_file.exceptions(ifstream::badbit);
        
        try {
            
            // Open files
            v_shader_file.open(vertex_file_path);
            f_shader_file.open(fragment_file_path);
            
            // Read file's buffer content into streams
            stringstream v_shader_stream, f_shader_stream;
            v_shader_stream << v_shader_file.rdbuf();
            f_shader_stream << f_shader_file.rdbuf();
            
            v_shader_file.close();
            f_shader_file.close();
            
            vertex_code = v_shader_stream.str();
            fragment_code = f_shader_stream.str();
            
        } catch (ifstream::failure e) {
            
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }
        
        const GLchar *v_shader_code = vertex_code.c_str();
        const GLchar *f_shader_code = fragment_code.c_str();
        
        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar info_log[512];
        
        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &v_shader_code, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::VERTEX:COMPILATION_FAILED\n" << info_log << std::endl;
        }
        
        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &f_shader_code, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::FRAGMENT:COMPILATION_FAILED\n" << info_log << std::endl;
        }
        
        // Shader program
        this->program = glCreateProgram();
        glAttachShader(this->program, vertex);
        glAttachShader(this->program, fragment);
        glLinkProgram(this->program);
        
        glGetProgramiv(this->program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->program, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        }
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void Shader::use() {
        glUseProgram(program);
    }
}
