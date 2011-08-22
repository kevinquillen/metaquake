/*
 * Common/Weapons/weapons.c, Common/Weapons/weapons.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

#include "meta.h"
void (entity targ, entity inflictor, entity attacker, float damage, float dtype) T_Damage;
void () player_run;
void(entity bomb, entity attacker, float rad, entity ignore, float damagetype) T_RadiusDamage;
void(vector org, float damage) SpawnBlood;
void() SuperDamageSound;

// called by worldspawn
void() W_Precache =
{
	precache_sound ("weapons/r_exp3.wav");  // new rocket explosion
	precache_sound ("weapons/rocket1i.wav");        // spike gun
	precache_sound ("weapons/sgun1.wav");
	precache_sound ("weapons/guncock.wav"); // player shotgun
	precache_sound ("weapons/ric1.wav");    // ricochet (used in c code)
	precache_sound ("weapons/ric2.wav");    // ricochet (used in c code)
	precache_sound ("weapons/ric3.wav");    // ricochet (used in c code)
	precache_sound ("weapons/spike2.wav");  // super spikes
	precache_sound ("weapons/tink1.wav");   // spikes tink (used in c code)
	precache_sound ("weapons/grenade.wav"); // grenade launcher
	precache_sound ("weapons/bounce.wav");          // grenade bounce
	precache_sound ("weapons/shotgn2.wav"); // super shotgun
};

float() crandom =
{
	return 2*(random() - 0.5);
};

/*
================
W_FireAxe
================
*/
void() W_FireAxe =
{
	local vector source;
	local vector org;
	local float damage;
	
	makevectors (self.v_angle);
	source = self.origin + '0 0 16';
	
	
	/* Check if there is a target within range*/
	traceline(source, source + v_forward*64, FALSE, self);
	if(trace_fraction >= 1.0)
		return;

	
	org = trace_endpos - v_forward*4;

	if(trace_ent.takedamage)
	{
		trace_ent.axhitme = 1;
		SpawnBlood(org, 20);
		
		/* Pick damage based on class */
		if(self.playerclass == CL_SILENT) /* Ninja */
			damage = 30+(random()*40);
  		else /* All others */
			damage = 20+(random()*25);
			
		DoDamage(trace_ent, self, self, damage, SH_UNKNOWN);
	}
	else
	{       // hit wall
		sound (self, CHAN_WEAPON, "player/axhit2.wav", 1, ATTN_NORM);

		WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
		WriteByte (MSG_MULTICAST, TE_GUNSHOT);

#ifdef QUAKEWORLD
  		WriteByte (MSG_MULTICAST, 3);
#endif
		
		WriteCoord (MSG_MULTICAST, org_x);
		WriteCoord (MSG_MULTICAST, org_y);
		WriteCoord (MSG_MULTICAST, org_z);
#ifdef QUAKEWORLD
		multicast (org, MULTICAST_PVS);
#endif
	}
};

//******************** plasma damage
void(float radius, float d_p, entity ignore) T_PlasmaDamage =
{
  local entity head;
  local float dist;

//  if (self.owner.items & IT_QUAD)
//    radius = radius * 2;

  head = findradius(self.origin,radius);

  while(head)
  {
    if ((head.health > 1) && (head != self) && (head != ignore) && head.classname != "door")
    {
        if (CanDamage(head, self)) {
            DoDamage(head,self,self.owner,d_p,SH_PLASMA); // do lots of damage to a radius
        }
    }
    head = head.chain;
  }

  WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
  WriteByte (MSG_BROADCAST, TE_LAVASPLASH);

  WriteCoord (MSG_BROADCAST, self.origin_x);
  WriteCoord (MSG_BROADCAST, self.origin_y);
  WriteCoord (MSG_BROADCAST, self.origin_z);
#ifdef QUAKEWORLD
  multicast (self.origin, MULTICAST_PHS);
#endif

};


//============================================================================


vector() wall_velocity =
{
	local vector    vel;
	
	vel = normalize (self.velocity);
	vel = normalize(vel + v_up*(random()- 0.5) + v_right*(random()- 0.5));
	vel = vel + 2*trace_plane_normal;
	vel = vel * 200;
	
	return vel;
};


