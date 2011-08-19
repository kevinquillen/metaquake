/*************************************************************************
 * Metamorphism Quake Server: Common/globals.c, Common/globals.qc        *
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
 
/****************\
 Global Functions
\****************/
float(float radius, string classtype, float max_no) CheckCount =
{
  local entity head;
  local float num;

  num = 0;
  head = findradius (self.origin, radius); // Search
  while(head) 
  {
    if ((head.classname == classtype) && (head.owner == self))
      num = num + 1;
    head = head.chain; // Check next object
  }

  if (num >= max_no)
    return 0;
  return 1;
};


void(entity e, float chan, string samp, float vol, float atten) sound = 
{
	if(!self.player_flags & PF_SILENT)
		dosound(e,chan,samp,vol,atten);
};

void(entity e, string str) temp_unavail;
