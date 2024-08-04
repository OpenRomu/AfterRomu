// Model_MDL.h: interface for the Model_MDL class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MODEL_MDL_H
#define MODEL_MDL_H
// #include "PhysEnv.h"
#include "studio.h"
#include <mmsystem.h>
#include <dsound.h>

class Model_MDL {
public:
    // tParticle			*m_CurrentSys,*m_TargetSys;

    int maxframes;
    vec3_t pos_touche;
    int idboite_touche;
    int idgroupe_touche;
    float g_lambert;
    v3_t g_vright;
    v3_t vec3_origin;
    unsigned int g_texnum;
    bool AfficheBoite;
    bool AniActFini;
    bool AniDepFini;
    bool optimize;
    v3_t g_xformverts[MAXSTUDIOVERTS];  // transformed vertices
    v3_t g_lightvalues[MAXSTUDIOVERTS]; // light surface normals
    v3_t* g_pxformverts;
    v3_t* g_pvlightvalues;

    v3_t g_lightvec;                  // light vector in model reference frame
    v3_t g_blightvec[MAXSTUDIOBONES]; // light vectors in bone reference frames
    int g_ambientlight;               // ambient world light
    float g_shadelight;               // direct world light
    v3_t g_lightcolor;

    int g_smodels_total; // cookie

    float g_bonetransform[MAXSTUDIOBONES][3][4]; // bone transformation matrix

    int g_chrome[MAXSTUDIOVERTS][2];    // texture coords for surface normals
    int g_chromeage[MAXSTUDIOBONES];    // last time chrome vectors were updated
    v3_t g_chromeup[MAXSTUDIOBONES];    // chrome vector "up" in bone reference frames
    v3_t g_chromeright[MAXSTUDIOBONES]; // chrome vector "right" in bone reference frames

    // mathlib functions

    inline vec3_t StudioCalcAttachments(bool moi);
    inline int VectorCompare(const v3_t& v1, const v3_t& v2) const;
    inline vec_t Q_rint(const vec_t& in) const;
    inline vec_t _DotProduct(const v3_t& v1, const v3_t& v2) const;
    inline void _VectorSubtract(const v3_t& va, const v3_t& vb, v3_t& out) const;
    inline void _VectorAdd(const v3_t& va, const v3_t& vb, v3_t& out) const;
    inline void _VectorCopy(const v3_t& in, v3_t& out) const;
    inline void _VectorScale(const v3_t& v, vec_t scale, v3_t& out) const;
    inline double VectorLength(const v3_t& v) const;
    inline void VectorMA(const v3_t& va, double scale, const v3_t& vb, v3_t& vc) const;
    inline void CrossProduct(const v3_t& v1, const v3_t& v2, v3_t& cross) const;
    inline vec_t VectorNormalize(v3_t& v) const;
    inline void VectorInverse(v3_t& v) const;

    void ClearBounds(v3_t mins, v3_t maxs);
    void AddPointToBounds(v3_t v, v3_t mins, v3_t maxs);
    void AngleMatrix(const v3_t angles, float matrix[3][4]);
    void AngleIMatrix(const v3_t angles, float matrix[3][4]);
    void R_ConcatTransforms(const float in1[3][4], const float in2[3][4], float out[3][4]);

    inline void VectorIRotate(const v3_t& in1, const float in2[3][4], v3_t& out) const;
    inline void VectorRotate(const v3_t& in1, const float in2[3][4], v3_t& out) const;
    inline void VectorTransform(const v3_t& in1, const float in2[3][4], v3_t& out) const;
    inline void VectorTransform2(const float* in1, float in2[3][4], float* out);
    void AngleQuaternion(const v3_t angles, v4_t quaternion);
    void QuaternionMatrix(const v4_t quaternion, float (*matrix)[4]);
    void QuaternionSlerp(const v4_t p, v4_t q, float t, v4_t qt);
    float intersect_sphere(const vec3_t& r, // point E
        const vec3_t& rv,                   // direction
        const vec3_t& s,                    // centre du cercle
        float sr);
    Model_MDL();
    virtual ~Model_MDL();
    void Cleanup(void);
    void Load(const char* name);
    void Draw(int justelerendu, double lumiere[3]);
    void DrawIni(void);
    void AdvanceFrame(float dt, int qui);

