/*************************************************************************
 * Metamorphism Quake Server: Common/cselect.c, Common/cselect.qc        *
 * Copyright (C) 1996-2000 by Lon Hohberger.                             *
 * Portions Copyright (C) 2011 by Patrick Baggett                        *
 *                                                                       *
 * This code is free software; you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or (at *
 * your option) any later version.                                       *
 *                                                                       *
 * This code is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * General Public License for more details.                              *
 *                                                                       *
 * Visit http://www.gnu.org for more information on the GPL.             *
 *                                                                       *
 *************************************************************************/

 #include "meta.h"
 
 void (vector org, entity death_owner) spawn_tdeath;
entity() SelectSpawnPoint;
void() player_pain;

void () ClassScout =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec scout.cfg\n");
  }
  self.saves = SH_SNIPER | SH_FALL | SH_SLIME | SH_WATER | SH_PLASMA;
  self.harms = SH_MAGIC | SH_FIRE;

  self.items = 0 | IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_AXE | IT_SUPER_NAILGUN; 
  self.ammo_shells = 50;
  self.ammo_cells = 0;
  self.ammo_nails = 150;
  self.ammo_rockets = 0;
  
  self.armorvalue = 0;
  self.armortype = 0;

  self.max_health = 110;
  self.max_arm = 100;
  self.max_ammo_shells = 100;
  self.max_ammo_nails = 250;
  self.max_ammo_rockets = 8;
  self.max_ammo_cells = 40;
  self.weight = 160;
  self.weapon = IT_SUPER_NAILGUN;
};

void () ClassSniper =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec sniper.cfg\n");
  }
  self.saves = SH_FALL | SH_PLASMA | SH_BULLETS | SH_MAGIC | SH_CRITHIT;
  self.harms = SH_LAVA;

  self.items = 0 | IT_SHOTGUN | IT_AXE | IT_SUPER_SHOTGUN;
  self.ammo_shells = 80;
  self.ammo_cells = 0;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;
  
  self.armorvalue = 0;
  self.armortype = 0;

  
  self.max_health = 130;
  self.max_arm = 80;
  self.max_ammo_shells = 200;
  self.max_ammo_nails = 100;
  self.max_ammo_rockets = 8;
  self.max_ammo_cells = 40;
  self.weight = 200;
  self.weapon = IT_SHOTGUN;
};

void () ClassFireElem =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec fireelem.cfg\n");
  }
  self.saves = SH_FIRE | SH_EXPLOSION | SH_LAVA | SH_INCINERATE | SH_FALL | SH_ELECTRICITY;
  self.harms = SH_WATER;

  self.player_flags = self.player_flags | PF_FLY | PF_REGEN | PF_MAGIC | PF_NO_HOOK;

  self.items = 0 | IT_AXE | IT_LIGHTNING;
  self.ammo_shells = 0;
  self.ammo_cells = 80;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  self.max_health = 150;
  self.max_arm = 0;
  self.max_ammo_shells = 0;
  self.max_ammo_nails = 0;
  self.max_ammo_rockets = 0;
  self.max_ammo_cells = 250;
  self.weight = 100;
  self.weapon = IT_AXE;
};

void () ClassMage =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec mage.cfg\n");
  }
  self.player_flags = self.player_flags | PF_MAGIC; 
  self.saves = SH_MAGIC | SH_PLASMA | SH_WATER | SH_BULLETS;
  self.harms = SH_LAVA | SH_SNIPER;

  self.items = 0 | IT_SHOTGUN | IT_AXE | IT_LIGHTNING;
  self.ammo_shells = 20;
  self.ammo_cells = 100;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  
  self.max_health = 90;
  self.max_arm = 80;
  self.max_ammo_shells = 50;
  self.max_ammo_nails = 100;
  self.max_ammo_rockets = 8;
  self.max_ammo_cells = 250;
  self.weight = 160;
  self.weapon = IT_LIGHTNING;
};

