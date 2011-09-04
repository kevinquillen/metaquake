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
#include "magic.h"

entity(float radius) ClosestTarget;
void(entity inflictor, entity attacker, float damage, float radius, entity ignore, float damagetype) T_AreaDamage;
void() TurnThink;

void() FireElemStasisField =
{
	if(!CheckHarmfulSpellTarget())
		return;
		
	//Must be a player
	if(self.enemy.classname != "player")
	{
		sprint(self, "You can only cast this on players!\n");
		self.attack_finished = time + 0.3;
		return;
	}

	if(!CheckMana(self, 70))
		return;

	sound (self, CHAN_WEAPON, "items/protect2.wav", 1, ATTN_NORM);
	
	
	//Enemy is invulnerable...
	self.enemy.invincible_finished = time + 4;
	self.enemy.invincible_time = 1;
	self.enemy.items = self.enemy.items | IT_INVULNERABILITY;
	
	//..but stuck unable to attack
	stuffcmd(self.enemy, "-attack");
	self.enemy.attack_finished = time + 4;
	self.enemy.pausetime = time + 4;
	cprint(self.enemy,"Stasis Field\n");

	//Show who was held
	sprint(self,"Held: ");
	sprint(self,self.enemy.netname);
	sprint(self,"\n");
	
	//Don't allow repeat casts on the same person immediately
	self.enemy = world;
	self.attack_finished = time + 0.5;
	return;
};

void() T_BeamTouch =
{
	if (other.takedamage != DAMAGE_AIM)
	{
		remove(self);
		return;
	}
	
	DoDamage(other, self, self.owner, 5+5*random(), SH_MAGIC);
	remove(self);
	return;
};

void() FireElemBeam =
{
	local entity b;
	 
	if(!CheckMana(self,3))
		return;
		
	sound(self, CHAN_WEAPON, "hknight/hit.wav", 1, ATTN_NORM);
  
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
	self.attack_finished = time + 0.15;
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
  T_RadiusDamage (self, self.owner, 180, self.owner, SH_FIRE);
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
	if((((self.ltime < time) || (self.enemy.health < 1)) || (pointcontents(self.enemy.origin)==CONTENT_WATER)) || (self.enemy.invincible_finished > time))
	{
		remove(self);
		return;
	}
	
	//Stay on top of enemy
	self.origin = self.enemy.origin;
	
	//Burn every 4 frames (0.8s)
	if(self.frame == 1)
		DoDamage(self.enemy, self, self.owner, 4, SH_FIRE);
		
	self.frame = self.frame + 1;
	if(self.frame > 4)
		self.frame = 1;
		
	self.nextthink = time + 0.2;
	self.think = BlazeHurt;
};

void() BlazeTouch = 
{
	//Doesn't get hurt
	if(!(other.takedamage == DAMAGE_AIM))
		return;
		
	self.solid = SOLID_NOT;
	self.touch = SUB_Null;
	self.enemy = other;
	self.movetype = MOVETYPE_NOCLIP;
	self.velocity = '0 0 0';	
	setmodel(self,"progs/s_explod.spr");
	self.think = BlazeHurt;
	self.nextthink = time + 0.2;
	self.ltime = time + 10;
	
	if(self.enemy.classname == "player")
		cprint(self.enemy,"You have been ignited!\n");
};

void() FirestormExplode =
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

	if(!CheckMana(self, 60))
		return;
		
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
	l.touch = FirestormExplode;
	setsize(l,'-1.3 -1.3 -1.3','1.3 1.3 1.3');
	l.think = SUB_Remove;
	l.nextthink = time + 5;
	l.velocity = v_forward * 800;
	l.angles = vectoangles(l.velocity);
	l.angles_x = l.angles_x + 90;
	self.attack_finished = time + 1.0;
};

void() Meltdown_Explode =
{
	//Lava splash
	WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
	WriteByte (MSG_BROADCAST, TE_LAVASPLASH);

	WriteCoord (MSG_BROADCAST, self.origin_x);
	WriteCoord (MSG_BROADCAST, self.origin_y);
	WriteCoord (MSG_BROADCAST, self.origin_z);
	#ifdef QUAKEWORLD
	multicast (self.origin, MULTICAST_PHS);
	#endif
	
	//Radius of 350
	T_AreaDamage(self, self.owner, 270, 450, world, SH_PLASMA);

	remove(self);
};

void() FireElemMeltdown =
{
	local entity meltdown;
	
	if(!CheckMana(self, 220))
		return;
	
	meltdown = spawn();
	setmodel(meltdown, "progs/s_explod.spr");
	setorigin(meltdown, self.origin + '0 0 16');
	setsize(meltdown,'0 0 0','0 0 0');
	meltdown.owner = self;
	meltdown.netname = "meltdown";
	meltdown.frame = 0;
	meltdown.effects = EF_BRIGHTLIGHT | EF_BRIGHTFIELD;
	meltdown.solid = SOLID_NOT;
	meltdown.touch = SUB_Null;
	meltdown.think = Meltdown_Explode;
	meltdown.nextthink = time + 4.0;
	sound(meltdown, CHAN_AUTO, "boss1/out1.wav", 1, ATTN_NORM);
};

//=============================================================================
//The functions below are the only ones called by magic.c
//=============================================================================

// Spell casting: by selection.
void() FireElemSpellName =
{
	sprint(self,"FireElem: ");
	if(self.spell == FIREELEM_SPELL_STASIS)
		sprint(self,"Stasis Field\n");
	else if(self.spell == FIREELEM_SPELL_BEAM)
		sprint(self,"Beam\n");
	else if(self.spell == FIREELEM_SPELL_FIRESTORM)
		sprint(self,"Firestorm\n");
	else if(self.spell == FIREELEM_SPELL_INFERNO)
		sprint(self,"Inferno\n");
	else
		sprint(self,"Meltdown\n");
};

void() FireElemCast =
{
	if(pointcontents(self.origin) == CONTENT_WATER)
	{
		sprint(self,"You can't cast in water!\n");
		self.attack_finished = time + 0.3;
		return;
	}

	if(self.spell == FIREELEM_SPELL_STASIS)
		FireElemStasisField();
	else if(self.spell == FIREELEM_SPELL_BEAM)
		FireElemBeam();
	else if(self.spell == FIREELEM_SPELL_FIRESTORM)
		FireElemFirestorm();
	else if(self.spell == FIREELEM_SPELL_INFERNO)
		FireElemInferno();
	else if(self.spell == FIREELEM_SPELL_MELTDOWN)
		FireElemMeltdown();
};