    void ExtractBbox(float* mins, float* maxs);

    int SetSequence(int iSequence);
    int SetSequence2(const char* name, int qui);
    int GetSequence(void) const { return m_sequence; }
    void GetSequenceInfo(float* pflFrameRate, float* pflGroundSpeed);

    float SetController(int iController, float flValue);
    float SetMouth(float flValue);
    float SetBlending(int iBlender, float flValue);
    int SetBodygroup(int iGroup, int iValue);
    int SetSkin(int iValue);
    bool raybox(vec3_t leminB, vec3_t lemaxB, vec3_t leorigin, vec3_t ledir, float* lecoord);

    v3_t m_origin;
    int lie;
    float matrice_pere[3][4];
    float matrice_main[3][4];
    v3_t pivot;
    v3_t flash;
    v3_t tete;
    v3_t piedgauche;
    v3_t pieddroit;
    v3_t dir;
    v3_t m_angles;
    v3_t yeux;
    int seq1;   // sequence index
    int seq2;   // sequence index
    float fps1; // frame
    float fps2; // frame
    int fps_arme;
    float le_fps_ori_seq1;
    float le_fps_ori_seq2;
    float fps_weapon;
    float ang_dos;
    char type[30];
    char name[30];
    int munition;

    double cur_scope;
    int scope;
    int balles;
    int puissance;
    float multi;
    int id_son;
    int calculeboite(vec3_t src, vec3_t normal);
    vec3_t poboite[30][8];
    int nb_boite;
    int m_sequence; // sequence index
    float m_frame;  // frame
    int pinocio;    // springs
    bool droitier;

protected:
    // entity settings
    int m_bodynum;        // bodypart selection
    int m_skinnum;        // skin group selection
    byte m_controller[4]; // bone controllers
    byte m_blending[2];   // animation blending
    byte m_mouth;         // mouth position

    // internal data
    studiohdr_t* m_pstudiohdr;
    mstudiomodel_t* m_pmodel;

    studiohdr_t* m_ptexturehdr;
    studioseqhdr_t* m_panimhdr[32];

    v4_t m_adj; // FIX: non persistant, make static

    studiohdr_t* LoadModel(const char* modelname);
    studioseqhdr_t* LoadDemandSequences(const char* modelname);

    void CalcBoneAdj(void);
    void CalcBoneQuaternion(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* q);
    void CalcBonePosition(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* pos);
    void CalcRotations(v3_t* pos, v4_t* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f);
    mstudioanim_t* GetAnim(mstudioseqdesc_t* pseqdesc);
    mstudioevent_t* GetEvents(mstudioseqdesc_t* pseqdesc);
    void SlerpBones(v4_t q1[], v3_t pos1[], v4_t q2[], v3_t pos2[], float s);
    void SetUpBones(int qui, int lerendu);
    void SetUpBonesIni(void);
    void DrawPoints(void);
    void drawBox(vec3_t* v);
    void Lighting(float* lv, int bone, int flags, v3_t normal);
    void Chrome(int* chrome, int bone, v3_t normal);
    void ParcoursBones();
    void SetupLighting(void);

    void SetupModel(int bodypart);

    void UploadTexture(mstudiotexture_t* ptexture, byte* data, byte* pal);
};

//////////////////////////////////////////////////////////////////
// math stuff
//////////////////////////////////////////////////////////////////

inline int Model_MDL::VectorCompare(const v3_t& v1, const v3_t& v2) const
{
    int i;

    for (i = 0; i < 3; i++)
        if (fabs(v1[i] - v2[i]) > EQUAL_EPSILON)
            return false;

    return true;
}

inline double Model_MDL::VectorLength(const v3_t& v) const
{
    int i;
    double length;

    length = 0;
    for (i = 0; i < 3; i++)
        length += v[i] * v[i];
    length = sqrt(length); // FIXME

    return length;
}

inline vec_t Model_MDL::Q_rint(const vec_t& in) const { return floor(in + 0.5f); }

