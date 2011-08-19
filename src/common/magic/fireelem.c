/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/fireelem.c,                   *
 *                            Common/Magic/fireelem.qc                   *
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

entity(float radius) ClosestTarget;
/*
  FireElem Spell Implementation
*/
entity() SelectSpawnPoint;
void() TurnThink;

// Hold Person
void() FireElemHoldPerson =
{
  if (self.ammo_cells < 70)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }
  self.ammo_cells = self.ammo_cells - 70;
  sound (self, CHAN_WEAPON, "items/protect2.wav", 1, ATTN_NORM);
  if (((self.enemy == world) || (self.enemy.deadflag)) || (self.enemy.classname != "player"))
  {
    sprint(self,"Casting failed!\n");
    return;
  }

  if (!(self.enemy.invincible_finished > time))
  {
    self.enemy.invincible_finished = time + 4;
    self.enemy.invincible_time = 1;
    self.enemy.items = self.enemy.items | IT_INVULNERABILITY;
  } else {
	  
	  self.enemy = self; // Cast on self if cast on someone with 666 ;)
	  self.invincible_finished = time + 4;
	  self.invincible_time = 1;
	  self.items = self.items | IT_INVULNERABILITY;
  }

  self.enemy.attack_finished = time + 4;
  self.enemy.pausetime = time + 4;
  if (self.enemy != self)
    cprint(self.enemy,"Stasis Field\n");
  else
	cprint(self,"Reflected by 666\n");

  sprint(self,"Held: ");
  sprint(self,self.enemy.netname);
  sprint(self,"\n");
  self.enemy = world;
};

/*
  Spell Name: Beam
  Spell Type: Aimed, Offensive
*/  
void() T_BeamTouch =
{
  if (other.takedamage != DAMAGE_AIM)
  {
    remove(self);
    return;
  }
  DoDamage(other,self,self.owner,12+8*random(),SH_MAGIC);
  remove(self);
  return;
};

void() FireElemBeam =
{
  local entity b;
	 
  if (self.ammo_cells < 3)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }
  self.ammo_cells = self.ammo_cells - 3;
//  sound (self, CHAN_WEAPON, "items/itemb.wav", 1, ATTN_NORM);
  
  b = spawn();
  b.netname = "beam";
  b.owner = self;
  b.solid = SOLID_BBOX;
  b.movetype = MOVETYPE_FLYMISSILE;
  setmodel(b,"progs/s_bubble.spr");
  makevectors(self.v_angle);
  b.origin = self.origin + v_up*16 + v_forward * 16;
  b.velocity = aim(self,10000);
  b.velocity = b.velocity * 2000;
  b.touch = T_BeamTouch;
  self.attack_finished = time + 0.1;
};


// Inferno
void() InfernoEx =
{
  if ((self.owner.deadflag) || (self.owner == world) || (!self.owner))
  {  
    remove(self);
    return;
  }

  sound(self,CHAN_VOICE,"weapons/r_exp3.wav",1,ATTN_NORM);
  self.owner.effects = self.owner.effects - (self.owner.effects & EF_BRIGHTLIGHT);
  T_RadiusDamage (self, self.owner, 180, self.owner, SH_INCINERATE);
  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_LAVASPLASH);
  WriteCoord (MSG_MULTICAST, self.origin_x);
  WriteCoord (MSG_MULTICAST, self.origin_y);
  WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif
  BecomeExplosion();
};
  
void(entity t, float f, float r, float w) TossInferno =
{
  t.netname = "raging inferno";
  t.origin = t.owner.origin;
  makevectors(t.owner.v_angle);
  setmodel(t,"progs/flame2.mdl");
  t.movetype = MOVETYPE_TOSS;
  t.solid = SOLID_TRIGGER;
  t.velocity = ((v_forward * f + v_right * r)*4) + (v_up * -200) + '0 0 400';
  t.nextthink = time + w;
  t.think = InfernoEx;
};
/*
void() tstop =
{
  self.velocity = '0 0 0';
  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_LIGHTNING2);
  WriteEntity(MSG_MULTICAST, self);
  WriteCoord (MSG_MULTICAST, self.origin_x);
  WriteCoord (MSG_MULTICAST, self.origin_y);
  WriteCoord (MSG_MULTICAST, self.origin_z);
  WriteCoord (MSG_MULTICAST, self.owner.origin_x);
  WriteCoord (MSG_MULTICAST, self.owner.origin_y);
  WriteCoord (MSG_MULTICAST, self.owner.origin_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif


  sound (self, CHAN_WEAPON, "misc/power.wav", 1, ATTN_NORM);
  self.effects = EF_BRIGHTLIGHT;
  self.think = SUB_Remove;
  self.nextthink = time + 3.6;
};
*/

