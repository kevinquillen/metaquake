/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/lich.c, Common/Magic/lich.qc  *
 * Copyright (C) 1999-2000 by Lon Hohberger.                             *
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

float(entity targ) infront;

void() litFork =
{
	local entity head,found;
	local float dist,dtmp;
	dist = 10000;

	head = findradius(self.origin,400);

	while(head)
	{
		if ((head.takedamage == DAMAGE_AIM) && (head.health > 0))
		{
			if (visible(head))
			{
				dtmp = fabs(vlen(self.origin - head.origin));
				if (dtmp < dist)
				{
					// 4/11 chance not to take closest
					if (random()*10 < 8)
						dist = dtmp;
					found = head;
				}
			}
		}
		head = head.chain;
	}
	
	if (found)		
	{
		sound(self,CHAN_AUTO,"weapons/lstart.wav",1,ATTN_NORM);
		WriteByte(MSG_MULTICAST,SVC_TEMPENTITY);
		WriteByte(MSG_MULTICAST,TE_LIGHTNING1);
		WriteEntity(MSG_MULTICAST,self);
		WriteCoord(MSG_MULTICAST,self.origin_x);
		WriteCoord(MSG_MULTICAST,self.origin_y);
		WriteCoord(MSG_MULTICAST,self.origin_z);
		WriteCoord(MSG_MULTICAST,found.origin_x);
		WriteCoord(MSG_MULTICAST,found.origin_y);
		WriteCoord(MSG_MULTICAST,found.origin_z);
#ifdef QUAKEWORLD
		multicast(self.origin,MULTICAST_PHS);
#endif
		if (found == self.owner)
		  DoDamage(found,self,self.owner,10+random()*50,SH_ELECTRICITY);
		else
		  DoDamage(found,self,self.owner,50+random()*140,SH_ELECTRICITY);
		if (random()*10 <= 3)
		{
			self.origin = found.origin;
			self.nextthink = time + 0.1;
			self.think = litFork;
			return;
		} else {
			remove(self);
			return;
		}
	}
		
	// Not reached
};

void() LichForked =
{
	local vector dir,org;
	local entity e;

	if (self.ammo_cells < 50)
	{
		sprint(self,"Insufficient Mana!\n");
		return;
	}

	DoDamage(self,world,self,3+random()*4,SH_ELECTRICITY);
	self.ammo_cells = self.ammo_cells - 50;
	dir = aim(self,10000);
	org = self.origin + '0 0 16';
	traceline(org, org + dir * 2048,FALSE,self);

	sound (self, CHAN_WEAPON, "weapons/lstart.wav", 1, ATTN_NORM);
	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  	WriteByte (MSG_MULTICAST, TE_LIGHTNING1);
  	WriteEntity (MSG_MULTICAST, self);
  	WriteCoord (MSG_MULTICAST, org_x);
  	WriteCoord (MSG_MULTICAST, org_y);
  	WriteCoord (MSG_MULTICAST, org_z); 
	WriteCoord (MSG_MULTICAST, trace_endpos_x); 
	WriteCoord (MSG_MULTICAST, trace_endpos_y); 
	WriteCoord (MSG_MULTICAST, trace_endpos_z); 
#ifdef QUAKEWORLD 
	multicast (self.origin, MULTICAST_PHS);
#endif
	// Spawn first fork -- can "deflect" off person hit.
	// Inherently evil.
	if (trace_fraction != 1.0)
	{
		if (trace_ent.takedamage == DAMAGE_AIM)
		{
			e = spawn();
			e.origin = trace_ent.origin;
			e.owner = self;
			e.netname = "bifurcating lightning";
			e.think = litFork;
			e.nextthink = time + 0.1;
		}
	}
	self.attack_finished = time + 0.6;
}; 


void() sprayTouch =
{
	if (other == self.owner)
		return;
	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte (MSG_MULTICAST, TE_WIZSPIKE);
	WriteCoord (MSG_MULTICAST, self.origin_x);
	WriteCoord (MSG_MULTICAST, self.origin_y);
	WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
	multicast(self.origin,MULTICAST_PVS);
#endif    
	if ((other.takedamage == DAMAGE_AIM) && (other.health>0))
	{
		DoDamage(other,self,self.owner,12+random()*26,SH_SLIME);
	}
	remove(self);
	return;
};


void() randomMove =
{
	makevectors(self.spawn_origin);
	self.velocity = v_forward * 800 + v_up * (random() * 50 - 25) + v_right * (random() * 50 - 25);
	self.angles = vectoangles(self.velocity);
	self.nextthink = time + 0.2;
	self.think = randomMove;
};

