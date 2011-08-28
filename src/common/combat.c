/*
 * Common/combat.c, Common/combat.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

#include "meta.h"

float (entity one, entity two) SameTeam;

void() T_MissileTouch;
void() info_player_start;
//void(entity targ, entity attacker, entity inflictor) ClientObituary;
void(entity targ, entity attacker, entity inflictor, float dam, float dtype) DeathMsg;

//============================================================================

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
float(entity targ, entity inflictor) CanDamage =
{
// bmodels need special checking because their origin is 0,0,0
	if (targ.movetype == MOVETYPE_PUSH)
	{
		traceline(inflictor.origin, 0.5 * (targ.absmin + targ.absmax), TRUE, self);
		if (trace_fraction == 1)
			return TRUE;
		if (trace_ent == targ)
			return TRUE;
		return FALSE;
	}
	
	traceline(inflictor.origin, targ.origin, TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '-15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '-15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;

	return FALSE;
};


/*
============
Killed
============
*/
void(entity targ, entity attacker, entity inflictor, float dam, float dtype) Killed =
{
	local entity oself;

	oself = self;
	self = targ;
	self.button1 = 0;

	if (self.health < -99)
		self.health = -99;		// don't let sbar look bad if a player

	if (self.movetype == MOVETYPE_PUSH || self.movetype == MOVETYPE_NONE)
	{	// doors, triggers, etc
		self.th_die ();
		self = oself;
		return;
	}

	self.enemy = attacker;
	self.spawn_origin = inflictor.origin;
	self.max_health = dam;

        DeathMsg(targ, attacker, inflictor, dam, dtype);
#ifdef QUAKEWORLD
#ifdef SPREEMSG
	SpreeMsg(attacker);
#endif
#endif

	
	self.takedamage = DAMAGE_NO;
	self.touch = SUB_Null;

	self.th_die ();
	
	self = oself;
};

//void() DoBlink;
/*
============
T_Damage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
============
*/
void(entity targ, entity inflictor, entity attacker, float damage, float dtype) T_Damage =
{
	local vector dir;
	local entity oldself;
	local float save;
	local float take;
	local float temp;

	if (!targ.takedamage)
		return;

	// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;

	if(inflictor.netname == "Call of Raiden")
		attacker = inflictor.owner;

	// check for godmode or invincibility
	if (targ.invincible_finished >= time)
	{
		if (self.invincible_sound < time)
		{
			sound (targ, CHAN_ITEM, "items/protect3.wav", 1, ATTN_NORM);
			self.invincible_sound = time + 2;
		}
	}
	else
	{

		// team play damage avoidance
		if (teamplay & TP_ON)		// teamplay must be on
		{
			temp = SameTeam(attacker,targ);
			if((teamplay & TP_TEAMSAFE) && (temp) && (attacker != targ))
				return;

			if ((teamplay & TP_SELFSAFE) && (attacker == targ))
				return;  

			if ((teamplay & TP_MIRRORDAMAGE) && (temp) && (attacker != targ))
				targ = attacker;		// do damage to self!
		}

		//Non-armor piercing rounds / damages other than magic
		if(inflictor.classname != "armor_pierce" && dtype != SH_MAGIC)
		{
			save = ceil(targ.armortype*damage);
			if(save >= targ.armorvalue)
			{
				save = targ.armorvalue;
				targ.armortype = 0;	// lost all armor
				targ.items = targ.items - (targ.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3));
			}
	
			targ.armorvalue = targ.armorvalue - save;
			take = ceil(damage-save);
		}
		else
			take = ceil(damage);

		if(targ.flags & FL_CLIENT)
		{
			targ.dmg_take = targ.dmg_take + take;
			targ.dmg_save = targ.dmg_save + save;
			targ.dmg_inflictor = inflictor;
		}

		targ.health = targ.health - take;
			
		if(targ.health <= 0)
		{
			Killed (targ, attacker, inflictor, damage, dtype);
			return;
		}
	} // end of "else not invicibile"
	

	// KNOCKBACK CALCULATION
	//==========================

	// don't react if it is the grappling hook or lich attacks or quake
	if(inflictor.classname == "hook")
		return;
	if(inflictor.classname == "blaze")
		return;

	//In demon/dog morph, don't knockback
	if (targ.player_flags & PF_DEMON)
		return;
	if (targ.player_flags & PF_DOGGIE)
		return;

	// Snipers don't move when using sniper rifle.
	//if (((targ.playerclass == CL_SNIPER) && (targ.weapon == IT_SHOTGUN)) && (targ.sg_mode == 0))
          //return;

// figure momentum add
	if ( (inflictor != world) && (targ.movetype == MOVETYPE_WALK) )
	{
		
		if (targ.pausetime < time + 0.2)
                  targ.pausetime = 0;
		dir = targ.origin - (inflictor.absmin + inflictor.absmax) * 0.5;
		dir = normalize(dir);
		targ.velocity = targ.velocity + dir * temp * 8;
	}


// react to the damage
	oldself = self;
	self = targ;

	if (self.th_pain)
	{
		self.th_pain (attacker, take);
	// nightmare mode monsters don't go into pain frames often
	}

	self = oldself;
};

/*
============
T_RadiusDamage
============
*/
void(entity inflictor, entity attacker, float damage, entity ignore, float damagetype) T_RadiusDamage =
{
	local   float   points;
	local   entity  head;
	local   vector  org;

	head = findradius(inflictor.origin, damage+40);
	
	while (head)
	{
		if (head != ignore)
		{
			if (head.takedamage)
			{
				org = head.origin + (head.mins + head.maxs)*0.5;
				points = 0.5*vlen (inflictor.origin - org);
				if (points < 0)
					points = 0;
				points = damage - points;
				
				if ((head == attacker) && (damagetype == SH_EXPLOSION))
					points = points * 0.7;
				if (points > 0)
				{
					if (CanDamage (head, inflictor))
					{
						DoDamage (head, inflictor, attacker, points, damagetype);
					}
				}
			}
		}
		head = head.chain;
	}
};

/*
============
T_AreaDamage
============
Like T_RadiusDamage() but with no damage attenutation and explicit radius
*/
void(entity inflictor, entity attacker, float damage, float radius, entity ignore, float damagetype) T_AreaDamage =
{
	local entity head;

	head = findradius(inflictor.origin, radius);
	
	while (head)
	{
		if(head != ignore)
		{
			if(head.takedamage)
				DoDamage (head, inflictor, attacker, damage, damagetype);
		}
		head = head.chain;
	}
};

/*
============
T_BeamDamage
============
*/
void(entity attacker, float damage) T_BeamDamage =
{
	local   float   points;
	local   entity  head;
	
	head = findradius(attacker.origin, damage+40);
	
	while (head)
	{
		if (head.takedamage)
		{
			points = 0.5*vlen (attacker.origin - head.origin);
			if (points < 0)
				points = 0;
			points = damage - points;
			if (head == attacker)
				points = points * 0.5;
			if (points > 0)
			{
				if (CanDamage (head, attacker))
					T_Damage (head, attacker, attacker, points,SH_ELECTRICITY);
			}
		}
		head = head.chain;
	}
};