void() FireElemInferno = 
{
  local entity f1, f2, f3, f4, t;
  local float f, r;

  if (self.ammo_cells < 100)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }
  sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
  self.ammo_cells = self.ammo_cells - 100;

  f1 = spawn();
  f2 = spawn();
  f3 = spawn();
  f4 = spawn();
//  t = spawn();

  f1.owner = f2.owner = f3.owner = f4.owner = self;
//  f1.enemy = f2.enemy = f3.enemy = f4.enemy = t;
/*    
  setmodel(t,"progs/v_spike.mdl");
  t.movetype = MOVETYPE_FLYMISSILE;
  t.origin = self.origin;
  t.velocity = '0 0 100';
  t.think = tstop;
  t.nextthink = time + 1;
  t.owner = self;
*/  
  TossInferno(f1, 16, 16, 1.1);
  TossInferno(f2, 16, -16, 1.2);
  TossInferno(f3, -16, 16, 1.3);
  TossInferno(f4, -16, -16, 1.4);
//  self.pausetime = time + 4;
  self.attack_finished = time + 3;
};

// Blaze
void() BlazeHurt =
{
  if ((((self.ltime < time) || (self.enemy.health < 1)) || (pointcontents(self.enemy.origin)==CONTENT_WATER)) || (self.enemy.invincible_finished > time))
  {
    remove(self);
    return;
  }
  self.origin = self.enemy.origin;
  if (self.frame == 1)
    DoDamage(self.enemy,self,self.owner,6,SH_FIRE);
  self.frame = self.frame + 1;
  if (self.frame > 4)
    self.frame = 1;
  self.nextthink = time + 0.2;
  self.think = BlazeHurt;
};

void() BlazeTouch = 
{
	if (!(other.takedamage == DAMAGE_AIM))
		return;
	self.solid = SOLID_NOT;
	self.touch = SUB_Null;
	self.enemy = other;
	self.movetype = MOVETYPE_NOCLIP;
	self.velocity = '0 0 0';
	centerprint(self.enemy,"You have been ignited!\n");
	setmodel(self,"progs/s_explod.spr");
	self.think = BlazeHurt;
	self.nextthink = time + 0.2;
	self.ltime = time + 10;
};

void() StormExplode =
{
  local entity e;

  self.netname = "firebringer";
  if (other.takedamage == DAMAGE_AIM)
  {
    self.velocity = '0 0 0';
    DoDamage(other,self,self.owner,60+40*random(),SH_FIRE);
  }

  self.touch = SUB_Null;
  self.health = 5;
  self.spawn_origin = self.velocity * 0.5;
  self.spawn_origin_z = 0;
  self.velocity = '0 0 0';
  self.solid = SOLID_NOT;
  self.angles = '0 0 0';

  while (self.health > 0)
  {
    e = spawn();
    e.netname = "fire storm";
    e.owner = self.owner;
    e.origin = self.origin;
    setmodel(e,"progs/flame2.mdl");
    e.movetype = MOVETYPE_TOSS;
    e.velocity_x = random()*300 - 150;
    e.velocity_y = random()*300 - 150;
    e.velocity_z = random()*100 + 200;
    e.velocity = e.velocity + self.spawn_origin;
    e.solid = SOLID_BBOX;
    setsize(e,'-1 -1 -1','1 1 1');
    e.touch = BlazeTouch;
    e.nextthink = time + 2;
    e.think = SUB_Remove;
    self.health = self.health - 1;
  }
  self.nextthink = time + 2;
  self.think = SUB_Remove;
};


void() FireElemFirestorm =
{
  local entity l;

  if (self.ammo_cells < 60)
  {
    sprint(self,"Insufficient Mana!\n");
    return;
  }

  self.ammo_cells = self.ammo_cells - 60;

  sound (self, CHAN_WEAPON, "misc/power.wav", 1, ATTN_NORM);
  l = spawn();
  l.owner = self;
  setmodel(l,"progs/flame2.mdl");
  l.frame = 1;
  makevectors(self.v_angle);
  l.effects = EF_DIMLIGHT;
  l.origin = self.origin + v_forward * 16 + v_up*16;
  l.movetype = MOVETYPE_FLYMISSILE;
  l.solid = SOLID_BBOX;
  l.touch = StormExplode;
  setsize(l,'-1.3 -1.3 -1.3','1.3 1.3 1.3');
  l.think = SUB_Remove;
  l.nextthink = time + 5;
  l.velocity = v_forward * 800;
  l.angles = vectoangles(l.velocity);
  l.angles_x = l.angles_x + 90;
  self.attack_finished = time + 1.0;
};


