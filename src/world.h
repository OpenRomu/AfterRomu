#ifndef __WORLDH__
#define __WORLDH__

#include "geom.h"
#include <vector>
#include <string>
using namespace std;
#include "glwin.h"
#include "texture.h"
#include "matrix.h"
#include "bsp.h"
#include "bitset.h"
#include "wad.h"
//#include <iostream>
#define distance_gun (20000.00f)
#define MAX_LIGHTMAPS    (4)

// #define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

//static double znear = 1.0;


typedef vector<int> int_vec;


static float
intersect_sphere(const vec3_t& r, //point E
                 const vec3_t& rv, // direction
                 const vec3_t& s, // centre du cercle
                 float sr) // rayon
{ ///http://www.acm.org/crossroads/xrds3-4/raytracing.html
		//sr = rayon
	vec3_t q = s-r;   
    float c = q.len();   //q vecteur entre origine et E 
						//c = distance entre origine et E
	float v = q.dot(rv);    // rv=EO=   q=s-r=V
	//v= distance entre r et le point d'intersection perpendiculaire a la normal

    float d = sr*sr - (c*c-v*v); // sr : rayon     c:
	//d = sqrt( sr^2 - (c^2 - v^2))
    
    if(d < 0.0f)
		return -1.0f;
    return v-sqrt(d);
}


struct frustum_t {
    double left;
    double right;
    double bottom;
    double top;
    double znear;
    double zfar;
};

//static frustum_t frustum;

/*static void
set_znear() 
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(frustum.left, frustum.right, 
              frustum.bottom, frustum.top, 
              znear, frustum.zfar);
    glMatrixMode(GL_MODELVIEW);
}*/

class collision_data {
public:
    collision_data() : found(false), nearest(-1.0f), dir_len(0.0f) {}

    vec3_t src;
    vec3_t dir;
    vec3_t ndir;
	vec3_t normal;
    float  dir_len;

    bool found;
    float nearest;
    vec3_t nearest_poly;
};





struct frustum_plane_t {
    vec3_t normal;
    float dist;
};


class texture_t {
public:
    texture_t() : t(0),anim(0) {}
    ~texture_t() { if(t) delete t; }

    void bind() { t->bind(); }
    Texture* t;
    string name;
	int anim;
};
class face_t {
public:
    face_t() : texture(-1), lightmap(0), next(0) {
        for(int i = 0; i < MAX_LIGHTMAPS;i++) {
            styles[i] = -1;
            lightmaps[i] = 0;
        }
    }
    ~face_t() { 
        for(int i = 0; i < MAX_LIGHTMAPS; i++) {
            if(lightmaps[i]) {
                delete lightmaps[i];
                lightmaps[i] = 0;
                styles[i] = -1;
            }
        }
    }

	int lw;
	int lh;
	int lightofs;

    int type;
    int flags;
    int texture; // index into texture
    Texture* lightmap; // current lightmap
    Texture* lightmaps[MAX_LIGHTMAPS];
    int         styles[MAX_LIGHTMAPS];

    dplane_t plane;
    plane_t p;
    int side;

    int first;
    int count;

    face_t* next;
};


class TCollisionPacket {
public:
    TCollisionPacket() : foundCollision(false), nearestDistance(-1.0f), stuck(false) {}
    vec3_t velocity;
    vec3_t sourcePoint;
    vec3_t eRadius;
    vec3_t lastSafePosition;
	bool stuck; 
	bool foundCollision;
	double    nearestDistance; // nearest distance to hit
	vec3_t nearestIntersectionPoint; // on sphere
	vec3_t nearestPolygonIntersectionPoint; // on polygon
};

class collision_tir {
public:
    collision_tir() : found(false), nearest(10000.00f) {}
    vec3_t pt;
    vec3_t normal;
    bool found;
    float nearest;
};
class Camera {
public:
    Camera(float x = 0.0f, float y = 0.0f, float z = 0.0f,
           float pitch = 0.0f, float roll = 0.0f, float yaw = 0.0f)
           : m_pitch(pitch), m_roll(roll), m_yaw(yaw)
    {
        m_eye[0] = x, m_eye[1] = y, m_eye[2] = z;             
    }

