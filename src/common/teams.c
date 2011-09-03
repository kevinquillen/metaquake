/*************************************************************************
 * Metamorphism Quake Server: Common/teams.c, Common/teams.qc            *
 * Copyright (C) 1996-1999 by Lon Hohberger.                             *
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
void(entity client, string s) cprint;
void() PlayerDie;
void(string s) bprint;
void(entity client,string s) sprint;

#include "meta.h"

// returns color of pants.
float (entity checkme) GetTeam =
{
#ifdef QUAKEWORLD
  local string s;
#endif
  local float f;

#ifdef QUAKEWORLD
  if (checkme.classname == "player")
  { 
    s = infokey(checkme,"bottomcolor");
    f = stof(s);
  } else {
    f = (checkme.team - 1);
  }
#else
  f = (checkme.team - 1);
#endif

  return f;
};

float(entity one, entity two) SameTeam =
{ 
  local float f1,f2;
  if (!(teamplay & TP_ON))
    return 0;
 
  f1 = GetTeam(one);
  f2 = GetTeam(two);

  if ((f1 == f2) && ((f1 + f2) > 0))
    return 1;
  return 0;
};

string(float f_team) TeamToColor =
{
  local string s;
  if (f_team == 0)
    s = "wyte";
  else if (f_team == 1)
    s = "brwn";
  else if (f_team == 2)
    s = "ltbl";
  else if (f_team == 3)
    s = "grun";
  else if (f_team == 4)
    s = "red.";
  else if (f_team == 5)
    s = "dkyl";
  else if (f_team == 6)
    s = "gold";
  else if (f_team == 7)
    s = "pink";
  else if (f_team == 8)
    s = "indg";
  else if (f_team == 9)
    s = "viol";
  else if (f_team == 10)
    s = "ofwh";
  else if (f_team == 11)
    s = "cyan";
  else if (f_team == 12)
    s = "yell";
  else if (f_team == 13)
    s = "blue";
  else
    s = "UNKN";

  return s;
};  

void(float f_team) bprint_teamcolor =
{
  local string s;

  if (f_team == 0)
    s = "White";
  else if (f_team == 1)
    s = "Brown";
  else if (f_team == 2)
    s = "Light Blue";
  else if (f_team == 3)
    s = "Green";
  else if (f_team == 4)
    s = "Red";
  else if (f_team == 5)
    s = "Dark Yellow";
  else if (f_team == 6)
    s = "Gold";
  else if (f_team == 7)
    s = "Pink";
  else if (f_team == 8)
    s = "Indigo";
  else if (f_team == 9)
    s = "Violet";
  else if (f_team == 10)
    s = "Off White";
  else if (f_team == 11)
    s = "Cyan";
  else if (f_team == 12)
    s = "Yellow";
  else if (f_team == 13)
    s = "Blue";
  else
    s = "UNKNOWN??";

  bprint(s);
};

void() CheckTeam =
{
  local string str;
  local float tc;
  local float f;

#ifdef QUAKEWORLD
  if ((deathmatch & MODE_CTF) && (teamplay & TP_TEAMLOCK))
  {
    str = infokey(self,"topcolor");
    tc = stof(str);
    if ((((self.fixed_team == TEAM1_COLOR) && (tc == TEAM2_COLOR)) ||
         ((self.fixed_team == TEAM2_COLOR) && (tc == TEAM1_COLOR)))
       && (self.wait <= time))
    {
      bprint(self.netname);
      bprint(" tried to crossdress.\n");
      if (deathmatch != DMMODE_CTF)
      self.frags = self.frags - 2;
      self.health = -1;
      str = ftos(self.fixed_team);
      stuffcmd(self,"color ");
      stuffcmd(self,str);
      stuffcmd(self,"\n");
      cprint(self,"*** Don't crossdress. It's lame.\n");
      PlayerDie();
      return;
    }
  }
#endif /* Cross Dress Check - QuakeWorld AND CTF */

  if ((!(teamplay & TP_TEAMLOCK)) || (self.fixed_team == 255))
    return;

  f = GetTeam(self); 
  if ((f != self.fixed_team) && (self.invincible_finished < time))
  {
    bprint(self.netname);
    bprint(" was wearing the wrong colors.\n");
    if(deathmatch != DMMODE_CTF)
		self.frags = self.frags - 1;
    self.health = -1;
    str = ftos(self.fixed_team);
    stuffcmd(self,"color ");
    stuffcmd(self,str);
    stuffcmd(self,"\n");
    PlayerDie();
    return;
  }

};

