/*************************************************************************
 * Metamorphism Quake Server: Common/Weapons/chgweap.c,                  *
 *                            Common/Weapons/chgweap.qc                  *
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

void() M_ChangeWeapon = 
{
	local float it, am, fl;
	
	it = self.items;
	am = 0;

	if (self.impulse == 1)
	{
		fl = IT_AXE;
		if(self.weapon == IT_AXE)
			self.axe_mode = self.axe_mode + 1;


		//No hook -> switch back to axe
		if(self.player_flags & PF_NO_HOOK)
			self.axe_mode = 0;

		// No one has a 3rd axe mode.
		if(self.axe_mode == 2)
			self.axe_mode = 0;

		//Heavy weapons guy does not get an axe, just a grappling hook
		if(self.playerclass == CL_HWEAP)
			self.axe_mode = 1;

		DisplayWeaponMode(IT_AXE);
	}
	else if (self.impulse == 2)
	{
		fl = IT_SHOTGUN;

                if (self.ammo_shells < 1)
			am = 1;

                if (self.weapon == IT_SHOTGUN)
                  self.sg_mode = self.sg_mode + 1;

// No one has a 3rd sg mode.
                if (self.sg_mode == 2)
                  self.sg_mode = 0;

// Multi weapon fixes:
		if ((self.sg_mode == 1) && (self.playerclass!=CL_SNIPER))
		  self.sg_mode = 0;
		if ((self.sg_mode == 1) && (self.ammo_shells < 10))
	 		self.sg_mode=0;

                if ((am != 1) && (self.items & fl)) {
		  DisplayWeaponMode(IT_SHOTGUN);
                }
	}
	else if (self.impulse == 3)
	{
		fl = IT_SUPER_SHOTGUN;
		if (self.ammo_shells < 2)
			am = 1;
			
		if (self.weapon == IT_SUPER_SHOTGUN)
			self.ssg_mode = self.ssg_mode + 1;
		if(self.ssg_mode == 2)
			self.ssg_mode = 0;
		
		//Not enough ammo to use the sawed shotgun, so go back to regular mode
		if(self.ammo_shells < 4 && self.ssg_mode == 1)
			self.ssg_mode = 0;
		
		if ((am != 1) && (self.items & fl))
		{
			DisplayWeaponMode(IT_SUPER_SHOTGUN);
		}
	}		
	else if (self.impulse == 4)
	{
		fl = IT_NAILGUN;

		if (self.ammo_nails < 1)
			am = 1;

                if ((am != 1) && (self.items & fl)) {
		  DisplayWeaponMode(IT_NAILGUN);
                }
	}
	else if (self.impulse == 5)
	{
		fl = IT_SUPER_NAILGUN;
                if (self.weapon == IT_SUPER_NAILGUN)
                  self.sng_mode = self.sng_mode + 1;

// Hweap class has the assault cannon.
                if ((self.sng_mode == 2) && (self.playerclass != CL_HWEAP))
                  self.sng_mode = 0;
                if ((self.sng_mode == 3) && (self.ammo_nails < 4) && (self.playerclass == CL_HWEAP))
                  self.sng_mode = 2;
                if ((self.sng_mode == 1) && (self.ammo_nails < 3) && (self.playerclass == CL_HWEAP))
                  self.sng_mode = 2;
                if ((self.sng_mode == 0) && (self.ammo_nails < 2) && (self.playerclass == CL_HWEAP))
                  self.sng_mode = 2;


// No one has a 4th snailgun mode.
                if (self.sng_mode == 4)
                  self.sng_mode = 0;
 		// Hweap only uses assault/siege cannon
		if ((self.sng_mode < 2) && (self.playerclass == CL_HWEAP))
                  self.sng_mode = 2;
               if ((self.ammo_nails < 2) && (self.sng_mode == 0))
                { self.sng_mode = 1; }
                if ((self.ammo_nails < 3) && (self.sng_mode == 1))
                { if (self.playerclass != CL_HWEAP)
                    am = 1;
                  else
                  {
                    self.sng_mode = 2;
                    if (self.ammo_nails < 1)
                      am = 1;
                  }
                }

// Multi weapon fixes:
                if ((am != 1) && (self.items & fl)) {
		  DisplayWeaponMode(IT_SUPER_NAILGUN);
                }
	}
	else if (self.impulse == 6)
	{
		fl = IT_GRENADE_LAUNCHER;
                if (self.weapon == IT_GRENADE_LAUNCHER)
                  self.gl_mode = self.gl_mode + 1;

// Specialist has a time bomb!
                if ((self.gl_mode >= 3) && (self.playerclass != CL_SPECIAL))
                  self.gl_mode = 0;
// No one has a 6th grenade type.
                if (self.gl_mode == 5)
                  self.gl_mode = 0;
		if (self.ammo_rockets < 1)
                  am = 1;
// Multi weapon fixes:
                if ((self.ammo_rockets < 3) && (self.gl_mode == 4))
                  self.gl_mode = 0;
                if ((self.ammo_rockets < 1) && (self.gl_mode == 2))
                  if (self.playerclass == CL_SPECIAL)
                    self.gl_mode = 3;
                  else
                    self.gl_mode = 0;
		if ((self.ammo_rockets < 45) && (self.gl_mode == 5))
                  self.gl_mode = 0;		


                if ((am != 1) && (self.items & fl)) {
		  DisplayWeaponMode(IT_GRENADE_LAUNCHER);
                }
	}
	else if (self.impulse == 7)
	{
                if (self.playerclass == CL_SPECIAL)
                  return;

		fl = IT_ROCKET_LAUNCHER;
                if (self.weapon == IT_ROCKET_LAUNCHER)
                  self.rl_mode = self.rl_mode + 1;

		if (self.ammo_rockets < 1)
			am = 1;

// Multi weapon fixes:
                if (self.rl_mode > 2) // no one has 3
                  self.rl_mode = 0;
                if ((self.rl_mode != 0) && (self.player_flags & PF_MAGIC))
                        self.rl_mode = 0;

                if ((am != 1) && (self.items & fl)) {
		  DisplayWeaponMode(IT_ROCKET_LAUNCHER);
                }
	}
	else if (self.impulse == 8)
	{
		fl = IT_LIGHTNING;
		if (self.player_flags & PF_MAGIC)
		{
			sprint(self,"Magic selected\n");
			self.tb_mode = 0;
			E_SpellName();
			self.weaponmodel = "";
			self.weapon = fl;        
			self.impulse = 0;
			return;
		}
                if (self.weapon == IT_LIGHTNING)
                  self.tb_mode = self.tb_mode + 1;

// No one has a 4th Thunderbolt mode.
                if (self.tb_mode > 2)
                  self.tb_mode = 0;
                if ((self.tb_mode == 2) && (self.playerclass != CL_SHOCK))
                  self.tb_mode = 0;
                if (self.tb_mode == 1)
		  if (!(self.playerclass == CL_SHOCK) && (self.ammo_cells < 40))
                    self.tb_mode = 0;
                if (self.ammo_cells < 1)
			am = 1;
// Multi weapon fixes:
                if ((am != 1) && (self.items & fl)) {
		  DisplayWeaponMode(IT_LIGHTNING);
                }
	}

        self.impulse = 0;
	if (!(self.items & fl))
	{	// don't have the weapon or the ammo
		sprint (self, "no weapon.\n");
		return;
	}
	
	if (am)
	{	// don't have the ammo
		sprint (self, "not enough ammo.\n");
		return;
	}

//
// set weapon, set ammo
//
	self.weapon = fl;		
	W_SetCurrentAmmo ();
	self.impulse = 0;
};

void() M_ChangeAxe = 
{
  if (self.player_flags & PF_NO_HOOK)
  {
    self.weapon = IT_AXE;
    return;
  }
  self.weapon = IT_AXE;
  self.axe_mode = self.impulse - 10;
  if (self.axe_mode > 1)
    self.axe_mode = 0;

  W_SetCurrentAmmo();
  DisplayWeaponMode(IT_AXE);
  self.impulse = 0;
};

void() M_ChangeSG = 
{
  self.impulse = 0;
};
void() M_ChangeSSG = 
{
  self.impulse = 0;
};
void() M_ChangeNG = 
{
  self.impulse = 0;
};
void() M_ChangeSNG = 
{
  if (((self.ammo_nails >= 1) && (self.playerclass == CL_HWEAP)) && (self.impulse <= 52))
  {
    self.sng_mode = 2;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_SUPER_NAILGUN);
    self.impulse = 0;
    return;
  }  
  if (((self.ammo_nails >= 4) && (self.playerclass == CL_HWEAP)) && (self.impulse == 53))
  {
    self.sng_mode = 3;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_SUPER_NAILGUN);
    self.impulse = 0;
    return;
  }  


  if (((self.ammo_nails >= 2) && (self.items & IT_SUPER_NAILGUN)) && (self.impulse == 50))
  {
    self.sng_mode = 0;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_SUPER_NAILGUN);
  }  
  if (((self.ammo_nails >= 3) && (self.items & IT_SUPER_NAILGUN)) && (self.impulse == 51))
  {
    self.sng_mode = 1;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_SUPER_NAILGUN);
  }  

  self.impulse = 0;
};
void() M_ChangeGL = 
{
  if (((self.ammo_rockets >= 1) && (self.impulse <= 62)) && (self.items & IT_GRENADE_LAUNCHER))
  {
    self.weapon = IT_GRENADE_LAUNCHER;
    self.gl_mode = self.impulse - 60;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_GRENADE_LAUNCHER);
    self.impulse = 0;
    return;
  }
  if ((self.playerclass == CL_SPECIAL) && (self.impulse <= 65))
  {
    if ((self.impulse == 64) && (self.ammo_rockets < 3)) 
      return;
    if ((self.impulse == 65) && (self.ammo_rockets < 30))
      return;

    self.weapon = IT_GRENADE_LAUNCHER;
    self.gl_mode = self.impulse - 60;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_GRENADE_LAUNCHER);

  }
  self.impulse = 0;
};
void() M_ChangeRL = 
{
  if (self.playerclass == CL_SPECIAL)
    return;

  if (self.player_flags & PF_MAGIC)
  {
    self.impulse = 70;
  }

  if (((self.ammo_rockets >= 1) && (self.impulse <= 72)) && (self.items & IT_ROCKET_LAUNCHER))
  {
    self.weapon = IT_ROCKET_LAUNCHER;
    self.rl_mode = self.impulse - 70;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_ROCKET_LAUNCHER);
  }

  if (((self.ammo_rockets >= 3) && (self.impulse == 73)) && (self.items & IT_ROCKET_LAUNCHER) && (self.playerclass == CL_SOLDIER))
  {
    self.weapon = IT_ROCKET_LAUNCHER;
    self.rl_mode = self.impulse - 70;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_ROCKET_LAUNCHER);
  }

  self.impulse = 0;
};
void() M_ChangeTB = 
{
  if (self.player_flags & PF_MAGIC)
  {
    self.impulse = 0;
    return;
  }

  self.tb_mode = self.impulse - 80;
  if (self.playerclass == CL_SHOCK)
  {
    if (self.tb_mode > 2)
    {
      self.tb_mode = 0;
    }
    self.weapon = IT_LIGHTNING;
    W_SetCurrentAmmo();
    DisplayWeaponMode(IT_LIGHTNING);
    self.impulse = 0;
    return;
  } else {
    if (self.items & IT_LIGHTNING)
    {
      if (self.tb_mode > 1)
      {
        self.tb_mode = 0;
      }
      if ((self.tb_mode == 1) && (self.ammo_cells < 40))
      {
        self.tb_mode = 0;
      }
      self.weapon = IT_LIGHTNING;
      W_SetCurrentAmmo();
      DisplayWeaponMode(IT_LIGHTNING);
    }
  }
  self.impulse = 0;
};

