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
 * bsp file loading
 */
#include "stdafx.h"
#include "bsp.h"
// #include "bspfile.h"
#include "exception.h"
#include <string.h>
#include <stdlib.h>
// #include <fstream.h>

// #include "cmdlib.h"

/*
================
filelength
================
*/
int filelength(FILE* f)
{
    int pos;
    int end;

    pos = ftell(f);
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, pos, SEEK_SET);

    return end;
}

FILE* SafeOpenRead(char* filename)
{
    FILE* f;

    f = fopen(filename, "rb");

    if (!f)
        throw basic_exception("Error opening %s: %s");

    return f;
}

void SafeRead(FILE* f, void* buffer, int count)
{
    if (fread(buffer, 1, count, f) != (size_t)count)
        throw basic_exception("File read failure");
}

char qproject[1024] = { '\0' };
char qdir[1024] = { '\0' };
char gamedir[1024] = { '\0' };

/*
==============
LoadFile
==============
*/
int LoadFile(char* filename, void** bufferptr)
{
    FILE* f;
    int length;
    void* buffer;

    f = SafeOpenRead(filename);
    length = filelength(f);
    buffer = malloc(length + 1);
    ((char*)buffer)[length] = 0;
    SafeRead(f, buffer, length);
    fclose(f);

    *bufferptr = buffer;
    return length;
}

char* ExpandPath(char* path)
{
    char* psz;
    static char full[1024];
    if (!qdir)
        throw basic_exception("ExpandPath called without qdir set");
    if (path[0] == '/' || path[0] == '\\' || path[1] == ':')
        return path;
    psz = strstr(path, qdir);
    if (psz)
        strcpy(full, path);
    else
        sprintf(full, "%s%s", qdir, path);

    return full;
}

char* copystring(char* s)
{
    char* b;
    b = (char*)malloc(strlen(s) + 1);
    strcpy(b, s);
    return b;
}

//--------------------------------------------------------------------

void* bsp_file_t::suck_lump(FILE* f, lump_t& l, int sz, int& count)
{
    fseek(f, l.fileofs, SEEK_SET);
    count = l.filelen / sz;
    byte* p = new byte[l.filelen];
    if (fread(p, 1, l.filelen, f) == l.filelen) {
        return p;
    }

    return 0;
}

bsp_file_t::bsp_file_t(const char* filename)
    : model_count(0)
    , models(0)
    , face_count(0)
    , faces(0)
    , plane_count(0)
    , planes(0)
    , edge_count(0)
    , edges(0)
    , edge_list_count(0)
    , edge_list(0)
    , vertex_count(0)
    , vertices(0)
    , texinfo_count(0)
    , texinfo(0)
    , tex_count(0)
    , textures(0)
    , lightmap_count(0)
    , lightmaps(0)
    , node_count(0)
    , nodes(0)
    , clipnode_count(0)
    , clipnodes(0)
    , leaf_count(0)
    , leaves(0)
    , marksurface_count(0)
    , marksurfaces(0)
    , vis_count(0)
    , vis(0)
    , entities_count(0)
    , entities(0)
{
    FILE* f = fopen(filename, "rb");
    if (!f) {
        throw basic_exception("bsp_file_t ctor: fopen failed");
    }

    dheader_t h;
    if (fread(&h, 1, sizeof(h), f) != sizeof(h)) {
        throw basic_exception("bsp_file_t ctor: fread(header)");
    }

    models = (dmodel_t*)suck_lump(f, h.lumps[LUMP_MODELS], sizeof(dmodel_t), model_count);
    faces = (dface_t*)suck_lump(f, h.lumps[LUMP_FACES], sizeof(dface_t), face_count);
    planes = (dplane_t*)suck_lump(f, h.lumps[LUMP_PLANES], sizeof(dplane_t), plane_count);
    edges = (dedge_t*)suck_lump(f, h.lumps[LUMP_EDGES], sizeof(dedge_t), edge_count);
    edge_list = (int*)suck_lump(f, h.lumps[LUMP_SURFEDGES], sizeof(int), edge_list_count);
    vertices = (dvertex_t*)suck_lump(f, h.lumps[LUMP_VERTEXES], sizeof(dvertex_t), vertex_count);
    texinfo = (texinfo_t*)suck_lump(f, h.lumps[LUMP_TEXINFO], sizeof(texinfo_t), texinfo_count);
    lightmaps = (byte*)suck_lump(f, h.lumps[LUMP_LIGHTING], sizeof(byte), lightmap_count);
    nodes = (dnode_t*)suck_lump(f, h.lumps[LUMP_NODES], sizeof(dnode_t), node_count);
    clipnodes = (dclipnode_t*)suck_lump(f, h.lumps[LUMP_CLIPNODES], sizeof(dclipnode_t), clipnode_count);
    leaves = (dleaf_t*)suck_lump(f, h.lumps[LUMP_LEAFS], sizeof(dleaf_t), leaf_count);
    marksurfaces = (unsigned short*)suck_lump(f, h.lumps[LUMP_MARKSURFACES], sizeof(unsigned short), marksurface_count);
    vis = (byte*)suck_lump(f, h.lumps[LUMP_VISIBILITY], sizeof(byte), vis_count);
    entities = (char*)suck_lump(f, h.lumps[LUMP_ENTITIES], sizeof(char), entities_count);

    ParseEntities();

    // textures
    byte* p = (byte*)suck_lump(f, h.lumps[LUMP_TEXTURES], sizeof(byte), tex_count);
    if (!p)
        throw out_of_memory();
    mip_header_t* m = (mip_header_t*)p;
    tex_count = m->count;
    textures = new miptex_t[tex_count];
    for (int i = 0; i < m->count; i++) {
        miptex_t* t = (miptex_t*)(p + m->offsets[i]);
        memcpy(textures + i, t, sizeof(miptex_t));
    }
    delete p;

#if 0
    tex_count = *(int*)p;
    textures = new miptex_t[tex_count];
    for(int i = 0; i < tex_count; i++) {
        miptex_t* t = (miptex_t*)(p+*(int*)(p+(i*sizeof(int*))+sizeof(int*)));
        memcpy(textures+i, t, sizeof(miptex_t));
    }
#endif

    fclose(f);
}

