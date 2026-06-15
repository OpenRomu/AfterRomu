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
 * OGL texture object encapsulation
 */
#include "stdafx.h"

#include "common.h"
#include "texture.h"
#include "gl/glu.h" // gluBuildMipmaps

Texture::Quality Texture::default_quality = Texture::linear_mipmap_linear;
// Texture::Quality Texture::default_quality = Texture::nearest_mipmap_nearest;

Texture::Texture(const unsigned char *data, int width, int height, int format, Quality quality) : m_id(0)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    if (quality == unspecified)
    {
        quality = default_quality;
    }

    int min, max;
    switch (quality)
    {
    case nearest:
        min = GL_NEAREST, max = GL_NEAREST;
        break;

    case linear:
        min = GL_LINEAR, max = GL_LINEAR;
        break;

    case nearest_mipmap_nearest:
        min = GL_NEAREST_MIPMAP_NEAREST, max = GL_LINEAR;
        break;

    case linear_mipmap_nearest:
        min = GL_LINEAR_MIPMAP_NEAREST, max = GL_LINEAR;
        break;

    case nearest_mipmap_linear:
        min = GL_NEAREST_MIPMAP_LINEAR, max = GL_LINEAR;
        break;

    case linear_mipmap_linear:
        min = GL_LINEAR_MIPMAP_LINEAR, max = GL_LINEAR;
        break;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, max);

    bool mipmap = false;
    switch (quality)
    {
    case nearest_mipmap_nearest:
    case linear_mipmap_nearest:
    case nearest_mipmap_linear:
    case linear_mipmap_linear:
        mipmap = true;
        break;
    }

    int internal_format = format;
    if (format == GL_RGB && config.bpp == 16)
    {
        internal_format = GL_RGB16;
    }

    if (mipmap)
    {
        gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, width, height, format, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }
}