void () ClassCleric =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec cleric.cfg\n");
  }
  self.player_flags = self.player_flags | PF_MAGIC;
  self.saves = SH_INCINERATE | SH_MAGIC; 
  self.harms = SH_PLASMA | SH_FIRE | SH_SLIME;

  self.items = 0 | IT_SHOTGUN | IT_AXE | IT_LIGHTNING;
  self.ammo_shells = 20;
  self.ammo_cells = 100;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;
  
  self.armorvalue = 0;
  self.armortype = 0;

  
  self.max_health = 120;
  self.max_arm = 120;
  self.max_ammo_shells = 50;
  self.max_ammo_nails = 100;
  self.max_ammo_rockets = 5;
  self.max_ammo_cells = 250;
  self.weight = 200;
  self.weapon = IT_LIGHTNING;
};

void () ClassHweap =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec hweap.cfg\n");
  }
  self.saves = SH_BULLETS | SH_EXPLOSION | SH_UNKNOWN | SH_ELECTRICITY;
  self.harms = SH_FALL | SH_WATER | SH_SLIME | SH_LAVA | SH_SNIPER | SH_FIRE | SH_INCINERATE;

  self.items = 0 | IT_SHOTGUN | IT_AXE | IT_SUPER_NAILGUN;
  self.ammo_shells = 20;
  self.ammo_cells = 0;
  self.ammo_nails = 60;
  self.ammo_rockets = 0;
  
  self.armorvalue = 0;
  self.armortype = 0;

  if (deathmatch == MODE_CTF) {
    self.max_health = 200;
    self.max_arm = 200;
    self.max_ammo_shells = 50;
    self.max_ammo_nails = 80;
    self.max_ammo_rockets = 8;
    self.max_ammo_cells = 80;
  } else {
    self.max_health = 150;
    self.max_arm = 150;
    self.max_ammo_shells = 50;
    self.max_ammo_nails = 80;
    self.max_ammo_rockets = 5;
    self.max_ammo_cells = 30;
  }
  self.weight = 310;
  self.weapon = IT_SUPER_NAILGUN;
  self.sng_mode = 2;
};

void () ClassSoldier =
{
	if (self.option_flags & OF_CE)
		stuffcmd(self,"exec soldier.cfg\n");
 
	self.saves = SH_INCINERATE;
	self.harms = SH_MAGIC | SH_SNIPER;

	self.items = 0 | IT_SHOTGUN | IT_AXE | IT_ROCKET_LAUNCHER | IT_ARMOR1;
	self.ammo_shells = 20;
	self.ammo_cells = 0;
	self.ammo_nails = 0;
	self.ammo_rockets = 20;

	self.armortype = 0.3;
	self.armorvalue = 35;
  
	self.max_health = 140;
	self.max_arm = 140;
	self.max_ammo_shells = 100;
	self.max_ammo_nails = 150;
	self.max_ammo_rockets = 50;
	self.max_ammo_cells = 40;
	self.weight = 260;
	self.weapon = IT_ROCKET_LAUNCHER;
};


void () ClassStandard =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec standard.cfg\n");
  }
  self.saves = SH_MAGIC;
  self.harms = 0;

  self.items = 0 | IT_SHOTGUN | IT_AXE;
  self.ammo_shells = 20;
  self.ammo_cells = 0;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  self.max_health = 100;
  self.max_arm = 200;
  self.max_ammo_shells = 100;
  self.max_ammo_nails = 200;
  self.max_ammo_rockets = 100;
  self.max_ammo_cells = 100;
  self.weight = 230;
  self.weapon = IT_SHOTGUN;
};


