#include "stdafx.h"
#include "world.h"
#include "exception.h"
#include "DXUtil.h"
#include "fastmath.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// #include <crtdbg.h>
int lesfaces;
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

template <const int elem_size> class array_t
{
  public:
#define CHUNK (4096)
    array_t()
    {
        size = CHUNK * elem_size;
        data = new float[size];
        count = 0;
    }

    ~array_t()
    {
        if (data)
        {
            delete data;
            data = 0;
        }
    }

    void add(float *src)
    {
        if (count >= size / elem_size)
        {
            // resize
            int s = size;
            size += CHUNK * elem_size;
            float *n = new float[size];
            for (int i = 0; i < s; i++)
            {
                n[i] = data[i];
            }
            delete data;
            data = n;
        }

        float *dst = data + count * elem_size;
        for (int i = 0; i < elem_size; i++)
        {
            *dst++ = *src++;
        }
        count++;
    }

    operator float *(void) const
    {
        return data;
    }
    const float *operator[](int i) const
    {
        return data + i * elem_size;
    }

    int current() const
    {
        return count;
    }

  private:
    int size;
    int count;
    float *data;
};

static array_t<3> vt_array;
static array_t<2> st_array;
static array_t<2> lst_array;

static bool point_in_poly21(const vec3_t &p, vec3_t *pf)
{
    double total = 0.0f;

    vec3_t v[32]; // BAD SHIT
    int i;
    for (i = 0; i < 4; i++)
    {
        int c = 0 + i;
        v[i] = p - pf[c];
        v[i].normalize();
    }

    for (i = 0; i < 3; i++)
    {
        total += acos(v[i].dot(v[i + 1]));
    }

    total += acos(v[3].dot(v[0])); //(float)

    if (((total - 6.28f) > -0.03f) && ((total - 6.28f) < 0.03f))
    { // viré le fabsf de merde
        return true;
    }

    return false;
}

static bool point_in_poly(const vec3_t &p, const face_t *f, float epsilon)
{
    vec3_t v[32]; // BAD SHIT
    int i;
    for (i = 0; i < f->count; i++)
    {
        int c = f->first + i;
        v[i] = p - vec3_t(vt_array[c]);
        v[i].normalize();
    }

    float total = 0.0f;
    for (i = 0; i < f->count - 1; i++)
    {
        total += (float)acos(v[i].dot(v[i + 1]));
    }
    total += (float)acos(v[f->count - 1].dot(v[0]));

    if (fabsf(total - 6.28f) < epsilon)
    {
        return true;
    }

    return false;
}

static bool point_in_poly2(const vec3_t &p, const face_t *f)
{
    double total = 0.0f;

    vec3_t v[32]; // BAD SHIT
    int i;
    for (i = 0; i < f->count; i++)
    {
        int c = f->first + i;
        v[i] = p - vec3_t(vt_array[c]);
        v[i].normalize();
    }

    for (i = 0; i < f->count - 1; i++)
    {
        total += acos(v[i].dot(v[i + 1]));
    }

    total += acos(v[f->count - 1].dot(v[0])); //(float)

    if (((total - 6.28f) > -0.03f) && ((total - 6.28f) < 0.03f))
    { // viré le fabsf de merde
        return true;
    }

    return false;
}

static vec3_t closest_on_line(const vec3_t &a, const vec3_t &b, const vec3_t &p)
{
    vec3_t c = p - a;
    vec3_t v = b - a;
    float d = v.len();
    if (d)
        v /= d; // normalize avoiding len() again (nasty sqrt)
    float t = v.dot(c);
    if (t < 0.0f)
        return a;
    if (t > d)
        return b;
    return a + v * t;
}

static vec3_t closest_on_poly(const vec3_t &p, const face_t *f)
{
    vec3_t v[32];
    float d[32];
    int i;
    for (i = 0; i < f->count - 1; i++)
    {
        int c = f->first + i;
        v[i] = closest_on_line(vec3_t(vt_array[c]), vec3_t(vt_array[c + 1]), p);
        vec3_t t = p - v[i];
        d[i] = t.len();
    }
    i = f->count - 1;
    v[i] = closest_on_line(vec3_t(vt_array[f->first + i]), vec3_t(vt_array[f->first]), p);
    vec3_t t = p - v[i];
    d[i] = t.len();

    double min = d[0];
    vec3_t r = v[0];

    for (i = 1; i < f->count; i++)
    {
        if (d[i] < min)
        {
            min = d[i];
            r = v[i];
        }
    }

    return r;
}

static float intersect(const vec3_t &r0, const vec3_t &rn, const vec3_t &p0, const vec3_t &pn)
{
    float epsilon = 0.3f;

    float d = -pn.dot(p0);

    float numer = pn.dot(r0) + d;
    float denom = pn.dot(rn);

    if (denom <= -epsilon || denom >= epsilon)
    {
        return -(numer / denom);
    }
    return -1.0f;
}

static bool check_collision(const face_t *f, collision_data &coldat, float radius)
{
    bool colisione = false;

    vec3_t r;
    vec3_t pt(vt_array[f->first]);
    vec3_t n = f->p.normal();
    // float radius = 25.0f;
    vec3_t s = coldat.src - n * radius;
    float t = f->p.dist_to_point(s);

    if (t > coldat.dir_len)
    {
        return false;
    }
    if (t < -2 * radius)
    {
        return false;
    }
    // colisione=true;
    // ici on est forcement dans le rond ou a fleur
    // soit on est dessous le plan
    if (t < 0.0f)
    {
        // t = intersect(s,n*radius,pt,n);
        // r = s + n*t;
        if (!f->p.intersect(s, n * radius, pt, r))
            return false;
    }
    else
    {

        // soit on est dessus le plan
        // t = intersect(s,coldat.dir,pt,n);
        // r = s + coldat.dir*t;
        if (!f->p.intersect2(s, coldat.dir, pt, r))
            return false;
    }

    if (!point_in_poly2(r, f))
    {
        r = closest_on_poly(r, f);
    }

    t = intersect_sphere(r, -coldat.ndir, coldat.src, radius);

    if (t >= 0.0f && t <= coldat.dir_len)
    { //
        if (!coldat.found || t < coldat.nearest)
        { //) {
            coldat.found = true;
            coldat.nearest = t;
            coldat.nearest_poly = r;
            coldat.normal = f->p.normal();
            colisione = true;
        }
    }
    else
    {
        int tu = 2;
    }

    return colisione;
}

static void check_collision_car(const face_t *f, collision_data &coldat, float radius)
{
    vec3_t r;
    vec3_t pt(vt_array[f->first]);
    vec3_t n = f->p.normal();
    // float radius = 25.0f;
    vec3_t s = coldat.src - n * radius;
    float t = f->p.dist_to_point(s); // distance entre l'estremité de la boule et la face

    if (t > coldat.dir_len)
    {
        return;
    }
    if (t < -2 * radius)
    {
        return;
    }

    if (t < 0.0f)
    {
        // t = intersect(s,n*radius,pt,n);
        // r = s + n*t;
        if (!f->p.intersect(s, n * radius, pt, r))
            return;
    }
    else
    {
        // t = intersect(s,coldat.dir,pt,n);
        // r = s + coldat.dir*t;
        if (!f->p.intersect(s, coldat.dir, pt, r))
            return;
    }

    if (!point_in_poly2(r, f))
    {
        r = closest_on_poly(r, f);
    }

    t = intersect_sphere(r, -coldat.ndir, coldat.src, radius);

    if (t >= 0.0f && t <= coldat.dir_len)
    {
        if (!coldat.found || t < coldat.nearest)
        {
            coldat.found = true;
            coldat.nearest = t;
            coldat.nearest_poly = r;
            coldat.normal = f->p.normal();
        }
    }
}

void check_tir(const face_t *f, collision_data &coldat)
{
    vec3_t r;
    vec3_t pt(vt_array[f->first]);
    vec3_t n = f->p.normal();

    vec3_t s = coldat.src;
    float t = f->p.dist_to_point(s);

    if (t > coldat.dir_len)
    {
        return; // coldat.nearest pas de collsion possible ,trop loin
    }
    else
    {
        if (t < 0)
        {
            return; // derreire ??
        }

        if (!f->p.intersect2(s, coldat.dir, pt, r))
        {
            return; // pas d'intersection
        }
        else
        {
            if (point_in_poly2(r, f))
            {
                s = r - coldat.src; // t= distance en src et r si
                t = s.len();
                if (t < coldat.nearest)
                {

                    coldat.found = true;
                    coldat.nearest = s.len();
                    coldat.nearest_poly = r;
                    coldat.normal = f->p.normal();
                }
            }
        }
    }
}

