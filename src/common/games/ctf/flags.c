/*************************************************************************
 * Metamorphism Quake Server: Common/Games/CTF/flags.c,                  *
 *                            Common/Games/CTF/flags.qc                  *
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

void() FlagTaken =
{
	local entity head;
	local float f;
	head = FindWorld("player");
	while(head)
	{
		f = SameTeam(self,head);
		if(f)
		{
			cprint(head,"Your team's flag has been taken!\n");
		}
		else
		{
			if (head != self.owner)
				cprint(head,"Your team has the enemy flag!\n");
			else
				cprint(head,"You have the enemy flag!\n");
		}
		head = head.goalentity;
	}
};

// Broadcast messages of a flag being returned to its base
void() FlagReturned =
{
  local entity head;
  local float f;
  head = FindWorld("player");
  while (head) {
    f = SameTeam(self,head);
    if (f)
    {
      cprint(head,"Your team's flag is back home!\n");
    } else {
      cprint(head,"Enemy flag has been returned to base.\n");
    }
    head=head.goalentity;
  }
};

// Broadcast a message of a flag that was captured
void() FlagCaptured =
{
  local entity head;
  local float f;
  head = FindWorld("player");
  while (head) {
    f = SameTeam(head,self);
    if (f)
    {
      head.frags = self.frags;
      if (head == other)
        cprint(head,"You captured the enemy flag!\n");
      else
        cprint(head,"Your team captured the flag!\n");
    } else {
      cprint(head,"Your team's flag was captured!\n");
    }
    head=head.goalentity;
  }
};

void() FlagThink;
void() T_FlagTouch;

void() FlagOK =
{
  self.think = FlagThink;
  self.touch = T_FlagTouch;
  self.nextthink = time;
};

void() FlagRespawn =
{
  local entity newf;
  newf = spawn();
  newf.effects = EF_DIMLIGHT;
  newf.movetype = MOVETYPE_NONE;
  newf.solid = SOLID_TRIGGER;
  newf.spawn_origin = self.spawn_origin;
  newf.origin = self.spawn_origin;
  newf.velocity = '0 0 0';
  newf.classname = self.classname;
  if (newf.classname == "item_flag_team1")
    setmodel(newf,"progs/w_g_key.mdl");
  if (newf.classname == "item_flag_team2")
    setmodel(newf,"progs/w_s_key.mdl");
  newf.team = self.team;
  setsize (newf, '-16 -16 -24', '16 16 32');
  newf.think = FlagOK;
  newf.nextthink = time;
  newf.frags = self.frags;
  remove(self);
};

void() FlagThrow =
{
  local entity newf;
  newf = spawn();
  newf.frags = self.frags;
  newf.ltime = self.ltime;
  newf.effects = self.effects;
  newf.movetype = MOVETYPE_TOSS;
  newf.solid = SOLID_TRIGGER;
  newf.origin = self.owner.origin;
  newf.velocity = self.owner.velocity;
  newf.velocity_z = newf.velocity_z + 500;
  newf.velocity_x = newf.velocity_y * 1.2 + random()*1000 - 500;
  newf.velocity_y = newf.velocity_y * 1.2 + random()*1000 - 500;
  newf.classname = self.classname;
  if (newf.classname == "item_flag_team1")
    setmodel(newf,"progs/w_g_key.mdl");
  if (newf.classname == "item_flag_team2")
    setmodel(newf,"progs/w_s_key.mdl");
  newf.team = self.team;
  newf.spawn_origin = self.spawn_origin;
  setsize (newf, '-16 -16 -24', '16 16 32');
  newf.think = FlagOK;
  newf.nextthink = time + 1;
  remove(self);
};

/*
  FlagTouch and FlagThink routines. The think routine is mainly used when a
  player has posession of the flag (or loses posession, or drops it, or
  captures it). It is also counts the time since it was last touched, so it
  will return to base. The touch routine is for when no one has the flag.
*/
void() T_FlagTouch;
void() FlagThink =
{
  local entity e;

 if (self.health >= 5)
 {
  self.health = 0;
  e = findradius(self.spawn_origin,2000);
  while (e)
  {
    if ((e.classname == "player") && (e.playerclass == CL_HWEAP))
    {
     if (!SameTeam(self,e)) {
      traceline(self.spawn_origin + '0 0 24', e.origin + '0 0 16', FALSE, self);

      if (trace_ent == e)
	{
	  WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	  WriteByte (MSG_MULTICAST, TE_LIGHTNING3);
	  WriteEntity (MSG_MULTICAST, self);
	  WriteCoord (MSG_MULTICAST, self.spawn_origin_x);
	  WriteCoord (MSG_MULTICAST, self.spawn_origin_y);
	  WriteCoord (MSG_MULTICAST, self.spawn_origin_z);
	  WriteCoord (MSG_MULTICAST, e.origin_x);
	  WriteCoord (MSG_MULTICAST, e.origin_y);
	  WriteCoord (MSG_MULTICAST, e.origin_z);
#ifdef QUAKEWORLD
	  multicast (self.spawn_origin, MULTICAST_PHS);
#endif
      
	  DoDamage (trace_ent, world, world, 65000, SH_ILLEGAL);
	}
    }
   }
   e = e.chain;
 }        
}
// if flag is sitting in world, go home after 60 sec.
  if ((self.origin != self.spawn_origin) && (self.owner == world))
  {
    if (self.ltime < time - 60)
    {
      self.origin = self.spawn_origin;
      FlagReturned();
      self.think = FlagRespawn;
      self.nextthink = time;
    }
  }

// drop flag if dead OR if you did a "dropflag"
  if (self.owner != world)
  {
    // Drop flag if dead.
    if (self.owner.deadflag)
    {
      bprint(self.owner.netname);
      bprint(" lost the ");
      bprint_teamcolor(GetTeam(self));
      bprint(" team's flag!\n");
      self.ltime = time;
      self.nextthink = time;
      self.think = FlagThrow;
      return;
    }

    // Place flag by owner
    makevectors (self.owner.v_angle);
    self.origin = self.owner.origin + -20*v_forward;
#ifdef QUAKEWORLD
    self.velocity = self.owner.velocity;
#endif

    // Drop flag if you did the drop impulse
    if (self.owner.player_flags & PF_DROPPED_FLAG)
    {
      bprint(self.owner.netname);
      bprint(" dropped the ");
      bprint_teamcolor(GetTeam(self));
      bprint(" team's flag!\n");
      self.owner.items = self.owner.items - (self.owner.items & self.items);
      self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_HAS_FLAG);
      self.owner.player_flags = self.owner.player_flags - (self.owner.player_flags & PF_DROPPED_FLAG);
      self.ltime = time + 60;
      self.nextthink = time;
      self.think = FlagThrow;
      return;
    }


    // Return flag if captured.
    if (!(self.owner.player_flags & PF_HAS_FLAG))
    {
      self.owner = world;
      self.think = FlagRespawn;
      self.nextthink = time;
      return;
    }

  }
  self.health = self.health + 1;
  self.nextthink = time + 0.1;
};