void () ClassSpecialist =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec special.cfg\n");
  }
  self.saves = SH_FIRE | SH_WATER | SH_ELECTRICITY;
  self.harms = SH_BULLETS | SH_FALL | SH_SLIME | SH_MAGIC;

  self.player_flags = self.player_flags;

  self.items = 0 | IT_SHOTGUN | IT_AXE | IT_GRENADE_LAUNCHER;
  self.ammo_shells = 20;
  self.ammo_cells = 0;
  self.ammo_nails = 0;
  self.ammo_rockets = 30;

  self.armorvalue = 0;
  self.armortype = 0;

  
  self.max_health = 135;
  self.max_arm = 150;
  self.max_ammo_shells = 50;
  self.max_ammo_nails = 50;
  self.max_ammo_rockets = 100;
  self.max_ammo_cells = 40;
  self.spell = 3;
  self.weight = 220;
  self.weapon = IT_GRENADE_LAUNCHER;
};

void () ClassShock =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec shock.cfg\n");
  }
  self.saves = SH_ELECTRICITY | SH_BULLETS;
  self.harms = SH_FIRE | SH_MAGIC;

  self.items = IT_SHOTGUN | IT_LIGHTNING | IT_SUPER_NAILGUN | IT_AXE;
  self.ammo_shells = 20;
  self.ammo_cells = 0;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  
  self.max_health = 125;
  self.max_arm = 150;
  self.max_ammo_shells = 30;
  self.max_ammo_nails = 60;
  self.max_ammo_rockets = 0;
  self.max_ammo_cells = 4;
  self.weight = 230;
  self.weapon = IT_LIGHTNING;
};

void () ClassDruid =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec druid.cfg\n");
  }
  self.player_flags = self.player_flags | PF_MAGIC | PF_FLY | PF_REGEN; 
  self.saves = SH_WATER | SH_SLIME | SH_LAVA | SH_FALL | SH_FIRE | SH_BULLETS;
  self.harms = SH_SNIPER;

  self.items = 0 | IT_SHOTGUN | IT_AXE | IT_SUPER_NAILGUN | IT_LIGHTNING;
  self.ammo_shells = 20;
  self.ammo_cells = 50;
  self.ammo_nails = 80;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  self.max_health = 115;
  self.max_arm = 115;
  self.max_ammo_shells = 100;
  self.max_ammo_nails = 200;
  self.max_ammo_rockets = 6;
  self.max_ammo_cells = 250;
  self.weight = 200;
  self.weapon = IT_LIGHTNING;
};


void () ClassSilent =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec ninja.cfg\n");
  }
  self.player_flags = self.player_flags | PF_SILENT | PF_MAGIC; 
  self.saves = SH_FALL | SH_WATER | SH_SLIME | SH_PLASMA;
  self.harms = SH_UNKNOWN;

  self.items = 0 | IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_AXE | IT_SUPER_NAILGUN | IT_LIGHTNING;
  self.ammo_shells = 50;
  self.ammo_cells = 125;
  self.ammo_nails = 120;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  self.max_health = 130;
  self.max_arm = 120;
  self.max_ammo_shells = 100;
  self.max_ammo_nails = 250;
  self.max_ammo_rockets = 0;
  self.max_ammo_cells = 250;
  self.weight = 80;
  self.weapon = IT_SUPER_NAILGUN;
};

void () ClassLich =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec lich.cfg\n");
  }
  self.player_flags = self.player_flags | PF_MAGIC | PF_REGEN; 
  // Start with mortal resists.:
  self.saves = SH_BULLETS | SH_UNKNOWN | SH_ELECTRICITY; 
  self.harms = 0;

  self.items = IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_LIGHTNING | IT_AXE;
  self.ammo_shells = 20;
  self.ammo_cells = 160;
  self.ammo_nails = 0;
  self.ammo_rockets = 0;

  self.armorvalue = 0;
  self.armortype = 0;

  
  self.max_health = 75;
  self.max_arm = 60;
  self.max_ammo_shells = 20;
  self.max_ammo_nails = 50;
  self.max_ammo_rockets = 2;
  self.max_ammo_cells = 250;
  self.weight = 190;
  self.weapon = IT_SUPER_SHOTGUN;
};

