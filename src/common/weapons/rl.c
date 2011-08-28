/*
 * Common/Weapons/rl.c, Common/Weapons/rl.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 *   Portions Copyright 2011 by Patrick Baggett
 */

/*
	OVERVIEW
	--------
	This file handles the rocket launcher code. The only function directly referenced is
	W_FireRL(), which is used in weapons.c.
	
	W_FireRocket() handles the logic of creating a projectile of the right type (depends on
	the shooter's "rl_mode" field) and with the right velocity etc.
	
	The main function for doing damage is T_MissileTouch(). All rocket launcher projectiles
	share this same code. Rockets copy the shooter's "rl_mode" field when fired, so this
	allows T_MissileTouch() to determine what type of explosion to have.
*/


#include "meta.h"
#include "weapons.h"
void(entity inflictor, entity attacker, float damage, float radius, entity ignore, float damagetype) T_AreaDamage;
void() player_rocket1;
void() muzzleflash;

/*
================
T_MissileTouch
================

When a rocket of any type touches an object, this is called. Note that
when the rocket was fired, it copied the object's 'rl_mode' member, so
the type of rocket that is being called can be determined by 'self.rl_mode'.
*/
void() T_MissileTouch =
{
	local float damg;

	//Don't explode on owner, unless it is a shalrath ball with no other targets
	//that turned on the shooter.
	//if((other == self.owner) && (self.enemy != self.owner))
	//	return;
	if(other.classname == "rocket")
		return;
	bprint("T_MissileTouch()\n");
	//Only explode once on touch
	if (self.voided)
		return;
	self.voided = 1;

	if(pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}

	//Calculate some damage
	if(self.rl_mode == RL_NORMAL)
		damg = 85 + random()*25;
	else if(self.rl_mode == RL_SHALRATH)
		damg = 40 + random()*15;
	else if(self.rl_mode == RL_DUMBFIRE)
		damg = 40 + random()*25;
	
	//Directly to damage to whatever is touched (except shalrath ball)
	if(other.health > 0 && self.rl_mode != RL_SHALRATH) 
	{
		other.deathtype = "rocket";//TODO: what is this for?
		DoDamage (other, self, self.owner, damg, SH_EXPLOSION);
	}

	//Create an explosion radius.	
	//Note that 'other' (param #4) is who to ignore when damaging. This is so that
	//the player doesn't take direct hit damange AND radius damage -- which would
	//effectively double the damage dealt to the victim. Of course, this only
	//holds true for the normal/dumbfire rockets -- the shalrath ball uses explosion
	//only damage.
	if(self.rl_mode == RL_NORMAL)
	{
		T_RadiusDamage(self, self.owner, damg, other, SH_EXPLOSION);
	}
	else if(self.rl_mode == RL_DUMBFIRE)
	{
		T_AreaDamage(self, self.owner, damg, 100, other, SH_EXPLOSION);
		
	}
	else //Shalrath ball does magic damage
		T_RadiusDamage(self, self.owner, damg, world, SH_MAGIC);

	//The explosion entity shows up 8 units backwards from the direction it was
	//traveling so it doesn't appear inside of the target.
	self.origin = self.origin - 8 * normalize(self.velocity);

	WriteByte(MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte(MSG_MULTICAST, TE_EXPLOSION);
	WriteCoord(MSG_MULTICAST, self.origin_x);
	WriteCoord(MSG_MULTICAST, self.origin_y);
	WriteCoord(MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
  	  multicast (self.origin, MULTICAST_PHS);
#endif

	BecomeExplosion();
};

void() ShalHome =
{	
	if(self.enemy == self.owner || self.enemy.takedamage != DAMAGE_AIM)
	{
		self.enemy = self.owner;
		self.owner = world;
	}
  
  if (self.ltime < time - 3.5)
  {
     self.think = SUB_Null;
     self.movetype = MOVETYPE_BOUNCE;
     self.velocity = self.velocity + '0 0 100';
     return;
  }
  if (self.enemy == world)
  {
    self.think = SUB_Null; 
    return;
  }
  if (self.enemy.takedamage != DAMAGE_AIM)
    return;

  if (self.enemy.health < 1)
  {
    self.enemy = world;
    return;
  }
  self.velocity = normalize((self.enemy.origin + '0 0 10') - self.origin) * 325;

  self.nextthink = time + 0.1;
  self.think = ShalHome; 
};

/*
================
W_FireRocket
================

This handles the code for firing any/all of the rocket modes.
*/
void() W_FireRocket =
{
	if (self.classname == "player")
	{
		if(self.playerclass != CL_HWEAP)
			player_rocket1();
		else
			player_hw_att1();
	}
	self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
	
	sound (self, CHAN_WEAPON, "weapons/sgun1.wav", 1, ATTN_NORM);

#ifdef QUAKEWORLD
	if (self.classname == "player")
	{
		msg_entity = self;
		WriteByte (MSG_ONE, SVC_SMALLKICK);
	}
#else
	self.punchangle_x = -2;
#endif

	//Spawn a rocket
	newmis = spawn();
	newmis.owner = self;
	newmis.movetype = MOVETYPE_FLYMISSILE;
	newmis.solid = SOLID_BBOX;
	newmis.effects = 0;
	newmis.rl_mode = self.rl_mode;
	newmis.classname = "rocket";
	newmis.touch = T_MissileTouch;
	newmis.voided = 0;

	makevectors(self.v_angle);

	//Autoaim disabled -> use player's forward vector        
	if (self.option_flags & OF_NO_MISSILEAIM)
		newmis.velocity = v_forward * 1000;
	else
	{
		newmis.velocity = aim(self, 1000);
		newmis.velocity = newmis.velocity * 1000;
	}
	newmis.angles = vectoangles(newmis.velocity);

	
		setsize(newmis, '0 0 0', '0 0 0');
	setorigin(newmis, self.origin + v_forward*8 + '0 0 16');

	
	if(self.rl_mode == RL_NORMAL) //Normal rockets
	{
		newmis.netname = "rocket";
		setmodel(newmis, "progs/missile.mdl");
		self.attack_finished = time + 1.0;
		newmis.nextthink = time + 5;
		newmis.think = SUB_Remove;
	}
	else if(self.rl_mode == RL_SHALRATH) //Shalrath ball
	{
		newmis.netname = "Shalrath Ball";
		newmis.velocity = v_forward * 320;
		self.attack_finished = time + 0.8;
		setmodel(newmis, "progs/v_spike.mdl");
		newmis.think = ShalHome;
		newmis.nextthink = time + 0.6; //Fly forwards a bit before homing
		newmis.ltime = time + 3.5;
		newmis.enemy = self.enemy;
		newmis.avelocity = '300 300 300';
	}
	else if(self.rl_mode == RL_DUMBFIRE) //Dumbfire rockets
	{
		newmis.netname = "missile";
		setmodel(newmis, "progs/missile.mdl");
		
		//Randomly move to the left/right of the player, then move forward.		
		
		newmis.origin = newmis.origin + (v_right * (random()*30 - 15));
		newmis.origin = newmis.origin + (v_up * (random()*30 - 15));		

		self.attack_finished = time + 0.3;
	}
	
	
};


void() W_FireRL =
{
	if (self.playerclass == CL_SPECIAL)
		return;
	
	if(self.rl_mode < NUM_RL_MODES)
		W_FireRocket();
};
