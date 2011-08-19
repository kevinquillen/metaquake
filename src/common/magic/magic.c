/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/magic.c,                      *
 *                            Common/Magic/magic.qc                      *
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
#include "magic.h"
#include "meta.h"

 /*
	Magic functions. These functions are the "interface" to magic. Only these
	functions are called by the rest of the game code. Each spellcasting class
	implements <Class>SpellName() and <Class>Cast() which are then used by this
	module.
 */

void() MageSpellName;
void() MageCast;
void() ClericSpellName;
void() ClericCast;
void() FireElemSpellName;
void() FireElemCast;
void() DruidSpellName;
void() DruidCast;
void() NinjaSpellName;
void() NinjaCast;
void() LichSpellName;
void() LichCast;

void() player_cast_ax1;
void() player_cast_gun1;

float() SetSpellNum =
{
	if(self.playerclass == CL_MAGE)
		return NUM_MAGE_SPELLS;
	else if(self.playerclass == CL_CLERIC)
		return NUM_CLERIC_SPELLS;
	else if(self.playerclass == CL_DRUID)
		return NUM_DRUID_SPELLS;
	else if(self.playerclass == CL_FIREELEM)
		return NUM_FIREELEM_SPELLS;
	else if(self.playerclass == CL_SILENT)
		return NUM_NINJA_SPELLS;
	else if(self.playerclass == CL_LICH)
		return NUM_LICH_SPELLS;
		
	return 0;
};

void() E_CastSpell =
{
	if(self.attack_finished > time)
		return;

	//Set the casting animation (fire elemental doesn't have one)
	if(self.playerclass != CL_FIREELEM)
	{
		if (self.weapon == IT_AXE)
			player_cast_ax1();
		else
			player_cast_gun1();
	}

	if(self.playerclass == CL_MAGE)
		MageCast();
	else if(self.playerclass == CL_CLERIC)
		ClericCast();
	else if(self.playerclass == CL_DRUID)
		DruidCast();
	else if(self.playerclass == CL_FIREELEM)
		FireElemCast();
	else if(self.playerclass == CL_SILENT)
		NinjaCast();
	else if(self.playerclass == CL_LICH)
		LichCast();
};

void() E_SpellName =
{
	if(self.playerclass == CL_MAGE)
		MageSpellName();
	if(self.playerclass == CL_CLERIC)
		ClericSpellName();
	if(self.playerclass == CL_DRUID)
		DruidSpellName();
	if(self.playerclass == CL_FIREELEM)
		FireElemSpellName();
	if(self.playerclass == CL_SILENT)
		NinjaSpellName();
	if(self.playerclass == CL_LICH)
		LichSpellName();
};

void() E_ShowSpellImpulse =
{
  local string strng;
  local float f;
  sprint(self,"");
  strng = ftos(self.spell + 140);
  sprint(self,strng);
  sprint(self,"‘ ");
  if (self.attack_finished > time)
  {
    sprint(self,"…W… ");
  }
};

void() E_NextSpell =
{
  self.spell = self.spell + 1;
  if (self.spell > SetSpellNum())
    self.spell = 0;
  E_ShowSpellImpulse();
  E_SpellName();
  self.impulse = 0;
};

void() E_PrevSpell =
{
  self.spell = self.spell - 1;
  if (self.spell < 0)
    self.spell = SetSpellNum();
  E_ShowSpellImpulse();
  E_SpellName();
  self.impulse = 0;
};

void() E_CastMagic =
{
  local float temp;
  temp = self.spell;
  self.spell = self.impulse - 140;
  E_ShowSpellImpulse();
  E_SpellName();
  E_CastSpell();
  self.spell = temp;
  self.impulse = 0;
};

void() E_CastMode =
{
  self.tb_mode = !self.tb_mode;
  if (self.tb_mode == 1)
    sprint(self,"Spell Mode: Directed\n");
  else
    sprint(self,"Spell Mode: Self\n");

};


//=======================================================================
// SPELL EFFECTS SYSTEM
//=======================================================================
/*
	This system keeps track of various spells affecting the player.
	It provides a simple way to add duration-based spells/effects. The
	spell effects are typeless entities that are linked via linked list
	to the entity that they are connected to. The following fields are
	used by the code:
	
	.owner -- Player who the effect is attached to
	.enemy -- Player who cast the spell
	.chain -- Next spell effect in the linked list, or world if none
	.think -- Function called automatically by Quake.
	.use -- Function called when spell expires.
	.nextthink -- When to call think() function
	.attack_finished -- When to remove this spell effect, or SPELL_NOEXPIRE.
	.netname -- Name displayed when the spell expires, or when killed by
	            a DoT spell.
	.voided -- Set to non-zero when the spell expires.
*/

//Check if an entity already has a spell effect. If so, return it
//otherwise return 'world'
entity(entity e, float spelleffect) Magic_HasEffect = 
{
	local entity head;
	
	head = e.spell_effects;
	
	while(head != world)
	{
		//Match?
		if(head.spell == spelleffect)
			return head;
		
		head = head.chain;
	}

	return world;
};

//Silently remove all spell effects from an entity. The effects
//are removed immediately using remove(). Their expiration function
//is called before they are removed.
void(entity e) Magic_RemoveAll =
{
	local entity head;
	local entity nexteff;
	local entity oldself;
	
	head = e.spell_effects;
	while(head != world)
	{
		nexteff = head.chain;
		head.voided = 1;
		
		//Call use() in context of 'head'
		oldself = self;
		self = head;
		head.use();
		self = oldself;
		
		remove(head);
		
		head = nexteff;
	}
	e.spell_effects = world;
};

