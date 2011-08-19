/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/mage.c, Common/Magic/mage.qc  *
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

void (vector org) spawn_tfog;
entity() SelectSpawnPoint;
float(float radius, string classtype, float max_no) CheckCount;

// Spell: Fireball, Type: Directed
/*
void() T_Burn =
{
  self.attack_finished = time + 0.5;
  if (self.attack_finished > time)
    return;

  if (other.takedamage==DAMAGE_AIM)// && (other != self.owner))
  {
    DoDamage(other,self,self.owner,3,SH_FIRE);
  }

};
*/

void() BecomeFire =
{
//  self.touch = T_Burn;
//  setsize (self,'-2 -2 -2','2 2 2');
//  self.solid = SOLID_TRIGGER;
  self.angles = '0 0 0';
  self.velocity = '0 0 0';
  self.origin_z = self.origin_z + 2;
  setmodel(self,"progs/flame2.mdl");
  self.think = SUB_Remove;
  self.nextthink = time + 0.5;
};  

void() T_FireTouch =
{
	local float damg;

	//Don't explode on the owner
	if(other == self.owner)
		return;

	if(self.voided)
		return;

	self.voided = 1;

 	self.movetype = MOVETYPE_NONE;

	if(pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}

	//TODO: wtf?
	if(self.netname == "eruption")
		self.owner = self.owner.owner;

	//TODO: why is this needed??
  if (self.health == 0)                   // damage according to type
    damg = 1 + random()*30;

  DoDamage(other, self, self.owner, damg, SH_FIRE);

  //TODO: why is self.armorvalue used for damage type instead of SH_FIRE?
  sound(self,CHAN_VOICE,"weapons/r_exp3.wav",1,ATTN_NORM);
  T_RadiusDamage (self, self.owner, 70, self.owner, self.armorvalue);

  BecomeFire ();
};

void() MageFire =
{
	local entity fireball;
  
	if(!CheckMana(self, 15))
		return;

	sound(self, CHAN_VOICE, "items/itembk2.wav", 1, ATTN_NORM);

	self.punchangle_x = -2;
	
	fireball = spawn ();
	fireball.netname="fireball";
	fireball.voided = 0;
	fireball.owner = self;
	fireball.movetype = MOVETYPE_FLYMISSILE;
	fireball.solid = SOLID_BBOX;
	fireball.classname = "magic_fire";
	fireball.velocity = aim(self, 900) * 900;
	fireball.touch = T_FireTouch;
	fireball.nextthink = time + 5;
	fireball.think = SUB_Remove;
	fireball.armorvalue = SH_FIRE;
	setmodel (fireball, "progs/lavaball.mdl");
	setsize(fireball,'-1 -1 -1','1 1 1');
	setorigin (fireball, self.origin + v_forward*8 + '0 0 16');
	self.attack_finished = time + 0.3;
};

void() MageCalm =
{
	if(!CheckMana(self, 40))
		return;
	
	sound(self, CHAN_AUTO, "enforcer/enfire.wav", 1, ATTN_NORM);

	//50% chance to resist, base
	if(CheckResist(self.enemy, 0.50))
	{
		ResistMsg(self.enemy, self);
		return;
	}
		
	if(self.enemy.takedamage == DAMAGE_AIM)
	{
		//Don't freeze your ally's weapons...
		if(teamplay && SameTeam(self, self.enemy))
			return;
			
		if(self.enemy.classname == "player")
		{
			cprint(self.enemy,"Your weapons freeze!\n");
			#ifndef QUAKEWORLD
				SpawnSurround(self.enemy, 40, 45); 
			#endif
			stuffcmd(self.enemy,"-attack\n");
		}
		sprint(self,self.enemy.netname);
		sprint(self," calmed down\n");

		if(self.enemy.harms & SH_MAGIC)
			self.enemy.attack_finished = time + 3;
		else
			self.enemy.attack_finished = time + 2;

		self.enemy = world;
	}
	self.attack_finished = time + 0.5;
};

// Spell: Invisibility, Type: Caster or Directed
void() T_InvisTouch =
{
  if (other == self.owner)
    return;
  if (other.player_flags & PF_DEMON)
    return;
  if ((other.classname == "player") && (other.invisible_finished < time))
  {
    cprint(other,"Mmmmm shadowy goodness\n");
    other.items = other.items | IT_INVISIBILITY;
    other.invisible_time = 1;
    other.invisible_finished = time + self.health;
    remove(self);
    return;
  } else {
    self.velocity = '0 0 0';
  }
};

