/**************************************************************************
 * Metamorphism Quake Server: Common/special.c, Common/special.qc        *
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
#include "version.h"
#include "meta.h"

void() ClassRespawn;

//Help system!
void() CreateHelp;
void() HelpNext;
void() HelpPrev;
void() HelpSelectClass;

void() S_JetOn = 
{
  if ((self.playerclass == CL_SCOUT) || (self.player_flags & PF_FLY))
    self.player_flags = self.player_flags | PF_SPEED;
};

void() S_JetOff = 
{
  self.player_flags = self.player_flags - (self.player_flags & PF_SPEED);
};

void() S_DropRune = 
{
  self.player_flags = self.player_flags - (self.player_flags & PF_HAS_SPECIAL);
};

#ifdef GAME_CTF
void() S_DropFlag = 
{
  self.player_flags = self.player_flags | PF_DROPPED_FLAG;
};
#endif

// Flash Light
void() FL =
{
  if (((self.owner.deadflag) || (self.id_number != self.owner.id_number)) ||
       (!(self.owner.player_flags & PF_LIGHT)))
  {
    self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_LIGHT);
    self.owner.impulse = 0;
    remove(self);
    return;
  }   
  if (self.health == 1)
  {
    makevectors (self.owner.v_angle);
    traceline (self.owner.origin + '0 0 16', (self.owner.origin + '0 0 16') + v_forward*300, FALSE, self);
    self.origin = trace_endpos;
  }
  self.nextthink = time + 0.05;
  self.think = FL;
};

void() S_ToggleLight =
{
  local entity l;

  if (self.player_flags & PF_LIGHT)
  {
    self.player_flags = self.player_flags - (self.player_flags & PF_LIGHT);
    centerprint(self,"Light Off");
    self.impulse = 0;
    return;
  }   

  l = spawn();
  l.id_number = self.id_number;
  l.owner = self;
  l.think = FL;
  l.nextthink = time + 0.1;
  l.movetype = MOVETYPE_NOCLIP;
  l.health = 1;				// Fixed distance
  makevectors(self.v_angle);
  l.effects = EF_DIMLIGHT;
  setmodel(l,"progs/s_bubble.spr");
  cprint(self,"Light ON");
  self.impulse = 0;
  self.player_flags = self.player_flags | PF_LIGHT;
}; 

void() S_MetaHelp = 
{
	if (self.impulse == 99)
	{
		if ((self.help != 0) && (self.help != 200))
		{
			self.help = 0;
			self.impulse = 0;
			PutClientInServer();
			return;
		}
		else
			CreateHelp();
	}
	
	if(self.impulse == 1)
		HelpNext();
	if(self.impulse == 2)
		HelpPrev();
	if(self.impulse == 3)
		HelpSelectClass();
	
	self.impulse = 0;
};

void() S_ServerRules = {};

#ifdef DEBUG
void() S_Location = 
{
  local string s;
  s = vtos (self.origin);
  sprint(self,"--> 'x y z' = ");
  sprint(self,s);
  sprint(self,"\n");
};
#endif


void() S_DisplayHandicap =
{
  local string s;
  local float f;
  f = self.handicap * 100;
  s = ftos(f);

  sprint(self,"Handicap: ");
  sprint(self,s);
  sprint(self,"%\n");
};

void() S_HandicapRaise = 
{
  if (self.handicap >= 1.0)
  {
    self.handicap = 1.0;
    S_DisplayHandicap();
    return;
  }

  self.handicap = self.handicap + 0.1;
  S_DisplayHandicap();

};

void() S_HandicapLower =
{
  if (self.handicap <= 0.1)
  {
    self.handicap = 0.1;
    S_DisplayHandicap();
    return;
  }
 
  self.handicap = self.handicap - 0.1;
  S_DisplayHandicap();

};

void() S_MortarPower =
{
  if (self.playerclass != CL_SPECIAL)
    return;

  
        self.spell = self.spell + 1;
        if (self.spell > 4) self.spell = 2;
        if (self.spell == 2)
          cprint(self,"Mortar Power 33%\n");
        if (self.spell == 3)
          cprint(self,"Mortar Power 67%\n");
        if (self.spell == 4)
          cprint(self,"Mortar Power 100%\n");
};

void() S_MissileAim =
{
  if (self.option_flags & OF_NO_MISSILEAIM)
  {
    self.option_flags = self.option_flags - (self.option_flags & OF_NO_MISSILEAIM);
    sprint(self,"Rocket Server Aim ON\n");
  } else {
    self.option_flags = self.option_flags | OF_NO_MISSILEAIM;
    sprint(self,"Rocket Server Aim OFF\n");
  }
};


void() S_AutoPush =
{
  if (self.option_flags & OF_AUTOPUSH)
  {
    self.option_flags = self.option_flags - (self.option_flags & OF_AUTOPUSH);
    sprint(self,"AutoPush disabled\n");
  } else {
    self.option_flags = self.option_flags | OF_AUTOPUSH;
    sprint(self,"AutoPush enabled\n");
  }
};

void() S_ClassExec =
{
  if (self.option_flags & OF_CLASSEXEC)
  {
    self.option_flags = self.option_flags - (self.option_flags & OF_CLASSEXEC);
    sprint(self,"ClassExec disabled\n");
  } else {
    self.option_flags = self.option_flags | OF_CLASSEXEC;
    sprint(self,"ClassExec enabled\n");
  }
};

void() S_GoRandom =
{
  if (self.option_flags & OF_RANDOM)
  {
    sprint(self,"You must reconnect to disable random classes.\n");
    return;
  }
  self.player_flags = self.player_flags | PF_HAS_CLASS;
  self.option_flags = self.option_flags | OF_RANDOM;
  ClassRespawn();

};

void(entity monkey) S_MetaInfo =
{
  sprint(monkey,"\nÕúEú‘úAúÕúOú“úPú»úIú”úM Quake Server\n");
  sprint(monkey,"  Copyright 1996-2000 by Lon Hohberger\n");
  sprint(monkey,"  Portions Copyright 1996 by id Software\n\n");
  sprint(monkey,"  --- N O  W A R R A N T Y ---\n");
  sprint(monkey,"  This server modification is free software,\n");
  sprint(monkey,"  and is distributed under the GNU General\n");
  sprint(monkey,"  Public License.  Visit http://www.gnu.org\n");
  sprint(monkey,"  for information regarding the GPL.\n\n");
  sprint(monkey,"Version ");
  sprint(monkey,VERSION);
  sprint(monkey,"\n\n");
};

#ifdef GAME_CTF
void() S_ChangeTeam =
{
	bprint(self.netname);
	bprint(" changes teams.\n");
	if (self.fixed_team == TEAM1_COLOR)
	{
		self.fixed_team = TEAM2_COLOR;
	} else {
		self.fixed_team = TEAM1_COLOR;
	}
};

// CTF Only
void() S_QueryFlag =
{
  local float t;
  local entity e;
  t = GetTeam(self);
  if (t==TEAM1_COLOR)
  {
    e = find(world,classname,"item_flag_team1");
  }
  else if (t==TEAM2_COLOR)
  {
    e = find(world,classname,"item_flag_team2");
  }
  if (e)
  {
    sprint(self,"[ Flag: ");
    if (e.owner==world)
      if (e.origin==e.spawn_origin)
        sprint(self,"at home ]\n");
      else
        sprint(self,"somewhere ]\n");
    else
    {
      sprint(self,"Carried by ");
      sprint(self,e.owner.netname);
      sprint(self," ]\n");
    }
  } else {
    sprint(self,"No flag, silly!\n");
  }
};
#endif