/*
================
SpawnMeatSpray
================
*/
void(vector org, vector vel) SpawnMeatSpray =
{
	local   entity missile;
	local   vector  org;

	missile = spawn ();
	missile.owner = self;
	missile.movetype = MOVETYPE_BOUNCE;
	missile.solid = SOLID_NOT;

	makevectors (self.angles);

	missile.velocity = vel;
	missile.velocity_z = missile.velocity_z + 250 + 50*random();

	missile.avelocity = '3000 1000 2000';
	
// set missile duration
	missile.nextthink = time + 1;
	missile.think = SUB_Remove;

	setmodel (missile, "progs/zom_gib.mdl");
	setsize (missile, '0 0 0', '0 0 0');            
	setorigin (missile, org);
};

/*
================
SpawnBlood
================
*/
void(vector org, float damage) SpawnBlood =
{
#ifdef QUAKEWORLD
 	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte (MSG_MULTICAST, TE_BLOOD);
	WriteByte (MSG_MULTICAST, 1);
	WriteCoord (MSG_MULTICAST, org_x);
	WriteCoord (MSG_MULTICAST, org_y);
	WriteCoord (MSG_MULTICAST, org_z);
	multicast (org, MULTICAST_PVS);
#else 
          particle (org, '0 0 0', 73, damage*2);
#endif

};

/*
================
spawn_touchblood
================
*/
void(float damage) spawn_touchblood =
{
	local vector    vel;

	vel = wall_velocity () * 0.2;
	SpawnBlood (self.origin + vel*0.01, damage);
};

/*
==============================================================================

MULTI-DAMAGE

Collects multiple small damages into a single damage

==============================================================================
*/

entity  multi_ent;
float   multi_damage;

vector  blood_org;
float   blood_count;

vector  puff_org;
float   puff_count;

void() ClearMultiDamage =
{
	multi_ent = world;
	multi_damage = 0;
	blood_count = 0;
	puff_count = 0;
};

void(float dtype) ApplyMultiDamage =
{
	if (!multi_ent)
		return;
	DoDamage (multi_ent, self, self, multi_damage, dtype);
};

void(entity hit, float damage, float dtype) AddMultiDamage =
{
	if (!hit)
		return;
	
	if (hit != multi_ent)
	{
		ApplyMultiDamage (dtype);
		multi_damage = damage;
		multi_ent = hit;
	}
	else
		multi_damage = multi_damage + damage;
};

void() Multi_Finish =
{
	if (blood_count)
	{
#ifdef QUAKEWORLD
 		WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
		WriteByte (MSG_MULTICAST, TE_BLOOD);
		WriteByte (MSG_MULTICAST, blood_count);
		WriteCoord (MSG_MULTICAST, blood_org_x);
		WriteCoord (MSG_MULTICAST, blood_org_y);
		WriteCoord (MSG_MULTICAST, blood_org_z);
		multicast (puff_org, MULTICAST_PVS);
#else
		SpawnBlood(puff_org, blood_count * 5);
#endif
	}
};

/*
==============================================================================
BULLETS
==============================================================================
*/

/*
================
TraceAttack
================
*/
void(float damage, vector dir, float dtype) TraceAttack =
{
	local   vector  vel, org;
	
	vel = normalize(dir + v_up*crandom() + v_right*crandom());
	vel = vel + 2*trace_plane_normal;
	vel = vel * 200;

	org = trace_endpos - dir*4;

	if (trace_ent.takedamage)
	{
		blood_count = blood_count + 1;
		blood_org = org;
		AddMultiDamage (trace_ent, damage, dtype);
	}
	else
	{
//                if (QUAKEWORLD)
		  //puff_count = puff_count + 1;
                //else
                // {
                  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
		  WriteByte (MSG_MULTICAST, TE_SPIKE);
		  WriteCoord (MSG_MULTICAST, org_x);
		  WriteCoord (MSG_MULTICAST, org_y);
		  WriteCoord (MSG_MULTICAST, org_z);
#ifdef QUAKEWORLD
	          multicast(org,MULTICAST_PVS);
#endif
		//}
	}
};