// Returned flag think
void() FlagRetThink =
{
  self.touch = T_FlagTouch;
  self.nextthink = time + 0.2;
  self.think = FlagThink;
};

// Flag touch routines.
void() T_FlagTouch =
{
  local float f;
  if (other.classname == "player")
  {
    // Capture the enemy flag if you run into your flag and it is at home.    
    if ((other.player_flags & PF_HAS_FLAG) && (self.origin == self.spawn_origin))
    {
      self.frags = self.frags + 1;		// flags keep global score
      FlagCaptured();
      sound(world,CHAN_VOICE,"boss1/sight1.wav",1,ATTN_NONE);
      other.player_flags = other.player_flags - (other.player_flags & PF_HAS_FLAG);
      other.items = other.items - (other.items & (IT_KEY1 | IT_KEY2));
      bprint(other.netname);
      bprint(" captured the ");
      if (self.team == TEAM1_COLOR)
        bprint_teamcolor(TEAM2_COLOR);
      else
        bprint_teamcolor(TEAM1_COLOR);

      bprint(" team's flag!\n");
    }

    // Return your own flag if you hit it and it is not at base.
    if (!(other.deadflag))
    {
      f = SameTeam(other,self);

      other.spawn_origin = self.spawn_origin;

      if (f && (self.origin != self.spawn_origin))
      {
        bprint(other.netname);
        bprint(" returned the ");
        bprint_teamcolor(GetTeam(self));
        bprint(" team's flag!\n");
        self.origin = self.spawn_origin;
        self.touch = SUB_Null;
        FlagReturned();
        self.think = FlagRespawn;
        self.nextthink = time;
        return;
      }
   
      // can't take flag?
      if (!f) {

        if (other.player_flags & PF_SPEED)
        {
          cprint(other,"You must turn off your jetpack!\n");
          return;
        }

        if (other.playerclass == CL_HWEAP)
        {
          cprint(other,"Base guards may not take flags.\n");
          return;
        }
      }

      // pick up the flag if it isn't yours
      if (!f)
      {
        bprint(other.netname);
        bprint(" stole the ");
        bprint_teamcolor(GetTeam(self));
        bprint(" team's flag!\n");
        sound(world,CHAN_VOICE,"misc/basekey.wav",1,ATTN_NONE);
        other.player_flags = other.player_flags | PF_HAS_FLAG;
        other.items = other.items | self.items;
        self.movetype = MOVETYPE_NOCLIP;
        self.owner = other;
        FlagTaken();
        self.origin = other.origin;
        self.touch = SUB_Null;
      }
    }
  }
};

