/*************************************************************************
 * Metamorphism Quake Server: common/class/hweap.c,                      *
 *                            common/class/hweap.qc                      *
 * Copyright (C) 2011 by Patrick Baggett                                 *
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
	This implements the animation framing using the Enforcer. Refer to
	Quake v1.06 source "enforcer.qc" for the complete framing.
	
	The "Heavy Weapons Guy" (hweap) uses the enforcer model, thus the 
	functions refer to "hweap" rather than "enforcer"
*/

#include "meta.h"

#define ENFORCER_PAIN_BASE 70
#define ENFORCER_PAIN_LENGTH 5

#define ENFORCER_DIE1_BASE		41
#define ENFORCER_DIE1_LENGTH	14

#define ENFORCER_DIE2_BASE		55
#define ENFORCER_DIE2_LENGTH	11

//Prototypes local to this module
void() hweap_pain;
void() hweap_die1;
void() hweap_die2;

//Prototypes from other modules
void() player_stand1;
void() PlayerDead;

// Public functions
//=============================================================================

void(float damage) player_hweap_pain =
{
	local string painnoise;

	//Don't go into pain animation for small stuff
	if(damage < 15)
		return;
	
	if(random() < 0.5)
		painnoise = "enforcer/pain1.wav";
	else
		painnoise = "enforcer/pain2.wav";
	
	sound(self, CHAN_VOICE, painnoise, 1, ATTN_NORM);
	
	self.walkframe = 0;
	hweap_pain();
};

void() player_hweap_die =
{
	self.walkframe = 0;
	if(random() < 0.5)
		hweap_die1();
	else
		hweap_die2();
	
};

// End public functions
//=============================================================================

void() hweap_pain = 
{
	//Done with pain animation?
	if(self.walkframe >= ENFORCER_PAIN_LENGTH)
	{
		player_stand1();
		return;
	}
	
	//Show pain animation
	self.frame = ENFORCER_PAIN_BASE + self.walkframe;

	//Advance pain animation
	self.walkframe = self.walkframe + 1;

	self.think = hweap_pain;
	self.nextthink = time;	
};

void() hweap_die1 =
{
	//At the end of the animation...
	if(self.walkframe >= ENFORCER_DIE1_LENGTH)
	{
		//Mark the player as dead so s/he can respawn
		PlayerDead();
		return;
	}

	//Set current frame
	self.frame = ENFORCER_DIE1_BASE + self.walkframe;

	//Advance frame
	self.walkframe = self.walkframe + 1;
	
	self.think = hweap_die1;
	self.nextthink = time;
};

void() hweap_die2 =
{
	//At the end of the animation...
	if(self.walkframe >= ENFORCER_DIE2_LENGTH)
	{
		//Mark the player as dead so s/he can respawn
		PlayerDead();
		return;
	}

	//Set current frame
	self.frame = ENFORCER_DIE2_BASE + self.walkframe;

	//Advance frame
	self.walkframe = self.walkframe + 1;
	
	self.think = hweap_die2;
	self.nextthink = time;
};
