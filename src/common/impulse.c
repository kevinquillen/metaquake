/*************************************************************************
 * Metamorphism Quake Server: Common/impulse.c, Common/impulse.qc        *
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

void() end_dog;
void() end_demon;

/*********************\
 Impulse Command Parse
\*********************/

void() ParseImpulse =
{
  if (parm2 == 1)
  {
	  bprint(";) Parm2\n");
  }

  if (self.sysop == 4)
    return;

  if (self.impulse == 0)
    return;

  if (self.player_flags & PF_CHEATER)
  {
    self.impulse = 0;
    sprint(self,"Fix your models first.\n");
    return;
  }

	//In help mode -> run help and stop
	if(self.help)
	{
		S_MetaHelp();
		return;
	}

	// Class Changing Impulses
	//===========================================
	if(self.impulse >= IMPULSE_CLASS_START && self.impulse <= IMPULSE_CLASS_END)
	{
		S_SelectClass();
		self.impulse = 0;
		return;
	}

  /*************************\
   Special Impulses (90-119)
  \*************************/
  if ((self.impulse >= 90) && (self.impulse <= 119))
  {
    if (self.impulse == 90)		// Throw hook
      W_IFireHook();
    if (self.impulse == 91)		// release hook
      W_IReleaseHook();
    if (self.impulse == 92)		// jetpack
      S_JetOn();
    if (self.impulse == 93)		// no jetpack
      S_JetOff();
    if (self.impulse == 94)		// Blow Pipe Bombs
      W_BlowPipeBombs();
    if (self.impulse == 95)
      S_ClassExec();
    if (self.impulse == 96)		// Drop Rune
      S_DropRune();
#ifdef GAME_CTF
    if (self.impulse == 97)		// Drop Flag (CTF)
      S_DropFlag();
#endif
    if (self.impulse == 98)		// Toggle Light
      S_ToggleLight();
    if (self.impulse == 99)		// HELP
      S_MetaHelp();
    if (self.impulse == 101)		// Server rules
      S_ServerRules();
    if (self.impulse == 102)
      HUD_Kill();
    if (self.impulse == 103)
      HUD_Spawn();
    if (self.impulse == 104)
      HUD_Up();
    if (self.impulse == 105)
      HUD_Down();
    if (self.impulse == 106)
      S_GoRandom();
#ifdef DEBUG
    if (self.impulse == 107)
      S_Location();
#endif
    if (self.impulse == 108)
      S_MortarPower();
    if (self.impulse == 109)
      S_MissileAim();
#ifdef GAME_CTF
    if (self.impulse == 110)
      S_QueryFlag();
#endif
    if (self.impulse == 111)
      S_AutoPush();
    if (self.impulse == 112)
	   S_MetaInfo(self);
    if (self.impulse == 113)
      S_CreateTeam();
    if (self.impulse == 114)
      S_AcceptMember();
    if (self.impulse == 115)
      S_CheckTeams();
    if (self.impulse == 116)
      S_ChangeTeam();
    if (self.impulse == 117)
	    S_Disband();
		
    self.impulse = 0;
    return;
  }

	/*****************\
	Special (UnMorph)
	\*****************/
	if(self.player_flags & (PF_DOGGIE | PF_DEMON))
	{
		if(self.impulse <= 89)
		{
			if (self.player_flags & PF_DOGGIE)
				end_dog();
			else 
				end_demon();
		}
		else
			self.impulse = 0;

		return;
  }

  /***************************\
   Weapon Change Impulses 1-89
  \***************************/
  if (self.impulse <= 89)
  {
    if (self.impulse <= 9)			// Cycle/Change
    { M_ChangeWeapon(); return; }

    if (self.impulse <= 19)			// Axe Mode
    { M_ChangeAxe(); return; }
    if (self.impulse <= 29)			// Shotgun Mode
    { M_ChangeSG(); return; }
    if (self.impulse <= 39)			// Super Shotgun Mode
    { M_ChangeSSG(); return; }
    if (self.impulse <= 49)			// Nailgun Mode
    { M_ChangeNG(); return; }
    if (self.impulse <= 59)			// Super Nailgun Mode
    { M_ChangeSNG(); return; }
    if (self.impulse <= 69)			// Grenade Launcher Mode
    { M_ChangeGL(); return; }
    if (self.impulse <= 79)			// Rocket Launcher Mode
    { M_ChangeRL(); return; }
    if (self.impulse <= 89)			// Thunderbolt Mode
    { M_ChangeTB(); return; }
  }

  /**************\
   Magic Handling
  \**************/
	if ((self.impulse >= 140) && (self.impulse <= 169))
	{
		if ((self.impulse >= 140) && (self.impulse <= 159))
		{
			E_CastMagic();
		}
		else
		{
			if (self.impulse == 160)
				E_NextSpell();
			if (self.impulse == 161)
				E_PrevSpell();
			if (self.impulse == 162)
				E_CastMode();
			if (self.impulse == 163)
				E_CastSpell();
		}
  
		self.impulse = 0;
		return;
	}

}; // End of ParseImpulse
