//
//  RestartPrimitivesApplication.hpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 4/3/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#ifndef RestartPrimitivesApplication_hpp
#define RestartPrimitivesApplication_hpp

#include "CKApp.hpp"

class RestartPrimitivesApplication: public CKApplication {
    
public:
    using CKApplication::CKApplication;
    virtual void initialize();
    virtual void display();
    virtual void terminate();
    virtual void resize(int width, int height);
    
private:
    GLuint cube_vertex_array;    
    GLuint cube_vertex_buffer;
    GLuint cube_element_buffer;
    
    GLuint program_id;
    GLuint shader_model_matrix;
    GLuint shader_projection_matrix;
};

#endif /* RestartPrimitivesApplication_hpp */
