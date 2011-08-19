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

#include "meta.h"
#include "magic.h"

void()muzzleflash;
void()SUB_regen;
float (entity targ) visible;
/*
  DRUID class
*/
float(float radius, string classtype, float max_no) CheckCount;


void() LitFX =
{
  local vector v;

  if (self.owner.waterlevel)
  {
    remove(self); return;
  }
  if (self.health)
  {
    v_x = random()*1000 - 500;
    v_y = random()*1000 - 500;
    v_z = 1000;
    self.health = self.health - 1;
    traceline(self.owner.origin + '0 0 16', self.owner.origin + v, FALSE, self);
    WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
    WriteByte (MSG_MULTICAST, TE_LIGHTNING1);
    WriteEntity (MSG_MULTICAST, self);
    WriteCoord (MSG_MULTICAST, self.owner.origin_x);
    WriteCoord (MSG_MULTICAST, self.owner.origin_y);
    WriteCoord (MSG_MULTICAST, self.owner.origin_z);
    WriteCoord (MSG_MULTICAST, trace_endpos_x);
    WriteCoord (MSG_MULTICAST, trace_endpos_y);
    WriteCoord (MSG_MULTICAST, trace_endpos_z);
#ifdef QUAKEWORLD
    multicast (self.origin, MULTICAST_PHS);
#endif
    self.think = LitFX;
    self.nextthink = time + 0.1;
    return;
  }

  sound (self, CHAN_AUTO, "weapons/lstart.wav", 1, ATTN_NORM);
  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_LIGHTNING3);
  WriteEntity (MSG_MULTICAST, self.owner);
  WriteCoord (MSG_MULTICAST, self.owner.origin_x);
  WriteCoord (MSG_MULTICAST, self.owner.origin_y);
  WriteCoord (MSG_MULTICAST, self.owner.origin_z);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_x);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_y);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif

  if (self.owner.health <= 0)
    DoDamage(self.enemy,self,self.owner, 40+random()*15, SH_ELECTRICITY);
  else
    DoDamage(self.enemy,self,self.owner, 85+random()*20, SH_ELECTRICITY);
 
  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_TAREXPLOSION);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_x);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_y);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_z);
#ifdef QUAKEWORLD
  multicast (self.enemy.origin, MULTICAST_PHS);
#endif
  
  remove(self);
};

void() DruidCall =
{
	local entity cl;
	if(!CheckMana(self, 35))
		return;

  muzzleflash();
  self.attack_finished = time + 0.6; // main offense

  makevectors(self.v_angle);
  traceline(self.origin + '0 0 16', (self.origin + '0 0 16') + v_forward * 1200, FALSE, self);

  if ((trace_fraction != 1.0) && (trace_ent.takedamage == DAMAGE_AIM))
  {
    cl = spawn();
    cl.netname = "call to Zeus";
    cl.owner = self; 
    cl.enemy = trace_ent;
    cl.health = 4;
    cl.think = LitFX;
    cl.nextthink = time;
    sound (self, CHAN_WEAPON, "misc/power.wav", 1, ATTN_NORM);
    return;
  }
  if (self.classname == "player")
  sprint(self,"Missed!\n");
};

void() DruidDMagic =
{
  local entity head;

  if (self.ammo_cells < 50)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }

  self.ammo_cells = self.ammo_cells - 50;

  head = findradius(self.origin, 700);

  while (head)
  {
    // added to make mages not take mana drain from them.
    if (head.classname == "magic_erupt")
    {
	    DoDamage(head.owner,head,head.owner,20+random()*40,SH_PLASMA);
	    remove(head);
    }
   
    if (((head.player_flags & PF_MAGIC) && (!(head.saves & SH_MAGIC))) && (head != self))
      head.ammo_cells = floor(head.ammo_cells/2);
    if (head.invisible_finished > time)
      head.invisible_finished = time + 0.1;
    if (head.super_damage_finished > time)
      head.super_damage_finished = time + 0.1;
    if ((head.classname == "player") && (head.player_flags & PF_MAGIC))
      cprint(head,"You feel a tremor in the mystical forces!\n");

    if ((head.classname == "item_artifact_super_damage") ||
        (head.classname == "item_artifact_invisibility") ||
        (head.classname == "item_artifact_invulnerability"))
    {
      head.nextthink = time + 45;
      head.model = string_null;
      head.solid = SOLID_NOT;
      head.think = SUB_regen;
    }

   head = head.chain;
  }
  self.attack_finished = time + 0.5;
};

// fdsa
void() spell_become_demon;
void() DruidDemon =
{
  if (self.ammo_cells < 20)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }
  if (self.player_flags & PF_EYES_ONLY)
    return;

  sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
  self.ammo_cells = self.ammo_cells - 20;
  spell_become_demon();
};

