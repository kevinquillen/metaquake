/*
 * Common/Weapons/tb.c, Common/Weapons/tb.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

 #include "meta.h"

// Thunderbolt attacks
float() W_BestWeapon;
void() W_SetCurrentAmmo;
void(float radius, float d_p, entity ignore) T_PlasmaDamage;
void() player_light1;
void() player_shot1;

void() E_CastSpell;
float (entity e) visible;

void(vector org, vector dir) LaunchLaser;
/*
===============================================================================
LIGHTNING
===============================================================================
*/

void(entity from, float damage) LightningHit =
{
#ifdef QUAKEWORLD
	  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	  WriteByte (MSG_MULTICAST, TE_LIGHTNINGBLOOD);
	  WriteCoord (MSG_MULTICAST, trace_endpos_x);
	  WriteCoord (MSG_MULTICAST, trace_endpos_y);
	  WriteCoord (MSG_MULTICAST, trace_endpos_z);
	  multicast (trace_endpos, MULTICAST_PVS);
#else
	  particle (trace_endpos, '0 0 100', 225, damage*4);
#endif
	DoDamage (trace_ent, from, from, damage, SH_ELECTRICITY);
};

/*
=================
LightningDamage
=================
*/
void(vector p1, vector p2, entity from, float damage) LightningDamage =
{
	local entity            e1, e2;
	local vector            f;
	
	f = p2 - p1;
	normalize (f);
	f_x = 0 - f_y;
	f_y = f_x;
	f_z = 0;
	f = f*16;

	e1 = e2 = world;

	traceline (p1, p2, FALSE, self);

	if (trace_ent.takedamage)
	{
		LightningHit (from, damage);
		if (self.classname == "player")
		{
			if (other.classname == "player")
				trace_ent.velocity_z = trace_ent.velocity_z + 400;
		}
	}
	e1 = trace_ent;

	traceline (p1 + f, p2 + f, FALSE, self);
	if (trace_ent != e1 && trace_ent.takedamage)
	{
		LightningHit (from, damage);
	}
	e2 = trace_ent;

	traceline (p1 - f, p2 - f, FALSE, self);
	if (trace_ent != e1 && trace_ent != e2 && trace_ent.takedamage)
	{
		LightningHit (from, damage);
	}
};


void() W_FireLightning =
{
	local   vector          org;
	local   float           cells;

	if (self.ammo_cells < 1)
	{
		self.weapon = W_BestWeapon ();
		W_SetCurrentAmmo ();
		return;
	}

// explode if under water
	if (self.waterlevel > 1)
	{
		if (deathmatch > 3)
		{
			if (random() <= 0.5)
			{
				self.deathtype = "selfwater";
				DoDamage (self, self, self.owner, 4000, SH_ELECTRICITY );
			}
			else
			{
				cells = self.ammo_cells;
				self.ammo_cells = 0;
				W_SetCurrentAmmo ();
				T_RadiusDamage (self, self, 35*cells, world, SH_ELECTRICITY);
				return;
			}
		}
		else
		{
			cells = self.ammo_cells;
			self.ammo_cells = 0;
			W_SetCurrentAmmo ();
			T_RadiusDamage (self, self, 35*cells, world, SH_ELECTRICITY);
			return;
		}
	}

	if (self.t_width < time)
	{
		sound (self, CHAN_WEAPON, "weapons/lhit.wav", 1, ATTN_NORM);
		self.t_width = time + 0.6;
	}
#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
        self.punchangle_x = -2;
#endif

	self.currentammo = self.ammo_cells = self.ammo_cells - 1;

	org = self.origin + '0 0 16';
	
	traceline (org, org + v_forward*600, TRUE, self);

	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte (MSG_MULTICAST, TE_LIGHTNING2);
	WriteEntity (MSG_MULTICAST, self);
	WriteCoord (MSG_MULTICAST, org_x);
	WriteCoord (MSG_MULTICAST, org_y);
	WriteCoord (MSG_MULTICAST, org_z);
	WriteCoord (MSG_MULTICAST, trace_endpos_x);
	WriteCoord (MSG_MULTICAST, trace_endpos_y);
	WriteCoord (MSG_MULTICAST, trace_endpos_z);
#ifdef QUAKEWORLD
	multicast (org, MULTICAST_PHS);
#endif

	LightningDamage (self.origin, trace_endpos + v_forward*4, self, 20+random()*10);
};

void() T_BFGTouch =
{
  if (self.voided)
    return;

  self.voided = 1;

  if (other.takedamage == DAMAGE_AIM)
  {
    DoDamage(other,self,self.owner,1000,SH_PLASMA);
  }

  T_PlasmaDamage(500,200, self.owner); 

  BecomeExplosion();
};

void() W_FireBFG =
{
  local entity missile;

  if ((self.ammo_cells < 40) && (self.playerclass != CL_MAGE))
  {
    self.tb_mode = 0;
    sprint(self,"Thunderbolt Mode\n");
    return;
  }
  player_rocket1();
  sound (self, CHAN_WEAPON, "weapons/sgun1.wav", 1, ATTN_NORM);

  self.punchangle_x = -10;
  if (self.playerclass != CL_MAGE)
    self.currentammo = self.ammo_cells = self.ammo_cells - 40;

  missile = spawn ();
  missile.owner = self;
#ifndef QUAKEWORLD
  missile.effects = EF_DIMLIGHT | EF_BRIGHTFIELD;
#else
  missile.effects = EF_DIMLIGHT | EF_RED;  
#endif
  missile.nextthink = time + 5;
  missile.think = SUB_Remove;

//  missile.avelocity = '400 400 400';
  missile.movetype = MOVETYPE_FLYMISSILE;
  missile.solid = SOLID_BBOX;
  missile.classname = "bfg";
  missile.netname = "BFG8999";
  missile.voided = 0;
		
// set missile speed	

  makevectors (self.v_angle);
  missile.velocity = aim(self, 1000);
  missile.velocity = missile.velocity * 400;
  missile.angles = vectoangles(missile.velocity);
	
  missile.touch = T_BFGTouch;

  setmodel (missile, "progs/s_light.spr");

  setsize (missile, '0 0 0', '0 0 0');
  setorigin (missile, self.origin + v_forward*8 + '0 0 16');
  self.attack_finished = time + 1.6;

};

