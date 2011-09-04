/*************************************************************************
 * Metamorphism Quake Server: Common/damage.c, Common/damage.qc          *
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


/**********************\
 Save/harms, armor, etc
\**********************/

  
float (entity targ, entity inflictor, entity attacker, float damg, float damagetype) SaveHarm =
{
	local float newdam, tmp;
	newdam = damg;

	//Fall damage is direct 50% modification
	if ((damagetype & SH_FALL) && (targ.harms & SH_FALL))
	{
		newdam = ceil(newdam * 1.5);
		return newdam;
	}

	//Weakness means +30% damage
	if(targ.harms & damagetype)
		newdam = ceil(newdam * 1.3);

	//Stop here if the target doesn't save vs this damage type
	if(!(targ.saves & damagetype))
		return newdam;

	// Logic for damage reduction
	//===========================

	//Fire damage, but inflictor (e.g. a fireball) is in water -> 1/2 damage
	if(damagetype & SH_FIRE)
	{
		if(pointcontents(inflictor.origin) == CONTENT_WATER)
			newdam = floor(newdam * 0.5);
		
		//Fire elementals take even less damage from fire (instead of -40% damage, -75%)
		if(targ.playerclass == CL_FIREELEM)
		{
			newdam = floor(newdam * 0.25);
			return newdam;
		}

	}
		
	//Target resists this damage type -> reduce damage by 40%
	if(targ.saves & damagetype)
		newdam = floor(newdam * 0.6);
 
	//Don't hurt self with fire/melee (TODO: is this needed?)
	if((damagetype & (SH_FIRE|SH_UNKNOWN)) && (attacker == targ))
		return 0;

	if(damagetype & SH_FALL)
		return 0;			// resilient person

	return newdam;
};
  
void (entity targ, 
      entity inflictor, 
      entity attacker, 
      float damg, 
      float damtype) DoDamage =
{
	local float newdam;


	newdam = damg;
	
	// check for quad damage powerup on the attacker
	if (attacker.super_damage_finished > time)
	{
		if(damtype == SH_PLASMA) //plasma only gets 2x damage
			newdam = newdam * 2;
		else
			newdam = newdam * 4;
	}
  
	// Check Quad for Liches -- attacker==world, targ==inflictor   TODO: wtf??
	if((attacker==world) && (targ==inflictor))
		newdam = newdam * 4;

	//Don't deal quad damage falls (divide by 4 since by this point, already applied x4 modifier)
	if((damtype == SH_FALL) && (targ.items & IT_QUAD)) 
		newdam = ceil(newdam / 4); //so reduce it back to normal

	//Target protected by sanctuary?
	if (targ.player_flags & PF_SANCTUARY)
	{
		newdam = newdam * 0.8;
	}

	//Apply any damage reduction
	newdam = SaveHarm(targ, inflictor, attacker, newdam, damtype);
	newdam = ceil(newdam);

	//Ninjas can deal critical hits on other players if that player does not have critical hit protection
	if(((((newdam != 0) && (attacker.playerclass == CL_SILENT)) && (attacker != targ)) && (targ.classname == "player")) &&
	(!(targ.saves&SH_CRITHIT)))
	{
		damg = random()*100;
		if (damg < 4)
		{
			newdam = newdam * 4;
			damtype = damtype | SH_CRITHIT;
		}
	}
  
	//Extra damage reduction for the case that a sniper or asniper shoots a scout.
	if((attacker.playerclass == CL_SNIPER || attacker.playerclass == CL_ASNIPER) && targ.playerclass == CL_SCOUT && damtype & SH_SNIPER)
		newdam = newdam * 0.5;
	   

	if (newdam != 0)
		T_Damage(targ, inflictor, attacker, newdam, damtype);
};
