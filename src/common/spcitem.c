/*************************************************************************
 * Metamorphism Quake Server: Common/spcitem.c, Common/spcitem.qc        *
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

entity() SelectSpawnPoint;

void() NewSpot;
void() T_RuneTouch;
void() OptionPickUp;

#define RUNE_HEALTH	"Rune of Health"
#define RUNE_ARMOR	"Rune of Armor"
#define RUNE_MANA	"Rune of Mana"
#define RUNE_OPT	"Option"
#define RUNE_ELEC	"Ion Shield"

//How much health/armor/mana the runes give
#define RUNE_HEALTH_AMOUNT	40
#define RUNE_ARMOR_AMOUNT	50
#define RUNE_MANA_AMOUNT	200

//How many frames before regenerating a point (larger implies slower regen)
#define RUNE_HEALTH_SPEED	3
#define RUNE_ARMOR_SPEED	2
#define RUNE_MANA_SPEED		1

void() TakeMe =
{
	if (self.classname == "special_health")
		self.touch = T_RuneTouch;
	else if(self.classname == "special_armor")
		self.touch = T_RuneTouch;
	else if(self.classname == "special_option")
		self.touch = OptionPickUp;
	else if(self.classname == "special_elec")
		self.touch = T_RuneTouch;
	else if(self.classname == "special_mana")
		self.touch = T_RuneTouch;
		
	self.think = NewSpot;
	self.nextthink = time + 59;
};

/*
==============
ThrowSpecial
==============
Deletes the given special item, and recreates it
somewhere else in the level.
*/
void() ThrowSpecial =
{
	local entity new_obj;

	new_obj = spawn();
	new_obj.effects = EF_DIMLIGHT;
	new_obj.movetype = MOVETYPE_TOSS;
	new_obj.solid = SOLID_TRIGGER;
	new_obj.think = TakeMe;
	new_obj.nextthink = time + 1;
	new_obj.classname = self.classname;
	new_obj.netname = self.netname;
	new_obj.health = self.health;
	new_obj.max_health = 0;

	if(self.classname == "special_health")
		setmodel(new_obj,"progs/end2.mdl");
	else if(self.classname == "special_armor")
		setmodel(new_obj,"progs/end3.mdl");
	else if(self.classname == "special_elec")
		setmodel(new_obj,"progs/end4.mdl");
	else if(self.classname == "special_option")
		setmodel(new_obj, "progs/v_spike.mdl");
	else if(self.classname == "special_mana")
		setmodel(new_obj, "progs/end1.mdl");
	
		

	setsize(new_obj,'-8 -8 -8','8 8 8');
	new_obj.solid = SOLID_TRIGGER;
	new_obj.velocity_z = 600;
	new_obj.velocity_x = random()*2000 - 1000;
	new_obj.velocity_y = random()*2000 - 1000;
	setorigin(new_obj,self.origin);

	remove(self);
};

/*
=====
NewSpot
*/
void() NewSpot =
{
	local entity spot;
	spot = SelectSpawnPoint();
	self.flags = FL_ITEM;
	self.origin = spot.origin + '0 0 16';
	ThrowSpecial();
};


void(vector org, float damage) SpawnBlood;

// Option
void() T_OptionRocketTouch =
{
	local float     damg;

	if(other == self.owner && self.owner != world)
		return;         // don't explode on owner

	//Don't explode option rocket on something owned by the player either
	if(other.owner == self.owner && self.owner != world)
		return;
	
	if (self.voided) {
		return;
	}
	self.voided = 1;

	if (pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}

	T_RadiusDamage (self, self.owner, 65, self.owner, SH_EXPLOSION); // shalrath ball

	self.origin = self.origin - 8 * normalize(self.velocity);

	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte (MSG_MULTICAST, TE_EXPLOSION);
	WriteCoord (MSG_MULTICAST, self.origin_x);
	WriteCoord (MSG_MULTICAST, self.origin_y);
	WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
  	multicast (self.origin, MULTICAST_PHS);
#endif
	BecomeExplosion();
};