bsp_file_t::~bsp_file_t()
{
    if (faces) {
        delete faces, faces = 0;
    }
    if (planes) {
        delete planes, planes = 0;
    }
    if (edges) {
        delete edges, edges = 0;
    }
    if (edge_list) {
        delete edge_list, edge_list = 0;
    }
    if (vertices) {
        delete vertices, vertices = 0;
    }
    if (texinfo) {
        delete texinfo, texinfo = 0;
    }
    if (textures) {
        delete textures, textures = 0;
    }
    if (lightmaps) {
        delete lightmaps, lightmaps = 0;
    }
    if (nodes) {
        delete nodes, nodes = 0;
    }
    if (clipnodes) {
        delete clipnodes, clipnodes = 0;
    }
    if (leaves) {
        delete leaves, leaves = 0;
    }
    if (marksurfaces) {
        delete marksurfaces, marksurfaces = 0;
    }
    if (models) {
        delete models, models = 0;
    }
    if (vis) {
        delete vis, vis = 0;
    }
    if (entities) {
        delete entities, entities = 0;
    }

    entity_t* map;
    epair_t* curr;
    epair_t* tmp;

    for (int i = 0; i < num_entities; i++) {
        map = &Lesentities[i];
        curr = map->epairs;

        while (curr) {
            free(curr->key);
            free(curr->value);
            tmp = curr;
            curr = curr->next;
            free(tmp);
        }
    }
}

/*
=================
ParseEpair
=================
*/
epair_t* bsp_file_t::ParseEpair(void)
{
    epair_t* e;

    e = (epair_s*)malloc(sizeof(epair_t));
    memset(e, 0, sizeof(epair_t));

    if (strlen(token) >= MAX_KEY - 1)
        throw basic_exception("ParseEpar: token too long");
    e->key = copystring(token);
    GetToken(false);
    if (strlen(token) >= MAX_VALUE - 1)
        throw basic_exception("ParseEpar: token too long");
    e->value = copystring(token);

    return e;
}

/*
================
ParseEntity
================
*/
bool bsp_file_t::ParseEntity(void)
{
    epair_t* e;
    entity_t* mapent;
    //	ofstream o("log/entity.log");

    if (!GetToken(true))
        return false;

    if (strcmp(token, "{"))
        throw basic_exception("ParseEntity: { not found");

    if (num_entities == MAX_MAP_ENTITIES)
        throw basic_exception("num_entities == MAX_MAP_ENTITIES");

    mapent = &Lesentities[num_entities];
    mapent->epairs = false; // debug bat

    num_entities++;
    //  o << num_entities << endl;
    do {
        if (!GetToken(true))
            throw basic_exception("ParseEntity: EOF without closing brace");
        if (!strcmp(token, "}"))
            break;
        e = ParseEpair();
        e->next = mapent->epairs;
        mapent->epairs = e;
    } while (1);

    return true;
}

void bsp_file_t::ParseEntities(void)
{
    num_entities = 0;
    ParseFromMemory(entities, entities_count);

    while (ParseEntity()) { }
}

void bsp_file_t::SetKeyValue(entity_t* ent, char* key, char* value)