void() LichAcidSpray =
{
	local entity e;
	local vector o;
	
	if(!CheckMana(self, 4))
		return;
	
	local entity e;
	e = spawn();
	e.owner = self;
	e.movetype = MOVETYPE_FLYMISSILE;
	e.solid = SOLID_BBOX;
	setmodel(e,"progs/k_spike.mdl");
	
	o = self.origin + '0 0 16';
	makevectors(self.v_angle);
	e.origin = o + v_forward * 16;
	setsize(e,'0 0 0','0 0 0');
	e.velocity = v_forward * 1000;
	e.spawn_origin = self.v_angle;
	e.angles = vectoangles(e.velocity);
	e.think = randomMove;
	e.nextthink = time + 0.1;
	e.touch = sprayTouch;
	e.netname = "acid spray";
	self.attack_finished = time + 0.1;

};

void() T_AcidBlobTouch =
{
	//Don't hit caster
	if(other == self.owner)
		return;

	//Don't explode on other projectiles (e.g. option rocket)
	if(other.classname == "missile")
		return;

	if (self.voided)
		return;

	self.voided = 1;
	sound(self,CHAN_AUTO,"blob/death1.wav",1,ATTN_NORM);
	
	WriteByte(MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte(MSG_MULTICAST, TE_TAREXPLOSION);
	WriteCoord(MSG_MULTICAST, self.origin_x);
	WriteCoord(MSG_MULTICAST, self.origin_y);
	WriteCoord(MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
	multicast (self.origin, MULTICAST_PHS);
#endif
	
	if (other == world)
		T_RadiusDamage(self,self.owner,120,self.owner,SH_SLIME);
	else
	 	DoDamage(other,self,self.owner,80+random()*40,SH_SLIME);
	
	remove(self);
};

void() LichAcidBlob =
{
	local entity e;
	local vector dir;
	if (self.ammo_cells < 30)
	{
		sprint(self,"Insufficient mana!\n");
		return;
	}
	
	self.ammo_cells = self.ammo_cells - 30;
	DoDamage(self,world,self,3+random()*4,SH_SLIME);
	sound(self,CHAN_WEAPON,"blob/hit1.wav",1,ATTN_NORM);
	e = spawn();
	e.owner = self;
	makevectors(self.v_angle);
	e.origin = self.origin + '0 0 32' + v_forward * 16;
	
	e.movetype = MOVETYPE_BOUNCE;
	e.solid = SOLID_BBOX;
	dir = aim(self,10000);
	e.touch = T_AcidBlobTouch;
	e.velocity = dir * 900 + v_up * 100;
	e.angles = self.angles;
	setsize(e,'0 0 0','0 0 0');
	setmodel(e,"progs/tarbaby.mdl");
	e.frame = 53;
	e.netname = "acid blob"; 
	self.attack_finished = time + 0.5;

};

void() LichDisenchant =
{
	local entity head;
	
	if(!CheckMana(self, 25))
		return;
	
	head = findradius(self.origin,800);
	while (head)
	{
		if (head.netname == "chromatic fireball")
			remove(head);
		if (head.netname == "firebringer")
			remove(head);
		if (((head.netname == "fire storm") || (head.netname == "tracker")) && (head.enemy == self))
			remove(head);
		if (head.classname == "player")
		{
			if (head.invincible_finished > (time + 3))
			{
				head.invincible_finished = time + 3;
			}
			if (head.super_damage_finished > (time + 3))
			{
				head.super_damage_finished = time + 1;
			}
			if(head.playerclass == CL_CLERIC)
			{
				cprint(head,"Your aura fades...\n");
				head.player_flags = head.player_flags - (head.player_flags & PF_SANCTUARY);
				head.max_ammo_cells = 250;
			}
			
			#if 0 //Not safe..rune of mana can mess this up
			if(head.playerclass == CL_MAGE)
			{
				cprint(head,"You feel less tough.\n");
				head.max_ammo_cells = 250;
				head.max_health = 90;
			}
			#endif
		}
		
		head = head.chain;
	}
	self.attack_finished = time + 0.3;
};

void() LichSilence =
{
	if (self.ammo_cells < 100)
	{
		sprint(self,"Insufficient mana!\n");
		return;
	}
	
	self.ammo_cells = self.ammo_cells - 100;
	
	DoDamage(self,world,self,5+random()*12,SH_UNKNOWN);
	
	self.attack_finished = time + 1;
	if (self.enemy == world)
		return;
	
	if (self.enemy.classname != "player")
		return;
	
	if ((self.enemy.playerclass == CL_DRUID) || (self.enemy.playerclass == CL_MAGE) || (self.enemy.playerclass == CL_CLERIC))
	{
		sprint(self,self.enemy.netname);
		sprint(self," has been SILENCED!\n");
		self.enemy.ammo_cells = 0;
		cprint(self.enemy,"You have been SILENCED!\n");
	} else {
		sprint(self,"Casting failed!\n");
	}
	self.enemy = world;

	
};

void(float resists) LichResist =
{
	if (resists == self.saves)
	{
		cprint(self,"You already have these resistances!\n");
		self.attack_finished = time + 0.2;
		return;
	}
	if(!CheckMana(self, 60))
		return;
		
	cprint(self,"Resistances changed!\n");
	
	self.saves = resists;
	self.harms = 0;
	self.attack_finished = time + 0.5;
};



void() LanceHit =
{
	if (other == world)
	{
		remove(self);
		return;
	}
	
	if (other == self.owner)
		return;
	
	DoDamage(other,self,self.owner,100+random()*250,SH_COLD);
	
	remove(self);
};

void() RemoveCheck =
{
	if (other == self.owner)
		return;
	remove(self);
};

void() spawnLanceTrail =
{
	local entity trail;
	self.health = self.health - 1;
	if (self.health <= 0)
	{
		self.think = SUB_Remove;
		self.nextthink = time + 5;
		return;
	}

	trail = spawn();
	trail.solid = SOLID_TRIGGER;
	trail.movetype = MOVETYPE_FLYMISSILE;
	trail.owner = self.owner;
#ifdef QUAKEWORLD
	trail.effects = EF_BLUE;
#endif
	trail.velocity = self.velocity;
	trail.origin = self.spawn_origin;
	trail.touch = RemoveCheck;
	setmodel(trail,"progs/s_bubble.spr");
	setsize(trail,'0 0 0','0 0 0');
	self.think = spawnLanceTrail;
	self.nextthink = time + 0.1;
};

void() LichIceLance = 
{
	local entity e;
	local vector dir;
	
	if(!CheckMana(self, 25))
		return;
	
	DoDamage(self,world,self,2+random()*4,SH_COLD);
	sound(self,CHAN_AUTO,"doors/ddoor2.wav",1,ATTN_NORM);
	e = spawn();
	e.solid = SOLID_BBOX;
	e.movetype = MOVETYPE_FLYMISSILE;
	setmodel(e,"progs/s_spike.mdl");
	dir = aim(self,10000);
	makevectors(self.v_angle);
	setsize(e,'0 0 0','0 0 0');
	e.origin = self.origin + '0 0 16' + v_forward * 8;
#ifdef QUAKEWORLD
	e.effects = EF_BLUE;
#endif
	e.think = spawnLanceTrail;
	e.nextthink = time + 0.1;
	e.spawn_origin = e.origin;
	e.health = 5;
	e.owner = self;
	e.velocity = dir * 2000;
	e.angles = vectoangles(e.velocity);
	e.netname = "ice lance";
	self.attack_finished = time + 0.5;
	e.touch = LanceHit;
};


void() FlameStrikeHit =
{
	if (other == self.owner)
		return;
	
	if (other == world)
		remove(self);
	if (other.takedamage == DAMAGE_AIM)
	{
		if ((random() * 9) < 1)
		{
			DoDamage(other,self,self.owner,150+random()*1000,SH_FIRE);
		} else {
			DoDamage(other,self,self.owner,100+random()*100,SH_FIRE);
		}
	}
	remove(self);
};

void() FlameStrikeThink =
{
	if (infront(self.enemy))
	{
		self.velocity = normalize((self.enemy.origin+'0 0 16') 
				- self.origin) * 900;
		self.angles = vectoangles(self.velocity);
		//self.angles_x = self.angles_x + 90;
	}
	self.nextthink = time + 0.1;
};

void() LichFlameStrike =
{
	local entity e;
	local vector org,dir;
	
	if (self.ammo_cells < 35)
	{
		sprint(self,"Insufficient mana!\n");
		return;
	}

	self.ammo_cells = self.ammo_cells - 35;
	DoDamage(self,self,self,4+random()*5,SH_FIRE);
	
	makevectors(self.v_angle);
	
	org = self.origin + '0 0 16' + v_forward * 16;
	dir = aim(self,10000);
	traceline(org,org + dir*1500,FALSE,self);
	
	
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

	if (trace_fraction != 1.0)
	{
		if (trace_ent.takedamage)
		{
			sound(self,CHAN_WEAPON,"misc/power.wav",1,ATTN_NORM);
			e = spawn();
			e.solid = SOLID_BBOX;
			e.takedamage = DAMAGE_AIM;
			e.health = 2;
			e.th_die = SUB_Remove;
			e.movetype = MOVETYPE_FLYMISSILE;
			e.velocity = dir * 900;
			e.owner = self;
			e.netname = "flame strike";
			e.origin = org;
			e.enemy = trace_ent;
			e.think = FlameStrikeThink;
#ifdef QUAKEWORLD
			e.effects = EF_RED;
#else
			e.effects = EF_DIMLIGHT | EF_BRIGHTFIELD;
#endif
			e.nextthink = time + 0.2;
			setmodel(e,"progs/lavaball.mdl");
			e.angles = vectoangles(e.velocity);
			e.touch = FlameStrikeHit;
			setsize(e,'0 0 0','0 0 0');
		}
	}
	
	self.attack_finished = time + 0.5;
};

void() DarkThink =
{
	if ((self.health >= 13) || (self.owner.health <= 0))
	{
		lightstyle(0,"m");
		remove(self);
		return;
	}
	
	if (self.health == 1)
		lightstyle(0,"b");
	if (self.health == 2)
		lightstyle(0,"c");
	if (self.health == 3)
		lightstyle(0,"d");
	if (self.health == 4)
		lightstyle(0,"e");
	if (self.health == 5)
		lightstyle(0,"f");
	if (self.health == 6)
		lightstyle(0,"g");
	if (self.health == 7)
		lightstyle(0,"h");
	if (self.health == 8)
		lightstyle(0,"i");
	if (self.health == 9)
		lightstyle(0,"j");
	if (self.health == 10)
		lightstyle(0,"k");
	if (self.health == 11)
		lightstyle(0,"l");
	
	self.health = self.health + 1;
	self.think = DarkThink;
	self.nextthink = time + 0.2;
};

void() LichDarkness =
{
	local entity e;
	
	if(self.ammo_cells < 70)
	{
		sprint(self,"Insufficient mana!\n");
		return;
	}
	
	e = find(world,classname,"magic_dark");
	if(e != world)
	{
		sprint(self, "A Darkness spell already in effect!\n");
		return;
	}
	
	self.ammo_cells = self.ammo_cells - 70;
	DoDamage(self,self,self,20+random()*15,SH_MAGIC);
	self.attack_finished = time + 0.8;
	if(random()*100 <= 25) //25% chance of spell failure
	{
		sprint(self,"Your magic fizzles harmlessly into the air.\n");
		return;
	}
	else
	{
		bprint(self.netname);
		bprint(" floods the area with darkness.\n");
		e = spawn();
		e.think = DarkThink;
		e.nextthink = time + 3;
		e.owner = self;
		e.classname = "magic_dark";
		lightstyle(0,"a");
	}
};

			

void() LichSpellName =
{
	sprint(self,"Lich: ");
	if (self.spell == 0)
		sprint(self,"Acid Spray");
	if (self.spell == 1)
		sprint(self,"Forked Lightning");
	if (self.spell == 2)
		sprint(self,"Acid Blob");
	if (self.spell == 3)
		sprint(self,"Ice Lance");
	if (self.spell == 4)
		sprint(self,"Flame Strike");
	if (self.spell == 5)
		sprint(self,"Disenchant");
	if (self.spell == 6)
		sprint(self,"Silence");
	if (self.spell == 7)
		sprint(self,"Resist Fire");
	if (self.spell == 8)
		sprint(self,"Resist Natural");
	if (self.spell == 9)
		sprint(self,"Resist Mortal");
	if (self.spell == 10)
		sprint(self,"Resist Magical");
	if (self.spell == 11)
		sprint(self,"Darkness");
	sprint(self,"\n");
};

void() LichCast =
{
	if (self.spell == 1)
		LichForked();
	if (self.spell == 0)
		LichAcidSpray();
	if (self.spell == 2)
		LichAcidBlob();
	if (self.spell == 3)
		LichIceLance();
	if (self.spell == 4)
		LichFlameStrike();
	if (self.spell == 5)
		LichDisenchant();
	if (self.spell == 6)
		LichSilence();
	if (self.spell == 7) //resist fire
		LichResist(SH_LAVA|SH_FIRE|SH_INCINERATE|SH_PLASMA);
	if (self.spell == 8) //resist natural
		LichResist(SH_LAVA|SH_SLIME|SH_WATER|SH_FALL);
	if (self.spell == 9) //resist mortal
		LichResist(SH_BULLETS|SH_UNKNOWN|SH_ELECTRICITY);
	if (self.spell == 10) //resist magical
		LichResist(SH_ELECTRICITY|SH_PLASMA|SH_MAGIC|SH_CRITHIT);
	if (self.spell == 11)
		LichDarkness();

};


