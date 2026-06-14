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
 * image manipulation helper
 */

#ifndef __image_h__
#define __image_h__

#include <stdio.h>

class Image
{
  public:
    Image();
    ~Image();

    bool load(const char *filename);

    // accessors
    int width() const
    {
        return m_width;
    }
    int height() const
    {
        return m_height;
    }
    int bpp() const
    {
        return m_bpp;
    }
    bool alpha() const
    {
        return m_alpha;
    }

    const unsigned char *data() const
    {
        return m_data;
    }

  protected:
    bool load_tga(const char *filename);
    bool load_bmp(const char *filename);
#ifdef _JPEG_SUPPORT
    bool load_jpg(const char *filename);
#endif // _JPEG_SUPPORT

    bool load_bmp_8bpp(FILE *f, unsigned int clr_used);
    bool load_bmp_true_color(FILE *f);

    void swap_scanlines();

  private:
    int m_width;
    int m_height;
    int m_bpp;
    unsigned char *m_data;
    bool m_alpha;
};

#endif // __image_h__