{

    /*for (ep=ent->epairs ; ep ; ep=ep->next)
            if (!strcmp (ep->key, key) )
            {
                    free (ep->value);
                    ep->value = copystring(value);
                    return;
            }
    ep = malloc (sizeof(*ep));
    ep->next = ent->epairs;
    ent->epairs = ep;
    ep->key = copystring(key);
    ep->value = copystring(value);*/
}

char* bsp_file_t::ValueForKey(entity_t* ent, char* key)

{
    epair_t* ep;
    //	epair_t *old_ep;
    //	char toto[10];
    // ent->epairs;
    for (ep = ent->epairs; ep; ep = ep->next) {

        if (!strcmp(ep->key, key))
            return ep->value;
        // old_ep=ep;
    }

    return "";
}

float bsp_file_t::FloatForKey(entity_t* ent, char* key)

{
    char* k;

    k = ValueForKey(ent, key);
    return atof(k);
}

void bsp_file_t::GetVectorForKey(entity_t* ent, char* key, vec3_t vec)
{
    char* k;
    int v1, v2, v3;

    k = ValueForKey(ent, key);
    // scanf into doubles, then assign, so it is vec_t size independent
    v1 = v2 = v3 = 0;
    sscanf(k, "%d %d %d", &v1, &v2, &v3);
    vec[0] = (float)v1;
    vec[1] = (float)v2;
    vec[2] = (float)v3;
}

/*
==============
ParseFromMemory
==============
*/
void bsp_file_t::ParseFromMemory(char* buffer, int size)
{
    script = scriptstack;
    script++;
    if (script == &scriptstack[MAX_INCLUDES])
        throw basic_exception("script file exceeded MAX_INCLUDES");
    strcpy(script->filename, "memory buffer");

    script->buffer = buffer;
    script->line = 1;
    script->script_p = script->buffer;
    script->end_p = script->buffer + size;

    endofscript = false;
    tokenready = false;
}
/*
==============
GetToken
==============
*/
bool bsp_file_t::GetToken(bool crossline)
{
    char* token_p;

    if (tokenready) // is a token allready waiting?
    {
        tokenready = false;
        return true;
    }

    if (script->script_p >= script->end_p)
        return EndOfScript(crossline);

//
// skip space
//
skipspace:
    while (*script->script_p <= 32) {
        if (script->script_p >= script->end_p)
            return EndOfScript(crossline);
        if (*script->script_p++ == '\n') {
            if (!crossline)
                throw basic_exception("Line %i is incomplete\n");
            scriptline = script->line++;
        }
    }

    if (script->script_p >= script->end_p)
        return EndOfScript(crossline);

    if (*script->script_p == ';' || *script->script_p == '#' ||         // semicolon and # is comment field
        (*script->script_p == '/' && *((script->script_p) + 1) == '/')) // also make // a comment field
    {
        if (!crossline)
            throw basic_exception("Line %i is incomplete\n");
        while (*script->script_p++ != '\n')
            if (script->script_p >= script->end_p)
                return EndOfScript(crossline);
        goto skipspace;
    }

    //
    // copy token
    //
    token_p = token;

    if (*script->script_p == '"') {
        // quoted token
        script->script_p++;
        while (*script->script_p != '"') {
            *token_p++ = *script->script_p++;
            if (script->script_p == script->end_p)
                break;
            if (token_p == &token[MAXTOKEN])
                throw basic_exception("Token too large on line %i\n");
        }
        script->script_p++;
    } else // regular token
        while (*script->script_p > 32 && *script->script_p != ';') {
            *token_p++ = *script->script_p++;
            if (script->script_p == script->end_p)
                break;
            if (token_p == &token[MAXTOKEN])
                throw basic_exception("Token too large on line %i\n");
        }

    *token_p = 0;

    if (!strcmp(token, "$include")) {
        GetToken(false);
        AddScriptToStack(token);
        return GetToken(crossline);
    }

    return true;
}

bool bsp_file_t::EndOfScript(bool crossline)
{
    if (!crossline)
        throw basic_exception("Line %i is incomplete\n");

    if (!strcmp(script->filename, "memory buffer")) {
        endofscript = true;
        return false;
    }

    free(script->buffer);
    if (script == scriptstack + 1) {
        endofscript = true;
        return false;
    }
    script--;
    scriptline = script->line;
    printf("returning to %s\n", script->filename);
    return GetToken(crossline);
}
/*
==============
AddScriptToStack
==============
*/
void bsp_file_t::AddScriptToStack(char* filename)
{
    int size;

    script++;
    if (script == &scriptstack[MAX_INCLUDES])
        throw basic_exception("script file exceeded MAX_INCLUDES");
    strcpy(script->filename, ExpandPath(filename));

    size = LoadFile(script->filename, (void**)&script->buffer);

    printf("entering %s\n", script->filename);

    script->line = 1;

    script->script_p = script->buffer;
    script->end_p = script->buffer + size;
}