//Add a spell effect to a given entity. If the spell effect already exists,
//then update its duration.
void(entity spelltarget, entity caster, float spelleffect, float duration) Magic_AddEffect =
{
	local entity prev;
	local entity fx;
	
	//Does the player already have this spell?
	fx = Magic_HasEffect(spelltarget, spelleffect);
	if(fx != world)
	{
		//Yes, so just update the duration
		if(duration == SPELL_NOEXPIRE)
			fx.attack_finished = SPELL_NOEXPIRE;
		else
			fx.attack_finished = time + duration;
		return;
	}
	
	//No, add a new effect
	fx = spawn();
		
	//No root?
	if(spelltarget.spell_effects == world)
		spelltarget.spell_effects = fx;
	else //Find end of spell effects chain
	{
		prev = spelltarget.spell_effects;
		
		while(prev.chain != world)
			prev = prev.chain;
		
		//Link
		prev.chain = fx;
	}
	
	
	//Set up spell effect
	fx.owner = spelltarget;
	fx.enemy = caster;
	SpellFX_Init(fx, spelleffect, duration);
};

//Checks if an entity has a spell effect that needs to be removed. This is
//called once per frame.
void(entity e) Magic_CheckExpired =
{
	local entity fx;
	local entity prev;
	local entity oldself;
	
	//No spell effects;
	if(e.spell_effects == world)
		return;

	//Dead player -> remove all effects
	if(e.deadflag)
	{
		Magic_RemoveAll(e);
		return;
	}

	fx = e.spell_effects;
	prev = world;
	
	while(fx)
	{
		//Spell expired?
		if(fx.attack_finished < time)
		{
			if(prev == world) //i.e removing head element
				e.spell_effects = fx.chain;
			else
				prev.chain = fx.chain;	//Remove 'fx' from the chain
			
			//Say who cast the spell
			if(fx.owner == fx.enemy)
			{
				sprint(fx.owner, "Your ");
				sprint(fx.owner, fx.netname);
				sprint(fx.owner, " spell has expired.\n");
			}
			else
			{
				sprint(fx.owner, "The ");
				sprint(fx.owner, fx.netname);
				sprint(fx.owner, " spell cast by ");
				sprint(fx.owner, fx.enemy.netname);
				sprint(fx.owner, " has expired.\n");
			}

			
			//Remove next frame
			fx.voided = 1;
			
			//Call use() in context of 'fx'
			oldself = self;
			self = fx;
			fx.use();
			self = oldself;
			
			fx.think = SUB_Remove;
			fx.nextthink = time;
		}
		
		//Next effect
		prev = fx;
		fx = fx.chain;		
	}//while(fx is not world)	
	
};

//==============================================================================
//Displays a message when 'resistor' resists 'caster's spell.
void(entity resistor, entity caster) ResistMsg =
{
	//Let the caster know the target resisted the spell
	sprint(caster, resistor.netname);
	sprint(caster, " resists your spell.\n");
		
	//Let the other player know they resisted the spell
	sprint(resistor, "You resist the spell cast by ");
	sprint(resistor, caster.netname);
	sprint(resistor, ".\n");

};

//Performs a magic save vs spell.
// resistor - The entity
// chance - The default chance to save (e.g. 0.25 = 25% chance to save)
// Returns true if they resist, false if they fail their resistance
float(entity resistor, float chance) CheckResist =
{
	local float final_chance;
	
	final_chance = chance;
	
	if(resistor.saves & SH_MAGIC)
	{
		final_chance = final_chance + 0.25;
	}
	else if(resistor.harms & SH_MAGIC)
	{
		final_chance = final_chance - 0.25;
	}

	//Always 5% chance to fail resistance check, always at least 5% chance to succeed
	if(final_chance > 0.95)
		final_chance = 0.95;
	else if(final_chance < 0.05)
		final_chance = 0.05;

	if(random() < final_chance)
		return 1;

	return 0;
};

float(entity caster, float amount) CheckMana =
{
	
	if(caster.ammo_cells >= amount)
	{
		caster.ammo_cells = caster.ammo_cells - amount;
		return 1;
	}

	//Not enough mana. Set the attack_finished field so we don't
	//get spammed with the message.
	sprint(caster, "Insufficient mana!\n");
	caster.attack_finished = time + 0.5;
	
	return 0;	
};

//Checks if the caster's target is one that can be helped (i.e. on the same team)
//Returns zero if the spell should not be cast on this target (helpful on enemy)
float() CheckHelpfulSpellTarget =
{
	if(!SameTeam(self, self.enemy))
	{
		sprint(self, "You cannot cast this spell on enemies!");
		return 0;
	}

	//Not on the same team / teamplay disabled
	return 1;
};

//Checks if the casters target is one that can be harmed (i.e. not on the same team or friendly fire enabled)
//Returns zero if the spell should not be cast on this target (harmful on ally)
float() CheckHarmfulSpellTarget =
{
	if(SameTeam(self, self.enemy))
	{
		sprint(self, "You cannot cast this spell on teammates!");
		return 0;
	}

	//On same team
	return 1;
};