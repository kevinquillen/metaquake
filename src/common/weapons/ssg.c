/*
 * Common/Weapons/ssg.c, Common/Weapons/ssg.qc
 *   Portions Copyright 1996 by id Software
 *   Portions Copyright 1997-2000 by Lon Hohberger
 *   Portions Copyright 2011 by Patrick Baggett
 */

#include "meta.h"
void() player_shot1;
void(float shotcount, vector dir, vector spread, float shotdamage, float dtype) FireBullets;

/*
================
W_FireSSG
================
The classic Quake Super Shotgun
*/
void() W_FireSSG =
{
	local vector dir;

	if (self.currentammo == 1)
	{
		W_FireSG ();
		return;
	}
		
	sound (self ,CHAN_WEAPON, "weapons/shotgn2.wav", 1, ATTN_NORM); 

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_BIGKICK);
#else
	self.punchangle_x = -4;	 
#endif

	self.currentammo = self.ammo_shells = self.ammo_shells - 2;
	dir = aim (self, 100000);
	FireBullets (9, dir, '0.14 0.14 0', 10, SH_BULLETS);

	self.attack_finished = time + 0.8;
};

/*
================
W_FireSawedSG
================
Sawed-barrel shotgun. Uses 4 shells, more bullets, and a larger spread.
*/
void() W_FireSawedSG =
{
	local vector dir;

	/* Fire SSG instead */
	if(self.currentammo < 4 && self.currentammo >= 2)
	{
		W_FireSSG();
		return;
	} else if(self.currentammo == 1)
	{
		W_FireSG();
		return;
	}

	sound(self, CHAN_WEAPON, "weapons/shotgn2.wav", 1, ATTN_NORM); 

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_BIGKICK);
#else
	self.punchangle_x = -4;	 
#endif

	self.currentammo = self.ammo_shells = self.ammo_shells - 4;
	dir = aim (self, 100000);
	FireBullets(13, dir, '0.28 0.14 0', 10, SH_BULLETS);

	self.attack_finished = time + 1.2;
	

};

/*
================
W_FireSuperShotgun
================
*/
void() W_FireSuperShotgun =
{
	if(self.ssg_mode == 0)
		W_FireSSG();
	else if(self.ssg_mode == 1)
		W_FireSawedSG();
};