    inline void move_delta(float xd, float yd, float zd) {
        m_eye[0]+=xd, m_eye[1]+=yd, m_eye[2]+=zd;
    }
    inline void move_delta(const vec3_t& v) {
        m_eye[0]+=v[0], m_eye[1]+=v[1], m_eye[2]+=v[2];
    }
    inline void move(float x, float y, float z) {
        m_eye[0]=x, m_eye[1]=y, m_eye[2]=z;
    }
    inline void move(const vec3_t& v) {
        m_eye[0]=v[0], m_eye[1]=v[1], m_eye[2]=v[2];
    }
    inline void rotate_delta(float pitch_delta,
                             float roll_delta,
                             float yaw_delta) 
    {
        m_pitch += pitch_delta;
        m_roll += roll_delta;
        m_yaw += yaw_delta;
        //pitch_bounds();
    }
    inline void rotate(float pitch, float roll, float yaw)  {
        m_pitch = pitch;
        m_roll = roll;
        m_yaw = yaw;
        //pitch_bounds();
    }
    inline void rotate_delta(const vec3_t& v) {
        m_pitch += v[0],m_roll += v[1],m_yaw += v[2];
        //pitch_bounds();
    }
    inline void rotate(const vec3_t& v)  {
        m_pitch = v[0],m_roll = v[1],m_yaw = v[2];
        //pitch_bounds();
    }

    inline void pitch_bounds() {
        if(m_pitch>25.0f) { 
            m_pitch=25.0f;
        }
        else 
        if(m_pitch<-25.f) {
            m_pitch=-25.0f;
        }
    }

    void load() const { glLoadMatrixf(cam); }

    inline void transform() {
        
        cam.load_identity();
        matrix_t r;
        r.xrot(m_pitch);
        cam*=r;

        r.load_identity();
        r.yrot(m_roll);
        cam*=r;

        r.load_identity();
        r.zrot(m_yaw);
        cam*=r;
		
	 	cam.translate(-m_eye[0],-m_eye[1],-m_eye[2]);

    }

    inline vec3_t right_vector() const {
        return vec3_t(cam[0],cam[4],cam[8]);
    }
    inline vec3_t up_vector() const {
        return vec3_t(cam[1],cam[5],cam[9]);
    }
    inline vec3_t fwd_vec() const {
        return vec3_t(cam[2],cam[6],cam[10]);
    }

    // accessors
    float pitch() const { return m_pitch; }
    float roll() const { return m_roll; }
    float yaw() const { return m_yaw; }

    const float* eye() const { return m_eye; }
    const matrix_t& matrix() const { return cam; }
	matrix_t cam;
private:
    float   m_eye[3];
    float   m_pitch;
    float    m_roll;
    float   m_yaw;

   
};

class world_t {
public:
    world_t() : wireframe_mode(false),anim_tex(0),bsp(0),epsilon(0.3f), mark_faces(0),SkyRange(6000.0),znear(1.0),use_multi(true),got_flag(false) {

			//_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF); 
// _CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE); 
// _CrtSetReportFile(_CRT_ASSERT,_CRTDBG_FILE_STDERR); 
//_CrtSetBreakAlloc(37152);


}
    ~world_t() { 
		cleanup();
	}
	float radius;
	float epsilon;
	vec3_t BspMin;
	vec3_t BspMax;
	vector<vec3_t>     pos_gign;
	vector<vec3_t>     pos_terro;
	vector<vec3_t>     pos_armor;
	vector<int>     pos_ladder;
	vector<entvars_s>  EntVar;
	vector<entvars_s>  EntVarInvi;
    bool got_flag;
	vec3_t flag_gign;
	vec3_t flag_terro;

	vec3_t flag_gign_rec;
	vec3_t flag_terro_rec;
int anim_tex;
float delta;
	bool wireframe_mode;

    Texture* TexturesSky[6];
	double SkyRange;
	bool collide;
	vec3_t normale_face_touchee;
	vec3_t trace;
	float SkyTaille;
	vec3_t SkyOri;
	void InitPositions();
	vec3_t RenvoiePosition(bool mod);

	//** CFT renvoie position pour les flags
	vec3_t RenvoiePositionFlag(int team,int quoi);
	//**

	void load(const char* file,int gamma);
    void wad(const char* file) { wads.add(file); }
	void InitSkyBox();
	void DessineSkyBox(vec3_t pos);
	void render_skyfaces(vec3_t pos);