/*
================
FireBullets

Used by shotgun, super shotgun, and enemy soldier firing
Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void(float shotcount, vector dir, vector spread, float shotdamage, float dtype) FireBullets =
{
	local   vector direction;
	local   vector  src;
	
	makevectors(self.v_angle);

	src = self.origin + v_forward*10;
	src_z = self.absmin_z + self.size_z * 0.7;

	ClearMultiDamage ();

	traceline (src, src + dir*2048, FALSE, self);
	puff_org = trace_endpos - dir*4;

	while (shotcount > 0)
	{
		direction = dir + crandom()*spread_x*v_right + crandom()*spread_y*v_up;
		traceline (src, src + direction*2048, FALSE, self);
		if (trace_fraction != 1.0)
			TraceAttack (shotdamage, direction, dtype);

		shotcount = shotcount - 1;
	}
	ApplyMultiDamage (dtype);
	Multi_Finish ();
};




//=============================================================================

/*
===============================================================================

PLAYER WEAPON USE

===============================================================================
*/

void() W_SetCurrentAmmo =
{
	player_run ();          // get out of any weapon firing states

	self.items = self.items - ( self.items & (IT_SHELLS | IT_NAILS | IT_ROCKETS | IT_CELLS) );
	
	if (self.weapon == IT_AXE)
	{
		if(self.playerclass == CL_BERSERK)
			self.weaponmodel = "";
		else
			self.weaponmodel = "progs/v_axe.mdl";
		self.currentammo = 0;
		self.weaponframe = 0;
	}
	else if (self.weapon == IT_SHOTGUN)
	{
		self.currentammo = self.ammo_shells;
		self.weaponmodel = "progs/v_shot.mdl";
		self.weaponframe = 0;
		self.items = self.items | IT_SHELLS;
	}
	else if (self.weapon == IT_SUPER_SHOTGUN)
	{
		self.currentammo = self.ammo_shells;
		self.weaponmodel = "progs/v_shot2.mdl";
		self.weaponframe = 0;
		self.items = self.items | IT_SHELLS;
	}
	else if (self.weapon == IT_NAILGUN)
	{
		self.currentammo = self.ammo_nails;
		self.weaponmodel = "progs/v_nail.mdl";
		self.weaponframe = 0;
		self.items = self.items | IT_NAILS;
	}
	else if (self.weapon == IT_SUPER_NAILGUN)
	{
		self.currentammo = self.ammo_nails;
		self.weaponmodel = "progs/v_nail2.mdl";
		self.weaponframe = 0;
		self.items = self.items | IT_NAILS;
	}
	else if (self.weapon == IT_GRENADE_LAUNCHER)
	{
		self.currentammo = self.ammo_rockets;
		self.weaponmodel = "progs/v_rock.mdl";
		self.weaponframe = 0;
		self.items = self.items | IT_ROCKETS;
	}
	else if (self.weapon == IT_ROCKET_LAUNCHER)
	{
		self.currentammo = self.ammo_rockets;
		self.weaponmodel = "progs/v_rock2.mdl";
		self.weaponframe = 0;
		self.items = self.items | IT_ROCKETS;
	}
	else if (self.weapon == IT_LIGHTNING)
	{
		self.weaponframe = 0;
		self.items = self.items | IT_CELLS;
		
		/* Magic makes Thunderbolt disappear */
		if(self.player_flags & PF_MAGIC)
			self.weaponmodel = "";
		else
			self.weaponmodel = "progs/v_light.mdl";
		
		self.currentammo = self.ammo_cells;
		
	}
	else
	{
		self.currentammo = 0;
		self.weaponmodel = "";
		self.weaponframe = 0;
	}
};

