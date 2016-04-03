//
//  pointsprites.hpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/31/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#ifndef pointsprites_hpp
#define pointsprites_hpp

#include "CKApp.hpp"
#include "tga.hpp"

class PointSpritesApplication : public CKApplication {
    
public:
    using CKApplication::CKApplication;
    
    virtual void initialize();
    virtual void display();
    virtual void terminate();
    virtual void resize(int width, int height);

    
private:
    GLuint program_id;
    GLuint sprite_texture;
    GLuint vertex_model_matrix;
    GLuint vertex_projection_matrix;
    GLuint vertex_array;
    GLuint vertex_buffer;
    TGA tga;
};


#endif /* pointsprites_hpp */
