/*************************************************************************
 * Metamorphism Quake Server: common/magic/spellfx.c,                    *
 * Copyright (C) 2011 by Patrick Baggett.                                *
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

/*
	This file contains the code for the spell effects implementation,
	i.e. the actual effects themselves, not the infrastructure. The
	only function called outside of this module is SpellFX_Init().

*/

#include "magic.h"
#include "meta.h"

// Regeneration spell. Restores 5 health / second until expired.
void() SpellFX_Regen_Think =
{
	local entity pl;
       
	pl = self.owner;
	
	//Regeneration only helps if under max health
	if(pl.health < pl.max_health)
	{
		pl.health = pl.health + 5;
		if(pl.health > pl.max_health)
			pl.health = pl.max_health;
			
		sound(pl, CHAN_ITEM, "items/r_item1.wav", 1, ATTN_NORM);
	}

	//Recast again in a second.
	self.nextthink = time + 1.0;
};

//Confusion spell: Messed with camera angles
void() SpellFX_Confusion_Think =
{
	self.owner.punchangle_z = self.owner.punchangle_z - 5;
	
	self.nextthink = time + 0.3;
};

//Confusion expire: restore camera angles
void() SpellFX_Confusion_Expire =
{
	self.owner.punchangle = '0 0 0';
	stuffcmd(self.owner, "v_idlescale 0\n");
};

//Drain spell: Drains 10 health / second (1 per 0.1s)
void() SpellFX_Drain_Think =
{

	//Drain a point of health directly (no pain animation)
	self.owner.health = self.owner.health - 1;
	
	//If they reach 1 or less, call DoDamage() so the caster gets a
	//kill (use SH_MAGIC so it ignores armor)
	if(self.owner.health <= 1)
		DoDamage(self.owner, self, self.enemy, 5, SH_MAGIC);

	//Take the drained health and give it to the caster
	if(self.enemy.health < self.enemy.max_health)
		self.enemy.health = self.enemy.health + 1;    

	//If either the caster or the target dies, then mark as done.
	if(self.enemy.deadflag || self.owner.deadflag)
	{
		self.voided = 1;
		return;
	}

	//Next frame in 0.1 secs
	self.nextthink = time + 0.1;
};

//==============================================================================
//This is the only function called directly from outside this module.

//Initialize a spell effect 'fx'
void(entity fx, float spelleffect, float duration) SpellFX_Init =
{
	//Set up duration of spell
	if(duration == SPELL_NOEXPIRE)
		fx.attack_finished = SPELL_NOEXPIRE;
	else
		fx.attack_finished = time + duration;

	//Set up name and functions
	//Reminder: .think() = per frame logic
	//          .use() = destructor, called when spell expires
	if(spelleffect == SPELLFX_REGEN)
	{
		fx.netname = "regeneration";
		fx.think = SpellFX_Regen_Think;
		fx.use = SUB_Null;
	}
	else if(spelleffect == SPELLFX_CONFUSION)
	{
		fx.netname = "confusion";
		fx.think = SpellFX_Confusion_Think;
		fx.use = SpellFX_Confusion_Expire;
	}
	else if(spelleffect == SPELLFX_DRAIN)
	{
		fx.netname = "drain spell";
		fx.think = SpellFX_Drain_Think;
		fx.use = SUB_Null;
	}

	fx.nextthink = time;
	fx.chain = world;
	fx.spell = spelleffect;
};
