/*************************************************************************
 * Metamorphism Quake Server: Common/Weapons/dspweap.c,                  *
 *                            Common/Weapons/dspweap.qc                  *
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

/******************************\
 Display Weapon mode for player
\******************************/
void(float wep) DisplayWeaponMode =
{
	local string s;

	if (wep == IT_AXE)
	{
		s = ftos(1);
		sprint(self,s);
		s = ftos(self.axe_mode);
		sprint(self,s);

		if(self.playerclass != CL_BERSERK)
		{
			sprint(self," AXE: ");
			if(self.axe_mode == 0)
				sprint(self,"Standard\n");
			else if (self.axe_mode == 1)
				sprint(self,"Grappling Hook\n");
		}
		else
		{
			sprint(self, " Long Sword\n");
		}
  }

  if (wep == IT_SHOTGUN)
  {
    s = ftos(2);
    if (self.playerclass == CL_SNIPER)
	 {
		 if (( self.sg_mode==1) && (self.ammo_shells < 10))
		 {
			 sprint(self,"AutoSnipe(TM): Not enough ammo.\n");
			 self.sg_mode=0;
		 }
	 }

    sprint(self,s);
    s = ftos(self.sg_mode);
    sprint(self,s);
    
    if (self.playerclass == CL_ASNIPER)
    {
	    sprint(self," SG: Assault Rifle\n");
	    return;
    }

    if (self.playerclass != CL_SNIPER)
      sprint(self," SG: Standard\n");
    else
      if (self.sg_mode == 0)
        sprint(self," SG: Full, 200d, 2.3s\n");
      else
        sprint(self," SG: Auto, 60d, 1.6s [10]\n");
  }

	if (wep == IT_SUPER_SHOTGUN)
	{
		s = ftos(3);
		sprint(self,s);
		s = ftos(self.ssg_mode);
		sprint(self,s);
	
		sprint(self," SSG: ");
	
		if(self.ssg_mode == 0)
			sprint(self,"Normal\n");
		else if(self.ssg_mode == 1)
			sprint(self,"Sawed Barrel\n");
	}

  if (wep == IT_NAILGUN)
  {
    s = ftos(4);
    sprint(self,s);
    s = ftos(self.ng_mode);
    sprint(self,s);
    sprint(self," NG: ");
    if (self.ng_mode == 0)
      sprint(self,"Standard\n");
    else if (self.ng_mode >= 1)
      sprint(self,"Spread\n");
  }

  if (wep == IT_SUPER_NAILGUN)
  {
    s = ftos(5);
    sprint(self,s);
    s = ftos(self.sng_mode);
    sprint(self,s);

    sprint(self," SNG: ");
    if (self.sng_mode == 0)
      sprint(self,"Standard\n");
    else if (self.sng_mode == 1)
      sprint(self,"Laser Cannon\n");
    else if (self.sng_mode == 2)
      sprint(self,"Siege Cannon\n");
    else if (self.sng_mode == 3)
      sprint(self,"Assault Cannon\n");
  }

  if (wep == IT_GRENADE_LAUNCHER)
  {
    s = ftos(6);
    sprint(self,s);
    s = ftos(self.gl_mode);
    sprint(self,s);

    sprint(self," GL: ");
    if (self.gl_mode == 0)
      sprint(self,"Standard\n");
    else if (self.gl_mode == 1)
      sprint(self,"Pipe Bombs\n");
    else if (self.gl_mode == 2)
      sprint(self,"Land Mines\n");
    else if (self.gl_mode == 3)
      sprint(self,"Mortar\n");
    else if (self.gl_mode == 4)
      sprint(self,"Vortex Cannon\n");
    else if (self.gl_mode == 5)
      sprint(self,"Time Bomb\n");
  }   

  if (wep == IT_ROCKET_LAUNCHER)
  {
    s = ftos(7);
    sprint(self,s);
    s = ftos(self.rl_mode);
    sprint(self,s);

    sprint(self," RL: ");

    if (self.rl_mode == 0)
      sprint(self,"Standard\n");
    else if (self.rl_mode == 1)
      sprint(self,"Shalrath Ball\n");
    else if (self.rl_mode == 2)
      sprint(self,"Dumbfire Rockets\n");
    else if (self.rl_mode == 3)
      sprint(self,"Bottle Rocket\n");
  }
  
  if (wep == IT_LIGHTNING)
  {
    s = ftos(8);
    sprint(self,s);
    s = ftos(self.tb_mode);
    sprint(self,s);

    sprint(self," TB: ");
    if (self.tb_mode == 0)
      sprint(self,"Standard\n");
    else if (self.tb_mode == 1)
      if (self.playerclass == CL_SHOCK)
        sprint(self,"Hyper Lasers\n");
      else
	sprint(self,"BFG\n");
    else if (self.tb_mode == 2)
      sprint(self,"Chain Lightning\n");
  }

};