/*
void() BHThink =
{
  local entity head;
  self.health = self.health - 1;
  if (self.health <= 0)
  {
    sound(self,CHAN_VOICE,"weapons/r_exp3.wav",1,ATTN_NORM);
    T_RadiusDamage (self, self.owner, 30, self.owner, SH_MAGIC);
    WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
    WriteByte (MSG_MULTICAST, TE_EXPLOSION);
    WriteCoord (MSG_MULTICAST, self.origin_x);
    WriteCoord (MSG_MULTICAST, self.origin_y);
    WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
    multicast (self.origin, MULTICAST_PHS);
#endif
    BecomeExplosion ();
    return;
  }

  head = findradius(self.origin, 650);
  while (head) {
    if (((head != self) && (head != self.owner)) &&
        ((head.solid == SOLID_BBOX) || (head.solid == SOLID_SLIDEBOX)))
    {
      if ((head.movetype != MOVETYPE_NONE) && (head.movetype!=MOVETYPE_NOCLIP))
        if ((head.invincible_finished < time) && (!(head.player_flags & PF_SPEED)))
          head.velocity = normalize(self.origin - head.origin) * (620 - 2*head.weight);
//                 (1000 - vlen(self.origin - head.origin));
    }
    head = head.chain;
  }

  self.nextthink = time + 0.1;
  self.think = BHThink;
};  

void() DruidSuck =
{
  local entity s;

  if (self.ammo_cells < 160)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }

  self.ammo_cells = self.ammo_cells - 160;

  s = spawn();
  s.owner = self;
  s.origin = self.origin;

  SpawnSurround(s, 50, 0);
  
  sound(s,CHAN_VOICE,"ambience/wind2.wav",1,ATTN_NONE);
  setmodel(s,"progs/h_player.mdl");
  setsize(s,'-16 -16 -16','16 16 16');
  s.angles = '0 0 0';
  s.avelocity = '250 250 250';
  s.solid = SOLID_BBOX;
  s.health = 40;		// Time in 1/10 sec
  s.nextthink = time + 0.1;
  s.think = BHThink; 
  self.attack_finished = time + 4.1;
};
*/
void() T_EruptTouch =
{
  local float     damg;
  if (other == self.owner)
    return;         // don't explode on owner
  if (pointcontents(self.origin) == CONTENT_SKY)
  {
    remove(self);
    return;
  }

  sound(self,CHAN_VOICE,"weapons/r_exp3.wav",1,ATTN_NORM);
  T_RadiusDamage (self, self.owner, 110, self.owner, SH_FIRE);
  BecomeExplosion ();
};



void() Erupt =
{
  local entity ball;
  if ((self.owner.deadflag) || (self.owner.id_number != self.id_number))
  {
	  remove(self);
	  return;
  }
  if (self.ltime < (time - 20))
  {
    cprint(self.owner,"The eruption subsides.\n");
    remove(self);
    return;
  }

  ball=spawn();
  ball.movetype=MOVETYPE_BOUNCE;
  ball.solid=SOLID_BBOX;
  ball.touch=T_EruptTouch;
  setmodel(ball,"progs/lavaball.mdl");
  setsize(ball,'0 0 0','0 0 0');
  ball.origin = self.origin;
  ball.velocity_x = random()*900 - 450;
  ball.velocity_y = random()*900 - 450;
  ball.netname = "eruption";
  ball.avelocity = '300 300 300';
  ball.velocity_z = random()*300 + 400;
  ball.owner = self.owner;
  self.nextthink = time + 0.3;
  self.think = Erupt;
};
void() DruidErupt =
{
	local entity e;
	if(self.watertype != CONTENT_LAVA)
	{
		sprint(self, "You must be in lava to cast this!");
		self.attack_finished = time + 0.2;
		return;
	}

	if(!CheckMana(self, 180))
		return;

  if (!CheckCount(2200,"magic_erupt",1))
  {
    cprint(self,"The Volcano God ignores your call\n");
    return;
  }

  cprint(self,"You summon the Volcano God\n");

  e = spawn();
  e.owner = self;
  e.origin = self.origin;
  e.id_number = self.id_number;
  e.ltime = time;
  e.think = Erupt;
  e.nextthink = time + 0.4;
  e.classname = "magic_erupt";
  self.attack_finished = time + 10;
};

void() DruidWind =
{
  local entity head;

  if (self.ammo_cells < 5)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }

  self.ammo_cells = self.ammo_cells - 5;
  head = findradius(self.origin,500);
  while (head)
  {
    if (((visible(head)) && (head.movetype != MOVETYPE_NONE)) && (head != self.owner))
    {
      if ((head.movetype != MOVETYPE_TOSS) && (head != self) && (!(head.saves & SH_MAGIC)))
      {
	head.player_flags = head.player_flags - (head.player_flags & PF_SPEED);
        head.velocity = normalize(head.origin - self.origin) * (1200 - head.weight);
        head.velocity_z = head.velocity_z + 100;
      }
    }
    head = head.chain;
  }
  
};

