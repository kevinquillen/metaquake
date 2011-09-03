/*************************************************************************
 * Metamorphism Quake Server: Common/Games/CTF/barrier.c,                *
 *                            Common/Games/CTF/barrier.qc                *
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

void() ctf_barrier_large =
{
	local float	oldz;
	if (deathmatch != DMMODE_CTF)
	{
          remove(self);
          return;
        }
	
	self.solid = SOLID_BBOX;
	self.movetype = MOVETYPE_TOSS;
	precache_model ("maps/b_explob.bsp");
	setmodel (self, "maps/b_explob.bsp");
	setsize (self, '0 0 0', '32 32 64');

	self.origin_z = self.origin_z + 2;
	oldz = self.origin_z;
	droptofloor();
	if (oldz - self.origin_z > 250)
	{
		dprint ("item fell out of level at ");
		dprint (vtos(self.origin));
		dprint ("\n");
		remove(self);
	}
};

void() ctf_barrier_small =
{
	local float	oldz;
	if (deathmatch != DMMODE_CTF)
	{
          remove(self);
          return;
        }
	
	self.solid = SOLID_BBOX;
	self.movetype = MOVETYPE_TOSS;
	precache_model2 ("maps/b_exbox2.bsp");
	setmodel (self, "maps/b_exbox2.bsp");
	setsize (self, '0 0 0', '32 32 32');

	self.origin_z = self.origin_z + 2;
	oldz = self.origin_z;
	droptofloor();
	if (oldz - self.origin_z > 250)
	{
		dprint ("item fell out of level at ");
		dprint (vtos(self.origin));
		dprint ("\n");
		remove(self);
	}
};

void() ctf_statue_shambler =
{
	local float	oldz;
	if (deathmatch != DMMODE_CTF)
	{
          remove(self);
          return;
        }
	
	self.solid = SOLID_BBOX;
	self.movetype = MOVETYPE_NONE;
	precache_model ("progs/shambler.mdl");
	setmodel (self, "progs/shambler.mdl");
	setsize(self,'-40 -40 -40','40 40 108');
	self.frame = self.spawnflags;

	self.origin_z = self.origin_z + 2;
	oldz = self.origin_z;
//	droptofloor();
	if (oldz - self.origin_z > 250)
	{
		dprint ("item fell out of level at ");
		dprint (vtos(self.origin));
		dprint ("\n");
		remove(self);
	}
};
