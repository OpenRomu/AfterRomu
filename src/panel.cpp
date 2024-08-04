#include "stdafx.h"
#include "exception.h"
#include "Panel.h"
#include "image.h"
#include "math.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "fastmath.h"
Panel::Panel(
    const char* lesammo, const char* ammo, const char* smoke, const char* vie, const char* players, const char* lazer)
    : courante(0)
    , tps_players(0.0f)
    , tps_vie(0.0f)

{
    Image img, img2, img3, img4, img5, img6, img7;
    img.load(lesammo);
    m_tex[0] = new Texture(img.data(), img.width(), img.height(), GL_RGBA);
    W[0] = img.width();
    H[0] = img.height();

    img2.load(ammo);
    W[1] = img2.width();
    H[1] = img2.height();
    m_tex[1] = new Texture(img2.data(), img2.width(), img2.height(), GL_RGBA);

    img3.load(vie);
    m_tex[2] = new Texture(img3.data(), img3.width(), img3.height(), GL_RGBA);
    W[2] = img3.width();
    H[2] = img3.height();

    img4.load(players);
    m_tex[3] = new Texture(img4.data(), img4.width(), img4.height(), GL_RGBA);
    W[3] = img4.width();
    H[3] = img4.height();

    img5.load("data/env/missile.bmp");
    m_tex[4] = new Texture(img5.data(), img5.width(), img5.height(), GL_RGBA);
    W[4] = img5.width();
    H[4] = img5.height();

    img6.load(smoke);
    m_tex[5] = new Texture(img6.data(), img6.width(), img6.height(), GL_RGBA);
    W[0] = img6.width();
    H[0] = img6.height();

    img7.load(lazer);
    m_tex[6] = new Texture(img7.data(), img7.width(), img7.height(), GL_RGBA);
    W[0] = img7.width();
    H[0] = img7.height();

    //	courante=m_tex[1]->id ;

    float inc = 1.0f / 1.0f; // 0.0625f;
    m_coords.resize(2);
    m_coords[0] = new Coords;
    m_coords[0]->c00[0] = 0;
    m_coords[0]->c00[1] = 0;
    m_coords[0]->c10[0] = 0 + inc;
    m_coords[0]->c10[1] = 0;
    m_coords[0]->c11[0] = 0 + inc, m_coords[0]->c11[1] = 0 + inc;
    m_coords[0]->c01[0] = 0;
    m_coords[0]->c01[1] = 0 + inc;

    /* inc = 1.0f/4.0f;//0.0625f;
     int c = 1;
     for(float y = 1-inc; y >= 0; y -= inc) {
         for(float x = 0; x < 1.0f; x += inc) {
             m_coords[c] = new Coords;
             if(!m_coords[0]) throw out_of_memory();
             m_coords[c]->c00[0] = x; m_coords[c]->c00[1] = y;
             m_coords[c]->c10[0] = x+inc; m_coords[c]->c10[1] = y;
             m_coords[c]->c11[0] = x+inc, m_coords[c]->c11[1] = y+inc;
             m_coords[c]->c01[0] = x; m_coords[c]->c01[1] = y+inc;
             c++;
         }
     }*/

    position = 4;
    m_coords[1] = new Coords;
    m_coords[1]->c00[0] = 0;
    m_coords[1]->c00[1] = 0;
    m_coords[1]->c10[0] = 0 + inc;
    m_coords[1]->c10[1] = 0;
    m_coords[1]->c11[0] = 0 + inc, m_coords[1]->c11[1] = 0 + inc;
    m_coords[1]->c01[0] = 0;
    m_coords[1]->c01[1] = 0 + inc;
}

Panel::~Panel()
{

    if (m_tex[0]) {
        m_tex[0]->destroy();
        delete m_tex[0];
        m_tex[0] = 0;
    }
    if (m_tex[1]) {
        m_tex[1]->destroy();
        delete m_tex[1];
        m_tex[1] = 0;
    }
    if (m_tex[2]) {
        m_tex[2]->destroy();
        delete m_tex[2];
        m_tex[2] = 0;
    }
    if (m_tex[3]) {
        m_tex[3]->destroy();
        delete m_tex[3];
        m_tex[3] = 0;
    }

    if (m_tex[4]) {
        m_tex[4]->destroy();
        delete m_tex[4];
        m_tex[4] = 0;
    }
    if (m_tex[5]) {
        m_tex[5]->destroy();
        delete m_tex[5];
        m_tex[5] = 0;
    }
    if (m_tex[6]) {
        m_tex[6]->destroy();
        delete m_tex[6];
        m_tex[6] = 0;
    }

    for (int c = 0; c < m_coords.size(); c++) {
        if (m_coords[c]) {
            delete m_coords[c];
        }
    }
}

