#include "stdafx.h"
#include "exception.h"
#include "cross.h"
#include "image.h"
#include "math.h"
Cross::Cross(const char *petite, const char *snipper, const char *noir)

{
    Image img1, img2, img3;
    img1.load(petite);
    m_tex[0] = new Texture(img1.data(), img1.width(), img1.height(), GL_RGBA);
    W[0] = img1.width();
    H[0] = img1.height();

    img2.load(snipper);
    W[1] = img2.width();
    H[1] = img2.height();
    m_tex[1] = new Texture(img2.data(), img2.width(), img2.height(), GL_RGBA);

    img3.load(noir);
    W[2] = img3.width();
    H[2] = img3.height();
    m_tex[2] = new Texture(img3.data(), img3.width(), img3.height(), GL_RGB);

    float inc = 1.0f / 1.0f; // 0.0625f;
    m_coords.resize(26);
    m_coords[0] = new Coords;
    m_coords[0]->c00[0] = 0;
    m_coords[0]->c00[1] = 0;
    m_coords[0]->c10[0] = 0 + inc;
    m_coords[0]->c10[1] = 0;
    m_coords[0]->c11[0] = 0 + inc, m_coords[0]->c11[1] = 0 + inc;
    m_coords[0]->c01[0] = 0;
    m_coords[0]->c01[1] = 0 + inc;

    inc = 1.0f / 4.0f; // 0.0625f;
    int c = 1;
    for (float y = 1 - inc; y >= 0; y -= inc)
    {
        for (float x = 0; x < 1.0f; x += inc)
        {
            m_coords[c] = new Coords;
            if (!m_coords[0])
                throw out_of_memory();
            m_coords[c]->c00[0] = x;
            m_coords[c]->c00[1] = y;
            m_coords[c]->c10[0] = x + inc;
            m_coords[c]->c10[1] = y;
            m_coords[c]->c11[0] = x + inc, m_coords[c]->c11[1] = y + inc;
            m_coords[c]->c01[0] = x;
            m_coords[c]->c01[1] = y + inc;
            c++;
        }
    }
    position = 10;
}

Cross::~Cross()
{
    if (m_tex[0])
    {
        m_tex[0]->destroy();
        delete m_tex[0];
        m_tex[0] = 0;
    }
    if (m_tex[1])
    {
        m_tex[1]->destroy();
        delete m_tex[1];
        m_tex[1] = 0;
    }
    if (m_tex[2])
    {
        m_tex[2]->destroy();
        delete m_tex[2];
        m_tex[2] = 0;
    }
    for (int c = 0; c < m_coords.size(); c++)
    {
        if (m_coords[c])
        {
            delete m_coords[c];
        }
    }
}

void Cross::print(int m_width, int m_height)

{
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    m_tex[courante]->bind();
    GLfloat ratio = m_height / H[courante];
    GLfloat WW = m_height;
    GLfloat HH = m_height;
    int id;
    if (courante == 1)
    {
        WW = m_height;
        HH = m_height;
        id = 0;
    }
    else
    {
        WW = 72;
        HH = 72;
        id = position;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBegin(GL_QUADS); // Start Drawing A Quad

    glTexCoord2fv(m_coords[(int)id]->c00);
    glVertex3f(m_width / 2 - WW / 2, m_height / 2 - HH / 2, 0.0f); // Bottom Left
    glTexCoord2fv(m_coords[(int)id]->c10);
    glVertex3f(m_width / 2 + WW / 2, m_height / 2 - HH / 2, 0.0f); // Bottom Right
    glTexCoord2fv(m_coords[(int)id]->c11);
    glVertex3f(m_width / 2 + WW / 2, m_height / 2 + HH / 2, 0.0f); // Top Right
    glTexCoord2fv(m_coords[(int)id]->c01);
    glVertex3f(m_width / 2 - WW / 2, m_height / 2 + HH / 2, 0.0f); // Top Left
                                                                   //}
                                                                   //	else
                                                                   //{

    glEnd();

    if (courante == 1)
    {
        m_tex[2]->bind();
        glBegin(GL_QUADS); // Start Drawing A Quad

        glTexCoord2fv(c00);
        glVertex3f(0, 0, 0.0f); // Bottom Left
        glTexCoord2fv(c10);
        glVertex3f((m_width - m_height) / 2, 0, 0.0f); // Bottom Right
        glTexCoord2fv(c11);
        glVertex3f((m_width - m_height) / 2, m_height, 0.0f); // Top Right
        glTexCoord2fv(c01);
        glVertex3f(0, m_height, 0.0f); // Top Left

        glEnd();

        glBegin(GL_QUADS); // Start Drawing A Quad

        glTexCoord2fv(c00);
        glVertex3f(m_width - (m_width - m_height) / 2, 0, 0.0f); // Bottom Left
        glTexCoord2fv(c10);
        glVertex3f(m_width, 0, 0.0f); // Bottom Right
        glTexCoord2fv(c11);
        glVertex3f(m_width, m_height, 0.0f); // Top Right
        glTexCoord2fv(c01);
        glVertex3f(m_width - (m_width - m_height) / 2, m_height, 0.0f); // Top Left

        glEnd();
    }

    // Done Drawing Quad
}