	bool DessineEntites (vec3_t pos);
    void projection_setup(int width, int height);
    void process_visible_faces(const Camera& cam);
	bool is_in_visibility_list(int_vec * v,int * idx_vis,int size_leaf);
	bool idx_is_in_visibility_list(int idx_leaf,int * idx_vis,int size_leaf);
	void process_visible_faces_col(const vec3_t& cam);
	void process_visible_faces3(const vec3_t& cam);
	void DessineFeuille (const Camera& c);
	void render_visible_faces(vec3_t &pos_light,bool is_light);
    void render_model(int i);
	void process_visible_faces_collide(const vec3_t& cam,const vec3_t& dest);
	void LoadEntVars(void);
	void render_entvars(const Camera& c);
	void render_entvarsInvi(const Camera& c);
	void calcul_visibility_entvars(void);
    // collision stuff
	void CheckCollision();
	void drawBox (vec3_t *v);
	void drawBoxEx (const vec3_t pos);
	void drawVector(const vec3_t pos,const vec3_t dir,int col);

     
  vec3_t check_collisions(const vec3_t& src, const vec3_t& dir,int cutoff,bool fill_colide_face);
 vec3_t check_collisions_yeux(const vec3_t& src, const vec3_t& dir,int cutoff,bool fill_colide_face);
	//collision_tir check_collisions_bat(const vec3_t& src, const vec3_t& dir,int cutoff);
 void check_collisions_bat(const vec3_t& src, vec3_t dir, collision_tir * out, float radius);
	collision_tir check_tirs(const vec3_t& src, const vec3_t& dir);
	collision_tir check_tirs_rec(const vec3_t& src, const vec3_t& dir,int lecutoff);
	collision_tir check_tirs_rec_old(const vec3_t& src, const vec3_t& dir,int lecutoff);
	bool LeJoueur_est_il_visible(const Camera& c,const vec3_t& pos);
	vec3_t check_collisions2(const vec3_t& src, const vec3_t& dir,int cutoff);
	
	//frustrum
	double znear ;
	frustum_t frustum;
	void set_znear();
	void ChangeGamma(unsigned char  *image, int size, float factor);
	//-----------------

	//---------------------- bounding box
	
	bool isect_const_xyz(const int quoi,const float x, const vec3_t& p1,const vec3_t& p2,vec3_t& out );
	bool pip_const_x(const vec3_t& p,const short* mins, const short* maxs);
	bool pip_const_y(const vec3_t& p,const short* mins, const short* maxs);
	bool pip_const_z(const vec3_t& p,const short* mins, const short* maxs);
	bool intersectbox(const vec3_t& p1,const vec3_t& p2,const short* vmin, const short* vmax);
    int line_test(float v0, float v1, float w0, float w1);
 	int BoxIntersectBox(const short* vmin1, const short* vmax1,const short* vmin2, const short* vmax2) ;

	enum {
				 OUTSIDE     = 0,
				 ISEQUAL     = (1<<0),
				 ISCONTAINED = (1<<1),
				 CONTAINS    = (1<<2),
				 CLIPS       = (1<<3),
			 };
	bool use_multi;
	float get_distance(const vec3_t& pos,const vec3_t& dir);
    //accessors
    int render_count() const { return rendered; }
	int	find_leaf(const vec3_t& coords);
    vec3_t GetPosition(const vec3_t& position, const vec3_t& velocity);
	vec3_t collideWithWorld(const  vec3_t&  position, const  vec3_t&  velocity);
	void AffecteGammaSurUneFace(int gamma);
	vec3_t ellipsoidRadius;
     TCollisionPacket  collision;
	 void cleanup();
void advance_frame(float delta_f);
    
protected:
    
    int find_texture(const char* name) {
        for(int i = 0; i < textures.size(); i++) {
            if(textures[i]->name == name) {
                return i;
            }
        }

        return -1;
    }

    void frustum_setup(const Camera& cam);
    bool frustum_cull(const short* mins, const short* maxs);
    void render_face(const face_t* f);
	void decompress();

private:
    bsp_file_t*         bsp;
    bitset*             mark_faces;
	bitset*             colide_faces;
   
    vector<face_t*>     faces;
    vector<texture_t*>  textures;
    wad_man_t           wads;
    vector<int_vec>     leaf_visibility;
  // visible faces by texture index
    vector<face_t*>     visible_faces;
	
    // frustum clipping planes
    frustum_plane_t frustum_planes[5];
	

    // frustum params
    double projection;
    double angle_horiz;
    double angle_vert;
    double sin_horiz;
    double cos_horiz;
    double sin_vert;
    double cos_vert;
bool colisionne;
    // statistics
    int clip_count;
    int rendered;
	
};

#endif