void() PsiStorm =
{
  self.origin_z = self.spawn_origin_z + (random()*260 - 130);
  self.origin_x = self.spawn_origin_x + (random()*260 - 130);
  self.origin_y = self.spawn_origin_y + (random()*260 - 130);
  self.enemy.origin_z = self.spawn_origin_z + (random()*260 - 130);
  self.enemy.origin_x = self.spawn_origin_x + (random()*260 - 130);
  self.enemy.origin_y = self.spawn_origin_y + (random()*260 - 130);
  T_RadiusDamage (self, self.owner, 70, world, SH_ELECTRICITY);
  T_RadiusDamage (self.enemy, self.owner, 70, world, SH_ELECTRICITY);
  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_LIGHTNING2);
  WriteEntity (MSG_MULTICAST, self);
  WriteCoord (MSG_MULTICAST, self.origin_x);
  WriteCoord (MSG_MULTICAST, self.origin_y);
  WriteCoord (MSG_MULTICAST, self.origin_z);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_x);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_y);
  WriteCoord (MSG_MULTICAST, self.enemy.origin_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif
  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_LIGHTNING2);
  WriteEntity (MSG_MULTICAST, self.enemy);
  WriteCoord (MSG_MULTICAST, self.origin_x);
  WriteCoord (MSG_MULTICAST, self.origin_y);
  WriteCoord (MSG_MULTICAST, self.origin_z);
  WriteCoord (MSG_MULTICAST, self.spawn_origin_x);
  WriteCoord (MSG_MULTICAST, self.spawn_origin_y);
  WriteCoord (MSG_MULTICAST, self.spawn_origin_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif

  self.health = self.health - 1;
  if (self.health <= 0)
  {
    remove(self.enemy);
    remove(self);
    return;
  }
  self.think = PsiStorm;
  self.nextthink = time + 0.1;
};

void() PS_Ready =
{
  local entity ps;
  ps = spawn();
  ps.netname = "PsiStorm";
  self.enemy = ps;
  ps.owner = self.owner;
  ps.spawn_origin = self.spawn_origin;
  self.velocity = '0 0 0';
  self.avelocity = '500 500 500';
  sound (self, CHAN_WEAPON, "misc/power.wav", 1, ATTN_NORM);
  self.spawn_origin = self.origin;
  self.think = PsiStorm;
  self.nextthink = time;
};

void() DruidPsionicStorm =
{
  local entity ps;

  if (self.ammo_cells < 85)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }

  self.ammo_cells = self.ammo_cells - 85;
  
  ps = spawn();
  ps.owner = self;
  ps.netname = "PsiStorm";
  ps.think = PS_Ready;
  ps.nextthink = time + 0.4;
  ps.solid = SOLID_NOT;
  ps.effects = EF_BRIGHTLIGHT;
  ps.movetype = MOVETYPE_NOCLIP;
  setmodel(ps,"progs/bolt.mdl");
  setsize(ps,'0 0 0','0 0 0');
  ps.origin = self.origin + '0 0 32';
  makevectors(self.v_angle);
  ps.velocity = v_forward * 1000;
  ps.angles = vectoangles (ps.velocity);
  ps.health = 20;
  self.attack_finished = time + 1.0;
};


void() DruidRegen =
{
	local entity spelltarget;
	
		
	if(self.tb_mode == CASTMODE_SELF)
	{
		if(!CheckMana(self, 75))
			return;
			
		spelltarget = self;
	}
	else
	{
		//Ensure beneficial spells follow teamplay rules
		if(!CheckHelpfulSpellTarget())
			return;

		if(!CheckMana(self, 75))
			return;
	
		spelltarget = self.enemy;
		
		//Let caster know who the spell was cast on
		sprint(self, "Regenerating: ");
		sprint(self, spelltarget.netname);
		sprint(self, "\n");
		
	}
		
	sound(self, CHAN_AUTO, "enforcer/enfire.wav", 1, ATTN_NORM);
	if(spelltarget.classname == "player")
		cprint(spelltarget, "Your wounds start to mend.");

	//Regenerate for 15 seconds
	Magic_AddEffect(spelltarget, self, SPELLFX_REGEN, 15);
	self.attack_finished = time + 1.0;
};

// Spell casting: by selection.
void() DruidSpellName =
{
	sprint(self,"Druid: ");
	if(self.spell == 0)
		sprint(self,"Call Lightning");
	else if(self.spell == 1)
		sprint(self,"Dispel Magic");
	else if(self.spell == 2)
		sprint(self,"Metamorphisis");
	else if(self.spell == 3)
		sprint(self,"Psionic Storm");
	else if(self.spell == 4)
		sprint(self,"Erupt");
	else if(self.spell == 5)
		sprint(self,"Sacred Wind");
	else if(self.spell == 6)
		sprint(self,"Combine");
	else if(self.spell == 7)
		sprint(self,"Regeneration");
	sprint(self,"\n");
};

void() DruidCast =
{
	if(self.spell == 0)
		DruidCall();
	if(self.spell == 1)
		DruidDMagic();
	if(self.spell == 2)
		DruidDemon();
	if(self.spell == 3)
		DruidPsionicStorm();
	if(self.spell == 4)
		DruidErupt();
	if(self.spell == 5)
		DruidWind();
	if(self.spell == 6)
		DoubleAux();
	if(self.spell == 7)
		DruidRegen();
};
