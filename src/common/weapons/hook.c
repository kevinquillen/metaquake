/*************************************************************************
 * Metamorphism Quake Server: Common/Weapons/hook.c,                     *
 *                            Common/Weapons/hook.qc                     *
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

float() CheckHook;
void() RetractHook =
{
  local float d, in_use, z_old, z_new;

  if (
     (self.owner.player_flags & PF_SPEED) ||
     (!(self.owner.player_flags & PF_HOOK)) ||
//     (!(self.owner.player_flags & PF_HOOK_ON)) ||
     (self.owner.deadflag) ||
     (self.owner.teleport_time > time) ||
     (self.enemy.teleport_time > time) ||
     (self.enemy.solid == SOLID_NOT) ||
     (vlen(self.enemy.velocity) > 520) || 
     (((self.owner.weapon == IT_AXE) && (self.owner.axe_mode == 1)) && (!self.owner.button0))
     )
  {
    self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_HOOK);
//    self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_HOOK_ON);
    remove(self);
    return;
  }

// Remove if touch Sky
  if (pointcontents(self.origin) == CONTENT_SKY)
  {
    self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_HOOK);
    remove(self);
    return;
  }

// stick to center of our enemy.
  if ((self.enemy.solid != SOLID_BSP) && (self.enemy.solid != SOLID_BBOX))
    self.origin = self.enemy.origin;

// Check to see if the hook is in use with hands -- that is -- not using the
// +hook commands. People not using the +hook commands get a bonus in
// speed and damage
  if (((self.owner.weapon == IT_AXE) &&
       (self.owner.axe_mode == 1)) && self.owner.button0)
    in_use = 1;
  else in_use = 0;

  z_old = self.owner.velocity_z;  

// Pull the person towards the hook head...
  self.owner.velocity = normalize(self.origin -
              (self.owner.origin + '0 0 16')) *
//              (self.owner.origin)) *
              (700 - self.owner.weight + (in_use * 50));
  z_new = self.owner.velocity_z;

// Don't slow people from falling -- pull down until a certain velocity is
// reached, then gravity kicks in...
  if ((z_old <= z_new) && (z_new < 0))
    self.owner.velocity_z = z_old;

// Ok -- pull lightly if the owner is close by -- eliminates jerkiness...
// Note that in QuakeWorld, predictions cause jerkiness anyway, and there
// is no way to remedy it.
#ifndef QUAKEWORLD
  if (self.owner.flags & FL_ONGROUND)
    self.owner.origin = self.owner.origin + '0 0 3';
#endif


  d = vlen(self.origin - self.owner.origin);
  if (d < 50)
    self.owner.velocity = normalize(self.origin - (self.owner.origin + '0 0 16')) * 110;
 
// Do damage to takedamage_entities...
  if ((self.enemy.takedamage) && (self.attack_finished <= time))
  {
    DoDamage (self.enemy, self, self.owner, 1 + (2 * in_use), SH_UNKNOWN);
    self.attack_finished = time + 0.1;
  }

  self.velocity = self.enemy.velocity;

// Make nasty noises ;)
  if (self.enemy.solid == SOLID_SLIDEBOX)
  {
     setorigin (self, self.enemy.origin);
     sound (self, CHAN_WEAPON, "blob/land1.wav", 1, ATTN_NORM);
  }

  self.nextthink = time + 0.01;         // SYS_TICRATE;
};

void() HookCheck =
{
  if (
     (self.owner.player_flags & PF_SPEED) ||
     (!(self.owner.player_flags & PF_HOOK)) ||
     (self.owner.deadflag) ||
     (self.owner.teleport_time > time) ||
     (((self.owner.weapon == IT_AXE) && (self.owner.axe_mode == 1)) && (!self.owner.button0))
     )
  {
    self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_HOOK);
    remove(self);
    return;
  }
  self.nextthink = time + 0.01;
  self.think = HookCheck;
};

void() LPosition =
{
  local float d;
  local vector p;
  local string s;

  d = vlen((self.owner.origin + '0 0 16') - self.enemy.origin);
  if ((d < 50) && (self.enemy.ltime < time - 1))
  {
    remove(self);
    return;
  }
  p = self.enemy.origin - (self.owner.origin + '0 0 16');
  self.angles = vectoangles(p);
  d = vlen(p);
  p = normalize(p);
  d = d / (self.enemy.health + 1);
  d = d * self.health;
  setorigin(self, self.owner.origin + (p * d));
  if ((self.enemy == world) || (!(self.owner.player_flags & PF_HOOK)))
  {
    remove(self);
    return;
  }
  self.think = LPosition;
  self.nextthink = time + 0.01;
};

void() T_HookTouch =
{
  if (self.voided)
    return;
  if (other == self.owner)
    return;
  if (other.owner == self.owner)
    return;
  self.voided = 1;
  self.enemy = other;
  self.velocity = '0 0 0';
  sound (self, CHAN_WEAPON, "weapons/bounce.wav", 1, ATTN_NORM);

//  self.owner.player_flags = self.owner.player_flags | PF_HOOK_ON;
  self.touch = SUB_Null;
  self.think = RetractHook;
  self.nextthink = time;
};

void(float num) MakeLink =              // CALLED BY THE HOOK ONLY
{
  local entity link;
  link = spawn();
  setmodel(link,"progs/spike.mdl");
  link.health = num;
  link.owner = self.owner;
  link.enemy = self;
  link.origin = self.origin;
  link.solid = SOLID_NOT;
  link.movetype = MOVETYPE_NOCLIP;
  link.think = LPosition;
  link.nextthink = time + 0.05;         // usual SYS_TICRATE
  link.angles = vectoangles(self.origin - (self.owner.origin + '0 0 16'));
};

void() MakeLinks =
{
  local float num;
  self.movetype = MOVETYPE_FLYMISSILE;
  self.solid = SOLID_TRIGGER;
  setmodel(self,"progs/v_spike.mdl");
  setsize (self, '0 0 0', '0 0 0');     
  makevectors(self.owner.v_angle);
  setorigin(self, self.owner.origin + v_forward * 20 + '0 0 16');
  self.avelocity = '250 250 250';
  self.velocity = v_forward*1700;
  self.touch = T_HookTouch;
  num = self.health;
  while (num > 0)
  {
    MakeLink(num);
    num = num - 1;
  }
  self.think = HookCheck;
  self.nextthink = time;
};

void() W_FireHook =
{
  local entity hookobj;

  if (self.player_flags & PF_NO_HOOK)
    return;
  if (self.player_flags & PF_DEMON)
    return;
  if (self.player_flags & PF_DOGGIE)
    return;

  if (self.player_flags & PF_SPEED)
    return;

  if (self.player_flags & PF_HOOK)
    return;

  if (self.teleport_time > time)
    return;

  sound (self, CHAN_WEAPON, "shalrath/attack2.wav", 1, ATTN_NORM);
  self.player_flags = self.player_flags | PF_HOOK;
  hookobj = spawn();
  hookobj.owner = self;
  hookobj.ltime = time;
  hookobj.classname = "hook";
  hookobj.health = 3;              // number of links...
  hookobj.think = MakeLinks;
  hookobj.nextthink = time;
  hookobj.voided = 0;
  hookobj.netname = "hook";
};

float() CheckHook =
{
  if (((self.weapon == IT_AXE) && (self.axe_mode == 1)) && (self.player_flags & PF_HOOK))
    return 1;
  return 0;
};

/**********\
 Impulse.qc
\**********/
void() W_IFireHook =
{
  if (!CheckHook())
    W_FireHook();
};

void() W_IReleaseHook = 
{
  self.player_flags = self.player_flags - (self.player_flags & PF_HOOK);
};
