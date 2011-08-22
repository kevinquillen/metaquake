/*
 * Common/items.c, Common/items.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

#include "meta.h"

void() W_SetCurrentAmmo;
/* ALL LIGHTS SHOULD BE 0 1 0 IN COLOR ALL OTHER ITEMS SHOULD
BE .8 .3 .4 IN COLOR */


void() SUB_regen =
{
  /* Prevent people from sitting on items when they want to respawn. */
  local entity e;
  e = findradius(self.origin,100);
  while(e)
  {
    if ((e.classname == "player") && (e.health > 0))
    {
      self.think = SUB_regen;
      self.nextthink = time + 2 + random() * 5;
      return;
    }
    e = e.chain;
  }

	self.model = self.mdl;          // restore original model
	self.solid = SOLID_TRIGGER;     // allow it to be touched again
	sound (self, CHAN_VOICE, "items/itembk2.wav", 1, ATTN_NORM);    // play respawn sound
	setorigin (self, self.origin);
};



/*QUAKED noclass (0 0 0) (-8 -8 -8) (8 8 8)
prints a warning message when spawned
*/
void() noclass =
{
	dprint ("noclass spawned at");
	dprint (vtos(self.origin));
	dprint ("\n");
	remove (self);
};


void() q_touch;

void() q_touch =
{
local entity    stemp;
local float     best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	self.mdl = self.model;

	sound (other, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	other.items = other.items | IT_QUAD;
	self.model = string_null;

// do the apropriate action
	other.super_time = 1;
	other.super_damage_finished = self.cnt;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


void(float timeleft) DropQuad =
{
	local entity    item;

	item = spawn();
	item.origin = self.origin;
	
	item.velocity_z = 300;
	item.velocity_x = -100 + (random() * 200);
	item.velocity_y = -100 + (random() * 200);
	
	item.flags = FL_ITEM;
	item.solid = SOLID_TRIGGER;
	item.movetype = MOVETYPE_TOSS;
	item.noise = "items/damage.wav";
	setmodel (item, "progs/quaddama.mdl");
#ifdef QUAKEWORLD
	item.effects = EF_BLUE;
#else
	item.effects = EF_DIMLIGHT;
#endif
	setsize (item, '-16 -16 -24', '16 16 32');
	item.cnt = time + timeleft;
	item.touch = q_touch;
	item.nextthink = time + timeleft;    // remove it with the time left on it
	item.think = SUB_Remove;
};


void() r_touch;

void() r_touch =
{
local entity    stemp;
local float     best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	self.mdl = self.model;

	sound (other, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	other.items = other.items | IT_INVISIBILITY;
	self.model = string_null;

// do the apropriate action
	other.invisible_time = 1;
	other.invisible_finished = self.cnt;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


void(float timeleft) DropRing =
{
	local entity    item;

	item = spawn();
	item.origin = self.origin;
	
	item.velocity_z = 300;
	item.velocity_x = -100 + (random() * 200);
	item.velocity_y = -100 + (random() * 200);
	
	item.flags = FL_ITEM;
	item.solid = SOLID_TRIGGER;
	item.movetype = MOVETYPE_TOSS;
	item.noise = "items/inv1.wav";
	setmodel (item, "progs/invisibl.mdl");
	setsize (item, '-16 -16 -24', '16 16 32');
	item.cnt = time + timeleft;
	item.touch = r_touch;
	item.nextthink = time + timeleft;    // remove after 30 seconds
	item.think = SUB_Remove;
};

/*
============
PlaceItem

plants the object on the floor
============
*/
void() PlaceItem =
{
	local float     oldz;

	self.mdl = self.model;          // so it can be restored on respawn
	self.flags = FL_ITEM;           // make extra wide
	self.solid = SOLID_TRIGGER;
	self.movetype = MOVETYPE_TOSS;  
	self.velocity = '0 0 0';
	self.origin_z = self.origin_z + 6;
	oldz = self.origin_z;
	if (!droptofloor())
	{
		dprint ("Bonus item fell out of level at ");
		dprint (vtos(self.origin));
		dprint ("\n");
		remove(self);
		return;
	}
};

/*
============
StartItem

Sets the clipping size and plants the object on the floor
============
*/
void() StartItem =
{
	self.nextthink = time + 0.2;    // items start after other solids
	self.think = PlaceItem;
};

/*
=========================================================================

HEALTH BOX

=========================================================================
*/
//
// T_Heal: add health to an entity, limiting health to max_health
// "ignore" will ignore max_health limit
//
float (entity e, float healamount, float ignore) T_Heal =
{
	if (e.health <= 0)
		return 0;
	if ((!ignore) && (e.health >= other.max_health))
		return 0;
	healamount = ceil(healamount);

	e.health = e.health + healamount;
	if ((!ignore) && (e.health >= other.max_health))
		e.health = other.max_health;
	
	//Only allow double maximum health
	if (e.health > e.max_health * 2)
		e.health = e.max_health * 2;
	return 1;
};

/*QUAKED item_health (.3 .3 1) (0 0 0) (32 32 32) rotten megahealth
Health box. Normally gives 25 points.
Rotten box heals 15 points,
Megahealth will add 100 health, up to 2x your max health
*/

#define H_ROTTEN  1
#define H_MEGA 2
.float  healamount, healtype;
void() health_touch;
void() item_health =
{       
#ifdef GAME_CTF
	if ( !(deathmatch&MODE_CTF) && (self.spawnflags & SF_ONLY_CTF))
	{
	  remove(self);
	  return;
	}

	if ( (deathmatch&MODE_CTF) && (self.spawnflags & SF_NOT_CTF))
	{
	  remove(self);
          return;
	}
#endif

	self.touch = health_touch;

	if (self.spawnflags & H_ROTTEN)
	{
		precache_model("maps/b_bh10.bsp");

		precache_sound("items/r_item1.wav");
		setmodel(self, "maps/b_bh10.bsp");
		self.noise = "items/r_item1.wav";
		self.healamount = 15;
		self.healtype = 0;
	}
	else
	if (self.spawnflags & H_MEGA)
	{
		precache_model("maps/b_bh100.bsp");
		precache_sound("items/r_item2.wav");
		setmodel(self, "maps/b_bh100.bsp");
		self.noise = "items/r_item2.wav";
		self.healamount = 100;
		self.healtype = 2;
	}
	else
	{
		precache_model("maps/b_bh25.bsp");
		precache_sound("items/health1.wav");
		setmodel(self, "maps/b_bh25.bsp");
		self.noise = "items/health1.wav";
		self.healamount = 25;
		self.healtype = 1;
	}
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();
};


void() health_touch =
{
	local float amount;
	local string s;
	
	if(other.classname != "player")
		return;
	

	if(self.healtype == 2) // Megahealth?  Ignore max_health...
	{
		if (!T_Heal(other, self.healamount, 1))
			return;
	}
	else
	{
		if (!T_Heal(other, self.healamount, 0))
			return;
	}
	
	sprint(other, "You receive ");
	s = ftos(self.healamount);
	sprint(other, s);
	sprint(other, " health\n");
	
// health touch sound
	sound(other, CHAN_ITEM, self.noise, 1, ATTN_NORM);

	stuffcmd (other, "bf\n");
	
	self.model = string_null;
	self.solid = SOLID_NOT;

	if (self.healtype == 2)
		self.nextthink = time + 50 + random()*30;
	else
		self.nextthink = time + 20;
		
	self.think = SUB_regen;
	
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};      

/*
===============================================================================

ARMOR

===============================================================================
*/
void() armor_touch =
{
	local   float   type, value, bit;
	
	if (other.health <= 0)
		return;
	if (other.classname != "player")
		return;

	if (self.classname == "item_armor1")
	{
		type = 0.3;
		value = 100;
		bit = IT_ARMOR1;
	}
	if (self.classname == "item_armor2")
	{
		type = 0.6;
		value = 175;
		bit = IT_ARMOR2;
	}
	if (self.classname == "item_armorInv")
	{
		type = 0.8;
		value = 250;
		bit = IT_ARMOR3;
	}

	if (other.max_arm <= value)
           value = other.max_arm;

	if (other.armortype*other.armorvalue >= type*value)
		return;
		
	other.armortype = type;
	other.armorvalue = value;
	other.items = other.items - (other.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3)) + bit;

	self.solid = SOLID_NOT;
	self.model = string_null;
	self.nextthink = time + 20;
	self.think = SUB_regen;

	sprint(other, "You got armor\n");
// armor touch sound
	sound(other, CHAN_ITEM, "items/armor1.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


/*QUAKED item_armor1 (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() item_armor1 =
{
	self.touch = armor_touch;
	precache_model ("progs/armor.mdl");
	setmodel (self, "progs/armor.mdl");
	self.skin = 0;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED item_armor2 (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() item_armor2 =
{
	self.touch = armor_touch;
	precache_model ("progs/armor.mdl");
	setmodel (self, "progs/armor.mdl");
	self.skin = 1;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED item_armorInv (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() item_armorInv =
{
#ifdef GAME_CTF
	if ( !(deathmatch&MODE_CTF) && (self.spawnflags & SF_ONLY_CTF))
	{
	  remove(self);
	  return;
	}
#endif

	self.touch = armor_touch;
	precache_model ("progs/armor.mdl");
	setmodel (self, "progs/armor.mdl");
	self.skin = 2;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*
===============================================================================

WEAPONS

===============================================================================
*/

void() bound_other_ammo =
{
	if (other.ammo_shells > other.max_ammo_shells)
		other.ammo_shells = other.max_ammo_shells;
	if (other.ammo_nails > other.max_ammo_nails)
		other.ammo_nails = other.max_ammo_nails;
	if (other.ammo_rockets > other.max_ammo_rockets)
		other.ammo_rockets = other.max_ammo_rockets;               
	if (other.ammo_cells > other.max_ammo_cells)
		other.ammo_cells = other.max_ammo_cells;         
};


float(float w) RankForWeapon =
{
	if (w == IT_LIGHTNING)
		return 1;
	if (w == IT_ROCKET_LAUNCHER)
		return 2;
	if (w == IT_SUPER_NAILGUN)
		return 3;
	if (w == IT_GRENADE_LAUNCHER)
		return 4;
	if (w == IT_SUPER_SHOTGUN)
		return 5;
	if (w == IT_NAILGUN)
		return 6;
	return 7;
};

float (float w) WeaponCode =
{
	if (w == IT_SUPER_SHOTGUN)
		return 3;
	if (w == IT_NAILGUN)
		return 4;
	if (w == IT_SUPER_NAILGUN)
		return 5;
	if (w == IT_GRENADE_LAUNCHER)
		return 6;
	if (w == IT_ROCKET_LAUNCHER)
		return 7;
	if (w == IT_LIGHTNING)
		return 8;
	return 1;
};

/*
=============
Deathmatch_Weapon

Deathmatch weapon change rules for picking up a weapon

.float          ammo_shells, ammo_nails, ammo_rockets, ammo_cells;
=============
*/
void(float old, float new) Deathmatch_Weapon =
{
	local float or, nr;

// change self.weapon if desired
	or = RankForWeapon (self.weapon);
	nr = RankForWeapon (new);
	if ( nr < or )
		self.weapon = new;
};

/*
=============
weapon_touch
=============
*/
float() W_BestWeapon;

void() weapon_touch =
{
	local   float   hadammo, best, new, old;
	local   entity  stemp;
	local   float   leave;

	// For client weapon_switch
	if (!(other.flags & FL_CLIENT))
		return;

	if (other.playerclass == CL_FIREELEM)
          return;

// if the player was using his best weapon, change up to the new one if better          
//	stemp = self;
//	self = other;
//	best = W_BestWeapon();
//	self = stemp;

	leave = 0;
	
	if (self.classname == "weapon_nailgun")
	{
		if (leave && (other.items & IT_NAILGUN) )
			return;
		hadammo = other.ammo_nails;                     
		new = IT_NAILGUN;
		other.ammo_nails = other.ammo_nails + 30;
		if (other.ammo_nails > other.max_ammo_nails)
		  other.ammo_nails = other.max_ammo_nails;
	}
	else if (self.classname == "weapon_supernailgun")
	{
		if (leave && (other.items & IT_SUPER_NAILGUN) )
			return;
		hadammo = other.ammo_rockets;                   
		new = IT_SUPER_NAILGUN;
		other.ammo_nails = other.ammo_nails + 30;
		if (other.ammo_nails > other.max_ammo_nails)
		  other.ammo_nails = other.max_ammo_nails;
	}
	else if (self.classname == "weapon_supershotgun")
	{
		if (leave && (other.items & IT_SUPER_SHOTGUN) )
			return;
		hadammo = other.ammo_rockets;                   
		new = IT_SUPER_SHOTGUN;
		other.ammo_shells = other.ammo_shells + 5;
		if (other.ammo_shells > other.max_ammo_shells)
		  other.ammo_shells = other.max_ammo_shells;
	}
	else if (self.classname == "weapon_rocketlauncher")
	{
		if (leave && (other.items & IT_ROCKET_LAUNCHER) )
			return;
		hadammo = other.ammo_rockets;                   
		new = IT_ROCKET_LAUNCHER;
		other.ammo_rockets = other.ammo_rockets + 5;
		if (other.ammo_rockets > other.max_ammo_rockets)
		  other.ammo_rockets = other.max_ammo_rockets;
	}
	else if (self.classname == "weapon_grenadelauncher")
	{
		if (leave && (other.items & IT_GRENADE_LAUNCHER) )
			return;
		hadammo = other.ammo_rockets;                   
		new = IT_GRENADE_LAUNCHER;
		other.ammo_rockets = other.ammo_rockets + 5;
		if (other.ammo_rockets > other.max_ammo_rockets)
		  other.ammo_rockets = other.max_ammo_rockets;
	}
	else if (self.classname == "weapon_lightning")
	{
		if (other.player_flags & PF_MAGIC)
		  return;

		if (leave && (other.items & IT_LIGHTNING))
			return;
		hadammo = other.ammo_rockets;                   
		new = IT_LIGHTNING;
		other.ammo_cells = other.ammo_cells + 15;
		if (other.ammo_cells > other.max_ammo_cells)
		  other.ammo_cells = other.max_ammo_cells;
	}
	else
		objerror ("weapon_touch: unknown classname");

	sprint (other, "You got the ");
	sprint (other, self.netname);
	sprint (other, "\n");
// weapon touch sound
	sound (other, CHAN_ITEM, "weapons/pkup.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");

	bound_other_ammo ();

// change to the weapon
	old = other.items;
	other.items = other.items | new;
	
	stemp = self;
	self = other;

/*
	if ( WeaponCode(new) <= w_switch )
	{
		if (self.flags & FL_INWATER)
		{
			if (new != IT_LIGHTNING)
			{
				Deathmatch_Weapon (old, new);
			}
		}
		else
		{                
			Deathmatch_Weapon (old, new);
		}
	}
*/
	W_SetCurrentAmmo();

	self = stemp;

	if (leave)
		return;

	// remove it in single player, or setup for respawning in deathmatch
	self.model = string_null;
	self.solid = SOLID_NOT;
	self.nextthink = time + 10;
	self.think = SUB_regen;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


/*QUAKED weapon_supershotgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() weapon_supershotgun =
{
	precache_model ("progs/g_shot.mdl");
	setmodel (self, "progs/g_shot.mdl");
	self.weapon = IT_SUPER_SHOTGUN;
	self.netname = "Double-barrelled Shotgun";
	self.touch = weapon_touch;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED weapon_nailgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() weapon_nailgun =
{
	precache_model ("progs/g_nail.mdl");
	setmodel (self, "progs/g_nail.mdl");
	self.weapon = IT_NAILGUN;
	self.netname = "nailgun";
	self.touch = weapon_touch;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED weapon_supernailgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() weapon_supernailgun =
{
	precache_model ("progs/g_nail2.mdl");
	setmodel (self, "progs/g_nail2.mdl");
	self.weapon = IT_SUPER_NAILGUN;
	self.netname = "Super Nailgun";
	self.touch = weapon_touch;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED weapon_grenadelauncher (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() weapon_grenadelauncher =
{
	precache_model ("progs/g_rock.mdl");
	setmodel (self, "progs/g_rock.mdl");
	self.weapon = 3;
	self.netname = "Grenade Launcher";
	self.touch = weapon_touch;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};

/*QUAKED weapon_rocketlauncher (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() weapon_rocketlauncher =
{
	precache_model ("progs/g_rock2.mdl");
	setmodel (self, "progs/g_rock2.mdl");
	self.weapon = 3;
	self.netname = "Rocket Launcher";
	self.touch = weapon_touch;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};


/*QUAKED weapon_lightning (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void() weapon_lightning =
{
	precache_model ("progs/g_light.mdl");
	setmodel (self, "progs/g_light.mdl");
	self.weapon = 3;
	self.netname = "Thunderbolt";
	self.touch = weapon_touch;
	setsize (self, '-16 -16 0', '16 16 56');
	StartItem ();
};


/*
===============================================================================

AMMO

===============================================================================
*/

void() ammo_touch =
{
local entity    stemp;
local float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

// if the player was using his best weapon, change up to the new one if better          
//	stemp = self;  
//	self = other;
//	best = W_BestWeapon();
//	self = stemp;

// shotgun
	if (self.weapon == 1)
	{
		if (other.ammo_shells >= other.max_ammo_shells)
			return;
		other.ammo_shells = other.ammo_shells + self.aflag;
		if (other.ammo_shells > other.max_ammo_shells)
                  other.ammo_shells = other.max_ammo_shells;
	}

// spikes
	if (self.weapon == 2)
	{
		if (other.ammo_nails >= other.max_ammo_nails)
			return;
		other.ammo_nails = other.ammo_nails + self.aflag;
		if (other.ammo_nails > other.max_ammo_nails)
                  other.ammo_nails = other.max_ammo_nails;
	}

//      rockets
	if (self.weapon == 3)
	{
		if (other.ammo_rockets >= other.max_ammo_rockets)
			return;
		other.ammo_rockets = other.ammo_rockets + self.aflag;
		if (other.ammo_rockets > other.max_ammo_rockets)
                  other.ammo_rockets = other.max_ammo_rockets;
	}

//      cells
	if (self.weapon == 4)
	{
		if (other.player_flags & PF_MAGIC)
		  return;

		if (other.ammo_cells >= other.max_ammo_cells)
			return;
		other.ammo_cells = other.ammo_cells + self.aflag;
		if (other.ammo_cells > other.max_ammo_cells)
                  other.ammo_cells = other.max_ammo_cells;
	}

	bound_other_ammo ();
	
	sprint (other, "You got the ");
	sprint (other, self.netname);
	sprint (other, "\n");
// ammo touch sound
	sound (other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");

// change to a better weapon if appropriate
/*
	if ( other.weapon == best )
	{
		stemp = self;
		self = other;
		self.weapon = W_BestWeapon();
		W_SetCurrentAmmo ();
		self = stemp;
	}
*/
// if changed current ammo, update it
	stemp = self;
	self = other;
	W_SetCurrentAmmo();
	self = stemp;

// remove it in single player, or setup for respawning in deathmatch
	self.model = string_null;
	self.solid = SOLID_NOT;
	self.nextthink = time + 20;

// Xian -- If playing in DM 3.0 mode, halve the time ammo respawns        

	self.think = SUB_regen;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};




#define AMMO_BIG 1	//Spawn with large size

/*QUAKED item_shells (0 .5 .8) (0 0 0) (32 32 32) big
*/

void() item_shells =
{
	self.touch = ammo_touch;

	if (self.spawnflags & AMMO_BIG)
	{
		precache_model ("maps/b_shell1.bsp");
		setmodel (self, "maps/b_shell1.bsp");
		self.aflag = 40;
	}
	else
	{
		precache_model ("maps/b_shell0.bsp");
		setmodel (self, "maps/b_shell0.bsp");
		self.aflag = 20;
	}
	self.weapon = 1;
	self.netname = "shells";
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();
};

/*QUAKED item_spikes (0 .5 .8) (0 0 0) (32 32 32) big
*/

void() item_spikes =
{
	self.touch = ammo_touch;

	if (self.spawnflags & AMMO_BIG)
	{
		precache_model ("maps/b_nail1.bsp");
		setmodel (self, "maps/b_nail1.bsp");
		self.aflag = 50;
	}
	else
	{
		precache_model ("maps/b_nail0.bsp");
		setmodel (self, "maps/b_nail0.bsp");
		self.aflag = 25;
	}
	self.weapon = 2;
	self.netname = "nails";
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();

};

/*QUAKED item_rockets (0 .5 .8) (0 0 0) (32 32 32) big
*/

void() item_rockets =
{
	self.touch = ammo_touch;

	if (self.spawnflags & AMMO_BIG)
	{
		precache_model ("maps/b_rock1.bsp");
		setmodel (self, "maps/b_rock1.bsp");
		self.aflag = 10;
	}
	else
	{
		precache_model ("maps/b_rock0.bsp");
		setmodel (self, "maps/b_rock0.bsp");
		self.aflag = 5;
	}
	self.weapon = 3;
	self.netname = "rockets";
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();

};


/*QUAKED item_cells (0 .5 .8) (0 0 0) (32 32 32) big
*/

void() item_cells =
{
	self.touch = ammo_touch;

	if (self.spawnflags & AMMO_BIG)
	{
		precache_model ("maps/b_batt1.bsp");
		setmodel (self, "maps/b_batt1.bsp");
		self.aflag = 12;
	}
	else
	{
		precache_model ("maps/b_batt0.bsp");
		setmodel (self, "maps/b_batt0.bsp");
		self.aflag = 6;
	}
	self.weapon = 4;
	self.netname = "cells";
	setsize (self, '0 0 0', '32 32 56');
	StartItem ();

};

/*
===============================================================================

KEYS

===============================================================================
*/

void() key_touch =
{
local entity    stemp;
local float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;
	if (other.items & self.items)
		return;

	sprint (other, "You got the ");
	sprint (other, self.netname);
	sprint (other, "\n");

	sound (other, CHAN_ITEM, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	other.items = other.items | self.items;

	self.solid = SOLID_NOT;
	self.model = string_null;

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


void() key_setsounds =
{
	if (world.worldtype == 0)
	{
		precache_sound ("misc/medkey.wav");
		self.noise = "misc/medkey.wav";
	}
	if (world.worldtype == 1)
	{
		precache_sound ("misc/runekey.wav");
		self.noise = "misc/runekey.wav";
	}
	if (world.worldtype == 2)
	{
		precache_sound2 ("misc/basekey.wav");
		self.noise = "misc/basekey.wav";
	}
};

/*QUAKED item_key1 (0 .5 .8) (-16 -16 -24) (16 16 32)
SILVER key
In order for keys to work
you MUST set your maps
worldtype to one of the
following:
0: medieval
1: metal
2: base
*/

void() item_key1 =
{
	if (world.worldtype == 0)
	{
		precache_model ("progs/w_s_key.mdl");
		setmodel (self, "progs/w_s_key.mdl");
		self.netname = "silver key";
	}
	else if (world.worldtype == 1)
	{
		precache_model ("progs/m_s_key.mdl");
		setmodel (self, "progs/m_s_key.mdl");
		self.netname = "silver runekey";
	}
	else if (world.worldtype == 2)
	{
		precache_model2 ("progs/b_s_key.mdl");
		setmodel (self, "progs/b_s_key.mdl");
		self.netname = "silver keycard";
	}
	key_setsounds();
	self.touch = key_touch;
	self.items = IT_KEY1;
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};

/*QUAKED item_key2 (0 .5 .8) (-16 -16 -24) (16 16 32)
GOLD key
In order for keys to work
you MUST set your maps
worldtype to one of the
following:
0: medieval
1: metal
2: base
*/

void() item_key2 =
{
	if (world.worldtype == 0)
	{
		precache_model ("progs/w_g_key.mdl");
		setmodel (self, "progs/w_g_key.mdl");
		self.netname = "gold key";
	}
	if (world.worldtype == 1)
	{
		precache_model ("progs/m_g_key.mdl");
		setmodel (self, "progs/m_g_key.mdl");
		self.netname = "gold runekey";
	}
	if (world.worldtype == 2)
	{
		precache_model2 ("progs/b_g_key.mdl");
		setmodel (self, "progs/b_g_key.mdl");
		self.netname = "gold keycard";
	}
	key_setsounds();
	self.touch = key_touch;
	self.items = IT_KEY2;
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};



/*
===============================================================================

END OF LEVEL RUNES

===============================================================================
*/

void() sigil_touch =
{
local entity    stemp;
local float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	centerprint (other, "You got the rune!");

	sound (other, CHAN_ITEM, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	self.model = string_null;
	serverflags = serverflags | (self.spawnflags & 15);
	self.classname = "";            // so rune doors won't find it
	
	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};


/*QUAKED item_sigil (0 .5 .8) (-16 -16 -24) (16 16 32) E1 E2 E3 E4
End of level sigil, pick up to end episode and return to jrstart.
*/

void() item_sigil =
{
	if (!self.spawnflags)
		objerror ("no spawnflags");

	precache_sound ("misc/runekey.wav");
	self.noise = "misc/runekey.wav";

	if (self.spawnflags & 1)
	{
		precache_model ("progs/end1.mdl");
		setmodel (self, "progs/end1.mdl");
	}
	if (self.spawnflags & 2)
	{
		precache_model2 ("progs/end2.mdl");
		setmodel (self, "progs/end2.mdl");
	}
	if (self.spawnflags & 4)
	{
		precache_model2 ("progs/end3.mdl");
		setmodel (self, "progs/end3.mdl");
	}
	if (self.spawnflags & 8)
	{
		precache_model2 ("progs/end4.mdl");
		setmodel (self, "progs/end4.mdl");
	}
	
	self.touch = sigil_touch;
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};

/*
===============================================================================

POWERUPS

===============================================================================
*/

void() powerup_touch;


void() powerup_touch =
{
local entity    stemp;
local float             best;

	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	sprint (other, "You got the ");
	sprint (other,  self.netname);
	sprint (other, "\n");

	self.mdl = self.model;
	
	//respawn time
	if ((self.classname == "item_artifact_invulnerability") ||
		(self.classname == "item_artifact_invisibility"))
		self.nextthink = time + 180 + random() * 60;
	else
		self.nextthink = time + 50 + random() * 30;
	
	self.think = SUB_regen;

	sound (other, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	stuffcmd (other, "bf\n");
	self.solid = SOLID_NOT;
	other.items = other.items | self.items;
	self.model = string_null;

// do the apropriate action
	if (self.classname == "item_artifact_envirosuit")
	{
		other.rad_time = 1;
		other.radsuit_finished = time + 30;
	}
	
	if (self.classname == "item_artifact_invulnerability")
	{
		other.invincible_time = 1;
		other.invincible_finished = time + 20;
	}
	
	if (self.classname == "item_artifact_invisibility")
	{
		other.invisible_time = 1;
		other.invisible_finished = time + 30;
	}

	if (self.classname == "item_artifact_super_damage")
	{
		other.super_time = 1;
		other.super_damage_finished = time + 20;
	}       

	activator = other;
	SUB_UseTargets();                               // fire all targets / killtargets
};



/*QUAKED item_artifact_invulnerability (0 .5 .8) (-16 -16 -24) (16 16 32)
Player is invulnerable for 30 seconds
*/
void() item_artifact_invulnerability =
{
#ifdef QUAKEWORLD
  local string str;
  str = infokey(world,"nopent");
  if (str == "1")
  {
    remove(self);
    return;
  }
#endif
#ifdef GAME_CTF
  if (deathmatch & MODE_CTF)
  {
    remove(self);
    return;
  }
#endif
  
  self.touch = powerup_touch;
  precache_model("progs/invulner.mdl");
  precache_sound("items/protect.wav");
  precache_sound("items/protect2.wav");
  precache_sound("items/protect3.wav");
  self.noise="items/protect.wav";
  setmodel(self,"progs/invulner.mdl");
  self.netname = "Pentagram of Protection";
#ifdef QUAKEWORLD
  self.effects = EF_RED;
#else
  self.effects = EF_DIMLIGHT;
#endif
  self.items = IT_INVULNERABILITY;
  setsize(self,'-16 -16 -24', '16 16 32');
  StartItem();
};

void() item_artifact_envirosuit=
{
  self.touch = powerup_touch;
  precache_model("progs/suit.mdl");
  precache_sound("items/suit.wav");
  precache_sound("items/suit2.wav");
  self.noise="items/suit.wav";
  setmodel(self,"progs/suit.mdl");
  self.netname = "Biosuit";
  self.items = IT_SUIT;
  setsize(self,'-16 -16 -24','16 16 32');
  StartItem();
};
void() item_artifact_invisibility =
{
#ifdef QUAKEWORLD
  local string str;

  str = infokey(world,"noring");
  if (str == "1")
  {
    remove(self);
    return;
  }
#endif

	self.touch = powerup_touch;

	precache_model ("progs/invisibl.mdl");
	precache_sound ("items/inv1.wav");
	precache_sound ("items/inv2.wav");
	precache_sound ("items/inv3.wav");
	self.noise = "items/inv1.wav";
	setmodel (self, "progs/invisibl.mdl");
	self.netname = "Ring of Shadows";
	self.items = IT_INVISIBILITY;
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};


/*QUAKED item_artifact_super_damage (0 .5 .8) (-16 -16 -24) (16 16 32)
The next attack from the player will do 4x damage
*/
void() item_artifact_super_damage =
{
#ifdef QUAKEWORLD
   local string str;
   str = infokey(world,"noquad");
   if (str == "1")
   {
     remove(self);
     return;
   }
#endif
   
#ifdef GAME_CTF
	if ( !(deathmatch&MODE_CTF) && (self.spawnflags & SF_ONLY_CTF))
	{
	  remove(self);
	  return;
	}

	if ((deathmatch&MODE_CTF) && (self.spawnflags & SF_NOT_CTF))
	{
	  remove(self);
          return;
	}
#endif

	self.touch = powerup_touch;

	precache_model ("progs/quaddama.mdl");
	precache_sound ("items/damage.wav");
	precache_sound ("items/damage2.wav");
	precache_sound ("items/damage3.wav");
	self.noise = "items/damage.wav";
	setmodel (self, "progs/quaddama.mdl");
	self.netname = "Quad Damage";
	self.items = IT_QUAD;
#ifdef QUAKEWORLD
	self.effects = EF_BLUE;
#else
	self.effects = EF_DIMLIGHT;
#endif
	setsize (self, '-16 -16 -24', '16 16 32');
	StartItem ();
};



/*
===============================================================================

PLAYER BACKPACKS

===============================================================================
*/

void() BackpackTouch =
{
	local string	s;
	local	float	best, old, new;
	local		entity	stemp;
	local	float	acount;
	
	if (other.classname != "player")
		return;
	if (other.health <= 0)
		return;

	acount = 0;
	sprint (other, "You get ");

        if (self.items & IT_SUIT)
          other.radsuit_finished = time + self.radsuit_finished;

	if (self.items)
		if ((other.items & self.items) == 0)
		{
			acount = 1;
			sprint (other, "the ");
			sprint (other, self.netname);
		}

// change weapons
	other.ammo_shells = other.ammo_shells + self.ammo_shells;
	other.ammo_nails = other.ammo_nails + self.ammo_nails;
	other.ammo_rockets = other.ammo_rockets + self.ammo_rockets;
	other.ammo_cells = other.ammo_cells + self.ammo_cells;

	new = self.items;
	if (!new)
		new = other.weapon;
	old = other.items;
	other.items = other.items | new;
	
	bound_other_ammo ();

	if (self.ammo_shells)
	{
		if (acount)
			sprint(other, ", ");
		acount = 1;
		s = ftos(self.ammo_shells);
		sprint (other, s);
		sprint (other, " shells");
	}
	if (self.ammo_nails)
	{
		if (acount)
			sprint(other, ", ");
		acount = 1;
		s = ftos(self.ammo_nails);
		sprint (other, s);
		sprint (other, " nails");
	}
	if (self.ammo_rockets)
	{
		if (acount)
			sprint(other, ", ");
		acount = 1;
		s = ftos(self.ammo_rockets);
		sprint (other, s);
		sprint (other, " rockets");
	}
	if (self.ammo_cells)
	{
		if (acount)
			sprint(other, ", ");
		acount = 1;
		s = ftos(self.ammo_cells);
		sprint (other, s);
		sprint (other, " cells");
	}
	
	sprint (other, "\n");
// backpack touch sound
	sound (other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM);
	stuffcmd (other, "bf\n");

// remove the backpack, change self to the player
	remove(self);
	self = other;

	W_SetCurrentAmmo ();
};

/*
===============
DropBackpack
===============
*/
/*
entity (string model) DropItem =
{
	local entity e;
	e = spawn();
	e.owner = world;
	e.origin = self.origin;
	e.movetype = MOVETYPE_TOSS;
	e.solid = SOLID_TRIGGER;
	setmodel(e,model);
	setsize(e,'-16 -16 -24', '16 16 32');
	e.movetype = MOVETYPE_TOSS;
	e.think = SUB_Remove;
	e.nextthink = time + 10;
	return e;
}; */
void() DropBackpack =
{
	local entity	item;

	if (!(self.ammo_shells + self.ammo_nails + self.ammo_rockets + self.ammo_cells))
		return;	// nothing in it

	if(self.playerclass == CL_FIREELEM || self.playerclass == CL_BERSERK)
		return;

	item = spawn();
	item.origin = self.origin - '0 0 24';
	self.items = self.items - (self.items & IT_INVULNERABILITY); //strip out invulnerability

	//Casters do not drop #8 (Thunderbolt/Magic)
	if (self.player_flags & PF_MAGIC)
	{
		self.items = self.items - (self.items & IT_LIGHTNING);
		self.ammo_cells = 0;
	}
	

	if (self.items & IT_QUAD)
		DropQuad(self.super_damage_finished - time + 2);
	if (self.items & IT_SUIT)
		item.radsuit_finished = self.radsuit_finished - time + 2;
	if (self.items & IT_INVISIBILITY)
		DropRing(self.invisible_finished - time + 2);

	item.items = self.weapon;
	item.ammo_shells = self.ammo_shells;
	item.ammo_nails = self.ammo_nails;
	item.ammo_rockets = self.ammo_rockets;
	item.ammo_cells = self.ammo_cells;

	item.velocity_z = 300;
	item.velocity_x = -100 + (random() * 200);
	item.velocity_y = -100 + (random() * 200);
	
	item.flags = FL_ITEM;
	item.solid = SOLID_TRIGGER;
	item.movetype = MOVETYPE_TOSS;
	setmodel (item, "progs/backpack.mdl");
	setsize (item, '-16 -16 0', '16 16 56');
	item.touch = BackpackTouch;
	self.effects = 0;
	
	item.nextthink = time + 120;    // remove after 2 minutes
	item.think = SUB_Remove;
};


