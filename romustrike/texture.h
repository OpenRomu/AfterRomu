/* 
 * HL rendering engine
 * Copyright (c) 2000,2001 Bart Sekura
 *
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and 
 * this permission notice appear in all copies of the software, 
 * derivative works or modified versions.
 *
 * THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * OpenGL texture object encapsulation
 */

#ifndef __texture_h__
#define __texture_h__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

#define TEXTURE_ANIMATED     (1<<0)
#define TEXTURE_RANDOM       (1<<1)
#define TEXTURE_LIQUID       (1<<2)
#define TEXTURE_SKY          (1<<3)
class rgb_image_t {
public:
    rgb_image_t() : data(0), data2(0), data4(0), data8(0), width(0), height(0) {}
    ~rgb_image_t() { 
        if(data) delete data, data = 0;
        if(data2) delete data2, data2 = 0;
        if(data4) delete data4, data4 = 0;
        if(data8) delete data8, data8 = 0;
        width = height = 0;
    }

    unsigned char* data;
    unsigned char* data2;
    unsigned char* data4;
    unsigned char* data8;
    int width;
    int height;
};

class Texture {
public:
    enum Quality {
        unspecified,
        nearest,
        linear,
        nearest_mipmap_nearest,
        linear_mipmap_nearest,
        nearest_mipmap_linear,
        linear_mipmap_linear
    };

public:
    Texture(const unsigned char* data,
            int width,
            int height,
            int format = GL_RGB,
            Quality quality = unspecified);
    ~Texture() { destroy(); }

    GLuint id() const { return m_id; }
    void bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }
	
    void destroy() { if(m_id) { glDeleteTextures(1, &m_id); } }

public:
    static Quality default_quality;

private:
   GLuint    m_id;
};

#endif // __texture_h__