float() W_BestWeapon =
{
	local   float   it;

 	if ((self.playerclass == CL_SHOCK) && (self.weapon == IT_LIGHTNING))
	{
//	  self.ammo_cells = 1;
	  self.attack_finished = time + 0.4;
          player_run();
	  return IT_LIGHTNING;
	}

	it = self.items;

        if (self.player_flags & PF_MAGIC)
        {
          return IT_LIGHTNING;
        }

	if (self.waterlevel <= 1 && self.ammo_cells >= 1 && (it & IT_LIGHTNING) )
        {
                self.tb_mode = 0;
                if (self.ammo_cells >= 40)
                  self.tb_mode = 1;
		return IT_LIGHTNING;
	}

	else if(self.ammo_nails >= 2 && (it & IT_SUPER_NAILGUN) )
        {
                self.sng_mode = 0;
                
                if (self.playerclass == CL_HWEAP)
                {
                  if (self.ammo_nails < 4)
                    self.sng_mode = 2;
                }
                if ((self.ammo_nails < 3) && (self.sng_mode == 1))
                  self.sng_mode = 0;

		return IT_SUPER_NAILGUN;
        }

	else if(self.ammo_shells >= 2 && (it & IT_SUPER_SHOTGUN) )
		return IT_SUPER_SHOTGUN;

	else if(self.ammo_nails >= 1 && (it & IT_NAILGUN) )
		return IT_NAILGUN;

	else if(self.ammo_shells >= 1 && (it & IT_SHOTGUN)  )
        {
                self.sg_mode = 0;
		return IT_SHOTGUN;
        }

	else if(self.ammo_rockets >= 1 && (it & IT_ROCKET_LAUNCHER) )
        {
                self.rl_mode = 0;
		return IT_ROCKET_LAUNCHER;
        }

	else if(self.ammo_rockets >= 1 && (it & IT_GRENADE_LAUNCHER) )
        {
                self.gl_mode = 0;
                if (self.playerclass == CL_SPECIAL)
                  self.gl_mode = 3;
		return IT_GRENADE_LAUNCHER;
        }

        self.axe_mode = 1;	// try hook
	return IT_AXE;
};

float() W_CheckNoAmmo =
{
	if (self.currentammo > 0)
		return TRUE;

	if (self.weapon == IT_AXE)
		return TRUE;

	if ((self.weapon == IT_LIGHTNING) && (self.player_flags & PF_MAGIC))
	  return TRUE;	

	self.weapon = W_BestWeapon ();

	W_SetCurrentAmmo ();
	
// drop the weapon down
	return FALSE;
};

/*
============
W_Attack

An attack impulse can be triggered now
============
*/
void()  player_axe1;
void()  player_axeb1;
void()  player_axec1;
void()  player_axed1;
void()  player_fe_axe1;
void()  player_hook1;
void()  player_shot1;
void()  player_hw_att1;
void()  player_nail1;
void()  player_hw_nail1;
void()  player_light1;
void()  player_rocket1;
void()  player_demon_atta1;
void() player_berserk_attack;

void() W_Attack =
{
	local float r;

	if(self.player_flags & PF_MISSILE)
  	  return;

	if(self.help)
		return;

	if(!W_CheckNoAmmo())
		return;

	if (self.player_flags & PF_DEMON)
	{
		player_demon_atta1();
		return;
	}
	else if(self.player_flags & PF_DOGGIE)
		return;

	makevectors     (self.v_angle);                 // calculate forward angle for velocity
	self.show_hostile = time + 1;   // wake monsters up

	if (self.weapon == IT_AXE)
	{
		self.attack_finished = time + 0.5;
	
		if (self.axe_mode == 1)
		{
			player_hook1();
			return;
		}

		// Berserk plays different sounds
		if(self.playerclass == CL_BERSERK)
		{
			player_berserk_attack();
			return;
		}
		else
			sound (self, CHAN_WEAPON, "weapons/ax1.wav", 1, ATTN_NORM);
		if (self.playerclass == CL_FIREELEM)
                { player_fe_axe1(); return; }

		r = random();
		if(r < 0.25)
			player_axe1();
		else if(r<0.5)
			player_axeb1();
		else if(r<0.75)
			player_axec1();
		else
			player_axed1();
	}
	else if (self.weapon == IT_SHOTGUN)
	{
		W_FireSG ();
	}
	else if (self.weapon == IT_SUPER_SHOTGUN)
	{
		if (self.playerclass != CL_HWEAP)
			player_shot1();
		else
			player_hw_att1();
		W_FireSuperShotgun();
	}
	else if (self.weapon == IT_NAILGUN)
	{
		if (self.playerclass != CL_HWEAP)
			player_nail1 ();
		else
			player_hw_nail1();
	}
	else if (self.weapon == IT_SUPER_NAILGUN)
	{
		if (self.playerclass != CL_HWEAP)
			player_nail1 ();
		else if (self.flags & FL_ONGROUND) // Heavy weapons characters must be on ground to fire
		{
			if (self.sng_mode == 2)
				player_hw_nail1();
			else if (self.sng_mode == 3)
				W_FireCannon();
		}
		else
			return;
               
	}
	else if (self.weapon == IT_GRENADE_LAUNCHER)
	{
		W_FireGL();
	}
	else if (self.weapon == IT_ROCKET_LAUNCHER)
	{
		W_FireRL();
	}
	else if (self.weapon == IT_LIGHTNING)
	{
		W_FireTB();
	}
};