void world_t::cleanup()
{
    int i;

    for (i = 0; i < faces.size(); i++)
    {
        if (faces[i])
            delete faces[i];
    }

    for (i = 0; i < textures.size(); i++)
    {
        if (textures[i])
            delete textures[i];
    }
    // colide_faces->clear_all();
    SAFE_DELETE(colide_faces);
    faces.clear();
    textures.clear();
    if (mark_faces)
    {
        delete mark_faces, mark_faces = 0;
    }

    if (bsp)
    {
        delete bsp, bsp = 0;
    }
    SAFE_DELETE(TexturesSky[0]);
    SAFE_DELETE(TexturesSky[1]);
    SAFE_DELETE(TexturesSky[2]);
    SAFE_DELETE(TexturesSky[3]);
    SAFE_DELETE(TexturesSky[4]);
    SAFE_DELETE(TexturesSky[5]);
    faces.clear();
    textures.clear();
    leaf_visibility.clear();

    visible_faces.clear();
}

void world_t::render_model(int i)
{
    if (i >= 0 && i < bsp->model_count)
    {
        glPushMatrix();
        dmodel_t *m = &bsp->models[i];
        glTranslatef(m->origin[0], m->origin[1], m->origin[2]);
        for (int c = 0; c < m->numfaces; c++)
        {
            render_face(faces[m->firstface + c]);
        }
        glPopMatrix();
    }
}

void world_t::projection_setup(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    double aspect = (double)height / (double)width;
    double fovx = 80; // 80 ;
    double rprojz = tan(deg2rad(fovx) * 0.5);

    glLoadIdentity();

    glFrustum(-rprojz, +rprojz, -aspect * rprojz, +aspect * rprojz, znear, 10000.0);

    frustum.left = -rprojz;
    frustum.right = +rprojz;
    frustum.bottom = -aspect * rprojz;
    frustum.top = +aspect * rprojz;
    frustum.znear = 1.0;
    frustum.zfar = 10000.0;
    // frustum.zfar = 6000.0;

    double s = sin(deg2rad(fovx) * 0.5);
    double c = cos(deg2rad(fovx) * 0.5);

    // precompute stuff for frustum culling
    projection = (c * (double)(width >> 1)) / s;
    angle_horiz = atan2((double)(width >> 1), projection);
    angle_vert = atan2((double)(height >> 1), projection);
    sin_horiz = sin(angle_horiz);
    sin_vert = sin(angle_vert);
    cos_horiz = cos(angle_horiz);
    cos_vert = cos(angle_vert);

    // back to model view
    glMatrixMode(GL_MODELVIEW);
}

void world_t::frustum_setup(const Camera &c)
{
    const matrix_t &m = c.matrix();
    const vec3_t &cam = c.eye();

    int i = 0;
    vec3_t v;

    // znear plane
    v = vec3_t(0, 0, 1);
    frustum_planes[i].normal = m * v;
    frustum_planes[i].dist = cam.dot(frustum_planes[i].normal);
    i++;

    // left plane
    v = vec3_t(cos_horiz, 0, sin_horiz);
    frustum_planes[i].normal = m * v;
    frustum_planes[i].dist = cam.dot(frustum_planes[i].normal);
    i++;

    // right plane
    v = vec3_t(-cos_horiz, 0, sin_horiz);
    frustum_planes[i].normal = m * v;
    frustum_planes[i].dist = cam.dot(frustum_planes[i].normal);
    i++;

    // top plane
    v = vec3_t(0, cos_vert, sin_vert);
    frustum_planes[i].normal = m * v;
    frustum_planes[i].dist = cam.dot(frustum_planes[i].normal);
    i++;

    // bottom plane
    v = vec3_t(0, -cos_vert, sin_vert);
    frustum_planes[i].normal = m * v;
    frustum_planes[i].dist = cam.dot(frustum_planes[i].normal);
    i++;
}

bool world_t::frustum_cull(const short *mins, const short *maxs)
{
    for (int i = 0; i < 5; i++)
    {
        const short *min = mins, *max = maxs;
        const float *n = frustum_planes[i].normal;
        float d = -frustum_planes[i].dist;
        d += (*n > 0 ? (*n * (float)*min) : (*n * (float)*max));
        min++, max++;
        n++;
        d += (*n > 0 ? (*n * (float)*min) : (*n * (float)*max));
        min++, max++;
        n++;
        d += (*n > 0 ? (*n * (float)*min) : (*n * (float)*max));
        min++, max++;
        n++;
        if (d > 0)
        {
            clip_count++;
            return true;
        }
    }
    return false;
}