inline void Model_MDL::VectorMA(const v3_t& va, double scale, const v3_t& vb, v3_t& vc) const
{
    vc[0] = va[0] + scale * vb[0];
    vc[1] = va[1] + scale * vb[1];
    vc[2] = va[2] + scale * vb[2];
}

inline void Model_MDL::CrossProduct(const v3_t& v1, const v3_t& v2, v3_t& cross) const
{
    cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
    cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
    cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

inline vec_t Model_MDL::_DotProduct(const v3_t& v1, const v3_t& v2) const
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline void Model_MDL::_VectorSubtract(const v3_t& va, const v3_t& vb, v3_t& out) const
{
    out[0] = va[0] - vb[0];
    out[1] = va[1] - vb[1];
    out[2] = va[2] - vb[2];
}

inline void Model_MDL::_VectorAdd(const v3_t& va, const v3_t& vb, v3_t& out) const
{
    out[0] = va[0] + vb[0];
    out[1] = va[1] + vb[1];
    out[2] = va[2] + vb[2];
}

inline void Model_MDL::_VectorCopy(const v3_t& in, v3_t& out) const
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
}

inline void Model_MDL::_VectorScale(const v3_t& v, vec_t scale, v3_t& out) const
{
    out[0] = v[0] * scale;
    out[1] = v[1] * scale;
    out[2] = v[2] * scale;
}

inline vec_t Model_MDL::VectorNormalize(v3_t& v) const
{
    int i;
    float length;

    if (fabs(v[1] - 0.000215956) < 0.0001) {
        i = 1;
    }

    length = 0;
    for (i = 0; i < 3; i++)
        length += v[i] * v[i];
    length = sqrt(length);
    if (length == 0)
        return 0;

    for (i = 0; i < 3; i++)
        v[i] /= length;

    return length;
}

inline void Model_MDL::VectorInverse(v3_t& v) const
{
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

inline void Model_MDL::VectorRotate(const v3_t& in1, const float in2[3][4], v3_t& out) const
{
    out[0] = DotProduct(in1, in2[0]);
    out[1] = DotProduct(in1, in2[1]);
    out[2] = DotProduct(in1, in2[2]);
}

// rotate by the inverse of the matrix
inline void Model_MDL::VectorIRotate(const v3_t& in1, const float in2[3][4], v3_t& out) const
{
    out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
    out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
    out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}

inline void Model_MDL::VectorTransform(const v3_t& in1, const float in2[3][4], v3_t& out) const
{
    out[0] = DotProduct(in1, in2[0]) + in2[0][3];
    out[1] = DotProduct(in1, in2[1]) + in2[1][3];
    out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}
inline void Model_MDL::VectorTransform2(const float* in1, float in2[3][4], float* out)
{
    out[0] = DotProduct(in1, in2[0]) + in2[0][3];
    out[1] = DotProduct(in1, in2[1]) + in2[1][3];
    out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

inline vec3_t Model_MDL::StudioCalcAttachments(bool moi)
{
    int i;
    mstudioattachment_t* pattachment;
    v3_t vout;
    /*if ( m_pStudioHeader->numattachments > 4 )
    {
            gEngfuncs.Con_DPrintf( "Too many attachments on %s\n", m_pCurrentEntity->model->name );
            exit( -1 );
    }*/

    // calculate attachment points
    pattachment = (mstudioattachment_t*)((byte*)m_pstudiohdr + m_pstudiohdr->attachmentindex);
    // for (i = 0; i < m_pstudiohdr->numattachments; i++)
    for (i = 0; i < 1; i++) {
        // pattachment[i].org=vec3_t (0.0f,0.0f,0.0f);
        // VectorTransform( pattachment[i].org, (*g_bonetransform)[pattachment[i].bone], m_pCurrentEntity->attachment[i]
        // );
        VectorTransform2(pattachment[i].org, (g_bonetransform)[pattachment[i].bone], vout);
    }
    if (!droitier && moi)
        vout[1] = -vout[1];

    vec3_t out2(vout[0], vout[1], vout[2]);
    return out2;
    // return pattachment[0].org;
}

#endif MODEL_MDL_H