/*
============
W_ChangeWeapon

============
*/
void() W_ChangeWeapon =
{
	local   float   it, am, fl;
	
	it = self.items;
	am = 0;
	
	if (self.impulse == 1)
	{
		fl = IT_AXE;
	}
	else if (self.impulse == 2)
	{
		fl = IT_SHOTGUN;
		if (self.ammo_shells < 1)
			am = 1;
	}
	else if (self.impulse == 3)
	{
		fl = IT_SUPER_SHOTGUN;
		if (self.ammo_shells < 2)
			am = 1;
	}               
	else if (self.impulse == 4)
	{
		fl = IT_NAILGUN;
		if (self.ammo_nails < 1)
			am = 1;
	}
	else if (self.impulse == 5)
	{
		fl = IT_SUPER_NAILGUN;
		if (self.ammo_nails < 2)
			am = 1;
	}
	else if (self.impulse == 6)
	{
		fl = IT_GRENADE_LAUNCHER;
		if (self.ammo_rockets < 1)
			am = 1;
	}
	else if (self.impulse == 7)
	{
		fl = IT_ROCKET_LAUNCHER;
		if (self.ammo_rockets < 1)
			am = 1;
	}
	else if (self.impulse == 8)
	{
		fl = IT_LIGHTNING;
		if (self.ammo_cells < 1)
			am = 1;
	}

	self.impulse = 0;
	
	if (!(self.items & fl))
	{       // don't have the weapon or the ammo
		sprint (self,  "No weapon.\n");
		return;
	}
	
	if (am)
	{       // don't have the ammo
		sprint (self,  "Not enough ammo.\n");
		return;
	}

//
// set weapon, set ammo
//
	self.weapon = fl;               
	W_SetCurrentAmmo ();
};

/*
============
CheatCommand
============
*/
void() CheatCommand =
{
//      if (deathmatch || coop)
		return;

	self.ammo_rockets = 100;
	self.ammo_nails = 200;
	self.ammo_shells = 100;
	self.items = self.items | 
		IT_AXE |
		IT_SHOTGUN |
		IT_SUPER_SHOTGUN |
		IT_NAILGUN |
		IT_SUPER_NAILGUN |
		IT_GRENADE_LAUNCHER |
		IT_ROCKET_LAUNCHER |
		IT_KEY1 | IT_KEY2;

	self.ammo_cells = 200;
	self.items = self.items | IT_LIGHTNING;

	self.weapon = IT_ROCKET_LAUNCHER;
	self.impulse = 0;
	W_SetCurrentAmmo ();
};

/*
============
CycleWeaponCommand

Go to the next weapon with ammo
============
*/
void() CycleWeaponCommand =
{
	local   float   it, am;
	
	it = self.items;
	self.impulse = 0;

	while (1)
	{
		am = 0;

		if (self.weapon == IT_LIGHTNING)
		{
			self.weapon = IT_AXE;
		}
		else if (self.weapon == IT_AXE)
		{
			self.weapon = IT_SHOTGUN;
			if (self.ammo_shells < 1)
				am = 1;
		}
		else if (self.weapon == IT_SHOTGUN)
		{
			self.weapon = IT_SUPER_SHOTGUN;
			if (self.ammo_shells < 2)
				am = 1;
		}               
		else if (self.weapon == IT_SUPER_SHOTGUN)
		{
			self.weapon = IT_NAILGUN;
			if (self.ammo_nails < 1)
				am = 1;
		}
		else if (self.weapon == IT_NAILGUN)
		{
			self.weapon = IT_SUPER_NAILGUN;
			if (self.ammo_nails < 2)
				am = 1;
		}
		else if (self.weapon == IT_SUPER_NAILGUN)
		{
			self.weapon = IT_GRENADE_LAUNCHER;
			if (self.ammo_rockets < 1)
				am = 1;
		}
		else if (self.weapon == IT_GRENADE_LAUNCHER)
		{
			self.weapon = IT_ROCKET_LAUNCHER;
			if (self.ammo_rockets < 1)
				am = 1;
		}
		else if (self.weapon == IT_ROCKET_LAUNCHER)
		{
			self.weapon = IT_LIGHTNING;
			if (self.ammo_cells < 1)
				am = 1;
		}
	
		if ( (self.items & self.weapon) && am == 0)
		{
			W_SetCurrentAmmo ();
			return;
		}
	}

};


