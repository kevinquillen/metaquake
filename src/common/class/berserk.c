/*************************************************************************
 * Metamorphism Quake Server: common/class/berserk.c,                    *
 *                            common/class/berserk.qc                    *
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
	This implements the animation framing using the Hell Knight. Refer to
	Quake v1.06 source "hknight.qc" for the complete framing.
*/
 
#include "meta.h"
 
//Stand animation
#define HKNIGHTFRAME_STAND_BASE 	0
#define HKNIGHTFRAME_STAND_LENGTH	9

//Die animation
#define HKNIGHTFRAME_DIE_BASE	42
#define HKNIGHTFRAME_DIE_LENGTH	12

//Run animation
#define HKNIGHTFRAME_RUN_BASE	29
#define HKNIGHTFRAME_RUN_LENGTH	8

//Attack animation 1
#define HKNIGHTFRAME_ATTACK1_BASE	112
#define HKNIGHTFRAME_ATTACK1_LENGTH	10
#define HKNIGHTFRAME_ATTACK1_SWINGSTART	5	//frame when attack actually goes off, relative to base frame
#define HKNIGHTFRAME_ATTACK1_SWINGEND	8	//last frame a swing occurs in
//Attack animation 2
#define HKNIGHTFRAME_ATTACK2_BASE	122
#define HKNIGHTFRAME_ATTACK2_LENGTH	11

//Pain
#define HKNIGHTFRAME_PAIN_BASE		37
#define HKNIGHTFRAME_PAIN_LENGTH	5

//Prototypes local to this module
void() berserk_die;
void() berserk_attack1;
void() berserk_attack2;
void() berserk_swing;
void() berserk_pain;

//Prototypes from other modules
void() PlayerDead;
void() player_stand1;


// Public functions
//=============================================================================


//Standing animation, called by player_stand1() in common/player.c
void() player_berserk_stand = 
{
	//Need to wrap stand animation?
	if(self.walkframe >= HKNIGHTFRAME_STAND_LENGTH) 
		self.walkframe = 0;
	self.frame = HKNIGHTFRAME_STAND_BASE + self.walkframe;	
};

//Death animation
void() player_berserk_die =
{
	self.walkframe = 0;
	berserk_die();
};

void() player_berserk_run =
{
	//Need to wrap run animation?
	if(self.walkframe >= HKNIGHTFRAME_RUN_LENGTH)
		self.walkframe = 0;
		
	self.frame = HKNIGHTFRAME_RUN_BASE + self.walkframe;
};

void() player_berserk_attack =
{
	local float r;
	self.walkframe = 0;
	
	//Play random swinging sound
	r = random();
	if(r < 0.33)
		sound(self, CHAN_WEAPON, "hknight/slash1.wav", 1, ATTN_NORM);
	else if(r < 0.66)
		sound(self, CHAN_WEAPON, "knight/sword1.wav", 1, ATTN_NORM);
	else
		sound(self, CHAN_WEAPON, "knight/sword2.wav", 1, ATTN_NORM);

	//Choose one of the two animations
	if(random() < 0.5)
		berserk_attack1();
	else
		berserk_attack1(); //TODO: set up framing for animation #2

	self.attack_finished = time + 0.7;		
};

void(float damage) player_berserk_pain =
{		
	//Play a hurt sound
	sound(self, CHAN_VOICE, "hknight/pain1.wav", 1, ATTN_NORM);
	
	//Yup, he's hardcore, okay.
	if(damage < 40)
		return;
	
	
	self.walkframe = 0;
	berserk_pain();
};

// End public functions
//=============================================================================


void() berserk_die =
{
	//At the end of the animation...
	if(self.walkframe >= HKNIGHTFRAME_DIE_LENGTH)
	{
		//Mark the player as dead so s/he can respawn
		PlayerDead();
		return;
	}

	//Set current frame
	self.frame = HKNIGHTFRAME_DIE_BASE + self.walkframe;

	//Advance frame
	self.walkframe = self.walkframe + 1;
	
	self.think = berserk_die;
	self.nextthink = time;
};

void() berserk_attack1 =
{
	//At the end of the animation...
	if(self.weaponframe >= HKNIGHTFRAME_ATTACK1_LENGTH)
	{
		self.weaponframe = 0;
		self.think = player_stand1;
		self.nextthink = time;
		return;
	}

	//Set current frame
	self.frame = HKNIGHTFRAME_ATTACK1_BASE + self.weaponframe;

	//Right before swinging the sword, show the camera winding up...
	if(self.weaponframe == HKNIGHTFRAME_ATTACK1_SWINGSTART - 1)
		self.punchangle_y = 5;
	
	if(self.weaponframe >= HKNIGHTFRAME_ATTACK1_SWINGSTART && self.weaponframe <= HKNIGHTFRAME_ATTACK1_SWINGEND)
		berserk_swing();
	
	//Advance frame
	self.weaponframe = self.weaponframe + 1;
		
	self.think = berserk_attack1;
	self.nextthink = time;
};

void() berserk_attack2 =
{
};

void() berserk_pain =
{
	//At the end of the animation...
	if(self.walkframe >= HKNIGHTFRAME_PAIN_LENGTH)
	{
		player_stand1();
		return;
	}

	//Set current frame
	self.frame = HKNIGHTFRAME_PAIN_BASE + self.walkframe;

	//Advance frame
	self.walkframe = self.walkframe + 1;
	
	
	self.think = berserk_pain;
	self.nextthink = time;	
};

//Function that actually traces a melee swing and does damage
void() berserk_swing =
{
	local vector org;
  
	makevectors (self.v_angle);
	org = self.origin + '0 0 4';
	traceline (org, org + v_forward*72, FALSE, self);
	if (trace_fraction == 1.0)
		return;

	org = trace_endpos - v_forward * 4;
	if (trace_ent.takedamage)
	{
		T_Damage(trace_ent, self, self, 20+10*random(), SH_UNKNOWN);
	}
	
};