void(float duration, float consume) DoInvis =
{
  local entity invisobj;
  if (self.ammo_cells < consume)
  {
    sprint(self,"Insufficient mana!\n");
    return;
  }
  self.ammo_cells = self.ammo_cells - consume;

  sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
  if (self.tb_mode == 0)
  {
    cprint(self,"You are invisible!\n");
    self.items = self.items | IT_INVISIBILITY;
    self.invisible_time = 1;
    self.invisible_finished = time + duration;
  } else {
    invisobj = spawn();
    invisobj.owner = self;
    invisobj.movetype = MOVETYPE_FLYMISSILE;
    invisobj.solid = SOLID_TRIGGER;
    setmodel(invisobj,"progs/invisibl.mdl"); 
    setsize (invisobj, '-16 -16 -24', '16 16 32');
    setorigin (invisobj, self.origin + v_forward*64);
    makevectors (self.v_angle);
    invisobj.velocity = aim(self, 400);
    invisobj.velocity = invisobj.velocity * 400;
    invisobj.angles = vectoangles(invisobj.velocity);
    invisobj.touch = T_InvisTouch;
    invisobj.think = SUB_Remove;
    invisobj.health = duration;
    invisobj.nextthink = time + duration;
  }
  self.attack_finished = time + 1.8;
};

void() MageInvis =
{
  DoInvis(20,75);
};


// Spell: Quad Damage, Type: Caster or Directed
void() T_QuadTouch =
{
  if (other == self.owner)
    return;
  if (other.takedamage == DAMAGE_AIM)
  {
    other.super_damage_finished = time + 15;
    other.effects = other.effects | EF_DIMLIGHT;
    cprint(other,"You feel blue.\n");
    other.items = other.items | IT_QUAD;
    other.super_time = 1;
    remove(self);
    return;
  } else {
    self.velocity = '0 0 0';
  }    
};

void() MageQuad =
{
	local entity quadobj;
 
	if(!CheckMana(self, 230))
		return;

  sound (self, CHAN_AUTO, "items/damage.wav", 1, ATTN_NORM);
  if (self.tb_mode == 0)
  {
    self.effects = self.effects | EF_DIMLIGHT;
    cprint(self,"You feel blue.\n");
    self.items = self.items | IT_QUAD;
    self.super_time = 1;
    self.super_damage_finished = time + 15;
  } else {
    quadobj = spawn();
    quadobj.owner = self;	// mage has to cast on self.
    quadobj.movetype = MOVETYPE_FLYMISSILE;
    quadobj.solid = SOLID_TRIGGER;
#ifdef QUAKEWORLD
    quadobj.effects = EF_BLUE;
#else
    quadobj.effects = EF_DIMLIGHT;
#endif
    setmodel(quadobj,"progs/quaddama.mdl"); 
    setsize (quadobj, '0 0 0', '0 0 0');     
    setsize (quadobj, '-16 -16 -24', '16 16 32');
    setorigin (quadobj, self.origin + v_forward*64);
    makevectors (self.v_angle);
    quadobj.velocity = aim(self, 400);
    quadobj.velocity = quadobj.velocity * 400;
    quadobj.touch = T_QuadTouch;
    quadobj.think = SUB_Remove;
    quadobj.nextthink = time + 20;
  }
  self.attack_finished = time + 2;
};

// Spell: Gate, Type: Caster
void() MageGate =
{
	local entity spot;

#ifdef GAME_CTF
	if(self.player_flags & PF_HAS_FLAG)
	{
		cprint(self,"You must drop the flag first!\n");
		return;
	}
#endif

	if(!CheckMana(self, 20))
		return;


  spawn_tfog (self.origin);
  self.teleport_time = time + 2;
  v_forward = '1333 0 0';
  spot = SelectSpawnPoint();
  cprint(self,"POOF!\n");
  self.origin = spot.origin + '0 0 1';
  self.angles = spot.angles;
  spawn_tfog (self.origin);
  self.attack_finished = time + 1.5;
};

// Spell: Greater Fireball, Type: fired--area (BFG for mage, sorta)
void() CFBDie =
{
  if (self.voided)
    return;

  self.voided = 1;
  self.owner.effects = self.owner.effects - (self.owner.effects & EF_BRIGHTLIGHT);

  sound (self, CHAN_WEAPON, "weapons/r_exp3.wav", 1, ATTN_NORM);

  T_PlasmaDamage(200,20+random()*20,world); 

  BecomeExplosion();
};


void() T_CFB =
{
  if (self.voided)
    return;

  self.voided = 1;

  sound (self, CHAN_WEAPON, "weapons/r_exp3.wav", 1, ATTN_NORM);
  //TODO: wtf 5K damage? 
  if(other.takedamage == DAMAGE_AIM)
  {
    DoDamage(other,self,self.owner,5000,SH_PLASMA);
  }
  
  T_PlasmaDamage(300,100+random()*20,world); 
    

  BecomeExplosion();
  remove(self);
};

