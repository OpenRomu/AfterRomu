#pragma once
#ifndef __FAKE_GL__
#define  __FAKE_GL__
#endif

typedef int DEVMODE;
typedef unsigned int GLuint;
typedef int RGBQUAD;

#define PFNGLACTIVETEXTUREARBPROC int
#define PFNGLMULTITEXCOORD2FVARBPROC int
#define PFNGLCLIENTACTIVETEXTUREARBPROC int

#define GL_LINEAR 1
#define GL_TEXTURE_MAG_FILTER 1
#define GL_TEXTURE_2D 1
#define GL_FEEDBACK 1

#define glClientActiveTextureARB int glClientActiveTextureARB_decoy
#define glMultiTexCoord2fvARB int glMultiTexCoord2fvARB_decoy

#define wglGetProcAddress(x)
#define wglMakeCurrent(x) 1

#define SetPixelFormat(x) 1

#define glColor4f(a, b, c, d)
#define glBlendFunc(a, b)
#define glDisable(a)
#define glEnable(a)
#define glEnd()
#define glVertex3f(a, b, c)
#define glBegin(a)
#define glTexCoord2fv(a)
#define glDepthMask(a)
#define glTexParameteri(a, b, c)
#define glVertex3fv(a)
#define glPolygonMode(a, b)
#define glTexCoord2f(a, b)
#define glPolygonOffset(a, b)
#define glBindTexture(a, b)
#define glMultMatrixf(a)
#define glPushMatrix()
#define glPopMatrix()
#define glRotatef(a, b, c, d)
#define glTranslatef(a, b, c, d)
#define glLoadIdentity()
#define glLoadMatrixf(a)