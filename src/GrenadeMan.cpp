#include "stdafx.h"
#include "glinc.h"
#include "GrenadeMan.h"
#include "matrix.h"
#include "particleBoom.h"
#include "particleSmoke.h"
// #include "world.h"
pGrenadeManager_t::pGrenadeManager_t()
{
    systems = 0;
    engineparts = 0;
}

pGrenadeManager_t::~pGrenadeManager_t()
{
    pGrenSystem_t *curr = systems, *temp;

    while (curr)
    {
        temp = curr;
        curr = curr->GetNext();
        delete temp;
    }
}

void pGrenadeManager_t::SystemNew(pGrenSystem_t *sys, vec3_t &pos, vec3_t &dir, bool die, DPNID id_lanceur)
{
    if (!sys)
        return;

    // if die == 1, kill all of the other systems before we add
    if (die)
        for (pGrenSystem_t *curr = systems; curr; curr = curr->GetNext())
            curr->Die();

    // setup and link in
    sys->SetEXPLODE(engineparts);

    sys->Init(pos, dir, id_lanceur);
    sys->SetTexture(ids[sys->GetType()]);
    sys->SetPrev(0);
    sys->SetNext(systems);

    if (systems)
        systems->SetPrev(sys);
    systems = sys;
}

void pGrenadeManager_t::SystemDelete(pGrenSystem_t *sys)
{
    if (sys->GetNext())
        sys->GetNext()->SetPrev(sys->GetPrev());
    if (sys->GetPrev())
        sys->GetPrev()->SetNext(sys->GetNext());

    if (!sys->GetNext() && !sys->GetPrev())
        systems = 0;
    if (!sys->GetPrev())
        systems = sys->GetNext();

    delete sys;
}

// this checks for collisions
void pGrenadeManager_t::Frame(float &frametime, vector<vec3_t> *lespos, vector<DPNID> *amoi,
                              vector<vec3_t> *lesposjoueur, DPNID mon_id, vector<DPNID> *lesid_joueur,
                              vector<Aplayer *> *lesjoueurs)
{
    pGrenSystem_t *curr = systems;
    pGrenSystem_t *n = 0;
    bool ok = 0;

    while (curr)
    {
        n = curr->GetNext();

        if (!(ok = curr->Frame(frametime, gravity, lespos, amoi, lesposjoueur, mon_id, lesid_joueur, lesjoueurs)))
        {

            //	int  i =engineplayers.size();
            SystemDelete(curr);
        }

        if (ok)
        {
            pGren_t *part = 0;

            for (part = curr->GetAlive()->next; part != curr->GetAlive(); part = part->next)
            {

                for (int o = 0; o < lesposjoueur->size(); o++)
                {
                    if (part->id_lanceur != lesid_joueur->at(o))
                    {
                        vec3_t d = lesposjoueur->at(o);
                        vec3_t lvdist = part->pos - d;
                        float dist = lvdist.len();
                        if (dist < 20.0f)
                        {
                            part->life = 1.0f; // missile qui touche le mur
                        }
                    }
                }

                if (part->life != 1.0f)
                {
                    // condition de changement d'etat explosion rebond
                    if (part->colle)
                    {
                    }
                    else
                    {

                        collision_tir col_tir;
                        /*col_tir=myworld->check_tirs(part->pos,(part->pos-part->pold));*/
                        myworld->process_visible_faces3(part->pos);

                        col_tir = myworld->check_tirs_rec(part->pos, (part->pos - part->pold), 5);

                        if (col_tir.found == true)
                        {
                            if (part->rebond)
                            {
                                if ((col_tir.normal[0] == 0.0f) && (col_tir.normal[1] == 0.0f) &&
                                    (col_tir.normal[2] == 0.0f))
                                {
                                    part->pos = part->pold;
                                    part->gravity = 0;
                                    part->vel = vec3_t(0.0f, 0.0f, 0.0f);
                                }
                                else
                                {
                                    curr->Collide(part, col_tir.normal);
                                }
                            }
                            else
                            {
                                part->life = 1.0f; // missile qui touche le mur
                            }
                        }
                        else
                        {
                            if (myworld->find_leaf(part->pos) == 0)
                            {

                                if (part->rebond)
                                {
                                    part->pos = part->pold;
                                    part->gravity = 0;
                                    part->vel = vec3_t(0.0f, 0.0f, 0.0f);
                                }
                                else
                                {
                                    part->life = 1.0f; // missile qui touche le mur
                                }
                            }
                        }

                        col_tir.found = false;
                        col_tir.nearest = 0.0f;
                    }
                }
            }
        }

        curr = n;
    }
    // lespos.push_back (vec3_t(0.0f,0.0f,0.0f));
}

void pGrenadeManager_t::Render(void)
{
    matrix_t mat, tmat; // static?
    vec3_t x, y;

    glGetFloatv(GL_MODELVIEW_MATRIX, mat.m);
    mat.Transpose(tmat);

    x[0] = -tmat[0];
    x[1] = -tmat[1];
    x[2] = -tmat[2];

    y[0] = tmat[4];
    y[1] = tmat[5];
    y[2] = tmat[6];

    /* x[0]=1;
     x[1]=0;
     x[2]=0;
      y[0]=0;
     y[1]=0;
     y[2]=1;
  */
    // glDisable(GL_DEPTH_FUNC);

    glEnable(GL_BLEND);
    // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);

    for (pGrenSystem_t *curr = systems; curr; curr = curr->GetNext())
    {
        curr->Render(x, y);
    }

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);
    //	glEnable(GL_DEPTH_FUNC);
}
