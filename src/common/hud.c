/*************************************************************************
 * Metamorphism Quake Server: Common/hud.c, Common/hud.qc                *
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

void(entity client, string h, string n, string t, string s, string hp, string fin, string crtn) targetp = #73;
void(entity client,
     string sa,
     string sb,
     string sc,
     string sd,
     string se,
     string sf,
     string sg) hud_print = #73;

void(entity client, string msg) HUD_Message =
{
  client.hud.classname = msg;
  client.hud.nextthink = time;
};

void(entity client, string name) HUD_Target =
{
  client.hud.netname = name;
  client.hud.nextthink = time;
};

void() W_FireSniper;
void() TargetID =
{
  local string name, t, pct;

  if (self.player_flags & PF_NOFRAME)
    return;

  if (self.enemy.invisible_finished > time)
  {
    if (self.player_flags & PF_LOCKED)
    { cprint(self,"Target lost!\n");  self.player_flags = self.player_flags - PF_LOCKED; }
    self.enemy = world;
  }
  
  // Only clerics retain targets after death
  if (self.enemy.deadflag && (self.playerclass != CL_CLERIC))
  {
	  self.enemy=world;
  }

  makevectors (self.v_angle);
  traceline (self.origin, self.origin + v_forward*1500, FALSE, self);
  if ((trace_ent != world) && (trace_ent.takedamage == DAMAGE_AIM))
       
  {
	  /* used when snipers were totally invisible
    if (((trace_ent.classname == "player") && (trace_ent.playerclass == CL_SNIPER)) && (trace_ent.weapon == IT_SHOTGUN))
    {
        msg_entity = self;
        WriteByte (MSG_ONE, SVC_TEMPENTITY);
        WriteByte (MSG_ONE, TE_LIGHTNINGBLOOD);
        WriteCoord (MSG_ONE, trace_ent.origin_x);
        WriteCoord (MSG_ONE, trace_ent.origin_y);
        WriteCoord (MSG_ONE, trace_ent.origin_z + 16);
    }
    	*********************************************/
    
    if ((self.enemy == trace_ent) && (time < self.ltime))
      return;

    if (((trace_ent.playerclass == CL_FIREELEM) || (trace_ent.invisible_finished > time)) || (trace_ent.playerclass == CL_LICH))
    {
      if (teamplay & TP_ON)
      {
        if (!SameTeam(self,trace_ent))
          return;
      } else {
        return;
      }
    }

    if (!(self.player_flags & PF_LOCKED))
      self.enemy = trace_ent;
    self.ltime = time + 1.75;
    if (self.enemy.classname == "player")
      name = trace_ent.netname;
    else
      name = trace_ent.classname;

    if (((self.playerclass == CL_SNIPER) && (self.weapon == IT_SHOTGUN)) &&
        ((self.sg_mode == 1) && (trace_ent.takedamage == DAMAGE_AIM)))
    {
      if (self.attack_finished < time)
        W_FireSniper();   
    }
    
    if (self.hud == world)
    {
      if ((teamplay & TP_ON) && (SameTeam(trace_ent,self)))
        t = " (friendly)";
      else t = "";

      pct = ftos((trace_ent.health*100) / trace_ent.max_health); 
 
      targetp(self,"\n\n\n\n\n\n\n\n\nœ Target ID œ\n",name," ",pct,"% ",t,"\n");
    } else {
      HUD_Target(self,name);
    }
  }
};

// called every 1.7 seconds
void() HUD_Update =
{
  local string down, hp, friend;
  local float pct;

  if (self.player_flags & PF_NOFRAME)
    return;

  if (self.id_number != self.owner.id_number)
  {
    self.owner.hud = world;
    remove(self);
    return;
  }

  if (self.health == 1)
    down = "\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 2)
    down = "\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 3)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 4)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 5)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 6)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 7)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 8)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 9)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 10)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 11)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 12)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 13)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 14)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 15)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 16)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 17)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 18)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 19)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  else if (self.health == 20)
    down = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

  if (self.netname != "(none)") {
    if (SameTeam(self.owner.enemy,self.owner) && (teamplay & TP_ON))
    {
      friend = "% (friendly)\n";
    } else {
      friend = "%\n";
    }

    pct = (self.owner.enemy.health / self.owner.enemy.max_health) * 100;
    hp = ftos(pct);
  } else {
    hp = " ";
    friend = "\n";
  }

  hud_print(self.owner,down,
            "Target: ",self.netname," ",hp,friend,
            self.classname);

  self.classname = "\n";
  self.netname = "(none)";
  self.nextthink = time + 1.7;
  self.think = HUD_Update;

};

void() HUD_Spawn =
{
  local entity e;
  if (self.hud != world)
    return;

  e = spawn();
  e.health = 1;
  e.owner = self;
  self.hud = e;
  e.id_number = self.id_number;
  e.netname = "(none)";
  e.classname = "HUD Turned On\n";
  e.think = HUD_Update;
  e.nextthink = time;
};

void() HUD_Up =
{
  if (!self.hud)
    return;
  self.hud.health = self.hud.health - 1;
  if (self.hud.health == 0)
    self.hud.health = 1;
};

void() HUD_Down =
{
  if (!self.hud)
    return;
  self.hud.health = self.hud.health + 1;
  if (self.hud.health == 21)
    self.hud.health = 20;
};

void() HUD_Kill =
{
  if (!self.hud)
    return;
  remove(self.hud);
  self.hud=world;
};
