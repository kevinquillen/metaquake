/*************************************************************************
 * Metamorphism Quake Server: Common/Class/ninja.c,                      *
 *                            Common/Class/ninja.qc                      *
 * Copyright (C) 1996-2000 by Lon Hohberger.                             *
 *                                                                       *
 * This code is free software; you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or (at *
 * your option) any later version.                                       *
 *                                                                       *
 * This code is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * General Public License for more details.                              *
 *                                                                       *
 * Visit http://www.gnu.org for more information on the GPL.             *
 *                                                                       *
 *************************************************************************/

#include "meta.h"

/* Ninja Specific */
void() Reflect =
{
  local entity head;

  if (((self.id_number != self.owner.id_number) ||
       (self.owner.playerclass != CL_SILENT)) ||
       (self.owner.deadflag))
  {
    remove(self);
    return;
  }

  head = findradius(self.owner.origin,40);
  while (head)
  {
    if ((head.owner != self.owner)
        && ((random()*10) < 7))
     {
       if ((head.netname == "rocket") ||
           (head.netname == "missile") ||
           (head.netname == "nail") ||
           (head.netname == "steel nail") ||
           (head.netname == "pineapple"))
       {
         head.owner = self.owner;
         head.velocity = -1*head.velocity;
       }
     }

    head = head.chain;
  }

  self.think = Reflect;
  self.nextthink = time + 0.1;
};

void() SpawnBugSpray =
{
  local entity st;
  st = spawn();
  st.owner = self;
  st.id_number = self.id_number;
  st.nextthink = time + 1;
  st.think = Reflect;
};
