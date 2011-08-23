/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/magic.h                       *
 * Copyright (C) 1996-2000 by Lon Hohberger.                             *
 * Portions (C) 2011 by Patrick Baggett                                  *
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
	Moved from magic.c as global to C preprocessor symbol since they are
	allowed.
 */
 
#ifndef __META_MAGIC_H
#define __META_MAGIC_H
 
#define NUM_MAGE_SPELLS 		9

#define NUM_DRUID_SPELLS		7

#define NUM_NINJA_SPELLS		4
#define NUM_LICH_SPELLS			11

//Mage spell numbers
#define MAGE_SPELL_CONFUSION	9

//Fire Elemental spell numbers
#define NUM_FIREELEM_SPELLS 		5
#define FIREELEM_SPELL_STASIS		0
#define FIREELEM_SPELL_BEAM			1
#define FIREELEM_SPELL_FIRESTORM	2
#define FIREELEM_SPELL_INFERNO		3
#define FIREELEM_SPELL_MELTDOWN		4


//Cleric spell numbers
#define NUM_CLERIC_SPELLS		12
#define CLERIC_SPELL_SANCT		0	//Sanctuary
#define CLERIC_SPELL_RUPTURE	1	//Rupture armor
#define CLERIC_SPELL_HEAL		2	//Heal
#define CLERIC_SPELL_GRHEAL		3	//Group Heal
#define CLERIC_SPELL_DRAIN		4	//Drain
#define CLERIC_SPELL_DIVINITY	5	//Divinity
#define CLERIC_SPELL_HOLYMISS	6	//Holy Missile
#define CLERIC_SPELL_MVESTMENT	7	//Magical Vestment
#define CLERIC_SPELL_CHECKTARG	8	//Check Target
#define CLERIC_SPELL_DISPELEVIL 9	//Dispel Evil
#define CLERIC_SPELL_AID		10	//Aid
#define CLERIC_SPELL_LOCK		11	//Lock target

// Casting mode =======================================
#define CASTMODE_SELF	0
#define CASTMODE_TARGET	1

// Spells in effect =======================================
#define SPELLFX_REGEN		1			//Regeneration (Druid)
#define SPELLFX_SANCT		2			//Sanctuary (Cleric)
#define SPELLFX_VITALITY	3			//Vitality (Mage)
#define SPELLFX_DEMON		4			//Demon Morph (Druid)
#define SPELLFX_SANCTUARY	5			//Sanctuary (Cleric)
#define SPELLFX_CONFUSION	6			//Confusion (Mage)
#define SPELLFX_DRAIN		7			//Drain (Cleric)

#define SPELL_NOEXPIRE	-1		//When used as the 'duration' argument for Magic_AddEffect(), the spell does not expire

#endif

