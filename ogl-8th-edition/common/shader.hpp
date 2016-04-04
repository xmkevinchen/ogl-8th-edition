//
//  shader.hpp
//  triangles
//
//  Created by Kevin Chen on 3/22/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#ifndef shader_hpp
#define shader_hpp

#include <GL/glew.h>

namespace Loader {
    class Shader {
    public:
        GLuint program;
        Shader(const GLchar *vertex_file_path, const GLchar *fragment_file_path);
        void use();
    };
}


#endif /* shader_hpp */
