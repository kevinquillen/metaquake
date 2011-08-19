/*
 * Common/Weapons/sg.c, Common/Weapons/sg.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1997-2000 by Lon Hohberger
 */

 #include "meta.h"
 
void() player_shot1;
void(float shotcount, vector dir, vector spread, float shotdamage, float dtype) FireBullets;

/*
================
W_FireShotgun
================
*/
void() W_FireShotgun =
{
	local vector dir;

	sound (self, CHAN_WEAPON, "weapons/guncock.wav", 1, ATTN_NORM); 

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
        self.punchangle_x = -2;
#endif
	
	self.currentammo = self.ammo_shells = self.ammo_shells - 1;

	dir = aim (self, 100000);
	FireBullets (5, dir, '0.04 0.04 0', 8, SH_BULLETS);
};

void() W_FireSniper =
{
	local float shotdamage;
        local vector dir;
	if ((self.ammo_shells < 10) && (self.sg_mode==1))
        {
          self.sg_mode = 0;
	  return;
	}
        player_shot1 ();
        sound (self, CHAN_WEAPON, "weapons/ric1.wav", 1, ATTN_NORM);

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
        self.punchangle_x = -2;
#endif

	self.currentammo = self.ammo_shells = self.ammo_shells - 1;
	dir = aim (self, 100000);

        shotdamage = 200;
	if ((fabs(self.velocity_x) > 30) || (fabs(self.velocity_y) > 30))
          shotdamage = 50;			// moving = weak
	if (self.sg_mode == 1)			// auto shoot?
        {
          // 10 Shells Per AutoSnipe
	  self.currentammo = self.ammo_shells = self.ammo_shells - 9;
	  shotdamage = ceil(shotdamage * 0.25);	// 25% of damage
	}

        FireBullets (1, dir, '0 0 0', shotdamage, SH_SNIPER);
        if (self.sg_mode == 1)
          self.attack_finished = time + 1.6;
        else
          self.attack_finished = time + 2.3;
};


void() player_hw_attb1;
void() sniper_attack;
void() W_FireSG =
{
  if (self.playerclass == CL_ASNIPER)
  {
	  player_shot1();
	  sniper_attack();
	  return;
  }
  if (self.playerclass != CL_SNIPER)
  {
    if (self.playerclass != CL_HWEAP)
      player_shot1();
    else
      player_hw_attb1();
    self.attack_finished = time + 0.4;
    W_FireShotgun();
  } else
    W_FireSniper();
};