void() CFBTrail =
{
  local vector v;
  local float sc;
  if ((((self.enemy == world) || (!self.enemy)) || (self.enemy.netname != "chromatic fireball")) || (self.id_number != self.enemy.id_number))
  {
    bprint("Trailer removed\n");
    remove(self);
    return;
  }
  makevectors(self.enemy.angles);
  v = self.enemy.origin + (self.health)*v_right*(self.player_flags);
  sc = vlen(v - self.origin)*(self.player_flags);
  self.velocity = normalize(v - self.origin) * sc;
  self.angles = vectoangles(self.velocity);
  self.angles_x = self.angles_x + 90;
  self.think = CFBTrail;
  self.nextthink = time + 0.1;  // minor change
};


void() SuperFireball =
{
  local entity m1, m2;

  self.movetype = MOVETYPE_FLYMISSILE;
  self.owner.effects = self.owner.effects - (self.owner.effects & EF_BRIGHTLIGHT);

  makevectors(self.owner.v_angle);

  sound (self, CHAN_WEAPON, "weapons/spike2.wav", 1, ATTN_NORM);
  self.effects = EF_BRIGHTLIGHT;
  self.netname = "chromatic fireball";
  makevectors(self.owner.v_angle);
  self.angles = vectoangles(v_forward);
  self.avelocity = '200 200 -200';
  self.velocity = v_forward * 650;
  self.th_die = CFBDie;
  self.health = 5;
  self.id_number = random()*5000;
  self.max_health = 5;
  self.touch = T_CFB;
  self.think = SUB_Remove;
  self.nextthink = time + 5;

  m1 = spawn();
  m2 = spawn();
  m1.id_number = m2.id_number = self.id_number;
  m1.player_flags = m2.player_flags = 15;
  setmodel(m1,"progs/flame2.mdl");
  setmodel(m2,"progs/flame2.mdl");
  m1.movetype = m2.movetype = MOVETYPE_FLYMISSILE;
  m1.solid = m2.solid = SOLID_BBOX;
  m1.enemy = m2.enemy = self;
  m1.owner = self.owner;
  m2.owner = self.owner;

  setsize(m1,'0 0 0','0 0 0');
  setsize(m2,'0 0 0','0 0 0');
  m1.origin = self.origin + '0 0 16' + v_right * 6;
  m2.origin = self.origin + '0 0 16' + v_right * -6;

  m1.velocity = v_forward * 400;
  m2.velocity = v_forward * 400;
  m1.angles = vectoangles(m1.velocity) + '90 0 0';
  m2.angles = vectoangles(m2.velocity) + '90 0 0';

  m1.think = CFBTrail;
  m2.think = CFBTrail;
  m1.nextthink = m2.nextthink = time + 0.1;
  m1.health = 1;
  m2.health = -1;
  m1.touch = BecomeFire;
  m2.touch = BecomeFire;

  m1.ltime = m2.ltime = time;
};

void() MageSuperFire =
{
	local entity cast;
	if(!CheckMana(self, 70))
		return;
		
	sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
  
	cast = spawn();
	cast.solid = SOLID_BBOX;
	cast.movetype = MOVETYPE_TOSS;
	cast.touch = CFBDie;
	cast.origin = self.origin + '0 0 16';
	makevectors (self.v_angle);
	cast.owner = self;
	setmodel(cast,"progs/lavaball.mdl");
	setsize(cast,'0 0 0','0 0 0');
	self.effects = self.effects | EF_BRIGHTLIGHT;
	cast.velocity = v_forward * 400 + '0 0 200';
	cast.nextthink = time + 0.5;
	cast.think = SuperFireball;
	self.attack_finished = time + 1.6;
	cast.netname = "baby fireball";
};



// Mage spell: Polymorph Self (dog)
void() spell_become_doggie;
void() MageDoggie =
{
	if(!CheckMana(self,10))
		return;
		
	spell_become_doggie();
};

void() MageVitality =
{
	if(!CheckMana(self, 75))
		return;
	
	//TODO: doesn't affect rune of mana
	if(self.max_ammo_cells < 250)
	{
		sprint(self,"Insufficient mana (or already cast)!\n");
		return;
	}
	
	sound (self, CHAN_AUTO, "enforcer/enfire.wav", 1, ATTN_NORM);

	//While active, maximum mana is reduced but maximum health in increased
	self.max_ammo_cells = self.max_ammo_cells - 75;
	self.health = self.health + 50;
	self.max_health = self.max_health + 50;
	
	cprint(self,"Your health increased!");

	self.attack_finished = time + 3;
};
 
