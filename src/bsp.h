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
 * This is based on HL SDK files
 */
#ifndef __bsp_h__
#define __bsp_h__

#include <stdio.h>
#include "geom.h"

#pragma pack(push)
#pragma pack(1)

typedef unsigned char byte;

typedef struct {
    unsigned int fileofs;
    unsigned int filelen;
} lump_t;

#define LUMP_ENTITIES 0
#define LUMP_PLANES 1
#define LUMP_TEXTURES 2
#define LUMP_VERTEXES 3
#define LUMP_VISIBILITY 4
#define LUMP_NODES 5
#define LUMP_TEXINFO 6
#define LUMP_FACES 7
#define LUMP_LIGHTING 8
#define LUMP_CLIPNODES 9
#define LUMP_LEAFS 10
#define LUMP_MARKSURFACES 11
#define LUMP_EDGES 12
#define LUMP_SURFEDGES 13
#define LUMP_MODELS 14

#define HEADER_LUMPS 15

typedef struct {
    int version;
    lump_t lumps[HEADER_LUMPS];
} dheader_t;

typedef struct {
    float mins[3], maxs[3];
    float origin[3];
    int bsp_node;   // first bsp tree node
    int clip_node;  // first clip node
    int clip2_node; // second clip node
    int unused;
    int visleafs;  // not including the solid leaf 0
    int firstface; // index into marksurfaces
    int numfaces;
} dmodel_t;

typedef struct {
    float point[3];
} dvertex_t;

#define MAX_MAP_HULLS 4

#define MAX_MAP_MODELS 400
#define MAX_MAP_BRUSHES 4096
#define MAX_MAP_ENTITIES 1024
#define MAX_MAP_ENTSTRING (128 * 1024)

#define MAX_MAP_PLANES 32767
#define MAX_MAP_NODES 32767     // because negative shorts are contents
#define MAX_MAP_CLIPNODES 32767 //
#define MAX_MAP_LEAFS 8192
#define MAX_MAP_VERTS 65535
#define MAX_MAP_FACES 65535
#define MAX_MAP_MARKSURFACES 65535
#define MAX_MAP_TEXINFO 8192
#define MAX_MAP_EDGES 256000
#define MAX_MAP_SURFEDGES 512000
#define MAX_MAP_TEXTURES 512
#define MAX_MAP_MIPTEX 0x200000
#define MAX_MAP_LIGHTING 0x200000
#define MAX_MAP_VISIBILITY 0x200000

#define MAX_MAP_PORTALS 65536

// key / value pair sizes

#define MAX_KEY 32
#define MAX_VALUE 1024
// 0-2 are axial planes
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2

// 3-5 are non-axial planes snapped to the nearest
#define PLANE_ANYX 3
#define PLANE_ANYY 4
#define PLANE_ANYZ 5

typedef struct {
    float normal[3];
    float dist;
    int type;
} dplane_t;

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct {
    short v[2]; // vertex numbers
} dedge_t;

typedef struct texinfo_s {
    float vecs[2][4]; // [s/t][xyz offset]
    int miptex;
    int flags;
} texinfo_t;

#define MAXLIGHTMAPS 4
typedef struct {
    short planenum;
    short side;

    int firstedge; // we must support > 64k edges
    short numedges;
    short texinfo;

    // lighting info
    byte styles[MAXLIGHTMAPS];
    int lightofs; // start of [numstyles*surfsize] samples
} dface_t;

struct mip_header_t {
    int count;
    int offsets[1];
};

#define MIPLEVELS 4
typedef struct miptex_s {
    char name[16];
    unsigned width, height;
    unsigned offsets[MIPLEVELS]; // four mip maps stored
} miptex_t;

typedef struct {
    int planenum;
    // short        children[2];    // negative numbers are -(leafs+1), not nodes
    short front;
    short back;
    short mins[3]; // for sphere culling
    short maxs[3];
    unsigned short firstface;
    unsigned short numfaces; // counting both sides
} dnode_t;

typedef struct {
    int planenum;
    short children[2]; // negative are contents
} dclipnode_t;

#define NUM_AMBIENTS 4 // automatic ambient sounds

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
// all other leafs need visibility info

typedef struct {
    int contents;
    int visofs; // -1 = no visibility info

    short mins[3]; // for frustum culling
    short maxs[3];

    unsigned short firstmarksurface;
    unsigned short nummarksurfaces;

    byte ambient_level[NUM_AMBIENTS];
} dleaf_t;

typedef struct epair_s {
    struct epair_s* next;
    char* key;
    char* value;
} epair_t;

typedef struct {
    float origin[3];
    int firstbrush;
    int numbrushes;
    epair_t* epairs;
} entity_t;

typedef struct {
    char filename[1024];
    char *buffer, *script_p, *end_p;
    int line;
} script_t;

#define MAXLEAF 15
typedef struct entvars_s {
    char classname[64];
    vec3_t origin;
    vec3_t angles;

    int rendermode;
    float renderamt;
    vec3_t rendercolor;
    int renderfx;

    int brush_model_index;
    int leaf_visibility_list[MAXLEAF];
    int size_leaf; // nombre de leaf visible
                   // StudioModel *studio_model;
} entvars_t;

#define MAX_INCLUDES 8
#define MAXTOKEN 512
#define TEX_SPECIAL 1

class bsp_file_t {
public:
    bsp_file_t(const char* filename);
    ~bsp_file_t();
    epair_t* ParseEpair(void);
    bool ParseEntity(void);
    void ParseEntities(void);
    void SetKeyValue(entity_t* ent, char* key, char* value);
    char* ValueForKey(entity_t* ent, char* key);
    float FloatForKey(entity_t* ent, char* key);
    void GetVectorForKey(entity_t* ent, char* key, vec3_t vec);
    void ParseFromMemory(char* buffer, int size);
    bool GetToken(bool crossline);
    bool EndOfScript(bool crossline);
    void AddScriptToStack(char* filename);
    // int    LoadFile (char *filename, void **bufferptr);
    // char *ExpandPath (char *path);

protected:
    static void* suck_lump(FILE* f, lump_t& l, int sz, int& count);

public: // data
    script_t scriptstack[MAX_INCLUDES];
    script_t* script;
    int scriptline;

    char token[MAXTOKEN];
    bool endofscript;
    bool tokenready; // only true if UnGetToken was just called

    int model_count;
    dmodel_t* models;

    int face_count;
    dface_t* faces;

    int plane_count;
    dplane_t* planes;

    int edge_count;
    dedge_t* edges;

    int edge_list_count;
    int* edge_list;

    int vertex_count;
    dvertex_t* vertices;

    int texinfo_count;
    texinfo_t* texinfo;

    int tex_count;
    miptex_t* textures;

    int lightmap_count;
    byte* lightmaps;

    int node_count;
    dnode_t* nodes;

    int clipnode_count;
    dclipnode_t* clipnodes;

    int leaf_count;
    dleaf_t* leaves;

    int marksurface_count;
    unsigned short* marksurfaces;

    int vis_count;
    byte* vis;

    int entities_count;
    char* entities;

    int num_entities;
    entity_t Lesentities[MAX_MAP_ENTITIES];
};

#pragma pack(pop)

#endif // __bsp_h__
