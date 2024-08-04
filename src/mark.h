/*
   Copyright (C) 2000 Nate Miller nkmiller@calpoly.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
#ifndef __MARKH__
#define __MARKH__

#include "geom.h"
#include "utility.h"

// tri fan
struct mMark_t {
    mMark_t()
    {
        next = prev = 0;
        alpha = 1;
    }

    uint life;
    vec3_t v[4];
    vec3_t normal;
    float alpha;
    int color;
    uint texture;
    mMark_t *next, *prev;
};

struct mMarkManager_t {
    mMarkManager_t();
    virtual ~mMarkManager_t();

    void SetTexture(uint t) { texture = t; }
    void SetTexture_sang(uint t) { texture_sang = t; }
    void SetTexture_trace(uint t) { texture_trace = t; }

    void Init(int n);
    void Frame(float& frametime);
    void Render(void);
    void AddMark(vec3_t& n, vec3_t& pt, int type_mark);
    void drawBox(vec3_t* v);

protected:
    mMark_t* MarkNew(void);
    void MarkDelete(mMark_t* m);

    uint texture;
    uint texture_sang;
    uint texture_trace;
    int marksMax;
    mMark_t* marksAll;
    mMark_t* marksFree;
    mMark_t marksUse;
};

#endif