/*
============
CycleWeaponReverseCommand

Go to the prev weapon with ammo
============
*/
void() CycleWeaponReverseCommand =
{
	local   float   it, am;
	
	it = self.items;
	self.impulse = 0;

	while (1)
	{
		am = 0;

		if (self.weapon == IT_LIGHTNING)
		{
			self.weapon = IT_ROCKET_LAUNCHER;
			if (self.ammo_rockets < 1)
				am = 1;
		}
		else if (self.weapon == IT_ROCKET_LAUNCHER)
		{
			self.weapon = IT_GRENADE_LAUNCHER;
			if (self.ammo_rockets < 1)
				am = 1;
		}
		else if (self.weapon == IT_GRENADE_LAUNCHER)
		{
			self.weapon = IT_SUPER_NAILGUN;
			if (self.ammo_nails < 2)
				am = 1;
		}
		else if (self.weapon == IT_SUPER_NAILGUN)
		{
			self.weapon = IT_NAILGUN;
			if (self.ammo_nails < 1)
				am = 1;
		}
		else if (self.weapon == IT_NAILGUN)
		{
			self.weapon = IT_SUPER_SHOTGUN;
			if (self.ammo_shells < 2)
				am = 1;
		}               
		else if (self.weapon == IT_SUPER_SHOTGUN)
		{
			self.weapon = IT_SHOTGUN;
			if (self.ammo_shells < 1)
				am = 1;
		}
		else if (self.weapon == IT_SHOTGUN)
		{
			self.weapon = IT_AXE;
		}
		else if (self.weapon == IT_AXE)
		{
			self.weapon = IT_LIGHTNING;
			if (self.ammo_cells < 1)
				am = 1;
		}
	
		if ( (it & self.weapon) && am == 0)
		{
			W_SetCurrentAmmo ();
			return;
		}
	}

};


/*
============
ServerflagsCommand

Just for development
============
*/
void() ServerflagsCommand =
{
	serverflags = serverflags * 2 + 1;
};


/*
============
W_WeaponFrame

Called every frame so impulse events can be handled as well as possible
============
*/
void() W_WeaponFrame =
{
	local float speedcap;
	if ((self.player_flags & PF_SPEED) && (!(self.player_flags & PF_MISSILE)))
	{
		makevectors(self.v_angle);
		if (self.player_flags & PF_FLY) 
			speedcap = 340;
		else
#ifdef GAME_CTF
		if (self.player_flags & PF_HAS_FLAG)
			speedcap = 375;
		else
#endif
		speedcap = 430;
	  
		self.velocity = v_forward * speedcap;
	}          

	if ((self.player_flags & PF_MISSILE) && (self.velocity == '0 0 0'))
	  self.velocity = '0 0 -100';

        TargetID();

	ParseImpulse ();

	if (time < self.attack_finished)
		return;


// check for attack
        if (((self.button0) && 
            (self.axe_mode == 1)) && ((self.weapon == IT_AXE)
             && (self.player_flags & PF_HOOK)))
          return;

	if (self.button0)
	{
		SuperDamageSound ();
		W_Attack ();
	}
};

/*
========
SuperDamageSound

Plays sound if needed
========
*/
void() SuperDamageSound =
{
	if (self.super_damage_finished > time)
	{
		if (self.super_sound < time)
		{
			self.super_sound = time + 1;
			sound (self, CHAN_BODY, "items/damage3.wav", 1, ATTN_NORM);
		}
	}
	return;
};