void() W_FireHyperLaser =
{
  local vector dir;  
 
  if (self.ammo_cells < 4)
    return;

  
#ifdef QUAKEWORLD
  msg_entity = self;
  WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
  self.punchangle_x = -2;
#endif

	// Shocktrooper only, so set cells to 0 (since max is 4)
  self.ammo_cells = 0;
  sound (self, CHAN_WEAPON, "enforcer/enfire.wav", 1, ATTN_NORM);

  dir = aim(self,10000);
  makevectors(self.v_angle);
  LaunchLaser(v_forward*16 + self.origin + '0 0 16', dir);
  LaunchLaser(v_forward*8 + self.origin + '0 0 16' + v_right * 4, dir);
  LaunchLaser(v_forward*8 + self.origin + '0 0 16' + v_right * -4, dir);
  self.attack_finished = time + 0.4;
};

// Blam.
// Lightning is cast and proceeds to chain through people until
// no one is in range.
// void(vector p1, vector p2, entity from, float damage) LightningDamage;
void() ChainLightning =
{
  local entity head, temp;
  local float dist;
  local   vector  org, dir;
  
  if (self.health < 1) { remove (self); return; }

  dist = 10000;
  head = findradius(self.origin, 500);

  while (head)
  {
    if ((head.health >= 1) && (head != self.owner) && (head.takedamage == DAMAGE_AIM))
    {
      if ((visible(head)) && (head != self.enemy))
      {
        if (!((teamplay) && (SameTeam(self.owner,head))))
        {
	 if (head.invincible_finished >= time)
         {
           remove(self);
           return;
         }

         sound (self, CHAN_WEAPON, "misc/power.wav", 1, ATTN_NORM);
           
         org = self.enemy.origin + '0 0 16';
         dir = head.origin + '0 0 16' - org;
         dir = normalize (dir);
         traceline (org, head.origin, TRUE, self);
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
         temp = self;
         self = self.owner;
         self = temp;
         self.enemy = head;
         DoDamage(self.enemy, self, self.owner, 10+random()*10, SH_ELECTRICITY);
         self.nextthink = time + 0.1;
         self.think = ChainLightning;
	 self.health = self.health - 1;
         return;                      
        }
      }
    }
    head = head.chain;
  }
  if (self.enemy != self.owner)
    DoDamage(self.enemy, self, self.owner, 1+random()*20, SH_ELECTRICITY);
  remove(self);
  return;
};

void() ShockChain =
{
  local entity clobj;
  if (self.ammo_cells < 4)
    return;

  sound (self, CHAN_AUTO, "shambler/sboom.wav", 1, ATTN_NORM);
  self.ammo_cells = 0;
  muzzleflash();
  self.attack_finished = time + 0.4; // main offense

  makevectors(self.v_angle);
  traceline(self.origin + '0 0 16', (self.origin + '0 0 16') + v_forward * 1200, FALSE, self);

  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
  WriteByte (MSG_MULTICAST, TE_LIGHTNING1);
  WriteEntity (MSG_MULTICAST, self);
  WriteCoord (MSG_MULTICAST, self.origin_x);
  WriteCoord (MSG_MULTICAST, self.origin_y);
  WriteCoord (MSG_MULTICAST, self.origin_z);
  WriteCoord (MSG_MULTICAST, trace_endpos_x);
  WriteCoord (MSG_MULTICAST, trace_endpos_y);
  WriteCoord (MSG_MULTICAST, trace_endpos_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif

  if (trace_fraction != 1.0)
  {
    if (trace_ent.takedamage == DAMAGE_AIM)
    {
      DoDamage(trace_ent,self,self, 20+random()*15, SH_ELECTRICITY);
      if (self.player_flags & PF_NOFRAME)
        return; // Inside Archon -- no chains
    
      clobj=spawn();
      clobj.origin = self.origin;
      clobj.owner = self;
      clobj.enemy = trace_ent;
      clobj.nextthink = time;
      clobj.think = ChainLightning;
      clobj.netname = "chain lightning";
      clobj.health = 16;
    } 
  }
};

void() W_FireTB =
{
  if (!self.player_flags & PF_MAGIC)
  { 
    if (self.tb_mode == 0)
    {
      self.attack_finished = time + 0.1;
      sound (self, CHAN_AUTO, "weapons/lstart.wav", 1, ATTN_NORM);
      player_light1();
    }
    else if (self.tb_mode == 1)
      if (self.playerclass == CL_SHOCK)
      {
	if (self.ammo_cells < 4)
          return;
        player_shot1();
        W_FireHyperLaser();
      }
      else  
        W_FireBFG();
    else if (self.tb_mode == 2)
    {
      player_shot1();
      ShockChain();
    }
      
  } else {
    E_CastSpell();
  }

};