void Panel::frame(float delta)
{
    if (tps_players < 30.0f) {
        tps_players = tps_players + delta;
    }
    if (tps_vie < 30.0f) {
        tps_vie = tps_vie + delta;
    }
}

void Panel::hit(void) { tps_vie = 0.0f; }

void Panel::hit_Romu(void) { tps_players = 0.0f; }

void Panel::print(int m_width, int m_height, int nbammo, int vie, int nb_player, char* weapon)

{
    int X = 27;
    int Y = 130;
    int i = 0;
    char vies[10];
    char players[10];
    char ammos[10];

    glEnable(GL_DEPTH_FUNC);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glDepthMask(0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    GLfloat ratio = m_height / H[courante];
    GLfloat WW = m_height;
    GLfloat HH = m_height;
    int id;
    WW = m_height / 10;
    HH = m_height / 10;

    if (strcmp(weapon, "plasma") && strcmp(weapon, "c4") && strcmp(weapon, "grenade") && strcmp(weapon, "fumigene")) {
        m_tex[1]->bind();
        id = 0;
        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords[(int)id]->c00);
        glVertex3f(20 + WW + -WW / 2 + i * (WW / 3), HH - HH / 2, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords[(int)id]->c10);
        glVertex3f(20 + WW + WW / 2 + i * (WW / 3), HH - HH / 2, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords[(int)id]->c11);
        glVertex3f(20 + WW + WW / 2 + i * (WW / 3), HH + HH / 2, 0.0f); // Top Right
        glTexCoord2fv(m_coords[(int)id]->c01);
        glVertex3f(20 + WW + -WW / 2 + i * (WW / 3), HH + HH / 2, 0.0f); // Top Left
        glEnd();
    }

    id = 0;
    if (!strcmp(weapon, "grenade")) {
        m_tex[0]->bind();
        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords[(int)id]->c00);
        glVertex3f(20 + WW + -WW / 4 + i * (WW / 2), HH - HH / 4, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords[(int)id]->c10);
        glVertex3f(20 + WW + WW / 4 + i * (WW / 2), HH - HH / 4, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords[(int)id]->c11);
        glVertex3f(20 + WW + WW / 4 + i * (WW / 2), HH + HH / 4, 0.0f); // Top Right
        glTexCoord2fv(m_coords[(int)id]->c01);
        glVertex3f(20 + WW + -WW / 4 + i * (WW / 2), HH + HH / 4, 0.0f); // Top Left
        glEnd();
    }
    id = 0;

    if (!strcmp(weapon, "fumigene")) {
        m_tex[5]->bind();
        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords[(int)id]->c00);
        glVertex3f(20 + WW + -WW / 4 + i * (WW / 2), HH - HH / 4, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords[(int)id]->c10);
        glVertex3f(20 + WW + WW / 4 + i * (WW / 2), HH - HH / 4, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords[(int)id]->c11);
        glVertex3f(20 + WW + WW / 4 + i * (WW / 2), HH + HH / 4, 0.0f); // Top Right
        glTexCoord2fv(m_coords[(int)id]->c01);
        glVertex3f(20 + WW + -WW / 4 + i * (WW / 2), HH + HH / 4, 0.0f); // Top Left
        glEnd();
    }

    if (!strcmp(weapon, "plasma")) {
        m_tex[4]->bind();
        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords[(int)id]->c00);
        glVertex3f(20 + WW + -WW / 2 + i * (WW / 2), HH - HH / 2, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords[(int)id]->c10);
        glVertex3f(20 + WW + WW / 2 + i * (WW / 2), HH - HH / 2, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords[(int)id]->c11);
        glVertex3f(20 + WW + WW / 2 + i * (WW / 2), HH + HH / 2, 0.0f); // Top Right
        glTexCoord2fv(m_coords[(int)id]->c01);
        glVertex3f(20 + WW + -WW / 2 + i * (WW / 2), HH + HH / 2, 0.0f); // Top Left
        glEnd();
    }

    if (!strcmp(weapon, "c4")) {
        m_tex[6]->bind();
        glBegin(GL_QUADS); // Start Drawing A Quad
        glTexCoord2fv(m_coords[(int)id]->c00);
        glVertex3f(20 + WW + -WW / 4 + i * (WW / 2), HH - HH / 4, 0.0f); // Bottom Left
        glTexCoord2fv(m_coords[(int)id]->c10);
        glVertex3f(20 + WW + WW / 4 + i * (WW / 2), HH - HH / 4, 0.0f); // Bottom Right
        glTexCoord2fv(m_coords[(int)id]->c11);
        glVertex3f(20 + WW + WW / 4 + i * (WW / 2), HH + HH / 4, 0.0f); // Top Right
        glTexCoord2fv(m_coords[(int)id]->c01);
        glVertex3f(20 + WW + -WW / 4 + i * (WW / 2), HH + HH / 4, 0.0f); // Top Left
        glEnd();
    }

    WW = m_height / 10;
    HH = m_height / 10;

    id = 1; // imbittable?
    if (tps_vie < 7.0f) {

        ratio = 3.0f + fsin(tps_vie * -10.0f) * exp(1.0f - tps_vie) / 2.0f; // de 4-10
    } else {
        ratio = 3.0f;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float ratioc = 0.0f;

    if (tps_vie < 7.0f) {
        ratioc = 1.0f + fsin(tps_vie * -10.0f) * exp(1.0f - tps_vie); // de 4-10
        // ratio= exp(1.0f-tps_players);//de 4-10
    } else {
        ratioc = 1.0f;
    }

    m_tex[2]->bind();
    glColor4f(1.0f, ratioc, ratioc, 1.0f);

    glBegin(GL_QUADS); // Start Drawing A Quad
    glTexCoord2fv(m_coords[(int)id]->c00);
    glVertex3f(X - H[2] / ratio, Y - W[2] / ratio, 0.0f); // Bottom Left
    glTexCoord2fv(m_coords[(int)id]->c10);
    glVertex3f(X + H[2] / ratio, Y - W[2] / ratio, 0.0f); // Bottom Right
    glTexCoord2fv(m_coords[(int)id]->c11);
    glVertex3f(X + H[2] / ratio, Y + W[2] / ratio, 0.0f); // Top Right
    glTexCoord2fv(m_coords[(int)id]->c01);
    glVertex3f(X - H[2] / ratio, Y + W[2] / ratio, 0.0f); // Top Left
    glEnd();
    int XX = 30;
    int YY = 70;

    if (tps_players < 7.0f) {
        ratio = 3.0f + fsin(tps_players * -10.0f) * exp(1.0f - tps_players); // de 4-10
        // ratio= exp(1.0f-tps_players);//de 4-10
    } else {
        ratio = 3.0f;
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    m_tex[3]->bind();
    glBegin(GL_QUADS); // Start Drawing A Quad
    glTexCoord2fv(m_coords[(int)id]->c00);
    glVertex3f(XX - H[2] / ratio, YY - W[2] / ratio, 0.0f); // Bottom Left
    glTexCoord2fv(m_coords[(int)id]->c10);
    glVertex3f(XX + H[2] / ratio, YY - W[2] / ratio, 0.0f); // Bottom Right
    glTexCoord2fv(m_coords[(int)id]->c11);
    glVertex3f(XX + H[2] / ratio, YY + W[2] / ratio, 0.0f); // Top Right
    glTexCoord2fv(m_coords[(int)id]->c01);
    glVertex3f(XX - H[2] / ratio, YY + W[2] / ratio, 0.0f); // Top Left
    glEnd();

    glDisable(GL_TEXTURE_2D);
    sprintf(vies, "%i%", vie);
    sprintf(players, "%i", nb_player);
    sprintf(ammos, "%i", nbammo);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); //

    glDisable(GL_BLEND);
    glRasterPos2i(17 + (-strlen(vies) * 5) + X - H[2] / 2 + H[2] / 4, Y - W[2] / 2);

#if defined(GLUT_BITMAP)
    for (unsigned int t = 0; t < strlen(vies); t++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *(vies + t));
#endif

    glRasterPos2i(15 + (-strlen(players) * 5) + XX - H[2] / 2 + H[2] / 4, -5 + YY - W[2] / 2);

#if defined(GLUT_BITMAP)
    for (unsigned int tt = 0; tt < strlen(players); tt++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *(players + tt));
#endif
    /*
    glRasterPos2i(30+(-strlen(ammos)*5)+XX*2-H[2]/2+H[2]/4,-5+YY-W[2]/2);
    for (unsigned int to=0;to<strlen(vies);to++)
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*(ammos+to));

    */
}