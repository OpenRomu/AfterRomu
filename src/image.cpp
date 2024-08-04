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
 * image manipulation support
 */
#include "stdafx.h"
#include "image.h"
#include "exception.h"
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _JPEG_SUPPORT
#include "ijl/ijl.h"
#endif // _JPEG_SUPPORT

Image::Image()
    : m_data(0)
    , m_width(0)
    , m_height(0)
    , m_bpp(0)
    , m_alpha(false)
{
}

Image::~Image()
{
    if (m_data) {
        delete m_data;
        m_data = 0;
    }
}

bool Image::load(const char* filename)
{
    if (filename) {
        if (strstr(filename, ".tga")) {
            return load_tga(filename);
        } else if (strstr(filename, ".bmp")) {
            return load_bmp(filename);
        }
#ifdef _JPEG_SUPPORT
        else if (strstr(filename, ".jpg")) {
            return load_jpg(filename);
        }
#endif // _JPEG_SUPPORT
    }

    return false;
}

bool Image::load_bmp_true_color(FILE* f)
{
    int bytes = m_bpp >> 3;
    unsigned int size = m_width * m_height * bytes;
    m_data = new unsigned char[size];
    if (!m_data)
        throw out_of_memory();
    if (fread(m_data, 1, size, f) == size) {
        // swap bgr to rgb
        for (unsigned int c = 0; c < size; c += bytes) {
            unsigned char tmp = m_data[c];
            m_data[c] = m_data[c + 2];
            m_data[c + 2] = tmp;
        }
        return true;
    }

    return false;
}

bool Image::load_bmp_8bpp(FILE* f, unsigned int clr_used)
{
    bool r = false;
    RGBQUAD* rgb = new RGBQUAD[clr_used];
    if (fread(rgb, sizeof(RGBQUAD), clr_used, f) == clr_used) {
        unsigned int size = m_width * m_height;
        unsigned char* tmp = new unsigned char[size];
        if (fread(tmp, 1, size, f) == size) {
            m_bpp = 24; // convert to RGB
            m_data = new unsigned char[size * 4];
            int c = 0;
            for (unsigned int i = 0; i < size; i++) {
                if (rgb[tmp[i]].rgbRed == 0 && rgb[tmp[i]].rgbGreen == 0 && rgb[tmp[i]].rgbBlue == 255) {
                    m_data[c++] = 0;
                    m_data[c++] = 0;
                    m_data[c++] = 0;
                    m_data[c++] = 0;
                } else {
                    m_data[c++] = rgb[tmp[i]].rgbRed;
                    m_data[c++] = rgb[tmp[i]].rgbGreen;
                    m_data[c++] = rgb[tmp[i]].rgbBlue;
                    m_data[c++] = 255;
                }
            }
            r = true; // success
        }
        if (tmp)
            delete tmp;
    }

    if (rgb)
        delete rgb;
    return r;
}

bool Image::load_bmp(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f) {
        return false;
    }

    BITMAPFILEHEADER hdr;
    if (fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr)) {
        return false;
    }

    if (hdr.bfType != 'MB') {
        return false;
    }

    BITMAPINFOHEADER bi;
    if (fread(&bi, 1, sizeof(bi), f) != sizeof(bi)) {
        return false;
    }

    if (bi.biCompression != BI_RGB) {
        return false;
    }

    m_width = bi.biWidth;
    m_height = bi.biHeight;
    m_bpp = bi.biBitCount;

    bool r = false;
    switch (m_bpp) {
    case 24:
    case 32:
        r = load_bmp_true_color(f);
        break;
    case 8:
        r = load_bmp_8bpp(f, bi.biClrUsed ? bi.biClrUsed : 256);
        break;
    }

    fclose(f);
    return r;
}

#pragma pack(push)
#pragma pack(1)