/* 
	Threewave Compliant CTF spawn points
*/

void() info_player_team1 =
{
};

void() info_player_team2 =
{
};

void() item_flag_team1 =
{
  local entity flag;
  if (deathmatch != DMMODE_CTF)
    return;

  precache_model ("progs/w_g_key.mdl");
  flag = spawn();
  flag.frags = 0;
  flag.team = TEAM1_COLOR + 1;
  setsize (flag, '-16 -16 -24', '16 16 32');
  flag.classname = "item_flag_team1";
  flag.effects = EF_DIMLIGHT;
  flag.items = IT_KEY2;
  flag.solid = SOLID_TRIGGER;
  flag.movetype = MOVETYPE_NONE;
  flag.velocity = '0 0 0';
  setmodel(flag,"progs/w_g_key.mdl");
  flag.touch = T_FlagTouch;
  flag.think = FlagThink;
  flag.nextthink = time + 0.1;
  flag.spawn_origin = self.origin;
  setorigin(flag, flag.spawn_origin);
  remove(self);
};

void() item_flag_team2 =
{
  local entity flag;
  if (deathmatch != DMMODE_CTF)
    return;

  precache_model ("progs/w_s_key.mdl");
  flag = spawn();
  flag.frags = 0;
  flag.team = TEAM2_COLOR + 1;
  setsize (flag, '-16 -16 -24', '16 16 32');
  flag.classname = "item_flag_team2";
  flag.effects = EF_DIMLIGHT;
  flag.items = IT_KEY1;
  flag.solid = SOLID_TRIGGER;
  flag.movetype = MOVETYPE_NONE;
  flag.velocity = '0 0 0';
  setmodel(flag,"progs/w_s_key.mdl");
  flag.touch = T_FlagTouch;
  flag.think = FlagThink;
  flag.nextthink = time + 0.1;
  flag.spawn_origin = self.origin;
  setorigin(flag, flag.spawn_origin);
  remove(self);
};
