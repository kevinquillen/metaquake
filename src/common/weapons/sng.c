/*
 * Common/Weapons/sng.c, Common/Weapons/sng.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

 #include "meta.h"

float() W_BestWeapon;
void() W_SetCurrentAmmo;
void(float damage) spawn_touchblood;
//=============================================================================

void() spike_touch;
void() superspike_touch;


/*
===============
launch_spike

Used for both the player and the ogre
===============
*/
void(vector org, vector dir) launch_spike =
{
	newmis = spawn ();
	newmis.voided=0;
	newmis.owner = self;
	newmis.movetype = MOVETYPE_FLYMISSILE;
        newmis.netname = "nail";
	newmis.solid = SOLID_BBOX;

	newmis.angles = vectoangles(dir);
	
	newmis.touch = spike_touch;
	newmis.classname = "spike";
	newmis.think = SUB_Remove;
	newmis.nextthink = time + 6;
	setmodel (newmis, "progs/s_spike.mdl");
	setsize (newmis, VEC_ORIGIN, VEC_ORIGIN);               
	setorigin (newmis, org);

	newmis.velocity = dir * 1000;
};

void() W_FireSuperSpikes =
{
	local vector    dir;
	local entity    old;
	
	sound (self, CHAN_WEAPON, "weapons/spike2.wav", 1, ATTN_NORM);
	self.attack_finished = time + 0.2;
	self.currentammo = self.ammo_nails = self.ammo_nails - 2;
	dir = aim (self, 1000);
	launch_spike (self.origin + '0 0 16', dir);
	newmis.touch = superspike_touch;
        newmis.netname = "steel nail";
	setsize (newmis, VEC_ORIGIN, VEC_ORIGIN);               
#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
        self.punchangle_x = -2;
#endif
};

void(float ox) W_FireSpikes =
{
	local vector    org, dir;
	local entity    old;
	local float 	dist;
        local float     t;
	local string stmp;	

	makevectors (self.v_angle);
	
	if (self.ammo_nails >= 2 && self.weapon == IT_SUPER_NAILGUN)
	{
		W_FireSuperSpikes ();
		return;
	}

	if (self.ammo_nails < 1)
	{
		self.weapon = W_BestWeapon ();
		W_SetCurrentAmmo ();
		return;
	}

	sound (self, CHAN_WEAPON, "weapons/rocket1i.wav", 1, ATTN_NORM);
	self.attack_finished = time + 0.2;
	self.currentammo = self.ammo_nails = self.ammo_nails - 1;
	makevectors(self.v_angle);

	dist = vlen(self.enemy.origin - self.origin);
	dir = self.origin + v_forward * dist;
  
       
	if ((self.enemy != world) && (vlen(self.enemy.origin - dir) < (dist * 0.30)))
        {

 

          
          dir = normalize(self.enemy.origin - ('0 0 16' + v_right * ox + self.origin));

//          org = ('0 0 16' + v_right * ox + self.origin);
//	  dist = vlen(self.enemy.origin - org);
//         t = dist / 1000;
/*          stmp = ftos(t);
          bprint("dbg: t=");
          bprint(stmp);
          stmp = ftos(dist);
          bprint(" d=");
          bprint(stmp); 
          bprint("\n");
          stmp = vtos(self.enemy.origin);
          bprint("dbg: O(c)=");
          bprint(stmp);
          stmp = vtos((self.enemy.velocity*t) + self.enemy.origin);
          bprint(" O(x)=");
          bprint(stmp);
          bprint("\n");
*/
//          dir = normalize(((self.enemy.velocity*t) + self.enemy.origin) - org) * 1000;


        }
        else
          dir = v_forward;

	launch_spike (self.origin + '0 0 16' + v_right*ox, dir);

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
        self.punchangle_x = -2;
#endif
};



.float hit_z;
void() spike_touch =
{
local float rand;
	if (other == self.owner)
		return;

	if (self.voided) {
		return;
	}
	self.voided = 1;

	if (other.solid == SOLID_TRIGGER)
		return; // trigger field, do nothing

	if (pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}
	
// hit something that bleeds
	if (other.takedamage)
	{
		spawn_touchblood (9);
		other.deathtype = "nail";
		DoDamage (other, self, self.owner, 9, SH_UNKNOWN);
		makevectors(self.angles);
		other.velocity = v_forward * (0.6 * vlen(self.velocity) - other.weight);
	}
	else
	{
		WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
		WriteByte (MSG_MULTICAST, TE_SPIKE);
		WriteCoord (MSG_MULTICAST, self.origin_x);
		WriteCoord (MSG_MULTICAST, self.origin_y);
		WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
		multicast (self.origin, MULTICAST_PHS);
#endif
	}

	remove(self);

};

void() superspike_touch =
{
local float rand;
	if (other == self.owner)
		return;

	if (self.voided) {
		return;
	}
	self.voided = 1;


	if (other.solid == SOLID_TRIGGER)
		return; // trigger field, do nothing

	if (pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}
	
// hit something that bleeds
	if (other.takedamage)
	{
		spawn_touchblood (18);
		other.deathtype = "supernail";
		DoDamage (other, self, self.owner, 16, SH_UNKNOWN);
	}
	else
	{
		WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
		WriteByte (MSG_MULTICAST, TE_SUPERSPIKE);
		WriteCoord (MSG_MULTICAST, self.origin_x);
		WriteCoord (MSG_MULTICAST, self.origin_y);
		WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
		multicast (self.origin, MULTICAST_PHS);
#endif
	}

	remove(self);
};

void(vector org, vector vec) LaunchLaser;
void() W_FireLaser =
{
  local vector dir;  
 
  if (self.ammo_nails < 3)
  {
    self.sng_mode = 0;
    return;
  }

  self.currentammo = self.ammo_nails = self.ammo_nails - 3;

#ifdef QUAKEWORLD
  msg_entity = self;
  WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
  self.punchangle_x = -2;
#endif

  sound (self, CHAN_WEAPON, "enforcer/enfire.wav", 1, ATTN_NORM);

  dir = aim(self,10000);
  makevectors(self.v_angle);
  LaunchLaser(v_forward*16 + self.origin + '0 0 16', dir);
  self.attack_finished = time + 0.2;
};

void() W_FireCannon =
{
  local vector dir;
//  player_shot1 ();
//  if (!self.flags & FL_ONGROUND)
//    return;

  sound (self, CHAN_WEAPON, "weapons/r_exp3.wav", 1, ATTN_NORM);
#ifdef QUAKEWORLD
  msg_entity = self;
  WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
  self.punchangle_x = -2;
#endif

  dir = aim (self, 100000);

  if ((self.ammo_nails < 8) && (self.sng_mode == 3))
    self.sng_mode = 2;

  if (self.sng_mode == 2)
  {
    self.velocity = '0 0 0';
    self.currentammo = self.ammo_nails = self.ammo_nails - 1;
    self.pausetime = time + 0.1;
    FireBullets (1, dir, '0.03 0.03 0', 15+random()*10, SH_BULLETS);
    self.attack_finished = time + 0.1;
  } else {
    player_hw_attb1 ();
    self.currentammo = self.ammo_nails = self.ammo_nails - 10;
    FireBullets (6, dir, '0.08 0.08 0.08', 15+random()*10, SH_BULLETS);
    self.attack_finished = time + 0.8;
  }

};

