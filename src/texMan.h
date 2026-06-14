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
#ifndef __TEXMANH__
#define __TEXMANH__

#include "utility.h"

struct texMan_t
{
    static void Release(void);
    static uint Load(char *name);

  protected:
    struct tEntry_t
    {
        tEntry_t()
        {
            name = 0;
            next = 0;
        }
        virtual ~tEntry_t()
        {
            delete[] name;
        }

        char *name;
        uint id;
        tEntry_t *next;
    };

    static tEntry_t *textures;
};

#endif