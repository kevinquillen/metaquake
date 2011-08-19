/*************************************************************************
 * Metamorphism Quake Server: Common/Class/sniper.c,                     *
 *                            Common/Class/sniper.qc                     *
 * Copyright (C) 2000 by Lon Hohberger.                                  *
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

void()player_shot1;

void ()sniper_attack=
{
	local float dchk,epchk,zchk,x;
	local string s;
	local vector endp,org,dir;
	local entity enthit;
	if (self.ammo_shells < 1)
	{
		return;
	}
	self.attack_finished = time + 0.8;
	self.currentammo = self.ammo_shells = self.ammo_shells - 1;
	sound(self,CHAN_WEAPON,"weapons/rocket1i.wav",1,ATTN_NORM);
	player_shot1();
	makevectors(self.v_angle);
	org = self.origin + '0 0 16';
	traceline(org,org + v_forward*2000,FALSE,self);

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte(MSG_ONE,SVC_BIGKICK);
#else
	self.punchangle_x = -4;
#endif

	org = trace_endpos - v_forward * 4;

	if (trace_ent == world)
	{
  		WriteByte(MSG_MULTICAST,SVC_TEMPENTITY);
		WriteByte(MSG_MULTICAST,TE_SPIKE);
		WriteCoord(MSG_MULTICAST,org_x);
		WriteCoord(MSG_MULTICAST,org_y);
		WriteCoord(MSG_MULTICAST,org_z);
#ifdef QUAKEWORLD
		multicast(org,MULTICAST_PVS);
#endif
		return;
	}

#ifdef QUAKEWORLD
        if ((trace_ent.classname != "Énhumaî…ÃÒ‘") && (trace_ent.classname != "player"))
#else
	if (trace_ent.classname != "player")
#endif
	{
		DoDamage(trace_ent,self,self,75,SH_SNIPER);
		return;
	}
	
	enthit = trace_ent;
	
	x=6;
	org = trace_ent.origin + '0 0 22';
	
	while (x>0)
	{
		// Check distance ;)
		if (vlen(org - trace_endpos) < 8) {
			sprint(self,"*** Head shot ***\n");
			DoDamage(enthit,self,self,190 + random()*60,SH_SNIPER);
			return;
		}

		// Advance shot through --
		traceline(trace_endpos,trace_endpos + v_forward*4,TRUE,self);
		x = x - 1;
	}
	DoDamage(enthit,self,self,40 + random() * 10, SH_SNIPER);
};

