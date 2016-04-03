//
//  tga.cpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/31/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//


#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include "tga.hpp"


bool TGALoader::loadTGA(TGA *tga, const char *filename) {
    
//    TGAHeader header;
    TGAHeader header;
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        cout << "could not open texture file" << "\n";
        return false;
    }
    
    if (fread(&header, sizeof(TGAHeader), 1, file) == 0) {
        cout << "could not read file header" << "\n";
        
        if (file != NULL) {
            fclose(file);
        }
        
        return false;
    }
    
    if (header.cmap_type != 0) {
        // TODO: Support paletted TGA files. Note, L8 files are actually stored as
        // paletted bitmaps with a 256 entry grayscale palette.
        
        if (file != NULL) {
            fclose(file);
        }
        
        return false;
    }
    
    if (!setup(tga, header)) {
        fclose(file);
        return false;
    }
    
    tga->image_data = (GLubyte *)malloc(tga->image_size);
    
    if (tga->image_data == NULL) {
        fclose(file);
        return false;
    }
    
    if (isCompressed(header)) {
        loadCompressedTGA(tga, file);
    } else {
        loadUncompressedTGA(tga, file);
    }
    
    
    return true;
}

bool TGALoader::setup(TGA *tga, const TGAHeader header) {
    tga->format = GL_UNSIGNED_BYTE;
    tga->bits_per_pixel = header.image_spec.bits_per_pixel;
    switch (header.image_spec.bits_per_pixel) {
            
        case 8:
            tga->format = GL_RED;
            break;
            
        case 16:
            switch (header.image_spec.alpha_depth) {
                case 0:
                    tga->format = GL_RG8;
                    break;
                    
                case 8:
                    tga->format = GL_RG;
                    break;
                    
                default:
                    break;
            }
            break;
            
        case 24:
            switch (header.image_spec.alpha_depth) {
                case 0:
                    tga->format = GL_BGR;
                    break;
                    
                default:
                    break;
            }
            break;
            
        case 32:
            switch (header.image_spec.alpha_depth) {
                case 8:
                    tga->format = GL_ABGR_EXT;
                    break;
                    
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
    
    if (tga->format == GL_UNSIGNED_BYTE) {
        return false;
    }
    
    tga->width = header.image_spec.size.width;
    tga->height = header.image_spec.size.height;
    tga->image_size = tga->width * tga->height * tga->bits_per_pixel / 8;
    
    return true;
}

bool TGALoader::isCompressed(const TGAHeader header) {
    return (header.image_type & 0x08) != 0;
}

bool TGALoader::loadUncompressedTGA(TGA *tga, FILE *file) {
    
    if (fread(tga->image_data, 1, tga->image_size, file) != tga->image_size) {
        if (tga->image_data != NULL) {
            free(tga->image_data);
        }
        
        fclose(file);
        return false;
    }
    
    // Byte Swapping Optimized By Steve Thomas
    for(GLuint cswap = 0; cswap < (int)tga->image_size; cswap += (tga->bits_per_pixel / 8)) {
        
        tga->image_data[cswap] ^= tga->image_data[cswap+2] ^=
        tga->image_data[cswap] ^= tga->image_data[cswap+2];
    }
    
    fclose(file);
    
    return true;
}

bool TGALoader::loadCompressedTGA(TGA *tga, FILE *file) {
    
    return true;
}


//bool TGALoader::loadCompressedTGA(Texture *texture, const char *filename, FILE *file) {
//    TGA tga;
//    
//    if (fread(tga.header, sizeof(tga.header), 1, file) == 0) {
//        printf("could not read info header\n");
//        if (file != NULL) {
//            fclose(file);
//        }
//        
//        return false;
//    }
//    
//    texture->width = tga.header[1] * 256 + tga.header[0];
//    texture->height = tga.header[3] * 256 + tga.header[2];
//    texture->bpp = tga.header[4];
//    tga.width = texture->width;
//    tga.height = texture->height;
//    tga.bpp = texture->bpp;
//    
//    if (texture->width <= 0 || texture->height <= 0 || (texture->bpp != 24 && texture->bpp != 32)) {
//        printf("invalid texture information\n");
//        if (file != NULL) {
//            fclose(file);
//        }
//        
//        return false;
//    }
//    
//    tga.bytesPerPixel = tga.bpp / 8;
//    tga.imageSize = tga.bytesPerPixel * tga.width * tga.height;
//    texture->imageData = (GLubyte *)malloc(tga.imageSize);
//    
//    if (texture->imageData == NULL) {
//        printf("could not allocate memory for image\n");
//        fclose(file);
//        return false;
//    }
//    
//    GLuint pixelcount = tga.height * tga.width;
//    GLuint currentpixel = 0;
//    GLuint currentbyte = 0;
//    GLubyte *colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);
//    
//    do {
//        
//        GLubyte chunckheader = 0;
//        
//        if (fread(&chunckheader, sizeof(GLubyte), 1, file) == 0) {
//            printf("could not read RLE header\n");
//            if (file != NULL) {
//                fclose(file);
//            }
//            
//            if (texture->imageData != NULL) {
//                free(texture->imageData);
//            }
//            
//            return false;
//        }
//        
//        if (chunckheader < 128) {
//            chunckheader++;
//        } else {
//            chunckheader -= 127;
//        }
//        
//        for (short counter = 0; counter < chunckheader; counter++) {
//            if (fread(colorbuffer, 1, tga.bytesPerPixel, file) != tga.bytesPerPixel) {
//                
//                if (file != NULL) {
//                    fclose(file);
//                }
//                
//                if (texture->imageData != NULL) {
//                    free(texture->imageData);
//                }
//                
//                if (colorbuffer != NULL) {
//                    free(colorbuffer);
//                }
//                
//                return false;
//            }
//            
//            texture->imageData[currentbyte] = colorbuffer[2];   // flip R and B vcolor value around in the process
//            texture->imageData[currentbyte + 1] = colorbuffer[1];
//            texture->imageData[currentbyte + 2] = colorbuffer[0];
//            
//            if (tga.bytesPerPixel == 4) {
//                texture->imageData[currentbyte + 3] = colorbuffer[3];
//            }
//            
//            currentbyte += tga.bytesPerPixel;
//            currentpixel++;
//            
//            if (currentpixel > pixelcount) {
//                if (file != NULL) {
//                    fclose(file);
//                }
//                
//                if (texture->imageData != NULL) {
//                    free(texture->imageData);
//                }
//                
//                if (colorbuffer != NULL) {
//                    free(colorbuffer);
//                }
//                
//                return false;
//            }
//        }
//        
//    } while (currentpixel < pixelcount);
//    fclose(file);
//    return true;
//}



