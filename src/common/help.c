/*************************************************************************
 * Metamorphism Quake Server: OldStuff/help.c                            *
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

/* HELP PAGE PROTOTYPES... don't remove: modify, and replace old pages w/
   new ones...

NOTE: These help pages are of a fixed size for one reason. The
  void(entity,string) centerprint statement has a limitation of (I think)
  256 bytes. So, our help pages are 30x7 = 210 bytes, which should leave
  plenty of slack.
*/

void() HelpOnHelp = { centerprint(self.owner, "\(\) Help \(\)\nPress BACKSPACE to quit.     \nPress 1 for Next Page        \nPress 2 for Previous Page    \nPress 3 to view topic/select \n        class                \n\(\) ... on help \(\)\n");};
void() HelpPage1 = { centerprint(self.owner, "\(\) Help \(\)\n>> Class Selection           \n   Help on Magic             \n   Help on the Hook          \n\n\n\(\) Main \(\)\n");};
void() HelpPage2 = { centerprint(self.owner, "\(\) Help \(\)\n   Class Selection           \n>> Help on Magic             \n   Help on the Hook          \n\n\n\(\) Main \(\)\n");};
void() HelpPage3 = { centerprint(self.owner, "\(\) Help \(\)\n   Class Selection           \n   Help on Magic             \n>> Help on the Hook          \n\n\n\(\) Main \(\)\n");};
void() HelpPage4 = { centerprint(self.owner, "\(\) Help  About Classes \(\)\nClasses have different amounts\nof health, ammo, and armor.\nAdditionally, they have vari-\nous abilities. Experiment!\n\(\) Classes \(\)\n"); };
void() HelpPage5 = { centerprint(self.owner, "\(\) Help \(\)\n>> Standard Quake Player     \n   Soldier - Rocket Boy      \n   Sniper - Meant to Camp    \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n\(\) Classes \(\)\n");};
void() HelpPage6 = { centerprint(self.owner, "\(\) Help \(\)\n   Standard Quake Player     \n>> Soldier - Rocket Boy      \n   Sniper - Meant to Camp    \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n\(\) Classes \(\)\n");};
void() HelpPage7 = { centerprint(self.owner, "\(\) Help \(\)\n   Standard Quake Player     \n   Soldier - Rocket Boy      \n>> Sniper - Meant to Camp    \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n\(\) Classes \(\)\n");};
void() HelpPage8 = { centerprint(self.owner, "\(\) Help \(\)\n   Soldier - Rocket Boy      \n   Sniper - Meant to Camp    \n>> Specialist - Bomberman    \n   Scout - Quick & Light     \n   Base Guard / Heavy Weapons\n\(\) Classes \(\)\n");};
void() HelpPage9 = { centerprint(self.owner, "\(\) Help \(\)\n   Sniper - Meant to Camp    \n   Specialist - Bomberman    \n>> Scout - Quick & Light     \n   Base Guard / Heavy Weapons\n   Mage - Magical Offense    \n\(\) Classes \(\)\n");};
void() HelpPage10 = { centerprint(self.owner, "\(\) Help \(\)\n   Specialist - Bomberman    \n   Scout - Quick & Light     \n>> Base Guard / Heavy Weapons\n   Mage - Magical Offense    \n   Cleric - Magical Defense  \n\(\) Classes \(\)\n");};
void() HelpPage11 = { centerprint(self.owner, "\(\) Help \(\)\n   Scout - Quick & Light     \n   Base Guard / Heavy Weapons\n>> Mage - Magical Offense    \n   Cleric - Magical Defense  \n   Druid - Frooty            \n\(\) Classes \(\)\n"); };
void() HelpPage12 = { centerprint(self.owner, "\(\) Help \(\)\n   Base Guard / Heavy Weapons\n   Mage - Magical Offense    \n>> Cleric - Magical Defense  \n   Druid - Frooty            \n   Shock Trooper - zzaapp    \n\(\) Classes \(\)\n");};
void() HelpPage13 = { centerprint(self.owner, "\(\) Help \(\)\n   Mage - Magical Offense    \n   Cleric - Magical Defense  \n>> Druid - Frooty            \n   Shock Trooper - zzaapp    \n   Fire Elemental - Hot.     \n\(\) Classes \(\)\n");};
void() HelpPage14 = { centerprint(self.owner, "\(\) Help \(\)\n   Cleric - Magical Defense  \n   Druid - Frooty            \n>> Shock Trooper - zzaapp    \n   Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n\(\) Classes \(\)\n");};
void() HelpPage15 = { centerprint(self.owner, "\(\) Help \(\)\n   Druid - Frooty            \n   Shock Trooper - zzaapp    \n>> Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n   Lich - Netherworld Magic  \n\(\) Classes \(\)\n");};
void() HelpPage16 = { centerprint(self.owner, "\(\) Help \(\)\n   Shock Trooper - zzaapp    \n   Fire Elemental - Hot.     \n>> Ninja - Quiet, fast       \n   Lich - Netherworld Magic  \n   Assault Sniper - Bang-pow \n\(\) Classes \(\)\n");};
void() HelpPage17 = { centerprint(self.owner, "\(\) Help \(\)\n   Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n>> Lich - Netherworld Magic  \n   Assault Sniper - Bang-pow \n   Berserk - Swords and armor\n\(\) Classes \(\)\n");};
void() HelpPage18 = { centerprint(self.owner, "\(\) Help \(\)\n   Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n   Lich - Netherworld Magic  \n>> Assault Sniper - Bang-pow \n   Berserk - Swords and armor\n\(\) Classes \(\)\n");};
void() HelpPage19 = { centerprint(self.owner, "\(\) Help \(\)\n   Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n   Lich - Netherworld Magic  \n   Assault Sniper - Bang-pow \n>> Berserk - Swords and armor\n\(\) Classes \(\)\n");};
void() HelpPage20 = { centerprint(self.owner, "\(\) Help \(\)\nThe hook on this server is a \ndefinite tool. It is designed\nfor combat, not fleeing. To  \nuse the hook, either:        \n                             \n\(\) The Hook \(\)\n");};
void() HelpPage21 = { centerprint(self.owner, "\(\) Help \(\)\n* Press the number '1' a few \n  times to select it.        \n* Bind a key to 'impulse 11'.\n* Bind a key to '+hook'. This\n  is strongly recommended!   \n\(\) The Hook \(\)\n");};
void() HelpPage22 = { centerprint(self.owner, "\(\) Help \(\)\nSome classes have the ability\nto cast spells. These classes\nmay not use the Thunderbolt  \nor the BFG because of the    \nnature in which they draw    \n\(\) Magic \(\)\n");};
void() HelpPage23 = { centerprint(self.owner, "\(\) Help \(\)\nmana. Impulse commands which \nchange spells and cast on the\nfly are:                     \nnextspell   - Next Spell     \nprevspell   - Previous Spell \n\(\) Magic \(\)\n");};
void() HelpPage24 = { centerprint(self.owner, "\(\) Help \(\)\nspellmode   - Spell Mode     \ncastspell   - Cast selected  \n              spell          \nIn addition to these, spells \nmay also be cast by binding a\n\(\) Magic \(\)\n");};
void() HelpPage25 = { centerprint(self.owner, "\(\) Help \(\)\na key to the number shown in \nbrackets when a spell is     \nselected or by selecting     \nweapon 8 (Thunderbolt) and   \npressing the attack button.  \n\(\) Magic \(\)\n");};


