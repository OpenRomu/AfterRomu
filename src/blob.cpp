// blob.cpp: implementation of the blob class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "stdafx.h"
// #include "stdafx.h"
#include "blob.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

blob::blob()
    : playersound(0)
    , my_world(NULL)
{
}

blob::~blob() { }

void blob::move(vec3_t dir)
{

    if (my_world) {
        collision_tir col_tir;
        vec3_t tmpdir = dir;

        my_world->process_visible_faces_collide(pos, tmpdir);

        // my_world->process_visible_faces3(curParticle->oldpos);

        // tmpdir=tmpdir*10.0f;

        //								my_world->process_visible_faces_collide(curParticle_old->pos,curParticle->pos-curParticle_old->pos);
        my_world->epsilon = 0.50f;

        my_world->collide = false;
        my_world->normale_face_touchee[0] = 0.0f;
        my_world->normale_face_touchee[1] = 0.0f;
        my_world->normale_face_touchee[2] = 0.0f;

        vec3_t new_pos;
        collision_tir d;
        my_world->check_collisions_bat(pos, tmpdir, &d, 25);

        pos = d.pt;
    } else
        pos = pos + dir;
}

void blob::render()
{
    glEnable(GL_CULL_FACE);
    my_world->drawBoxEx(pos);
}

void blob::ping()
{
    if (playersound->IsSoundPlaying() == TRUE) {
        playersound->Reset();
    }
    D3DVECTOR vpos;
    D3DVECTOR vv;
    vv.x = 0.0f;
    vv.z = 1.0f;
    vv.y = 0.0f;

    vpos.x = pos[0];
    vpos.y = pos[1];
    vpos.z = pos[2];

    playersound->Positionne3Dbuffer(&vpos, &vv);

    playersound->Play(0, 0); // DSBPLAY_LOOPING
}