#ifdef QUAKEWORLD /* Dynamic Teams unavailable in QW */
void() UpdateTeam =
{
  local entity p;
  local string str;   
  local float members,f;

  if (teamplay == 0)
  {
    bprint("+++ ");
    bprint_teamcolor(self.team);
    bprint(" team deleted: Teamplay is off\n");
    remove(self);
    return;
  }

  if (self.armorvalue != 0)
  {
    bprint("+++ ");
    bprint_teamcolor(self.team);
    bprint(" team deleted by ");
    bprint(self.owner.netname);
    bprint("\n");
    remove(self);
  }


  if (self.health > 0)
  {
    self.health = self.health + 1;
    if ((self.goalentity == world) || (self.goalentity.classname != "player"))
    {
      self.ammo_shells = 0;
      self.health = 0;
      self.goalentity = world;
    }
    if (self.health >= 12)
    {
      if (self.goalentity != world)
      {
        sprint(self.goalentity,"+++ Your request was not granted.\n");
        self.ammo_shells = 0;
      }
      self.health = 0;
      self.goalentity = world;
    }
  }

  members = 0;

  // Hard-line check for team leader -- JIC
  if ((self.owner != world) && (self.owner.classname == "player"))
  {
    f = GetTeam(self.owner);
    if (f != self.team)
    {
      self.owner.teament = world;
      self.owner = world;
    }
  }

  p = find(world,classname,"player");
  while (p)
  {
    f = GetTeam(p);
    
    if (p.teament == self)
    {
      if (f != self.team)
      {
        p.teament = world;
        if (p == self.owner)
        {
          self.owner = world;
        }
        
      } else {
        if ((self.owner == world) || (self.owner.classname != "player"))
        {
          self.owner = p;
          bprint("+++ ");
          bprint(p.netname);
          bprint(" now leads the ");
          bprint_teamcolor(self.team);
          bprint(" team.\n");
        }
        members = members + 1;
      }
    } else {
      str = infokey(p,"team");
      if (str == self.netname)
      {
        stuffcmd(p,"team \"\"\n");
        sprint(p,"+++ You must join ");
        sprint(p,self.owner.netname);
        sprint(p,"'s team to use that team name.\n");
      }
      if (f == self.team)
      {
        stuffcmd(p,"color 0\n");
        if (self.goalentity == p)
        {
          sprint(p,"+++ Your request is still pending.\n");
          self.ammo_shells = self.ammo_shells + 1;
          if (self.ammo_shells >= 4)
          {
             sprint(p,"*** You have been kicked for team spam.\n");
             stuffcmd(p,"disconnect\n");
          }
        } else {
          if ((self.goalentity != world) && (self.goalentity.classname == "player"))
          {
            sprint(p,"+++ Someone else's request is pending to join that team.\n");
          } else {
            self.goalentity = p;
            sprint(p,"+++ Requested to join ");
            sprint(p,self.owner.netname);
            sprint(p,"'s team.\n");
            sprint(self.owner,"+++ ");
            sprint(self.owner,p.netname);
            sprint(self.owner," has requested to be on your team.\n");
            sprint(self.owner,"+++ Type 'accept' to allow.\n");
            self.health = 1; // if self.health reaches 10, reset.
          }
        }
      }
    }

    if ((self.goalentity == p) && (self.player_flags != 0))
    {
      self.ammo_shells = 0;
      bprint("+++ ");
      bprint(p.netname);
      bprint(" has joined the ");
      bprint_teamcolor(self.team);
      bprint(" team.\n");
      stuffcmd(p,"color ");
      str = ftos(self.team);
      stuffcmd(p,str);
      stuffcmd(p,"\n");
      stuffcmd(p,"team ");
      stuffcmd(p,self.netname);
      stuffcmd(p,"\n");
      self.goalentity = world;
      p.teament = self;
      self.player_flags = 0;
    }

    p = find(p,classname,"player");
  }

  if (members == 0)
  {
    bprint("+++ ");
    bprint_teamcolor(self.team);
    bprint(" team deleted: No members\n");
    remove(self);
    return;
  }
 
  self.nextthink = time + 1;
  self.think = UpdateTeam;
};
#endif /* UpdateTeam */