void () ClassASniper =
{
  if (self.option_flags & OF_CE)
  {
    stuffcmd(self,"exec asniper.cfg\n");
  }
  self.saves = SH_PLASMA | SH_FIRE | SH_MAGIC;
  self.harms = 0;

  self.items = 0 | IT_SHOTGUN | IT_AXE;
  self.ammo_shells = 20;
  self.ammo_cells = 0;
  self.ammo_nails = 0;
  self.ammo_rockets = 20;

  self.max_health = 125;
  self.max_arm = 125;
  self.max_ammo_shells = 20;
  self.max_ammo_nails = 100;
  self.max_ammo_rockets = 8;
  self.max_ammo_cells = 40;
  self.weight = 200;
  self.weapon = IT_SHOTGUN;
};

void() ClassBerserk =
{
	if (self.option_flags & OF_CE)
	{
		stuffcmd(self,"exec berkserk.cfg\n");
	}
	self.saves = 0;
	self.harms = 0;
	self.player_flags = self.player_flags; 

	self.items = IT_AXE;
	self.ammo_shells = 0;
	self.ammo_cells = 0;
	self.ammo_nails = 0;
	self.ammo_rockets = 0;

	self.max_health = 300;
	self.max_arm = 50;
	self.max_ammo_shells = 0;
	self.max_ammo_nails = 0;
	self.max_ammo_rockets = 0;
	self.max_ammo_cells = 0;
	self.weight = 115;
	self.weapon = IT_AXE;
};

void() ClassSet =
{
	if(self.playerclass == CL_STANDARD)
		ClassStandard();
	else if(self.playerclass == CL_SCOUT)
		ClassScout();
	else if(self.playerclass == CL_SNIPER)
		ClassSniper();
	else if(self.playerclass == CL_FIREELEM)
		ClassFireElem();
	else if(self.playerclass == CL_MAGE)
		ClassMage();
	else if(self.playerclass == CL_CLERIC)
		ClassCleric();
	else if(self.playerclass == CL_HWEAP)
		ClassHweap();
	else if(self.playerclass == CL_SOLDIER)
		ClassSoldier();
	else if(self.playerclass == CL_SPECIAL)
		ClassSpecialist();
	else if(self.playerclass == CL_SHOCK)
		ClassShock();
	else if(self.playerclass == CL_DRUID)
		ClassDruid();
	else if(self.playerclass == CL_SILENT)
		ClassSilent();
	else if(self.playerclass == CL_LICH)
		ClassLich();
	else if(self.playerclass == CL_ASNIPER)
		ClassASniper();
	else if(self.playerclass == CL_BERSERK)
		ClassBerserk();
  
  self.option_flags = self.option_flags - (self.option_flags & OF_CE);

  self.health = self.max_health;
};

//void() SpawnRaidens;
void() SpawnSniperTrack;
void() SpawnPipeCleaner;

void() DoStats =
{
  self.touch = SUB_Null;
  
//  self.weapon = IT_AXE;
  self.weaponmodel = "progs/v_axe.mdl";
  self.weaponframe = 0;
  self.impulse = 0;
  self.health = self.max_health;

  self.invincible_time = 0;

  self.radsuit_finished = 0;
  self.invisible_finished = 0;
  self.invincible_finished = 0;
  self.super_damage_finished = 0;

  self.air_finished = time + 12;
  self.dmg = 2;                   // initial water damage

	// class specific stats
	//if (self.player_flags & PF_GLOW)
		//self.effects = self.effects | EF_DIMLIGHT;
		
	if (self.playerclass == CL_FIREELEM)
	{
		self.frame = 0;
		setmodel (self, "progs/s_explod.spr");
	}

  if (self.playerclass == CL_SCOUT)
  {
    SpawnSniperTrack();
    SpawnPipeCleaner();
  }

  if (self.playerclass == CL_SHOCK)
    SpawnRaidens();

  if (self.playerclass == CL_SILENT)
    SpawnBugSpray();
  
};

void() PlayerDie;