#define HELP_MAX 25

#define HELP_PAGE_CLASSSELECT 1
#define HELP_PAGE_HELPONMAGIC 2
#define HELP_PAGE_HELPONHOOK 3
#define HELP_PAGE_MAGIC 22
#define HELP_PAGE_HOOK 20
#define HELP_PAGE_CLASSSTART 5
#define HELP_PAGE_CLASSEND HELP_PAGE_CLASSSTART + CLASS_COUNT

void() HelpNext =
{
	if (self.help == 100)
	{
		self.help = 1;
		return;
	}

	self.help = self.help + 1;
	if(self.help > HELP_MAX)
		self.help = 1;
};

void() HelpPrev =
{
	if (self.help == 100)
	{
		self.help = 1;
		return;
	}
	self.help = self.help - 1;
	if (self.help < 1)
		self.help = HELP_MAX;
};

void() PutClientInServer;
void() HelpSelectClass =
{
	local entity spot;
	local float theclass;
	
	//If they select the "Class Selection", then
	//jump to the page with their class.
	if(self.help == 1)
	{
		self.help = HELP_PAGE_CLASSSTART+self.playerclass;
		return;
	}

	//If they select the "Help on Magic", jump to that page
	if (self.help == HELP_PAGE_HELPONMAGIC)
	{
		self.help = HELP_PAGE_MAGIC;
		return;
	}
  
	if(self.help == HELP_PAGE_HELPONHOOK)
	{
		self.help = HELP_PAGE_HOOK;
		return;
	}

	self.impulse = 0;
	if(self.help < HELP_PAGE_CLASSSTART || self.help >= HELP_PAGE_CLASSEND)
	{
		sprint(self,"But you are not looking at a class page!\n");
		return;
	}
	else
	{
		theclass = self.help - HELP_PAGE_CLASSSTART;

		#ifdef QUAKEWORLD
		if (!CheckClassCounts(theclass))
		{
			sprint(self,"Maximum percentage for that class has been reached.\n");
			return;
		}
		#endif
	}
  
  self.playerclass = theclass;

  self.help = 0;
  self.player_flags = self.player_flags | PF_HAS_CLASS;
  self.id_number = random()*5000;
  PutClientInServer();
  if (!self.player_flags & PF_MAGIC)
    centerprint(self,"\n\n\n\n\n\n\n\n\n\nClass set!\n");
  else
    centerprint(self,"You have magic!\n[   previous spell     \n]   next spell         \nP   toggle casting type\n    (impulse 198)      \n'   cast current spell \n    (impulse 199)      \n");

};

