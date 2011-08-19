/*************************************************************************
 * Metamorphism Quake Server: Common/Class/scout.c,                      *
 *                            Common/Class/scout.qc                      *
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
/* Scout Specific */

void() AutoTrak =
{
  local entity head;
  local vector v;

  if (((self.id_number != self.owner.id_number) ||
       (self.owner.playerclass != CL_SCOUT)) ||
       (self.owner.deadflag))
  {
    remove(self);
    return;
  }

  if (self.ltime < time)
  {
    head = findradius(self.owner.origin,380);
    while (head)
    {
      if ((head.classname == "player") && 
          (head.playerclass == CL_SNIPER) &&
          (head.weapon == IT_SHOTGUN))
      {
        msg_entity = self.owner;
        WriteByte (MSG_ONE, SVC_TEMPENTITY);
        WriteByte (MSG_ONE, TE_TELEPORT);
        WriteCoord (MSG_ONE, head.origin_x);
        WriteCoord (MSG_ONE, head.origin_y);
        WriteCoord (MSG_ONE, head.origin_z);
    
      }
      head = head.chain;
    }
    self.ltime = time + 0.7;
  }

  self.enemy = self.owner.enemy;

  if (self.enemy == world)
  { 
    self.origin = self.owner.origin;
    self.think = AutoTrak;
    self.nextthink = time + 0.01;
    return;
  } 
 
  if ((self.owner.deadflag) || (self.id_number != self.owner.id_number))
  {
    remove(self);
    return;
  }
  
  v = normalize((self.enemy.origin + '0 0 16') - (self.owner.origin + '0 0 16'));
  self.origin = self.owner.origin + '0 0 16' + v*20;
  self.angles = vectoangles(v);
  self.velocity = self.owner.velocity;

  self.think = AutoTrak;
  self.nextthink = time + 0.01;
};


void() SpawnSniperTrack =
{
/*
  local entity st;
  st = spawn();
  st.owner = self;
  st.id_number = self.id_number;
  st.nextthink = time + 1;
  st.think = SniperFind;
*/
  local entity at;
  at = spawn();
  at.owner = self;
  at.think = AutoTrak;
  at.nextthink = time;
  at.movetype = MOVETYPE_NOCLIP;
  at.solid = SOLID_TRIGGER;
  at.id_number = self.id_number;
  setmodel(at,"progs/s_spike.mdl");
};






void() PBPickUp =
{
  local entity head;

  if (((self.id_number != self.owner.id_number) ||
       (self.owner.playerclass != CL_SCOUT)) ||
       (self.owner.deadflag))
  {
    remove(self);
    return;
  }

  // does not work with jetpack on... or off ground
  if ((self.owner.player_flags & PF_SPEED) && (!(self.owner.flags & FL_ONGROUND)))
  {
    self.nextthink = time + 0.3;
    self.think = PBPickUp;
    return;
  }

  head = findradius(self.owner.origin,50);
  while (head)
  {
    if ((head.classname == "pipebomb") && (head.owner != self.owner))
    {
      sound (self.owner, CHAN_VOICE, "misc/runekey.wav", 1, ATTN_NORM);
       
      remove(head);
      self.think = PBPickUp;
      self.nextthink = time + 0.3;
    }
    /*
    if (head.classname == "timebomb") 
    {
      sound(self,CHAN_VOICE,"misc/runekey.wav",1,ATTN_NONE);
      bprint(self.owner.netname);
      bprint(" disarmed ");
      bprint(head.owner.netname);
      bprint("'s time bomb.\n");
      remove(head);
      self.think = PBPickUp;
      self.nextthink = time + 0.3;
    }
    */
    head = head.chain;
  }

  self.nextthink = time + 0.3;
  self.think = PBPickUp;
};

void() SpawnPipeCleaner =
{
  local entity pc;
  pc = spawn();
  pc.owner = self;
  pc.id_number = self.id_number;
  pc.nextthink = time + 1;
  pc.think = PBPickUp;
};