/*
==================
HasRune()
==================
Check if a player already has a rune. Basically, we search
for the runes, then check if the owner == me. We record which
rune it was and then send a message at the end saying that which
rune the player would have to drop.
*/
float(entity me) HasRune =
{
	local float hasrune;
	local string droprune;
	local entity head;
	
	droprune = "<none>";
	
	head = find(world,classname,"special_health");
	if(head != world && head.owner == me)
		droprune = "You must drop the Rune of Health first!";

	head = find(world,classname,"special_armor");
	if(head != world && head.owner == me)
		droprune = "You must drop the Rune of Armor first!";

	head = find(world,classname,"special_elec");
	if(head != world && head.owner == me)
		droprune = "You must drop the Ion Shield first!";
  
	head = find(world, classname, "special_mana");
	if(head != world && head.owner == me)
		droprune = "You must drop the Rune of Mana first!";
		
	head = find(world, classname, "special_option");
	if(head != world && head.owner == me)
		droprune = "You must drop the Option first!";

	//Player *does* have a rune
	if(droprune != "<none>")
	{
		cprint(me, droprune);
		hasrune = 1;
	}
	else
		hasrune = 0;
		
	return hasrune;
};

void() OptionThink;
void() OptionPickUp =
{
	local entity head;
	local string droprune;

	
	//Only players may pick up the option
	if(other.classname != "player")
		return;

	// todo: is this needed ?
	if(self.attack_finished > time)
		return;
	
	//Clerics cannot pickup the option
	if(other.playerclass == CL_CLERIC)
	{
		cprint(other, "Clerics may not touch artifacts!");
		return;
	}
	
	self.attack_finished = time + 0.2;

	if(HasRune(other))
		return;
 
	setmodel(self,"progs/invisibl.mdl");
	other.player_flags = other.player_flags | PF_HAS_SPECIAL;
	self.owner = other;
	self.effects = 0;
	self.solid = SOLID_NOT;
	self.movetype = MOVETYPE_NOCLIP;
	self.angles = '0 0 0';
	self.avelocity = '300 300 300';
	self.think = OptionThink;
	self.nextthink = time + 0.1;
	self.touch = SUB_Null;
	
	cprint(other,"You got the Option!");
};

void() OptionThink =
{
  local entity missile;
  local vector org;

  if (!(self.owner.player_flags & PF_HAS_SPECIAL))
  {
    self.origin = self.owner.origin + '0 0 16';
    self.owner = world;
    ThrowSpecial();
    self.think = SUB_Null;
    self.touch = OptionPickUp;
    return;
  }

  if (vlen(self.origin - (self.owner.origin + '0 0 16')) > 64)
    self.origin = self.owner.origin + '0 0 16';

  if (vlen(self.origin - (self.owner.origin + '0 0 16')) > 32)
    self.velocity = normalize((self.owner.origin + '0 0 16') - self.origin)
      * 32 + self.owner.velocity;

  if ((self.attack_finished < time) &&
     (self.owner.button0))
  {
    missile = spawn ();
    missile.owner = self.owner;
    missile.movetype = MOVETYPE_FLYMISSILE;
    missile.solid = SOLID_BBOX;

    setmodel (missile, "progs/missile.mdl");
    setsize(missile,'0 0 0','0 0 0');
    makevectors(self.owner.v_angle);
    missile.origin = self.origin + v_forward * 4;
    missile.velocity = v_forward * 1000;
    missile.angles = vectoangles(self.velocity);
    missile.classname = "missile";
    missile.angles = vectoangles(missile.velocity);
    missile.touch = T_OptionRocketTouch;
    missile.nextthink = time + 5;
    missile.think = SUB_Remove;
    missile.netname = "option rocket";
    self.attack_finished = time + 0.6;
  }        

  self.think = OptionThink;
  self.nextthink = time + 0.1;
};




// Rune of Health
void() T_RuneTouch;

