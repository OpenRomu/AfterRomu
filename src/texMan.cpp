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
#include "stdafx.h"
#include "gltga.h"
#include "texMan.h"

texMan_t::tEntry_t *texMan_t::textures = 0;

void texMan_t::Release(void)
{
    tEntry_t *curr = textures, *temp = 0;

    while (curr)
    {
        temp = curr;
        curr = curr->next;
        glDeleteTextures(1, &temp->id);
        delete temp;
    }
}

uint texMan_t::Load(char *name)
{
    if (!name)
        return 0;

    for (tEntry_t *curr = textures; curr; curr = curr->next)
    {
        if (!strcmp(curr->name, name))
        {
            return curr->id;
        }
    }

    glTga_t img;

    if (img.Load(name) != 1)
        return 0;

    tEntry_t *n = new tEntry_t;

    img.GenId(1, &n->id);
    img.SetId(n->id);
    img.SetMipMap(1);
    img.SetMinFilter(GL_LINEAR_MIPMAP_NEAREST);
    img.SetMagFilter(GL_LINEAR);
    img.Upload(1);

    n->name = new char[strlen(name) + 1];
    strcpy(n->name, name);
    n->next = textures;
    textures = n;

    return n->id;
}