void() MageSwarm =
{
	local entity fireball;
	if(!CheckMana(self, 15))
		return;
		
  fireball = spawn ();
  fireball.netname = "meteor swarm";
  fireball.voided = 0;
  fireball.owner = self;
  fireball.movetype = MOVETYPE_BOUNCE;
  fireball.solid = SOLID_BBOX;
  fireball.classname = "magic_fire";
  makevectors(self.v_angle);
  fireball.velocity = v_forward * 600 + v_up * 100;
  fireball.velocity_x = fireball.velocity_x + (random()*300 - 150);  
  fireball.velocity_y = fireball.velocity_y + (random()*300 - 150);  
  fireball.touch = T_FireTouch;
  fireball.nextthink = time + 5;
  fireball.think = SUB_Remove;
  fireball.armorvalue = SH_UNKNOWN;
  setmodel (fireball, "progs/lavaball.mdl");
  setsize(fireball,'-1 -1 -1','1 1 1');
  setorigin (fireball, self.origin + v_forward*8 + '0 0 16');
  self.attack_finished = time + 0.2;

};

//Called when the mage's confusion projection explodes
void() ConfusionTouch =
{
	local entity head;
	local entity conf;
	
	//Ignore caster
	if(self.owner == other)
		return;
	
	//Lava splash
	WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
	WriteByte (MSG_BROADCAST, TE_LAVASPLASH);

	WriteCoord (MSG_BROADCAST, self.origin_x);
	WriteCoord (MSG_BROADCAST, self.origin_y);
	WriteCoord (MSG_BROADCAST, self.origin_z);
	#ifdef QUAKEWORLD
	multicast (self.origin, MULTICAST_PHS);
	#endif

	
	head = findradius(self.origin, 250);
	while(head)
	{
	
		if(head.health >= 1 && head.classname == "player")
		{
			if(
			!(head == self.owner) &&
			(!teamplay  || (teamplay && !SameTeam(self.owner,head)))
			)
			{
				if(!CheckResist(head, 0.20)) //20% chance to resist by default
				{
					Magic_AddEffect(head, self.owner, SPELLFX_CONFUSION, 3.5); 
					//Give player concusion
					stuffcmd(conf.enemy, "v_idlescale 140\n");
				}
				else //Show a resist message
				{
					ResistMsg(head, self.owner);
				}
				
			}
		}
    
		head = head.chain;
	}

	self.owner.effects = self.owner.effects - EF_BRIGHTLIGHT;
	remove(self);
};

void() MageConfusion =
{
	local entity cast;
	
	if(!CheckMana(self, 100))
		return;

	sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
  
	cast = spawn();
	cast.solid = SOLID_BBOX;
	cast.movetype = MOVETYPE_FLYMISSILE;
	cast.touch = ConfusionTouch;
	cast.origin = self.origin + '0 0 16';
	cast.owner = self;
	cast.velocity = v_forward * 400 + '0 0 30';
	//makevectors(self.v_angle);
	//cast.angles = vectoangles(v_forward);
	//cast.velocity = v_forward * 650;

	cast.nextthink = time + 4.0;
	cast.think = ConfusionTouch;
	
	self.attack_finished = time + 1.6;
	self.effects = self.effects | EF_BRIGHTLIGHT;

	makevectors(self.v_angle);
	setmodel(cast,"progs/s_light.spr");
	setsize(cast,'0 0 0','0 0 0');

};


// Spell casting: by selection.
void() MageSpellName =
{
  sprint(self,"Mage: ");
  if (self.spell == 0)
    sprint(self,"Fireball\n");
  if (self.spell == 1)
    sprint(self,"Freeze\n");
  if (self.spell == 2)
    sprint(self,"Invisibility\n");
  if (self.spell == 3)
    sprint(self,"Quad Damage\n");
  if (self.spell == 4)
    sprint(self,"Gate Travel\n");
  if (self.spell == 5)
    sprint(self,"Chromatic Fireball\n");
  if (self.spell == 6)
    sprint(self,"Polymorph Self (Dog)\n");
  if (self.spell == 7)
    sprint(self,"Vitality\n");
  if (self.spell == 8)
    sprint(self,"Meteor Swarm\n");
	if(self.spell == MAGE_SPELL_CONFUSION)
		sprint(self, "Confusion\n");
};

void() MageCast =
{
  if(self.spell == 0)
    MageFire();
  if (self.spell == 1)
    MageCalm();
  if (self.spell == 2)
    MageInvis();
  if (self.spell == 3)
    MageQuad();
  if (self.spell == 4)
    MageGate();
  if (self.spell == 5)
    MageSuperFire();
  if (self.spell == 6)
    MageDoggie();
  if (self.spell == 7)
    MageVitality();
  if (self.spell == 8)
    MageSwarm();
	if(self.spell == MAGE_SPELL_CONFUSION)
		MageConfusion();
};	