void() RuneThink =
{
	//Check if:
	//a) owner is dead
	//b) owner changed
	//c) owner dropped special item ("droprune" unsets PF_HAS_SPECIAL)
	if ((self.owner.deadflag) || (self.owner.id_number != self.id_number) || (!(self.owner.player_flags & PF_HAS_SPECIAL)))
	{
		//Drop max health back down to the original value
		if (self.classname == "special_health")
		{
			if(self.owner.health > self.owner.max_health)
				self.owner.health = self.owner.max_health;
		}

		//Drop armor back down to original value
		if(self.classname == "special_armor")
		{
			self.owner.max_arm = self.owner.max_arm - self.health;
			
			//Don't let them keep excess armor
			if(self.owner.armorvalue > self.owner.max_arm)
				self.owner.armorvalue = self.owner.max_arm;
		}
		
		//Drop mana back down to original value
		if(self.classname == "special_mana")
		{
			self.owner.max_ammo_cells = self.owner.max_ammo_cells - self.health;
			
			//Don't let them keep excess mana
			if(self.owner.ammo_cells > self.owner.max_ammo_cells)
				self.owner.ammo_cells = self.owner.max_ammo_cells;
		}

		//Rethrow the item
		self.origin = self.owner.origin + '0 0 16';
		self.owner = world;
		self.touch = SUB_Null;
		self.nextthink = time;
		self.think = ThrowSpecial;
		return;
	}
	
	if ((self.classname == "special_health") && (self.owner.health < self.owner.max_health + self.health))
	{
		//Use max_health as a counter so that every few frames the owner gets health
		self.max_health = self.max_health + 1;
		if (self.max_health >= RUNE_HEALTH_SPEED)
			self.max_health = 0;
			
		if (self.max_health == 0)
			self.owner.health = self.owner.health + 1;
	}
  
	//Only regenerate 70% of the maximum armor. Note that the rune increased the maximum armor
	//value by RUNE_ARMOR_AMOUNT, so this is still quite a bit of armor
	if((self.classname == "special_armor") && (self.owner.armorvalue < floor(self.owner.max_arm * 0.7)))
	{
		//Owner doesn't have any armor?
		if(self.owner.armorvalue < 1)
		{
			//Give them yellow armor
			self.owner.items = self.owner.items - (self.owner.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3)) + IT_ARMOR2;
			self.owner.armortype = 0.6;
			self.owner.armorvalue = 1;
		}
		
		//Owner has green armor -> Upgrade to yellow
		if (self.armortype == 0.3)
		{
			self.owner.items = self.owner.items - (self.owner.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3)) + IT_ARMOR2;
			self.owner.armortype = 0.6;
			self.owner.armorvalue = floor(0.5 * self.owner.armorvalue);
		}

		//Use max_health as a counter so that every few frames the owner gets armor
		self.max_health = self.max_health + 1;
		if(self.max_health >= RUNE_ARMOR_SPEED)
			self.max_health = 0;
		if(self.max_health == 0)
			self.owner.armorvalue = self.owner.armorvalue + 1;

	}
	
	if(self.classname == "special_mana" && self.owner.ammo_cells < self.owner.max_ammo_cells + self.health)
	{
		//Use max_health as a counter so that every few frames the owner gets mana
		if(self.max_health >= RUNE_MANA_SPEED)
			self.max_health = 0;
			
		if(self.max_health == 0)
			self.owner.ammo_cells = self.owner.ammo_cells + 1;
	}

  self.nextthink = time + 0.1;
};