void() S_CreateTeam =
{
#ifndef QUAKEWORLD
  sprint(self,"*** Team creation is available only in QuakeWorld\n");
#else
  local entity newteam;
  local string str;

  if (deathmatch != MODE_DM)
  {
    sprint(self,"*** Teams may not be created in non-DM modes!\n");
    return;
  }

  if (GetTeam(self) == 0)
  {
    sprint(self,"*** You may not use this color for a team.\n");
    return;
  }   

  newteam = find(world,classname,"teament");
  while(newteam)
  {
    if (newteam.team == GetTeam(self))
    {
      sprint(self,"*** That color has been taken!\n");
      return;
    }
    newteam = find(newteam,classname,"teament");
  }

  newteam = spawn();
  newteam.team = GetTeam(self);
  newteam.netname = TeamToColor(newteam.team);
  newteam.classname = "teament";
  newteam.ammo_shells = 0;
  bprint("+++ ");
  bprint(self.netname);
  bprint(" has created the ");
  bprint_teamcolor (newteam.team);
  bprint(" (");
  bprint(newteam.netname);
  bprint(") team.\n");
  stuffcmd(self,"team ");
  stuffcmd(self,newteam.netname);
  stuffcmd(self,"\n");
  newteam.think = UpdateTeam;
  newteam.owner = self;
  newteam.goalentity = world;
  newteam.nextthink = time + 0.5;
  self.teament = newteam;
  newteam.player_flags = 0;
#endif /* QuakeWorld portion */
};

void() S_CheckTeams = 
{
#ifndef QUAKEWORLD
  sprint(self,"*** Command not available.\n");
#else
  local entity teamid;
  local string s;
  sprint(self,"CC Team Leader\n-- ---- ------\n00 wyte [RESERVED]\n");
  teamid = find(world,classname,"teament");
  while(teamid)
  {
    s = ftos(teamid.team);
    if (teamid.team < 10)
    {
      sprint(self,"0");
    }
    sprint(self,s);

    sprint(self," "); 
    s = TeamToColor(teamid.team);
    sprint(self,s);
    sprint(self," ");
    if (teamid.owner != world)
    {
      sprint(self,teamid.owner.netname);
    } else {
      sprint(self,"UNKNOWN");
    }
    sprint(self,"\n");
    teamid = find(teamid,classname,"teament");
  }
#endif
};  

void() S_AcceptMember =
{
#ifndef QUAKEWORLD 
  sprint(self,"*** Command not available.\n");
#else
  if ((self.teament == world) || (self.teament.classname != "teament"))
  {
    sprint(self,"*** You are not a team leader!\n");
    return;
  }
  if (self.teament.owner != self)
  {
    sprint(self,"*** Sorry, only ");
    sprint(self,self.teament.owner.netname);
    sprint(self," may accept new members.\n");
    return;
  }
  self.impulse = 0;

  if ((self.teament.goalentity != world) && (self.teament.goalentity.classname == "player"))
  {
    self.teament.player_flags = 1;
  }
#endif
};
  
    
void() S_Disband =
{
#ifndef QUAKEWORLD 
  sprint(self,"*** Command not available.\n");
#else
  if ((self.teament == world) || (self.teament.classname != "teament"))
  {
    sprint(self,"*** You are not a team leader!\n");
    return;
  }
  if (self.teament.owner != self)
  {
    sprint(self,"*** Sorry, only ");
    sprint(self,self.teament.owner.netname);
    sprint(self," may disband.\n");
    return;
  }
  self.impulse = 0;
  
  // set flag... checked next round
  self.teament.armorvalue = 1;

#endif
};
  
    
    
    
