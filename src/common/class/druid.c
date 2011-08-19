/*************************************************************************
 * Metamorphism Quake Server: Common/Class/druid.c,                      *
 *                            Common/Class/druid.qc                      *
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

// (Double-Druid Combination) / Archon? ;)
#include "meta.h"
void() DoubleUpdate =
{
  if ((self.enemy.deadflag || (self.enemy.id_number != self.id_number))
     || (self.health != self.owner.id_number))
  {
    self.owner.playerclass = CL_DRUID;
    self.netname = "loss of power";
    self.owner.frags = self.owner.frags + 1;
    self.owner.health = 1;
    self.owner.takedamage = DAMAGE_AIM;
    DoDamage(self.owner,self,self.owner,20,SH_UNKNOWN);
    remove(self);
    return;
  }

  self.owner.origin = self.enemy.origin;
  self.owner.velocity = self.enemy.velocity;

  self.nextthink = time + 0.01;
  self.think = DoubleUpdate;

}; 


//-------------------------------------------------------------------

void() DoubleMain =
{
  // reset health, ammo
//  setmodel(self.enemy,"progs/s_bubble.spr");
  self.enemy.player_flags = self.enemy.player_flags | PF_EYES_ONLY;
  self.enemy.ammo_rockets = 0;
  self.enemy.max_ammo_rockets = 0;
 
  self.enemy.max_health = 300;
  self.enemy.health = 300;
  self.enemy.max_arm = 50;
  self.enemy.armorvalue = 0;
  self.enemy.armortype = 0;
  self.enemy.effects;
};

void() DoubleAux = 
{
  local entity e;
  if ((self.enemy.playerclass != CL_DRUID) || (self.enemy.spell != 6))
  {
    sprint(self,"Target is not willing!\n");
    return;
  }

  if (self.player_flags & PF_EYES_ONLY)
    return;
  if (self.player_flags & PF_NOFRAME)
    return;

  e = spawn();
  e.owner = self;
  e.enemy = self.enemy;
  e.think = DoubleUpdate;
  e.nextthink = time;
  e.id_number = self.enemy.id_number;
  e.health = self.id_number;

  self.weapon = IT_LIGHTNING;
//  self.items = IT_LIGHTNING;
  // only spells.....
  self.items = self.items - (self.items & (IT_SHOTGUN | IT_SUPER_NAILGUN |
IT_NAILGUN | IT_GRENADE_LAUNCHER | IT_ROCKET_LAUNCHER | IT_AXE));

  self.ammo_shells = self.max_ammo_shells = 0;
  self.ammo_nails = self.max_ammo_nails = 0;
  self.ammo_rockets = self.max_ammo_rockets = 0;

  // go to eyes model
  setmodel(self,"progs/flame2.mdl");
  self.effects = EF_DIMLIGHT;

  // drop runes
  self.player_flags = self.player_flags - (self.player_flags & PF_HAS_SPECIAL);
  // drop flags
#ifdef GAME_CTF
  self.player_flags = self.player_flags | PF_DROPPED_FLAG;
#endif
  self.player_flags = self.player_flags | PF_NO_HOOK | PF_NOFRAME;
  self.max_health = 1;
  self.health = 1;
  setsize(self,'0 0 0','0 0 0');
  self.solid = SOLID_NOT;
  self.movetype = MOVETYPE_NOCLIP;
  self.takedamage = 0; // invincible. Die iff main dies.
  self.frame = 1; // large flame?
  DoubleMain();
};