void() ElecThink =
{
	local vector org, endp;
	local entity shock;

	if ((self.owner.deadflag) || (!(self.owner.player_flags & PF_HAS_SPECIAL)))
	{
		self.origin = self.owner.origin + '0 0 16';
		self.owner = world;
		self.touch = SUB_Null;
		self.nextthink = time;
		self.think = ThrowSpecial;
		return;
	}

  org = self.origin = self.owner.origin + '0 0 16';
  shock = ClosestTarget(70);
  if (!shock)
  {
    self.nextthink = time + 0.1;
    self.think = ElecThink;
    return;
  }
    
	//Don't shock while waist-deep or more in water
	if (self.owner.waterlevel >= 2)
	{
		self.nextthink = time + 0.1;
		self.think = ElecThink;
		return;
	}

	endp = shock.origin + '0 0 16';
  
  WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
  WriteByte (MSG_BROADCAST, TE_LIGHTNING2);
  WriteEntity (MSG_BROADCAST, self.owner);
  WriteCoord (MSG_BROADCAST, org_x);
  WriteCoord (MSG_BROADCAST, org_y);
  WriteCoord (MSG_BROADCAST, org_z);
  WriteCoord (MSG_BROADCAST, endp_x);
  WriteCoord (MSG_BROADCAST, endp_y);
  WriteCoord (MSG_BROADCAST, endp_z);

  sound (self.owner, CHAN_BODY, "weapons/lhit.wav", 1, ATTN_NORM);
  DoDamage(shock,self,self.owner,6+random()*8,SH_ELECTRICITY);

  self.nextthink = time + 0.1;
  self.think = ElecThink;
};

void() T_RuneTouch =
{
	local entity head;
	local string msg;

	if(other.classname != "player")
		return;

	if(other.playerclass == CL_CLERIC)
	{
		cprint(other, "Clerics may not touch artifacts!");
		return;
	}
	
	if(self.attack_finished > time)
		return;
		
	self.attack_finished = time + 0.2;

	//Special check for Rune of Mana:
	//Player must be a spellcaster
	if (self.classname == "special_mana" && !(other.player_flags & PF_MAGIC))
	{
		cprint(other,"You must be a spellcaster to get the\n   Rune of Mana\n");
		return;
	}
	
	
	if(HasRune(other))
		return; 
 
	if(self.classname == "special_armor")
	{
		msg = "You got the Rune of Armor!";
		other.max_arm = other.max_arm + self.health;
	}
	else if(self.classname == "special_mana") 
		msg = "You got the Rune of Mana!";
	else if(self.classname == "special_health")
		msg = "You got the Rune of Health!";
	else if(self.classname == "special_elec")
		msg = "You got the Ion Shield!";
		 
	cprint(other, msg);
	other.player_flags = other.player_flags | PF_HAS_SPECIAL;
	self.effects = 0;
	setmodel(self,"");
	self.touch = SUB_Null;
	self.owner = other;
	self.nextthink = time;
	self.id_number = other.id_number;
	
	if(self.classname == "special_elec")
		self.think = ElecThink;
	else
		self.think = RuneThink;
};

/*
=====================
SpawnSpecials()
=====================
This creates the special objects on a level start.
These are sort of "template" entities -- the first
call to their think function (NewSpot()) will actually
create them, but they copy their properties.
*/
void() SpawnSpecials =
{
	local entity rh, ra, re, rm, ro;


	
	//Rune of Health
	rh = spawn();
	rh.classname = "special_health";
	rh.netname = RUNE_HEALTH;
	rh.health = RUNE_HEALTH_AMOUNT;	
	rh.think = NewSpot;
	rh.nextthink = time + 1;

	//Rune of Armor
	ra = spawn();
	ra.classname = "special_armor";
	ra.netname = RUNE_ARMOR;
	ra.health = RUNE_ARMOR_AMOUNT;	
	ra.think = NewSpot;
	ra.nextthink = time + 2;

	//Ion Shield
	re = spawn();
	re.classname = "special_elec";
	re.netname = RUNE_ELEC;
	re.think = NewSpot;
	re.nextthink = time + 3;
  
	//Rune of Mana
	rm = spawn();
	rm.classname = "special_mana";
	rm.netname = RUNE_MANA;
	rm.health = RUNE_MANA_AMOUNT;
	rm.think = NewSpot;
	rm.nextthink = time + 4;

	//Option
	ro = spawn();
	ro.classname = "special_option";
	ro.netname = RUNE_OPT;
	ro.think = NewSpot;
	ro.nextthink = time + 5;
};

