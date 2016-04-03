//
//  tga.hpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/31/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#ifndef tga_hpp
#define tga_hpp

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma pack(push)
#pragma pack(1)

typedef struct {    
    unsigned char id_length;
    unsigned char cmap_type;
    unsigned char image_type;
    
    struct {
        unsigned short table_offset;
        unsigned short entry_count;
        unsigned char  entry_size;
    } cmap_spec;
    
    struct {
        struct {
            unsigned short x;
            unsigned short y;
        } origin;
        struct {
            unsigned short width;
            unsigned short height;
        } size;
        unsigned char bits_per_pixel;
        struct {
            unsigned char alpha_depth: 4;
            unsigned char orientation: 2;
            unsigned char :2;
        };
        
    } image_spec;
    
} TGAHeader;
#pragma pack(pop)

typedef struct {
    
    GLuint format;
    GLuint bits_per_pixel;
    GLuint width;
    GLuint height;
    GLuint image_size;
    GLubyte *image_data;
    
} TGA;

class TGALoader {
public:
    static bool loadTGA(TGA *tga, const char *filename);
    
private:
    static bool loadUncompressedTGA(TGA *tga, FILE *file);	// Load an Uncompressed file
    static bool loadCompressedTGA(TGA *tga, FILE *file);		// Load a Compressed file
    static bool isCompressed(const TGAHeader header);
    static bool setup(TGA *tga, const TGAHeader header);
    
};


#endif /* tga_hpp */