/*
void() ProbeTouch =
{
  if (other.classname != "player")
    return;
  other.origin_z = other.origin_z + 1;
  other.flags = other.flags - (other.flags & FL_ONGROUND);
  other.velocity = self.velocity * 2;
  other.velocity_z = 1000;
  remove(self);
  return;
};

void() ProbeActiveThink =
{
  if ((pointcontents(self.origin) == CONTENT_WATER) || (self.ltime < time))
  {
    remove(self);
    return;
  }
  
  self.enemy = ClosestTarget(500);
  if (self.enemy.classname == "player")
  {
    self.touch = ProbeTouch;
    self.velocity = normalize((self.enemy.origin+'0 0 16') - self.origin)*2000;
    self.think = SUB_Remove;
    self.nextthink = time + 2;
    return;
  }

  self.think = ProbeActiveThink;
  self.nextthink = time + 0.1;

};

void() ProbeInertThink =
{
  local entity s;
  if (self.velocity == '0 0 0')
  {
    s = spawn();
    s.ltime = time + 40;
    s.owner = self.owner;
    s.movetype = MOVETYPE_FLYMISSILE;
    s.solid = SOLID_BBOX;
    setsize(s,'0 0 0','0 0 0');
    setmodel(s,"progs/lavaball.mdl");
    s.origin = self.origin + '0 0 16';
    s.think = ProbeActiveThink;
    s.nextthink = time + 0.1;
    remove(self);
    return;
  }

  self.think = ProbeInertThink;
  self.nextthink = time + 0.1;
};

void() FireElemProbe =
{
  local entity p1, p2;

  if (!CheckCount(2000,"magic_fireprobe",4))
  {
    cprint(self,"There are 4 already out!\n");
    self.attack_finished = time + 0.5;
    return;
  } 

  if (self.ammo_cells < 50)
  {
    sprint(self,"Insufficient Mana!\n");
    return;
  }

  self.ammo_cells = self.ammo_cells - 50;

  p1 = spawn();
  p2 = spawn();
  p1.solid = p2.solid = SOLID_BBOX;
  p1.movetype = p2.movetype = MOVETYPE_BOUNCE;
  p1.owner = p2.owner = self;
  p1.classname = "magic_fireprobe";
  p2.classname = "magic_fireprobe";

  setmodel(p1,"progs/lavaball.mdl");
  setmodel(p2,"progs/lavaball.mdl");

  makevectors(self.v_angle);
  p1.origin = self.origin + v_forward * 8 + v_right * -8 + v_up * 16;
  p2.origin = self.origin + v_forward * 8 + v_right * 8 + v_up * 16;

  setsize(p1,'0 0 0','0 0 0');
  setsize(p2,'0 0 0','0 0 0');

  p1.velocity = normalize(
                (self.origin + v_forward * 32 + v_up * 32 + v_right * -32) -
                self.origin)
                *2000;
  p2.velocity = normalize(
                (self.origin + v_forward * 32 + v_up * 32 + v_right * 32) -
                self.origin)
                *2000;

  p1.think = ProbeInertThink;
  p2.think = ProbeInertThink;
  p1.nextthink = p2.nextthink = time + 1;
  self.attack_finished = time + 1.1;
};
*/

// Spell casting: by selection.
void() FireElemSpellName =
{
  sprint(self,"FireElem: ");
  if (self.spell == 0)
    sprint(self,"Stasis Field\n");
  if (self.spell == 1)
    sprint(self,"Beam\n");
  if (self.spell == 2)
    sprint(self,"Firestorm\n");
  if (self.spell == 3)
    sprint(self,"Inferno\n");
};

void() FireElemCast =
{
  if (pointcontents(self.origin)==CONTENT_WATER)
  {
    sprint(self,"You can't cast underwater!\n");
    return;
  }

	if(self.spell == 0)
		FireElemHoldPerson();
	if(self.spell == 1)
		FireElemBeam();
	if(self.spell == 2)
		FireElemFirestorm();
	if(self.spell == 3)
		FireElemInferno();
};