void() ZeroStats =
{
   local entity spot;
   
   self.axe_mode = 0;
   self.sg_mode = 0;
   self.ssg_mode = 0;
   self.ng_mode = 0;
   self.sng_mode = 0;
   self.gl_mode = 0;
   self.rl_mode = 0;
   self.tb_mode = 0;
 
   if (deathmatch == MODE_CTF)
   {
   // Trap in case level isn't set up completely...
     if (spot == world)
       spot = SelectSpawnPoint();
   } else 
     spot = SelectSpawnPoint ();

   self.classname = "player";
   self.takedamage = DAMAGE_AIM;
   self.solid = SOLID_SLIDEBOX;
   self.movetype = MOVETYPE_WALK;

   self.show_hostile = 0;
   self.flags = FL_CLIENT;

   self.attack_finished = time;
   self.th_pain = player_pain;
   self.th_die = PlayerDie;
   self.think = SUB_Null;
	
   self.deadflag = DEAD_NO;
   self.pausetime = 0;
	
   self.origin = spot.origin + '0 0 1';
   self.angles = spot.angles;
   self.fixangle = TRUE;

   self.player_flags = (self.player_flags & (PF_HAS_CLASS | PF_CHEATER));// | PF_UNLOCKED));
   self.effects = 0;
   self.items = 0;
   spawn_tdeath (self.origin, self);
};

#ifdef QUAKEWORLD
float(float pc) CheckClassCounts =
{
  local entity head;
  local float clcount,tp;

  if (infokey(world,"classlimit")!="1")
    return 1;

  if (pc == CL_SOLDIER)
    return 1;

  head = FindWorld("player");
  while (head)
  {
    if (head.playerclass == pc)
      clcount = clcount + 1;

    tp = tp + 1;
    head = head.goalentity;
  }

  if (tp <= 4)
    tp = 32;
  tp = ceil(tp * 0.25);

  if (tp>=clcount)
    return 1;

  sprint(self,"Unable to select class: Maximum Percentage Reached!\n");
  cprint(self,"Unable to select class:\nMaximum Percentage Reached!\n");
  return 0;
};
#endif

void () S_SelectClass =
{
	local float oldclass;
	if (self.option_flags & OF_RANDOM)
	{
		cprint(self,"You cannot change classes in RANDOM mode!\n");
		return;
	}
 
	#ifdef QUAKEWORLD
	if(!CheckClassCounts(self.impulse - IMPULSE_CLASS_START))
		return;
	#endif

	oldclass = self.playerclass;
	
	//This is where the class actually gets set
	if (self.impulse != 0)
		self.playerclass = self.impulse - IMPULSE_CLASS_START;
	
	#ifdef GAME_CTF
	if (deathmatch != MODE_CTF) {
	#endif
	
	//Only lose a frag if choosing a class other than standard and not the same as your current class.
	if(self.playerclass != CL_STANDARD && (oldclass != self.playerclass))
		self.frags = self.frags - 1;
	
	#ifdef GAME_CTF
	}
	#endif
	
	self.impulse = 0;
	self.player_flags = self.player_flags | PF_HAS_CLASS;

	self.id_number = random()*5000;
  
	if(self.option_flags & OF_CLASSEXEC)
		self.option_flags = self.option_flags | OF_CE;

	PutClientInServer();
	if(!self.player_flags & PF_MAGIC)
		centerprint(self,"Class set!\n");

};

void() ClassRespawn =
{
  if (self.option_flags & OF_RANDOM)
  {
    self.playerclass = 0;
    while ((self.playerclass < 1) || (self.playerclass > 13))
    {
      self.playerclass = rint(random()*14);
    }
  }

  ZeroStats();
  ClassSet();
  DoStats();
  
	if(self.playerclass == CL_STANDARD)
		sprint(self,"--> You have not picked a class!\n");

  self.items = self.items | IT_INVULNERABILITY;
  self.invincible_time = 1;
  self.invincible_finished = time + 3;
  self.attack_finished = time + 3;

};