/*
	Useful macro to generate help if/else blocks:
	
	HELPPAGE(x) ->

	--	
	if(self.owner.help == x)
		HelpPage'x'();
	else
	--
	
	Note that it ends with an 'else' block, so the last HELPPAGE(x) invocation must
	have a clause to be the else clause. Also, this means using a semicolon at the
	end of the macro invocation is likely an error.
*/
#define HELPPAGE(pg)		if(self.owner.help == pg) HelpPage##pg(); else

void() HelpRefresh =
{
	if((self.health > 19) || (self.owner.help != self.max_health))
	{
			
		HELPPAGE(1)
		HELPPAGE(2)
		HELPPAGE(3)
		HELPPAGE(4)
		HELPPAGE(5)
		HELPPAGE(6)
		HELPPAGE(7)
		HELPPAGE(8)
		HELPPAGE(9)
		HELPPAGE(10)
		HELPPAGE(11)
		HELPPAGE(12)
		HELPPAGE(13)
		HELPPAGE(14)
		HELPPAGE(15)
		HELPPAGE(16)
		HELPPAGE(17)
		HELPPAGE(18)
		HELPPAGE(19)
		HELPPAGE(20)
		HELPPAGE(21)
		HELPPAGE(22)
		HELPPAGE(23)
		HELPPAGE(24)
		HELPPAGE(25)
		if (self.owner.help == 100)
			HelpOnHelp();



		self.max_health = self.owner.help;
		self.health = 0;
	}

  if (self.owner.help == 0)
  {
    centerprint(self.owner,"\n");
    self.owner.player_flags = self.owner.player_flags | PF_HAS_CLASS;
    self.owner.takedamage = DAMAGE_AIM;
    self.owner.solid = SOLID_SLIDEBOX;
    bprint(self.owner.netname);
    bprint(" returned from help.\n");
    remove(self);
    return;
  }
  self.nextthink = time + 0.1;
  self.health = self.health + 1;

};

/*
  The help system is an entity that talks to the player.
  It "thinks" every 0.1 seconds, and updates the current help message every
  2.0 seconds. This way, it is more responsive to a player's impulse commands
  and will update the a new page (if necessary) instantly.
*/

void() CreateHelp =
{
	local entity helpsystem;
	if (self.option_flags & OF_RANDOM)
	{
		cprint(self,"You cannot enter help while in RANDOM mode!\n");
		return;
	}
	
	//Spawn the help entity
	helpsystem = spawn();
	helpsystem.origin = self.origin;
	helpsystem.classname = "helpsys";
	helpsystem.owner = self;
  
	self.items = IT_AXE;
	self.weapon = IT_AXE;
	self.ammo_rockets = 0;
	self.ammo_cells = 0;
	self.ammo_nails = 0;
	self.ammo_shells = 0;
	self.player_flags = (self.player_flags & PF_HAS_CLASS) | PF_NOFRAME;
	self.effects = 0;
	self.frame = 0;
	self.velocity = '0 0 0';
	#ifdef QUAKEWORLD
	setmodel(self,"");
	#else
	setmodel(self,"");
	#endif

	self.movetype = MOVETYPE_NONE;
	self.solid = SOLID_NOT;
	self.takedamage = DAMAGE_NO;
	self.help = 100;
	helpsystem.nextthink = time + 0.1;
	helpsystem.think = HelpRefresh;
	
	//Explain why s/he disappeared
	bprint(self.netname);
	bprint(" has entered help.\n");
};