/*
  Raiden Devices for the Shock Trooper!
*/
void() RaidenThink =
{
	if ((self.owner.deadflag) || (self.id_number != self.owner.id_number) || (self.owner.help))
	{
		BecomeExplosion();
		return;
	}

  if ((self.attack_finished <= time) && (!self.weapon))
  {
    if (self.axe_mode)
      cprint(self.owner,"Raiden Devices charged!");
    
    self.effects = EF_DIMLIGHT;
#ifdef QUAKEWORLD
    if (self.axe_mode)
      self.effects = self.effects | EF_BLUE;
    else
      self.effects = self.effects | EF_RED;
#endif
    self.weapon = 1;
    self.health = 30;
  }

  makevectors(self.owner.v_angle);

  if (self.health)
    if (self.axe_mode)
      self.origin = self.owner.origin + v_up * 20 + v_right * 20;
    else
      self.origin = self.owner.origin + v_up * 20 + v_right * -20;
  else
  {
    makevectors(self.owner.angles);
    if (self.axe_mode)
      self.origin = self.owner.origin + v_up * 12 + v_right * 8 + v_forward * -12;
    else
      self.origin = self.owner.origin + v_up * 12 + v_right * -8 + v_forward * -12;
  }

  if (self.health)
    self.angles = self.owner.v_angle;
  else 
    self.angles = '-90 0 0';
  self.angles_x = -1 * self.angles_x;
#ifdef QUAKEWORLD
  self.velocity = self.owner.velocity;
#endif

  if (  (((self.owner.button0 &&
           (self.owner.weapon == IT_AXE)
          )
          &&
          (!self.owner.axe_mode)
         ) 
        ||
         (self.ammo_cells)
        )
       
     && self.health
     && (self.ltime < time))
  {
    self.ammo_cells = 1;
    self.ltime = time + 0.1;
    self.attack_finished = time + 75;
    self.weapon = 0;

    if (self.super_sound < time)
    {
      self.super_sound = time + 1;
      sound (self, CHAN_BODY, "boss2/pop2.wav", 1, ATTN_NORM);
    }

    if ((self.axe_mode) && (self.health == 30))
      sound(self.owner,CHAN_WEAPON,"boss2/pop2.wav",1,ATTN_NORM);

    self.health = self.health - 1;
    if (self.health < 0)
      self.health = 0;
    
    if (!self.owner.waterlevel)
   {
    traceline (self.origin, self.owner.origin + v_forward*250, TRUE, self.owner);

    WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
    WriteByte (MSG_BROADCAST, TE_LIGHTNING2);
    WriteEntity (MSG_BROADCAST, self);
    WriteCoord (MSG_BROADCAST, self.origin_x);
    WriteCoord (MSG_BROADCAST, self.origin_y);
    WriteCoord (MSG_BROADCAST, self.origin_z);
    WriteCoord (MSG_BROADCAST, trace_endpos_x);
    WriteCoord (MSG_BROADCAST, trace_endpos_y);
    WriteCoord (MSG_BROADCAST, trace_endpos_z);

    LightningDamage(self.origin, trace_endpos, self, 100+random()*20);
   }
  }

  if (self.health == 0)
  {
    self.ammo_cells = 0;
    self.effects = 0;
  }

  self.nextthink = time + 0.01;
  self.think = RaidenThink;
};

void() SpawnRaidens =
{
  local entity r1, r2;

  r1 = spawn();
  r2 = spawn();
  r1.netname = "Call of Raiden";
  r2.netname = "Call of Raiden";
  r1.owner = r2.owner = self;
  r1.movetype = r2.movetype = MOVETYPE_NOCLIP;
  r1.solid = r2.solid = SOLID_TRIGGER;
  r1.id_number = r2.id_number = self.id_number;

  r1.attack_finished = time + 60;
  r2.attack_finished = time + 60;

  r1.axe_mode = 1;

  setmodel(r1,"progs/grenade.mdl");
  setmodel(r2,"progs/grenade.mdl");

  r1.think = RaidenThink;
  r2.think = RaidenThink;
  r1.nextthink = time;
  r2.nextthink = time;

};
