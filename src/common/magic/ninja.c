/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/ninja.c,                      *
 *                            Common/Magic/ninja.qc                      *
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

void(vector org, entity owner) spawn_tdeath;

void() NinjaInvis =
{
  DoInvis(10,100);
};

void() markTouch =
{
  if (other != self.enemy)
    return;
  if (self.voided)
    return;
  self.voided = 1;

  DoDamage(other,self,self.owner,30000,SH_UNKNOWN);
  remove(self);
  return;
};

void() markChase =
{
  local float vel;

  if ((self.enemy.id_number != self.health) || (self.owner.id_number != self.id_number))
  {
    remove(self);
    return;
  }
  if ((time - 120) > self.ltime)
  {
    sprint(self.owner,"Your mark has worn off\n");
    remove(self);
    return;
  }

#ifdef QUAKEWORLD
  if (self.effects == EF_BLUE)
    self.effects = EF_DIMLIGHT;
  else if (self.effects == EF_DIMLIGHT)
    self.effects = EF_RED;
  else
    self.effects = EF_BLUE;
#endif

  vel = vlen(self.enemy.velocity);
  if (vel < 130)
    vel = 130;
  vel = vel - 55;
  if (self.enemy.player_flags & PF_SPEED)
    vel = vel + 65; // Trackers are good for anti scout/druid if you can hit it

  if (visible(self.enemy))
    self.velocity = normalize((self.enemy.origin + '0 0 10') - self.origin) * vel;
  self.angles = vectoangles (self.velocity);

  self.nextthink = time + 0.2; 
  self.think = markChase;
};

void() NinjaMark =
{
  local entity mark;
  if (self.ammo_cells < 200)
  {
    sprint(self,"Insufficient Mana!\n");
    return;
  }
  if (!CheckCount(10000,"deathmark",1))
  {
    cprint(self,"Someone is already marked!\n");
    return;
  }
  self.ammo_cells = self.ammo_cells - 30;

  makevectors(self.v_angle);
  traceline(
	(self.origin + '0 0 16') + v_forward * 150,
	(self.origin + '0 0 16') + v_forward * 700,
	FALSE,
	self);

  self.enemy = world;

#ifdef QUAKEWORLD
    WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
    WriteByte (MSG_MULTICAST, TE_LIGHTNINGBLOOD);
    WriteCoord (MSG_MULTICAST, trace_endpos_x);
    WriteCoord (MSG_MULTICAST, trace_endpos_y);
    WriteCoord (MSG_MULTICAST, trace_endpos_z);
    multicast (trace_endpos, MULTICAST_PVS);
#else
    particle (trace_endpos, '0 0 100', 225, 20);
#endif


  self.attack_finished = time + 1;
 	
  if (trace_fraction != 1.0)
  {
    if (trace_ent.takedamage == DAMAGE_AIM)
      self.enemy = trace_ent;
  } else
    return;

  if (self.enemy == world)
   return;

  self.ammo_cells = self.ammo_cells - 170;   

  mark = spawn();
  if (self.enemy.classname == "player")
  {
    centerprint(self.enemy,"  @@  @@  \n          \n @      @ \n  @@@@@@  \n");
    bprint (self.enemy.netname);
    bprint (" was marked for death by ");
    bprint (self.netname);
    bprint("\n");
  }

  setmodel(mark,"progs/k_spike.mdl");
  mark.netname = "tracker";
  mark.movetype = MOVETYPE_FLYMISSILE;
  mark.origin = self.origin + '0 0 16';
  setsize(mark,'0 0 0','0 0 0');
  mark.owner = self;
  mark.enemy = self.enemy;
  mark.health = self.enemy.id_number;
  mark.id_number = self.id_number;
  mark.think = markChase;
  mark.nextthink = time + 0.2;
  mark.touch = markTouch;
  mark.effects = EF_DIMLIGHT;
  mark.solid = SOLID_BBOX;
  mark.classname = "deathmark";
  mark.ltime = time;

};

void() NinjaPort =
{
  
  if (self.ammo_cells < 120)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }

#ifdef GAME_CTF
  if (self.player_flags & PF_HAS_FLAG)
  {
    cprint(self,"You must drop the flag first!\n");
    return;
  }
#endif
   
  self.ammo_cells = self.ammo_cells - 120;
  spawn_tdeath(self.spawn_origin,self);
  self.origin = self.spawn_origin;
};

void() NinjaSetPort =
{
  cprint(self,"Location Set!\n");
  self.spawn_origin = self.origin;
  self.attack_finished = time + 0.5;
};

void() Ensnare = 
{
  if (other.classname == "player")
  {
    other.pausetime = time + 2.5;
    sprint(other,"Hmmm... Sticky.\n");
    sprint(self.owner,"Snag!\n");
  }
  remove(self);
};

void() NinjaEnsnare =
{
  local entity e;
  if (self.ammo_cells < 30)
  {
    sprint(self,"Insufficient Mana!\n");
    return;
  }
  self.ammo_cells = self.ammo_cells - 30;
  e = spawn();
  e.owner = self;
  setmodel(e,"progs/tarbaby.mdl");
  e.movetype = MOVETYPE_FLYMISSILE;
  e.solid = SOLID_BBOX; 
  e.origin = self.origin + '0 0 16';
  makevectors(self.v_angle);
  e.velocity = v_forward * 1000;
  e.think = SUB_Remove;
  e.nextthink = time + 2;
  e.touch = Ensnare;
  self.attack_finished = time + 0.8;
  setsize(e,'-1.2 -1.2 -1.2','1.2 1.2 1.2');
};

void() NinjaSpellName =
{
  sprint(self,"Ninja: ");
  if (self.spell == 0)
    sprint(self,"Invisibility\n");
  if (self.spell == 1)
    sprint(self,"Mark of Death\n");
  if (self.spell == 2)
    sprint(self,"Teleport\n");
  if (self.spell == 3)
    sprint(self,"Set Teleport Dest.\n");
  if (self.spell == 4)
    sprint(self,"Ensnare\n");
};

void() NinjaCast =
{
  if (self.spell==0)
    NinjaInvis();
  if (self.spell==1)
    NinjaMark();
  if (self.spell==2)
    NinjaPort();
  if (self.spell==3)
    NinjaSetPort();
  if (self.spell==4)
    NinjaEnsnare();
};