struct tga_hdr {
    char offset;
    char cmap_type;
    char image_type;
    unsigned short cmap_start;
    unsigned short cmap_length;
    char cmap_bits;
    unsigned short hoffset;
    unsigned short voffset;
    unsigned short width;
    unsigned short height;
    char bits_per_pixel;
    char flags;
};

#pragma pack(pop)

bool Image::load_tga(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f) {
        return false;
    }

    tga_hdr hdr;
    if (fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr)) {
        return false;
    }

    // validate hdr
    m_width = hdr.width, m_height = hdr.height;
    m_bpp = hdr.bits_per_pixel;

    unsigned int bytes = m_bpp >> 3;
    unsigned int size = m_height * m_width * bytes;
    m_data = new unsigned char[size];
    if (!m_data) {
        throw out_of_memory();
    }

    if (hdr.offset) {
        fseek(f, hdr.offset, SEEK_CUR);
    }

    if (fread(m_data, 1, size, f) != size) {
        return false;
    }

    // swap bgr to rgb for bpp>8
    if (bytes > 1) {
        for (unsigned int c = 0; c < size; c += bytes) {
            unsigned char tmp = m_data[c];
            m_data[c] = m_data[c + 2];
            m_data[c + 2] = tmp;
        }
    }

    if (hdr.flags & 0x20) { // bottom up, need to swap scanlines
        swap_scanlines();
    }

    fclose(f);
    return true;
}

#ifdef _JPEG_SUPPORT
bool Image::load_jpg(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f) {
        return false;
    }

    JPEG_CORE_PROPERTIES j;
    memset(&j, 0, sizeof(JPEG_CORE_PROPERTIES));

    if (ijlInit(&j) != IJL_OK) {
        throw basic_exception("Can't initialize intel jpeg library!");
    }

    fseek(f, 0, SEEK_END);
    unsigned int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* tmp = new unsigned char[size];
    if (!tmp)
        throw out_of_memory();

    if (fread(tmp, 1, size, f) != size) {
        throw basic_exception("fread() jpeg");
    }

    j.JPGBytes = tmp;
    j.JPGSizeBytes = size;
    if (ijlRead(&j, IJL_JBUFF_READPARAMS) != IJL_OK) {
        delete tmp;
        fclose(f);
        throw basic_exception("Can't read jpeg header");
    }

    m_width = j.JPGWidth;
    m_height = j.JPGHeight;
    m_bpp = j.JPGChannels << 3;

    int imageSize = j.JPGWidth * j.JPGHeight * j.JPGChannels;

    m_data = new unsigned char[imageSize];
    if (!m_data) {
        throw out_of_memory();
    }

    j.DIBWidth = j.JPGWidth;
    j.DIBHeight = j.JPGHeight;
    j.DIBChannels = j.JPGChannels;
    j.DIBBytes = m_data;

    switch (j.JPGChannels) {
    case 1:
        j.DIBColor = IJL_G;
        break;
    default:
    case 3:
        j.DIBColor = IJL_RGB;
        break;
    }

    if (ijlRead(&j, IJL_JBUFF_READWHOLEIMAGE) != IJL_OK) {
        delete m_data, m_data = 0;
        delete tmp, tmp = 0;
        throw basic_exception("can't read jpeg file");
    }

    if (ijlFree(&j) != IJL_OK) { }

    swap_scanlines();
    delete tmp;
    fclose(f);
    return true;
}
#endif // _JPEG_SUPPORT

void Image::swap_scanlines()
{
    int bytes = m_bpp >> 3;
    unsigned char* tmp = new unsigned char[m_width * bytes];
    for (int i = 0; i < m_height / 2; i++) {
        memcpy(tmp, m_data + i * m_width * bytes, m_width * bytes);
        memcpy(m_data + i * m_width * bytes, m_data + (m_height - i - 1) * m_width * bytes, m_width * bytes);
        memcpy(m_data + (m_height - i - 1) * m_width * bytes, tmp, m_width * bytes);
    }
    delete tmp;
}