int world_t::find_leaf(const vec3_t &coords)
{
    if (bsp)
    {
        int i = bsp->models[0].bsp_node;
        while (i >= 0)
        {
            dnode_t *n = &bsp->nodes[i];
            dplane_t *p = &bsp->planes[n->planenum];

            float d;
            if (p->type <= PLANE_Z)
            { // easier for axial planes
                d = coords[p->type] - p->dist;
            }
            else
            {
                // f(x,y,z) = Ax+Ay+Az-D
                d = p->normal[0] * coords[0] + p->normal[1] * coords[1] + p->normal[2] * coords[2] - p->dist;
            }

            if (d >= 0)
            { // in front or on the plane
                i = n->front;
            }
            else
            { // behind the plane
                i = n->back;
            }
        }

        return -(i + 1);
    }
    else
    {
        return 0;
    }
}
void world_t::advance_frame(float delta_f)
{
    delta = delta + delta_f;
    if (delta > 0.5f)
    {
        anim_tex = anim_tex + 1;
        anim_tex = anim_tex % 8;
        delta = 0;
    }
}
void world_t::render_face(const const face_t *f)
{

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (use_multi)
    { //
        glActiveTextureARB(GL_TEXTURE0_ARB);
        int offset;
        bool cc = false;
        if (textures[f->texture]->anim > 1)
            offset = anim_tex % textures[f->texture]->anim;
        else
            offset = 0;

        if (!cc)
            textures[f->texture + offset]->bind();

        glEnable(GL_TEXTURE_2D);

        if (f->lightmap)
        {
            glActiveTextureARB(GL_TEXTURE1_ARB);
            f->lightmap->bind();
            glEnable(GL_TEXTURE_2D);
        }

        if (f->styles[0] == 1) // bc optimis 16/05/08
        {
            glLineWidth(2.0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glDrawArrays(f->type, f->first, f->count);

        if (f->lightmap)
        {
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);
        }
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        if (wireframe_mode) // bc optimis 16/05/08
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
        // a virer car gourmand
        textures[f->texture]->bind();

        glTexCoordPointer(2, GL_FLOAT, 0, st_array);
        glDrawArrays(f->type, f->first, f->count);

        if (f->lightmap)
        {
            glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            f->lightmap->bind();
            glTexCoordPointer(2, GL_FLOAT, 0, lst_array);
            glDrawArrays(f->type, f->first, f->count);

            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }
    }
}

vec3_t world_t::check_collisions(const vec3_t &src, const vec3_t &dir, int cutoff, bool fill_colide_face)
{

    // p<<" start"<<endl;

    float epsilon = 0.05f;

    if (++cutoff > 5)
        return src;

    collision_data coldat;
    coldat.src = src;
    coldat.dir = dir;
    coldat.dir_len = dir.len();
    // coldat.nearest =10000.0f;

    if (coldat.dir_len < epsilon)
    {
        return src;
    }

    coldat.ndir = coldat.dir / coldat.dir_len;

    int face_idx = 0;

    if (fill_colide_face && cutoff == 1)
        colide_faces->clear_all();

    for (int i = 0; i < visible_faces.size(); i++)
    {
        face_t *f = visible_faces[i];
        while (f)
        {
            face_idx++;
            bool ca_colide = check_collision(f, coldat, radius);
            if (ca_colide && fill_colide_face)
            {
                colide_faces->set(face_idx);
            }
            f = f->next;
        }
    }

    if (coldat.found)
    {
        collide = true;

        vec3_t s = src;
        if (coldat.nearest >= epsilon)
        {
            s += coldat.ndir * (coldat.nearest - epsilon); // bug de traversage ici le 11/04/2007
        }

        vec3_t dst = src + dir;
        vec3_t n = s - coldat.nearest_poly;

        n.normalize();
        float t = intersect(dst, n, coldat.nearest_poly, n);

        vec3_t newdst = dst + n * t;
        vec3_t newdir = newdst - coldat.nearest_poly;
        normale_face_touchee = coldat.normal;

        return check_collisions(s, newdir, cutoff, fill_colide_face);
    }
    else
    {
        if (cutoff == 1)
            int pas_colis = 1;
    }

    // return src+(coldat.ndir*(coldat.dir_len));

    return src + (coldat.ndir * (coldat.dir_len - epsilon));
}

vec3_t world_t::check_collisions_yeux(const vec3_t &src, const vec3_t &dir, int cutoff, bool fill_colide_face)
{
    float epsilon = 0.05f;

    if (++cutoff > 5)
        return src;

    collision_data coldat;
    coldat.src = src;
    coldat.dir = dir;
    coldat.dir_len = dir.len();
    // coldat.nearest =10000.0f;

    if (coldat.dir_len < epsilon)
    {
        return src;
    }

    // coldat.ndir = coldat.dir/coldat.dir_len;
    vec3_t t = coldat.dir;
    t.normalize();
    coldat.ndir = t;

    int face_idx = 0;

    if (fill_colide_face)
        colide_faces->clear_all();

    for (int i = 0; i < visible_faces.size(); i++)
    {
        face_t *f = visible_faces[i];
        while (f)
        {
            face_idx++;
            bool ca_colide = check_collision(f, coldat, radius);
            //	check_tir (f, coldat);
            if (ca_colide && fill_colide_face)
            {
                colide_faces->set(face_idx);
            }

            f = f->next;
        }
    }

    if (coldat.found)
    {
        collide = true;

        vec3_t s = src;
        if (coldat.nearest >= epsilon)
        {
            s += coldat.ndir * (coldat.nearest - epsilon);
        }

        vec3_t dst = src + dir;
        vec3_t n = s - coldat.nearest_poly;

        n.normalize();
        float t = intersect(dst, n, coldat.nearest_poly, n);

        vec3_t newdst = dst + n * t;
        vec3_t newdir = newdst - coldat.nearest_poly;
        normale_face_touchee = coldat.normal + normale_face_touchee;
        normale_face_touchee.normalize();

        return check_collisions(s, newdir, cutoff, fill_colide_face);
    }

    // return src+(coldat.ndir*(coldat.dir_len));

    return src + (coldat.ndir * (coldat.dir_len - epsilon));
}
void world_t::check_collisions_bat(const vec3_t &src, vec3_t dir, collision_tir *out, float radius)
{

    epsilon = 0.01f;
    collision_data coldat;

    coldat.src = src;
    coldat.dir = dir;
    coldat.dir_len = dir.len();
    coldat.ndir = dir;

    if (coldat.dir_len < epsilon)
    {
        // memcpy(out,&src,sizeof(collision_tir));
        out->found = false;

        return;
    }
    else
    {

        coldat.ndir.normalize();

        for (int i = 0; i < visible_faces.size(); i++)
        {
            face_t *f = visible_faces[i];
            while (f)
            {
                check_collision_car(f, coldat, radius);
                // check_tir (f, coldat);
                f = f->next;
            }
        }

        // return ;

        if (coldat.found)
        {
            out->found = true;
            out->normal = coldat.normal;
            out->nearest = coldat.nearest;
        }
        else
        {
            out->found = false;
            out->nearest = coldat.nearest;
        }
    }
}

//////////////////////////////////////////////////////////////
collision_tir world_t::check_tirs(const vec3_t &src, const vec3_t &dir)
{
    collision_tir tmp;

    collision_data coldat;
    coldat.src = src;
    coldat.dir = dir;
    coldat.dir_len = dir.len();
    coldat.nearest = distance_gun;

    coldat.ndir = coldat.dir;
    coldat.ndir.normalize();

    for (int i = 0; i < visible_faces.size(); i++)
    {

        face_t *f = visible_faces[i];

        if (f)
        {
            if ((textures[f->texture]->name.compare((string) "{") != 1) && textures[f->texture]->name != "sky")
            {
                while (f)
                {
                    lesfaces = lesfaces + 1;
                    check_tir(f, coldat);
                    f = f->next;
                }
            }
        }
    }

    if (coldat.found)
    {
        vec3_t s = src;
        s += coldat.ndir * (coldat.nearest);
        tmp.pt = s;
        tmp.found = true;
        tmp.nearest = coldat.nearest;
        tmp.normal = coldat.normal;
        return tmp;
    }
    else
    {
        tmp.pt = src + dir;
        tmp.found = false;
        tmp.nearest = dir.len();
        return tmp;
    }
}

//------------- liste de faces a collisionner

void world_t::process_visible_faces_collide(const vec3_t &cam, const vec3_t &dest)
{
    rendered = 0;
    // setup for frustum culling
    // frustum_setup(c);

    vec3_t cam2;
    cam2[0] = cam[0] + dest[0];
    cam2[1] = cam[1] + dest[1];
    cam2[2] = cam[2] + dest[2];

    // clear stuff
    mark_faces->clear_all();
    zero_dwords((void *)&visible_faces[0], visible_faces.size());

    // find leaf we're in
    int idx = find_leaf(cam);
    if (idx == -1)
        idx = -1;
    int_vec &v = leaf_visibility[idx];
    short maxs[3];
    short mins[3];

    maxs[0] = cam[0] + 200;
    maxs[1] = cam[1] + 200;
    maxs[2] = cam[2] + 200;

    mins[0] = cam[0] - 200;
    mins[1] = cam[1] - 200;
    mins[2] = cam[2] - 200;

    // go thru leaf visibility list
    for (int i = 0; i < v.size(); i++)
    {
        const dleaf_t &leaf = bsp->leaves[v[i]];

        // discard leafs outside frustum
        // if (intersectbox(cam,cam2,leaf.mins,leaf.maxs))
        if (BoxIntersectBox(leaf.mins, leaf.maxs, mins, maxs) != OUTSIDE)
        // if(frustum_cull2(leaf.mins , leaf.maxs,mins,maxs))
        {
            unsigned short *p = bsp->marksurfaces + leaf.firstmarksurface;
            for (int x = 0; x < leaf.nummarksurfaces; x++)
            {
                // don't render those already rendered
                int face_idx = *p++;

                // if  (textures[faces[face_idx]->texture]->name.compare((string)"!")==0)
                //{
                // textures[faces[face_idx]->texture]->name.c_str()
                int res = strncmp(textures[faces[face_idx]->texture]->name.c_str(), "!", 1);
                if (res == 0)
                    continue;
                if (!mark_faces->test(face_idx))
                {

                    // back face culling
                    face_t *f = faces[face_idx];
                    float d = cam.dot(f->plane.normal) - f->plane.dist;
                    if (f->side)
                    {
                        if (d > 0)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if (d < 0)
                        {
                            continue;
                        }
                    }

                    // mark face as visible
                    mark_faces->set(face_idx);
                    int idx = f->texture;
                    f->next = visible_faces[idx];
                    visible_faces[idx] = f;
                    rendered++;
                }
            }
        }
    }

    if (v.size() == 0)
    {
        // set all face BC le 09/08/2006
        face_t *f;
        dface_t *r;
        for (int i = 0; i < bsp->face_count; i++)
        {
            face_t *f = faces[i];
            int idx = f->texture;

            f->next = visible_faces[idx];
            visible_faces[idx] = f;
        }
    }

    for (int ii = 0; ii < EntVar.size(); ii++)
    {
        entvars_s c;
        c = EntVar[ii];
        if (strcmp(EntVar[ii].classname, "func_illusionnary") == 0)
            continue;
        //	if ((EntVar[ii].rendermode==2) || (EntVar[ii].rendermode==5))
        //		continue;

        if (EntVar[ii].brush_model_index < 0)
            continue;

        const dmodel_t &model = bsp->models[EntVar[ii].brush_model_index];

        // discard leafs outside frustum
        short min[3];
        short max[3];
        min[0] = model.mins[0];
        min[1] = model.mins[1];
        min[2] = model.mins[2];
        max[0] = model.maxs[0];
        max[1] = model.maxs[1];
        max[2] = model.maxs[2];

        if (BoxIntersectBox(min, max, mins, maxs) != OUTSIDE)
        // if(frustum_cull2(leaf.mins , leaf.maxs,mins,maxs))
        {
            if (is_in_visibility_list(&v, EntVar[ii].leaf_visibility_list, EntVar[ii].size_leaf))
            {

                unsigned int p = model.firstface;
                // unsigned short* p = bsp->marksurfaces + short(model.firstface);
                for (int x = 0; x < model.numfaces; x++)
                {
                    // don't render those already rendered
                    int face_idx = p++;

                    if (!mark_faces->test(face_idx))
                    {

                        // back face culling
                        face_t *f = faces[face_idx];

                        // if (textures[f->texture]->name!="aaatrigger")
                        if (!(f->flags & TEX_SPECIAL))
                        {
                            // mark face as visible
                            mark_faces->set(face_idx);
                            int idx = f->texture;
                            f->next = visible_faces[idx];
                            visible_faces[idx] = f;
                            rendered++;
                        }
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////
collision_tir world_t::check_tirs_rec(const vec3_t &src, const vec3_t &dir, int lecutoff)
{
    collision_tir tmp;
    if (lecutoff > 0)
    {

        collision_data coldat;
        coldat.src = src;
        coldat.dir = dir;
        coldat.dir_len = dir.len();
        coldat.nearest = distance_gun;

        coldat.ndir = coldat.dir;
        coldat.ndir.normalize();

        for (int i = 0; i < visible_faces.size(); i++)
        {

            face_t *f = visible_faces[i];

            if (f)
            {
                if ((textures[f->texture]->name.compare((string) "{") != 1) && textures[f->texture]->name != "sky")
                {
                    while (f)
                    {
                        lesfaces = lesfaces + 1;
                        check_tir(f, coldat);
                        f = f->next;
                    }
                }
            }
        }

        if (coldat.found)
        {
            vec3_t s = src;
            s += coldat.ndir * (coldat.nearest);
            tmp.pt = s;
            tmp.found = true;
            tmp.nearest = coldat.nearest;
            tmp.normal = coldat.normal;
        }
        else
        {

            tmp.normal = vec3_t(0.0f, 0.0f, 0.0f);
            tmp.pt = src;
            tmp.found = false;
            tmp.nearest = 0;
        }
    }

    else
    {

        tmp.pt = src;
        tmp.found = false;
        tmp.nearest = dir.len();
    }
    return tmp;
}

collision_tir world_t::check_tirs_rec_old(const vec3_t &src, const vec3_t &dir, int lecutoff)
{
    collision_tir tmp;
    if (lecutoff > 0)
    {

        collision_data coldat;
        coldat.src = src;
        coldat.dir = dir;
        coldat.dir_len = dir.len();
        coldat.nearest = distance_gun;

        coldat.ndir = coldat.dir;
        coldat.ndir.normalize();

        for (int i = 0; i < visible_faces.size(); i++)
        {

            face_t *f = visible_faces[i];

            if (f)
            {
                if ((textures[f->texture]->name.compare((string) "{") != 1) && textures[f->texture]->name != "sky")
                {
                    while (f)
                    {
                        lesfaces = lesfaces + 1;
                        check_tir(f, coldat);
                        f = f->next;
                    }
                }
            }
        }

        if (coldat.found)
        {
            vec3_t s = src;
            s += coldat.ndir * (coldat.nearest);
            tmp.pt = s;
            tmp.found = true;
            tmp.nearest = coldat.nearest;
            tmp.normal = coldat.normal;
        }
        else
        {
            if (find_leaf(src + dir) == 0)
            {
                lecutoff--;
                tmp = check_tirs_rec(src, dir * 0.5f, lecutoff);
            }
            else
            {
                tmp.normal = vec3_t(0.0f, 0.0f, 0.0f);
                tmp.pt = src;
                tmp.found = false;
                tmp.nearest = 0;
            }
        }
    }

    else
    {

        tmp.pt = src;
        tmp.found = false;
        tmp.nearest = dir.len();
    }
    return tmp;
}

vec3_t world_t::check_collisions2(const vec3_t &src, const vec3_t &dir, int cutoff)
{
    float epsi;
    epsi = 0.001f;

    if (++cutoff > 5)
    {

        return src;
    }

    collision_data coldat;
    coldat.src = src;
    coldat.dir = dir;
    coldat.dir_len = dir.len();

    if (coldat.dir_len < epsi)
    {

        return src;
    }

    coldat.ndir = coldat.dir / coldat.dir_len;
    for (int i = 0; i < visible_faces.size(); i++)
    {
        face_t *f = visible_faces[i];
        while (f)
        {
            check_collision(f, coldat, radius);
            f = f->next;
        }
    }

    if (coldat.found)
    {
        vec3_t s = src;

        if (coldat.nearest >= epsi)
        {
            s += coldat.ndir * (coldat.nearest - epsi);
        }

        vec3_t dst = src + dir;
        vec3_t n = s - coldat.nearest_poly;

        n.normalize();
        float t = intersect(dst, n, coldat.nearest_poly, n);

        vec3_t newdst = dst + n * t;
        vec3_t newdir = newdst - coldat.nearest_poly;

        return check_collisions(s, newdir, cutoff, false);
    }

    return src + (coldat.ndir * (coldat.dir_len - epsi));
}

//////////////////////////////////////////////////////////////
face_t *skyfaces = NULL;
static int current_style = 1;

bool world_t::is_in_visibility_list(int_vec *v, int *idx_vis, int size_leaf)
{

    for (int i = 0; i < v->size(); i++)
    {
        for (int j = 0; j < size_leaf; j++)
        {
            if (idx_vis[j] == v->at(i))
                return true;
        }
    }

    return false;
}
bool world_t::idx_is_in_visibility_list(int idx_leaf, int *idx_vis, int size_leaf)
{

    for (int j = 0; j < size_leaf; j++)
    {
        if (idx_leaf == idx_vis[j])
            return true;
    }

    return false;
}
void world_t::process_visible_faces(const Camera &c)
{

    rendered = 0;
    // setup for frustum culling
    frustum_setup(c);
    const vec3_t &cam = c.eye();

    // clear stuff
    mark_faces->clear_all();
    zero_dwords((void *)&visible_faces[0], visible_faces.size());
    // visible_faces.clear ();

    // find leaf we're in
    int idx = find_leaf(cam);
    if (idx == -1)
    {
        idx = -1;
    }
    else
    {
    }

    int_vec &v = leaf_visibility[idx];

    if (v.size() == 0)
    {
        // set all face BC le 09/08/2006
        face_t *f;
        dface_t *r;
        for (int i = 0; i < bsp->face_count; i++)
        {
            face_t *f = faces[i];
            int idx = f->texture;

            f->next = visible_faces[idx];
            visible_faces[idx] = f;
        }
    }
    else
    {
        // go thru leaf visibility list
        for (int i = 0; i < v.size(); i++)
        {

            const dleaf_t &leaf = bsp->leaves[v[i]];

            // for(int i = 0; i < bsp->leaf_count ; i++) {

            // const dleaf_t& leaf = bsp->leaves[i];

            // discard leafs outside frustum
            //
            if (!frustum_cull(leaf.mins, leaf.maxs))
            {
                unsigned short *p = bsp->marksurfaces + leaf.firstmarksurface;
                for (int x = 0; x < leaf.nummarksurfaces; x++)
                {
                    // don't render those already rendered
                    int face_idx = *p++;
                    if (!mark_faces->test(face_idx))
                    {

                        // back face culling
                        face_t *f = faces[face_idx];
                        float d = cam.dot(f->plane.normal) - f->plane.dist;
                        //
                        if (f->side)
                        {
                            if (d > 0)
                            {
                                continue;
                            }
                        }
                        else
                        {
                            if (d < 0)
                            {
                                continue;
                            }
                        }

                        // mark face as visible
                        mark_faces->set(face_idx);

                        int idx = f->texture;

                        if (wireframe_mode)
                            f->styles[0] = 1;

                        f->next = visible_faces[idx];
                        visible_faces[idx] = f;

                        rendered++;

                        // bc addon skyface

                        // chose lightmap style
                        f->lightmap = f->lightmaps[0];
                        if (current_style != 0)
                        {
                            for (int c = 0; c < MAX_LIGHTMAPS; c++)
                            {
                                if (f->styles[c] == current_style)
                                {
                                    f->lightmap = f->lightmaps[c];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------

    for (int ii = 0; ii < EntVar.size(); ii++)
    {

        if ((EntVar[ii].rendermode == 2) || (EntVar[ii].rendermode == 5))
            continue;

        if (EntVar[ii].brush_model_index < 0)
            continue;

        const dmodel_t &model = bsp->models[EntVar[ii].brush_model_index];

        // discard leafs outside frustum
        short min[3];
        short max[3];
        min[0] = model.mins[0];
        min[1] = model.mins[1];
        min[2] = model.mins[2];
        max[0] = model.maxs[0];
        max[1] = model.maxs[1];
        max[2] = model.maxs[2];

        if (!frustum_cull(min, max))
        // if(frustum_cull2(leaf.mins , leaf.maxs,mins,maxs))
        {
            bool mark_as_visible;
            if (is_in_visibility_list(&v, EntVar[ii].leaf_visibility_list, EntVar[ii].size_leaf))
            {
                // dans la liste des visible
                mark_as_visible = true;
            }
            else
            {
                mark_as_visible = false;
            }
            if (mark_as_visible || wireframe_mode)
            {
                unsigned int p = model.firstface;
                // unsigned short* p = bsp->marksurfaces + short(model.firstface);
                for (int x = 0; x < model.numfaces; x++)
                {
                    // don't render those already rendered
                    int face_idx = p++;

                    // int res=strncmp( textures[faces[face_idx]->texture]->name.c_str (), "!" , 1 );
                    //	if (res==0)
                    //		continue;

                    if (!mark_faces->test(face_idx))
                    {

                        // back face culling
                        face_t *f = faces[face_idx];
                        // float d = cam.dot(f->plane.normal)-f->plane.dist;
                        // if(f->side) {
                        //     if(d>0) { continue; }
                        // } else {
                        //     if(d<0) { continue; }
                        // }

                        // if (textures[f->texture]->name!="aaatrigger")
                        if (!(f->flags & TEX_SPECIAL))
                        {
                            // mark face as visible
                            mark_faces->set(face_idx);
                            int idx = f->texture;
                            if (wireframe_mode)
                            {
                                if (!mark_as_visible)
                                    f->styles[0] = 1;
                                else
                                    f->styles[0] = 0;
                            }

                            f->next = visible_faces[idx];
                            visible_faces[idx] = f;
                            rendered++;
                        }

                        // chose lightmap style
                        // f->lightmap = f->lightmaps[0];
                        // if(current_style != 0) {
                        //    for(int c = 0; c < MAX_LIGHTMAPS; c++) {
                        //        if(f->styles[c] == current_style) {
                        //            f->lightmap = f->lightmaps[c];
                        //            break;
                        //        }
                        //   }
                        //}
                    }
                }
            }
        }
    }

    // rendered=bsp->num_entities  ;
}
void world_t::process_visible_faces_col(const vec3_t &cam)
{
    rendered = 0;

    // clear stuff
    mark_faces->clear_all();
    zero_dwords((void *)&visible_faces[0], visible_faces.size());

    // find leaf we're in
    int idx = find_leaf(cam);
    int_vec &v = leaf_visibility[idx];

    // go thru leaf visibility list
    for (int i = 0; i < v.size(); i++)
    {

        const dleaf_t &leaf = bsp->leaves[v[i]];

        // discard leafs outside frustum
        //
        //        if(!frustum_cull(leaf.mins, leaf.maxs)) {
        unsigned short *p = bsp->marksurfaces + leaf.firstmarksurface;
        for (int x = 0; x < leaf.nummarksurfaces; x++)
        {
            // don't render those already rendered

            int face_idx = *p++;
            if (textures[faces[face_idx]->texture]->name.compare((string) "!") == 1)
                continue;

            if (!mark_faces->test(face_idx))
            {

                // back face culling
                face_t *f = faces[face_idx];
                float d = cam.dot(f->plane.normal) - f->plane.dist;
                if (f->side)
                {
                    if (d > 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if (d < 0)
                    {
                        continue;
                    }
                }

                // mark face as visible
                mark_faces->set(face_idx);
                int idx = f->texture;
                f->next = visible_faces[idx];
                visible_faces[idx] = f;
                rendered++;

                // chose lightmap style
                f->lightmap = f->lightmaps[0];
                if (current_style != 0)
                {
                    for (int c = 0; c < MAX_LIGHTMAPS; c++)
                    {
                        if (f->styles[c] == current_style)
                        {
                            f->lightmap = f->lightmaps[c];
                            break;
                        }
                    }
                }
            }
        }
    }
    //  }
}

void world_t::process_visible_faces3(const vec3_t &cam)
{
    rendered = 0;
    // setup for frustum culling
    // frustum_setup(c);
    // const vec3_t& cam = c.eye();

    // clear stuff
    mark_faces->clear_all();
    // visible_faces.clear();
    zero_dwords((void *)&visible_faces[0], visible_faces.size());

    // find leaf we're in
    int idx = find_leaf(cam);
    int_vec &v = leaf_visibility[idx];

    // go thru leaf visibility list
    for (int i = 0; i < v.size(); i++)
    {

        const dleaf_t &leaf = bsp->leaves[idx];

        // discard leafs outside frustum
        //
        // if(!frustum_cull(leaf.mins, leaf.maxs)) {
        unsigned short *p = bsp->marksurfaces + leaf.firstmarksurface;
        for (int x = 0; x < leaf.nummarksurfaces; x++)
        {
            // don't render those already rendered
            int face_idx = *p++;
            if (!mark_faces->test(face_idx))
            {

                // back face culling
                face_t *f = faces[face_idx];
                float d = cam.dot(f->plane.normal) - f->plane.dist;
                if (f->side)
                {
                    if (d > 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if (d < 0)
                    {
                        continue;
                    }
                }

                // mark face as visible
                mark_faces->set(face_idx);
                int idx = f->texture;
                f->next = visible_faces[idx];
                visible_faces[idx] = f;
                rendered++;
            }
        }
    }
}

void world_t::drawBox(vec3_t *v)
{

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1, 0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 10; i++)
        glVertex3fv(v[i & 7]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(v[6]);
    glVertex3fv(v[0]);
    glVertex3fv(v[4]);
    glVertex3fv(v[2]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(v[1]);
    glVertex3fv(v[7]);
    glVertex3fv(v[3]);
    glVertex3fv(v[5]);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
}
void world_t::drawBoxEx(const vec3_t pos)
{
    vec3_t po[8];

    vec3_t bbmin;
    bbmin[0] = pos[0] - 2;
    bbmin[1] = pos[1] - 2;
    bbmin[2] = pos[2] - 2;

    vec3_t bbmax;
    bbmax[0] = pos[0] + 2;
    bbmax[1] = pos[1] + 2;
    bbmax[2] = pos[2] + 2;

    po[0][0] = bbmin[0];
    po[0][1] = bbmax[1];
    po[0][2] = bbmin[2];

    po[1][0] = bbmin[0];
    po[1][1] = bbmin[1];
    po[1][2] = bbmin[2];

    po[2][0] = bbmax[0];
    po[2][1] = bbmax[1];
    po[2][2] = bbmin[2];

    po[3][0] = bbmax[0];
    po[3][1] = bbmin[1];
    po[3][2] = bbmin[2];

    po[4][0] = bbmax[0];
    po[4][1] = bbmax[1];
    po[4][2] = bbmax[2];

    po[5][0] = bbmax[0];
    po[5][1] = bbmin[1];
    po[5][2] = bbmax[2];

    po[6][0] = bbmin[0];
    po[6][1] = bbmax[1];
    po[6][2] = bbmax[2];

    po[7][0] = bbmin[0];
    po[7][1] = bbmin[1];
    po[7][2] = bbmax[2];

    glColor4f(1, 0, 0, 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 10; i++)
        glVertex3fv(po[i & 7]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(po[6]);
    glVertex3fv(po[0]);
    glVertex3fv(po[4]);
    glVertex3fv(po[2]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(po[1]);
    glVertex3fv(po[7]);
    glVertex3fv(po[3]);
    glVertex3fv(po[5]);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
}

void world_t::drawVector(const vec3_t pos, const vec3_t dir, int col)
{
    vec3_t po[8];

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    if (col == 0)
        glColor4f(1, 1, 0, 0.5f);
    else
        glColor4f(0, 0, 1, 0.5f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3fv(pos + vec3_t(2.0f, -1.0f, 0.0f));
    glVertex3fv(pos + dir * 50.0f);
    glVertex3fv(pos - vec3_t(2.0f, 1.0f, 0.0f));
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
}
void world_t::DessineFeuille(const Camera &c)
{
    const vec3_t &cam = c.eye();
    int idx = find_leaf(cam);
    if (idx == 0)
    {
        return;
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1, 0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    vec3_t po[8];
    vec3_t bbmin;
    bbmin[0] = bsp->leaves[idx].mins[0];
    bbmin[1] = bsp->leaves[idx].mins[1];
    bbmin[2] = bsp->leaves[idx].mins[2];

    vec3_t bbmax;
    bbmax[0] = bsp->leaves[idx].maxs[0];
    bbmax[1] = bsp->leaves[idx].maxs[1];
    bbmax[2] = bsp->leaves[idx].maxs[2];

    po[0][0] = bbmin[0];
    po[0][1] = bbmax[1];
    po[0][2] = bbmin[2];

    po[1][0] = bbmin[0];
    po[1][1] = bbmin[1];
    po[1][2] = bbmin[2];

    po[2][0] = bbmax[0];
    po[2][1] = bbmax[1];
    po[2][2] = bbmin[2];

    po[3][0] = bbmax[0];
    po[3][1] = bbmin[1];
    po[3][2] = bbmin[2];

    po[4][0] = bbmax[0];
    po[4][1] = bbmax[1];
    po[4][2] = bbmax[2];

    po[5][0] = bbmax[0];
    po[5][1] = bbmin[1];
    po[5][2] = bbmax[2];

    po[6][0] = bbmin[0];
    po[6][1] = bbmax[1];
    po[6][2] = bbmax[2];

    po[7][0] = bbmin[0];
    po[7][1] = bbmin[1];
    po[7][2] = bbmax[2];

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 10; i++)
        glVertex3fv(po[i & 7]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(po[6]);
    glVertex3fv(po[0]);
    glVertex3fv(po[4]);
    glVertex3fv(po[2]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    glVertex3fv(po[1]);
    glVertex3fv(po[7]);
    glVertex3fv(po[3]);
    glVertex3fv(po[5]);
    glEnd();

    // glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix
}
bool world_t::DessineEntites(vec3_t pos)
{

    bool resultat = false;

    vec3_t po[8];
    short bbmin[3];
    short bbmax[3];
    short bbmin2[3];
    short bbmax2[3];

    bbmin2[0] = pos[0] - 30;
    bbmin2[1] = pos[1] - 30;
    bbmin2[2] = pos[2] - 30;
    bbmax2[0] = pos[0] + 30;
    bbmax2[1] = pos[1] + 30;
    bbmax2[2] = pos[2] + 30;

    for (int i = 0; i < pos_ladder.size(); i++)
    {
        bbmin[0] = bsp->models[pos_ladder[i]].origin[0] + bsp->models[pos_ladder[i]].mins[0];
        bbmin[1] = bsp->models[pos_ladder[i]].origin[1] + bsp->models[pos_ladder[i]].mins[1];
        bbmin[2] = bsp->models[pos_ladder[i]].origin[2] + bsp->models[pos_ladder[i]].mins[2];
        bbmax[0] = bsp->models[pos_ladder[i]].origin[0] + bsp->models[pos_ladder[i]].maxs[0];
        bbmax[1] = bsp->models[pos_ladder[i]].origin[1] + bsp->models[pos_ladder[i]].maxs[1];
        bbmax[2] = bsp->models[pos_ladder[i]].origin[2] + bsp->models[pos_ladder[i]].maxs[2];

        if (BoxIntersectBox(bbmin2, bbmax2, bbmin, bbmax) != OUTSIDE)
        {
            resultat = true;
            break;
        }
    }

    return resultat;
}

float world_t::get_distance(const vec3_t &pos, const vec3_t &dir)
{
    vec3_t dst;
    vec3_t new_dir;

    new_dir[0] = dir[0];
    new_dir[1] = dir[1];
    new_dir[2] = dir[2];

    dst = pos + new_dir;

    // find leaf we're in
    int idx = find_leaf(dst);
    if (idx == 0)
    {
        // on traverse !
        if (new_dir.len() > 0.1f)
        { // a voir avec un cutoff
            new_dir = new_dir / 2;
            return get_distance(pos, new_dir);
        }
        else
        {
            return 0;
        }
    }

    else
    {
        // on est dans une leaf
        // parcours
        return new_dir.len();
    }
}

void world_t::render_visible_faces(vec3_t &pos_light, bool is_light)

{
    // bool islight=true;

    if (is_light)
    {
        GLfloat bleu[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat white[] = {0.0, 0.0, 0.0, 1.0};
        // vec3_t lum=vec3_t(125.0f,1600.0f,59.0f);

        //  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        //
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

        //  glLightfv(GL_LIGHT0, GL_AMBIENT, white);

        glLightfv(GL_LIGHT1, GL_AMBIENT, white);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, bleu);
        glLightfv(GL_LIGHT1, GL_SPECULAR, bleu);
        GLfloat position[4];
        position[0] = pos_light[0];
        position[1] = pos_light[1];
        position[2] = pos_light[2];
        position[3] = 1.0f;
        glLightfv(GL_LIGHT1, GL_POSITION, position);
        glEnable(GL_LIGHT1);
        // glEnable(GL_LIGHT0);

        // glLightfv(GL_LIGHT0, GL_AMBIENT, white);
        // glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
        // glLightfv(GL_LIGHT0, GL_SPECULAR, white);
        glEnable(GL_LIGHTING);
        // glEnable(GL_LIGHT0);

        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL); //
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // BART fix

    int face_idx = 0;
    lesfaces = 0;
    for (int i = 0; i < visible_faces.size(); i++)
    {
        face_t *f = visible_faces[i];
        if ((f) && (textures[f->texture]->name != "sky"))
        {
            if (textures[f->texture]->name.compare((string) "{") == 1)
            {
                glAlphaFunc(GL_GREATER, 0.3f);
                glEnable(GL_ALPHA_TEST);
            }

            while (f)
            {
                face_idx++;
                lesfaces = lesfaces + 1;

                render_face(f);
                f = f->next;
            }
        }
    }
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_FUNC);
    glDepthMask(GL_TRUE);
    if (is_light)
        glDisable(GL_LIGHTING);
}

void world_t::CheckCollision()
{

}

void world_t::decompress()
{

    for (int i = 0; i < bsp->leaf_count; i++)
    {
        int_vec visible;
        dleaf_t &leaf = bsp->leaves[i];
        int v = leaf.visofs;
        const byte *vis = bsp->vis;
        for (int c = 1; c < bsp->models[0].visleafs; v++)
        {
            if (vis[v] == 0)
            {
                v++;
                c += (vis[v] << 3);
            }
            else
            {
                for (byte bit = 1; bit; bit <<= 1, c++)
                {
                    if (vis[v] & bit)
                    {
                        visible.push_back(c);
                    }
                }
            }
        }

        leaf_visibility.push_back(visible);
    }
}

void world_t::load(const char *file, int gamma)
{

    //	cleanup();
    flag_gign_rec = vec3_t(0.0f, 0.0f, 0.0f);
    flag_terro_rec = vec3_t(0.0f, 0.0f, 0.0f);
    flag_terro = vec3_t(0.0f, 0.0f, 0.0f);
    flag_gign = vec3_t(0.0f, 0.0f, 0.0f);
    got_flag = false;

    bsp = new bsp_file_t(file);

    if (!bsp)
        throw out_of_memory();

    // basic setup
    decompress();
    // setup...
    mark_faces = new bitset(bsp->face_count);
    if (!mark_faces)
        throw out_of_memory();
    colide_faces = new bitset(bsp->face_count);

    faces.resize(bsp->face_count); // for 1:1 index mapping

    // setup textures, faces and lightmaps...
    for (int i = 0; i < bsp->face_count; i++)
    {

        dface_t *f = bsp->faces + i;
        face_t *face = new face_t;
        if (!face)
            throw out_of_memory();

        // get plane info
        memcpy(&face->plane, bsp->planes + f->planenum, sizeof(dplane_t));
        face->side = f->side;

        // determine best primitive type
        switch (f->numedges)
        {
        case 3:
            face->type = GL_TRIANGLES;
            break;
        case 4:
            face->type = GL_QUADS;
            break;
        default:
            face->type = GL_POLYGON;
        }

        // get texture
        // create a new one if it doesn't exist yet
        texinfo_t *ti = bsp->texinfo + f->texinfo;
        face->flags = ti->flags;
        const char *tex_name = strlwr(bsp->textures[ti->miptex].name);
        int x = find_texture(tex_name);
        if (x == -1)
        {
            int i; // multi texture
            if (strcspn(tex_name, "+") != 0)
            {
                i = 1;
            }
            else
            {
                i = 8;
            }
            int anim = 0;
            int first_text = 0;
            for (int j = 0; j < i; j++)
            {
                texture_t *t = new texture_t;
                if (!t)
                    throw out_of_memory();
                char buff[100];
                if (strcspn(tex_name, "+") == 0)
                {
                    string s;
                    s = (string)tex_name;
                    s = s.substr(3);
                    char bufsf[3];
                    sprintf(bufsf, "+%d~", j);
                    string ss;
                    ss = (string)bufsf + s;

                    strncpy(buff, ss.c_str(), 30);
                }
                else
                {
                    sprintf(buff, "%s", tex_name);
                }

                t->name = buff;

                rgb_image_t r;
                wads.get(buff, &r);
                if (r.data)
                {

                    if (strcspn(buff, "{") == 0)
                    {

                        t->t = new Texture(r.data, r.width, r.height, GL_RGBA);
                        // t->t = new Texture(img.data(), img.width(), img.height(), GL_ALPHA);
                    }
                    else
                    {
                        t->t = new Texture(r.data, r.width, r.height, GL_RGB);
                        // t->t = new Texture(img.data(), img.width(), img.height(), GL_RGB);
                    }
                    if (!t->t)
                        throw out_of_memory();

                    textures.push_back(t);
                    if (first_text == 0)
                        first_text = textures.size() - 1;
                    anim++;
                }
                else
                {
                    int sr = 12;
                    delete (t);
                }
            }
            x = textures.size() - 1;

            if (first_text > 0)
            {
                textures[first_text]->anim = anim;
                x = first_text;
            }
            else
                x = textures.size() - 1;

            // delete(t);
        }
        face->texture = x; // texture index

        //
        // the following computations are based on:
        // PolyEngine (c) Alexey Goloshubin
        //

        // compute s and t extents
        float min[2], max[2];
        min[0] = min[1] = 100000;
        max[0] = max[1] = -100000;
        int c;
        for (c = 0; c < f->numedges; c++)
        {
            dvertex_t *v;
            int eidx = bsp->edge_list[f->firstedge + c];
            if (eidx >= 0)
            {
                v = bsp->vertices + bsp->edges[eidx].v[0];
            }
            else
            {
                v = bsp->vertices + bsp->edges[-eidx].v[1];
            }

            // compute extents
            for (int x = 0; x < 2; x++)
            {
                float d = v->point[0] * ti->vecs[x][0] + v->point[1] * ti->vecs[x][1] + v->point[2] * ti->vecs[x][2] +
                          ti->vecs[x][3];

                if (d < min[x])
                    min[x] = d;
                if (d > max[x])
                    max[x] = d;
            }
        }
        face->lw = 0;
        face->lh = 0;
        face->lightofs = 0;

        int lw, lh;
        if (face->flags == 0)
        {
            // compute lightmap size
            int size[2];
            for (c = 0; c < 2; c++)
            {
                float tmin = (float)floor(min[c] / 16.0f);
                float tmax = (float)ceil(max[c] / 16.0f);

                size[c] = (int)(tmax - tmin);
            }
            lw = size[0] + 1;
            lh = size[1] + 1;

            int lsz = lw * lh * 3;

            face->lw = lw;
            face->lh = lh;
            face->lightofs = f->lightofs;

            // et oui c ici la banane avec xp et shogun et wog
            // en fait le lsz pour certaines faces est egale a 20 000 voir 70 000
            // alors que pour la plupart ca ne depasse pas 700
            // donc apparemment le memcopy explose....
            // donc j'ai ete violent si < 1000 on cree le lightmap
            // sinon
            // quedal
            // et le pire c que ca a l'air de marcher lol

            if (lsz <= 1000) // SHOGUN WAYOFGUN XP
            {

                for (c = 0; c < 1; c++)
                {
                    if (f->styles[c] == -1)
                        break;
                    face->styles[c] = f->styles[c];
                    rgb_image_t r;
                    r.width = lw;
                    r.height = lh;
                    r.data = new byte[lsz];
                    memcpy(r.data, bsp->lightmaps + f->lightofs + (lsz * c), lsz);
                    // ChangeGamma(r.data,lsz,gamma);
                    face->lightmaps[c] =
                        new Texture(r.data, r.width, r.height, GL_RGB, Texture::nearest_mipmap_nearest);
                }

                face->lightmap = face->lightmaps[0];
            }
            else
            {
                int r = 0;
            }
        }

#if 0
        static ofstream o("log/lightmaps.log");  
        o << "face[" << i << "]: ";
        for(c = 0; c < MAX_LIGHTMAPS; c++) {
            o << face->styles[c] << " ";
        }
        o << endl;
#endif

        //////////////////////////////////////////////////////

        float is = 1.0f / (float)bsp->textures[ti->miptex].width;
        float it = 1.0f / (float)bsp->textures[ti->miptex].height;

        face->first = vt_array.current();
        face->count = f->numedges;
        for (c = 0; c < f->numedges; c++)
        {
            float v[7];
            int eidx = *(bsp->edge_list + f->firstedge + c);
            if (eidx < 0)
            {
                eidx = -eidx;
                dedge_t *e = bsp->edges + eidx;
                v[0] = bsp->vertices[e->v[1]].point[0];
                v[1] = bsp->vertices[e->v[1]].point[1];
                v[2] = bsp->vertices[e->v[1]].point[2];
            }
            else
            {
                dedge_t *e = bsp->edges + eidx;
                v[0] = bsp->vertices[e->v[0]].point[0];
                v[1] = bsp->vertices[e->v[0]].point[1];
                v[2] = bsp->vertices[e->v[0]].point[2];
            }

            vec3_t vertex = v;
            float s = vertex.dot(ti->vecs[0]) + ti->vecs[0][3];
            float t = vertex.dot(ti->vecs[1]) + ti->vecs[1][3];

            v[3] = s * is;
            v[4] = t * it;

            if (face->flags == 0)
            {
                // compute lightmap coords
                float mid_poly_s = (min[0] + max[0]) / 2.0f;
                float mid_poly_t = (min[1] + max[1]) / 2.0f;
                float mid_tex_s = (float)lw / 2.0f;
                float mid_tex_t = (float)lh / 2.0f;
                float ls = mid_tex_s + (s - mid_poly_s) / 16.0f;
                float lt = mid_tex_t + (t - mid_poly_t) / 16.0f;
                ls /= (float)lw;
                lt /= (float)lh;

                v[5] = ls;
                v[6] = lt;
            }

            ////////////////////////
            vt_array.add(v);
            st_array.add(v + 3);
            lst_array.add(v + 5);
        }

        ///////////////////////////////
        vec3_t n = face->plane.normal;
        float d = face->plane.dist;
        if (face->side)
        {
            face->p = plane_t(-n, d);
        }
        else
        {
            face->p = plane_t(n, -d);
        }

        // add face
        faces[i] = face;
    }

    // set visible faces size to total texture count
    visible_faces.resize(textures.size());

    // setup vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vt_array);

    if (use_multi)
    {
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, st_array);

        glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, lst_array);
    }
    InitSkyBox();
    InitPositions();
    LoadEntVars();
    //	AffecteGammaSurUneFace(gamma);
}

void world_t::ChangeGamma(unsigned char *image, int size, float factor)
{

    // Go through every pixel in the lightmap
    for (int i = 0; i < size / 3; i++, image += 3)
    {
        // Holds the information for modifying the gamma
        float scale = 1, temp = 0,

              // Holds the color information
            red = 0, green = 0, blue = 0;

        // Get the colors stored in the image
        red = (float)image[0];
        green = (float)image[1];
        blue = (float)image[2];

        // Multiply in the passed in factor of the image and make sure it is in scale
        red = red + factor * 10.0f;     /// 255.0f;
        green = green + factor * 10.0f; // / 255.0f;
        blue = blue + factor * 10.0f;   // / 255.0f;

        if (red > 255.0f)
            red = 255.0f;

        if (green > 255.0f)
            green = 255.0f;
        if (blue > 255.0f)
            blue = 255.0f;

        // Input the new values back into the image
        image[0] = (unsigned char)red;
        image[1] = (unsigned char)green;
        image[2] = (unsigned char)blue;
    }
}
void world_t::set_znear()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(frustum.left, frustum.right, frustum.bottom, frustum.top, znear, frustum.zfar);
    glMatrixMode(GL_MODELVIEW);
}
bool world_t::LeJoueur_est_il_visible(const Camera &c, const vec3_t &pos)
{

    frustum_setup(c);

    short maxs[3];
    short mins[3];

    maxs[0] = pos[0] + 10.0f;
    maxs[1] = pos[1] + 10.0f;
    maxs[2] = pos[2] + 10.0f;

    mins[0] = pos[0] - 10.0f;
    mins[1] = pos[1] - 10.0f;
    mins[2] = pos[2] - 10.0f;

    if (frustum_cull(mins, maxs))
    {
        return false; // il y est pas
    }
    else
    {
        return true;
    }
}

void world_t::render_entvars(const Camera &c)
{
    int ent_index;
    int model_index;
    // int pass;
    vec3_t v1, v2, v3;
    //	float vl[3];
    //	float dott;
    float d;
    int ii;

    v1 = c.fwd_vec();
    const vec3_t &cam = c.eye();

    for (int pass = 0; pass < 1; pass++)
    {
        for (ii = EntVar.size() - 1; ii >= 0; ii--)
        // for( ii = 0; ii <EntVar.size()  ; ii++)
        {
            if (EntVar[ii].brush_model_index <= 0)
                continue;

            model_index = EntVar[ii].brush_model_index;

            // glPushMatrix();

            dmodel_t *m = &bsp->models[model_index];

            short min[3];
            short max[3];
            min[0] = m->mins[0];
            min[1] = m->mins[1];
            min[2] = m->mins[2];
            max[0] = m->maxs[0];
            max[1] = m->maxs[1];
            max[2] = m->maxs[2];

            if (frustum_cull(min, max))
                continue;

            // glTranslatef(m->origin[0],m->origin[1],m->origin[2]);

            glEnable(GL_BLEND); // Turn Blending On
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glDisable(GL_ALPHA_TEST);
            if (EntVar[ii].rendermode == 4)
            {
                glAlphaFunc(GL_GREATER, 0.3f);
                glEnable(GL_ALPHA_TEST);
            }

            // glDepthMask(GL_FALSE);
            // glDisable(GL_DEPTH_FUNC);

            glColor4f(1.0, 1.0, 1.0, EntVar[ii].renderamt);

            //		glColor4f(0.0f,0.0f,1.0f,0.5f);
            // glDepthMask(GL_FALSE);
            //				glEnable(GL_DEPTH_FUNC);
            // glDepthMask(GL_TRUE);

            for (int c = 0; c < m->numfaces; c++)
            {
                face_t *face = faces[m->firstface + c];

                if (face == NULL)
                    continue;

                // is this NOT a special texture?
                if (!(face->flags & TEX_SPECIAL))
                    d = cam.dot(face->plane.normal) - face->plane.dist;

                render_face(face);
            }

            glDisable(GL_BLEND); // Turn Blending Off
            glDisable(GL_ALPHA_TEST);

            // glPopMatrix();
        }
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void world_t::render_entvarsInvi(const Camera &c)
{
    int ent_index;
    int model_index;
    // int pass;
    vec3_t v1, v2, v3;
    float vl[3];
    float dott;
    float d;
    int ii;

    v1 = c.fwd_vec();
    const vec3_t &cam = c.eye();

    for (ii = 0; ii < EntVarInvi.size(); ii++)
    {

        if (EntVarInvi[ii].brush_model_index <= 0)
            continue;

        model_index = EntVarInvi[ii].brush_model_index;

        dmodel_t *m = &bsp->models[model_index];

        short min[3];
        short max[3];
        min[0] = m->mins[0];
        min[1] = m->mins[1];
        min[2] = m->mins[2];
        max[0] = m->maxs[0];
        max[1] = m->maxs[1];
        max[2] = m->maxs[2];
        if (frustum_cull(min, max))
            continue;

        glEnable(GL_BLEND); // Turn Blending On
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(1.0, 1.0, 1.0, EntVarInvi[ii].renderamt);

        for (int c = 0; c < m->numfaces; c++)
        {
            face_t *face = faces[m->firstface + c];

            if (face == NULL)
                continue;

            // is this NOT a special texture?
            if (!(face->flags & TEX_SPECIAL))
                render_face(face);
        }
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void world_t::AffecteGammaSurUneFace(int gamma)
{

    for (int J = 0; J < faces.size(); J++)
    {
        face_t *f = faces[J];

        if (f->flags == 0)
        {

            // on nettoie
            for (int i = 0; i < MAX_LIGHTMAPS; i++)
            {
                if (f->lightmaps[i])
                {
                    delete f->lightmaps[i];
                    f->lightmaps[i] = 0;
                }
            }

            int lsz = f->lw * f->lh * 3;

            // et oui c ici la banane avec xp et shogun et wog
            // en fait le lsz pour certaines faces est egale a 20 000 voir 70 000
            // alors que pour la plupart ca ne depasse pas 700
            // donc apparemment le memcopy explose....
            // donc j'ai ete violent si < 1000 on cree le lightmap
            // sinon
            // quedal
            // et le pire c que ca a l'air de marcher lol

            if (lsz <= 1000) // SHOGUN WAYOFGUN XP
            {

                for (int c = 0; c < 1; c++)
                {
                    if (f->styles[c] == -1)
                        break;

                    rgb_image_t r;
                    r.width = f->lw;
                    r.height = f->lh;
                    r.data = new byte[lsz];
                    memcpy(r.data, bsp->lightmaps + f->lightofs + (lsz * c), lsz);

                    ChangeGamma(r.data, lsz, gamma);
                    f->lightmaps[c] = new Texture(r.data, r.width, r.height, GL_RGB, Texture::linear_mipmap_linear);
                }

                f->lightmap = f->lightmaps[0];
            }
        }
    }
}

void world_t::calcul_visibility_entvars(void)
{
    for (int ii = 0; ii < EntVar.size(); ii++)
    {

        if ((EntVar[ii].rendermode == 2) || (EntVar[ii].rendermode == 5))
            continue;

        if (EntVar[ii].brush_model_index < 0)
            continue;

        const dmodel_t &model = bsp->models[EntVar[ii].brush_model_index];

        vec3_t center;
        center[0] = (model.mins[0] + model.maxs[0]) / 2.0f;
        center[1] = (model.mins[1] + model.maxs[1]) / 2.0f;
        center[2] = (model.mins[2] + model.maxs[2]) / 2.0f;

        int idxbb = find_leaf(center);
        if (!idx_is_in_visibility_list(idxbb, EntVar[ii].leaf_visibility_list, EntVar[ii].size_leaf))
        {
            if (EntVar[ii].size_leaf < 15)
            {
                EntVar[ii].leaf_visibility_list[EntVar[ii].size_leaf] = idxbb;
                EntVar[ii].size_leaf++;
            }
        }

        unsigned int p = model.firstface;
        // unsigned short* p = bsp->marksurfaces + short(model.firstface);
        for (int x = 0; x < model.numfaces; x++)
        {
            // don't render those already rendered
            int face_idx = p++;
            face_t *f = faces[face_idx];
            if (!(f->flags & TEX_SPECIAL))
            {

                vec3_t pt(vt_array[f->first]);
                int idx = find_leaf(pt);
                if (!idx_is_in_visibility_list(idx, EntVar[ii].leaf_visibility_list, EntVar[ii].size_leaf))
                {
                    if (EntVar[ii].size_leaf < 15)
                    {
                        EntVar[ii].leaf_visibility_list[EntVar[ii].size_leaf] = idx;
                        EntVar[ii].size_leaf++;
                    }
                }
            }
        }
        // discard leafs outside frustum
        /*
         */
    }
}