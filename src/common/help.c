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

Well, I think I just killed the server. All the lazy or illiterate people will
just assume people who play on the server are cheating and will disconnect.

I added a message for people who don't choose a class. Every time someone
respawns without having chosen a class, a message appears in the center of
their screen saying to "Press BACKSPACE for help on picking classes".

                                                -- Lon

P.S. I don't care if people are too lazy to read the help files. I worked
TOO much on this server to have people bitch at me because it requires
a little reading :)

NOTE: These help pages are of a fixed size for one reason. The
  void(entity,string) centerprint statement has a limitation of (I think)
  256 bytes. So, our help pages are 30x7 = 210 bytes, which should leave
  plenty of slack.
*/
/*
void() HelpOnHelp = { centerprint(self.owner, "
»  Help » \n
Press BACKSPACE to quit.     \n
Press 1 for Next Page        \n
Press 2 for Previous Page    \n
Press 3 to view topic/select \n
        class                \n
»  ... on help » \n");
};

void() HelpPage1 = { centerprint(self.owner, "
»  Help » \n
>> Class Selection           \n
   Help on Magic             \n
   Help on the Hook          \n
\n
\n
»  Main » \n");
};

void() HelpPage2 = { centerprint(self.owner, "
»  Help » \n
   Class Selection           \n
>> Help on Magic             \n
   Help on the Hook          \n
\n
\n
»  Main » \n");
};

void() HelpPage3 = { centerprint(self.owner, "
»  Help » \n
   Class Selection           \n
   Help on Magic             \n
>> Help on the Hook          \n
\n
\n
»  Main » \n");
};

void() HelpPage4 = { centerprint(self.owner, "
»  Help - About Classes (P 1/2)» \n
Classes have different amounts\n
of health, ammo, and armor.\n
Additionally, they have vari-\n
ous abilities. (next page).
»  Classes » \n");
};

void() HelpPage5 = { centerprint(self.owner, "
»  Help - About Classes (P 2/3)» \n
S - Resists slime\n
L - Resists lava\n
E - Resists electrity\n
C - Resists cold\n
I - Resists fire\n
»  Classes » \n");
};
void() HelpPage6 = { centerprint(self.owner, "
»  Help - About Classes (P 3/3)» \n
F - Can fly\n

»  Classes » \n");
};

void() HelpPage4 = { centerprint(self.owner, "
»  Help - Standard Quake Guy » \n

   Soldier - Rocket Boy      \n
   Sniper - Meant to Camp    \n
   Specialist - Bomberman    \n
   Scout - Quick & Light     \n
»  Classes » \n");
}; 

void() HelpPage5 = { centerprint(self.owner, "
»  Help » \n
   Standard Quake Player     \n
>> Soldier - Rocket Boy      \n
   Sniper - Meant to Camp    \n
   Specialist - Bomberman    \n
   Scout - Quick & Light     \n
»  Classes » \n");
"); 
void() HelpPage6 = { centerprint(self.owner, "
»  Help » \n
   Standard Quake Player     \n
   Soldier - Rocket Boy      \n
>> Sniper - Meant to Camp    \n
   Specialist - Bomberman    \n
   Scout - Quick & Light     \n
»  Classes » \n");
};
 
void() HelpPage7 = { centerprint(self.owner, "
»  Help » \n
   Soldier - Rocket Boy      \n
   Sniper - Meant to Camp    \n
>> Specialist - Bomberman    \n
   Scout - Quick & Light     \n
   Base Guard / Heavy Weapons\n
»  Classes » \n");
}; 

void() HelpPage8 = { centerprint(self.owner, "
»  Help » \n
   Sniper - Meant to Camp    \n
   Specialist - Bomberman    \n
>> Scout - Quick & Light     \n
   Base Guard / Heavy Weapons\n
   Mage - Magical Offense    \n
»  Classes » \n");
}; 

void() HelpPage9 = { centerprint(self.owner, "
»  Help » \n
   Specialist - Bomberman    \n
   Scout - Quick & Light     \n
>> Base Guard / Heavy Weapons\n
   Mage - Magical Offense    \n
   Cleric - Magical Defense  \n
»  Classes » \n");
}; 

void() HelpPage10 = { centerprint(self.owner, "
»  Help » \n
   Scout - Quick & Light     \n
   Base Guard / Heavy Weapons\n
>> Mage - Magical Offense    \n
   Cleric - Magical Defense  \n
   Druid - Frooty            \n
»  Classes » \n"); 
};

void() HelpPage11 = { centerprint(self.owner, "
»  Help » \n
   Base Guard / Heavy Weapons\n
   Mage - Magical Offense    \n
>> Cleric - Magical Defense  \n
   Druid - Frooty            \n
   Shock Trooper - zzaapp    \n
»  Classes » \n");
}; 

void() HelpPage12 = { centerprint(self.owner, "
»  Help » \n
   Mage - Magical Offense    \n
   Cleric - Magical Defense  \n
>> Druid - Frooty            \n
   Shock Trooper - zzaapp    \n
   Fire Elemental - Hot.     \n
»  Classes » \n");
}; 

void() HelpPage13 = { centerprint(self.owner, "
»  Help » \n
   Cleric - Magical Defense  \n
   Druid - Frooty            \n
>> Shock Trooper - zzaapp    \n
   Fire Elemental - Hot.     \n
   Ninja - Quiet, fast       \n
»  Classes » \n");
}; 

void() HelpPage14 = { centerprint(self.owner, "
»  Help » \n
   Cleric - Magical Defense  \n
   Druid - Frooty            \n
   Shock Trooper - zzaapp    \n
>> Fire Elemental - Hot.     \n
   Ninja - Quiet, fast       \n
»  Classes » \n");
};


void() HelpPage15 = { centerprint(self.owner, "
»  Help » \n
   Cleric - Magical Defense  \n
   Druid - Frooty            \n
   Shock Trooper - zzaapp    \n
   Fire Elemental - Hot.     \n
>> Ninja - Quiet, fast       \n
»  Classes » \n");
};


void() HelpPage16 = { centerprint(self.owner, "
»  Help » \n
The hook on this server is a \n
definite tool. It is designed\n
for combat, not fleeing. To  \n
use the hook, either:        \n
                             \n
»  The Hook » \n");
}; 

void() HelpPage17 = { centerprint(self.owner, "
ÄÇ Help ÄÇ\n
* Press the number '1' a few \n
  times to select it.        \n
* Bind a key to 'impulse 11'.\n
* Bind a key to '+hook'. This\n
  is strongly recommended!   \n
»  The Hook » \n");
}; 

void() HelpPage18 = { centerprint(self.owner, "
»  Help » \n
Some classes have the ability\n
to cast spells. These classes\n
may not use the Thunderbolt  \n
or the BFG because of the    \n
nature in which they draw    \n
»  Magic » \n");
}; 

void() HelpPage19 = { centerprint(self.owner, "
»  Help » \n
mana. Impulse commands which \n
change spells and cast on the\n
fly are:                     \n
nextspell   - Next Spell     \n
prevspell   - Previous Spell \n
»  Magic » \n");
}; 

void() HelpPage20 = { centerprint(self.owner, "
»  Help » \n
spellmode   - Spell Mode     \n
castspell   - Cast selected  \n
              spell          \n
In addition to these, spells \n
may also be cast by binding a\n
»  Magic » \n");
}; 

void() HelpPage21 = { centerprint(self.owner, "
»  Help » \n
a key to the number shown in \n
brackets when a spell is     \n
selected or by selecting     \n
weapon 8 (Thunderbolt) and   \n
pressing the attack button.  \n
»  Magic » \n");
}; 
*/

void() HelpOnHelp = { centerprint(self.owner, "»  Help » \nPress BACKSPACE to quit.     \nPress 1 for Next Page/Down   \nPress 2 for Previous Page/Up \nPress 3 to view topic/select \n        class                \n»  ... on help » \n");};

void() HelpPage1 = { centerprint(self.owner, "»  Help » \n>> Class Selection           \n   Help on Magic             \n   Help on the Hook          \n\n\n»  Main » \n");
};

void() HelpPage2 = { centerprint(self.owner, "»  Help » \n   Class Selection           \n>> Help on Magic             \n   Help on the Hook          \n\n\n»  Main » \n");
};

void() HelpPage3 = { centerprint(self.owner, "»  Help » \n   Class Selection           \n   Help on Magic             \n>> Help on the Hook          \n\n\n»  Main » \n");
};

void() HelpPage4 = { centerprint(self.owner, "»  Help » \n>> Standard Quake Player     \n   Soldier - Rocket Boy      \n   Sniper - Meant to Camp    \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n»  Classes » \n");
}; 

void() HelpPage5 = { centerprint(self.owner, "»  Help » \n   Standard Quake Player     \n>> Soldier - Rocket Boy      \n   Sniper - Meant to Camp    \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n»  Classes » \n");
};
 
void() HelpPage6 = { centerprint(self.owner, "»  Help » \n   Standard Quake Player     \n   Soldier - Rocket Boy      \n>> Sniper - Meant to Camp    \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n»  Classes » \n");
};
 
void() HelpPage7 = { centerprint(self.owner, "»  Help » \n   Soldier - Rocket Boy      \n   Sniper - Meant to Camp    \n>> Specialist - Bomberman    \n   Scout - Quick & Light     \n   Base Guard / Heavy Weapons\n»  Classes » \n");
}; 

void() HelpPage8 = { centerprint(self.owner, "»  Help » \n   Sniper - Meant to Camp    \n   Specialist - Bomberman    \n>> Scout - Quick & Light     \n   Base Guard / Heavy Weapons\n   Mage - Magical Offense    \n»  Classes » \n");
}; 

void() HelpPage9 = { centerprint(self.owner, "»  Help » \n   Specialist - Bomberman    \n   Scout - Quick & Light     \n>> Base Guard / Heavy Weapons\n   Mage - Magical Offense    \n   Cleric - Magical Defense  \n»  Classes » \n");
}; 

void() HelpPage10 = { centerprint(self.owner, "»  Help » \n   Scout - Quick & Light     \n   Base Guard / Heavy Weapons\n>> Mage - Magical Offense    \n   Cleric - Magical Defense  \n   Druid - Frooty            \n»  Classes » \n"); 
};

void() HelpPage11 = { centerprint(self.owner, "»  Help » \n   Base Guard / Heavy Weapons\n   Mage - Magical Offense    \n>> Cleric - Magical Defense  \n   Druid - Frooty            \n   Shock Trooper - zzaapp    \n»  Classes » \n");
}; 

void() HelpPage12 = { centerprint(self.owner, "»  Help » \n   Mage - Magical Offense    \n   Cleric - Magical Defense  \n>> Druid - Frooty            \n   Shock Trooper - zzaapp    \n   Fire Elemental - Hot.     \n»  Classes » \n");
}; 

void() HelpPage13 = { centerprint(self.owner, "»  Help » \n   Cleric - Magical Defense  \n   Druid - Frooty            \n>> Shock Trooper - zzaapp    \n   Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n»  Classes » \n");
}; 

void() HelpPage14 = { centerprint(self.owner, "»  Help » \n   Cleric - Magical Defense  \n   Druid - Frooty            \n   Shock Trooper - zzaapp    \n>> Fire Elemental - Hot.     \n   Ninja - Quiet, fast       \n»  Classes » \n");
};

void() HelpPage15 = { centerprint(self.owner, "»  Help » \n   Cleric - Magical Defense  \n   Druid - Frooty            \n   Shock Trooper - zzaapp    \n   Fire Elemental - Hot.     \n>> Ninja - Quiet, fast       \n»  Classes » \n");
};

void() HelpPage16 = { centerprint(self.owner, "»  Help » \nThe hook on this server is a \ndefinite tool. It is designed\nfor combat, not fleeing. To  \nuse the hook, either:        \n\n»  The Hook » \n");
}; 

void() HelpPage17 = { centerprint(self.owner, "…  Help … \n* Press the number '1' a few \n  times to select it.        \n* Bind a key to 'impulse 11'.\n* Bind a key to '+hook'. This\n  is strongly recommended!   \n»  The Hook » \n");
}; 

void() HelpPage18 = { centerprint(self.owner, "»  Help » \nSome classes have the ability\nto cast spells. These classes\nmay not use the Thunderbolt  \nor the BFG because of the    \nnature in which they draw    \n»  Magic » \n");
}; 

void() HelpPage19 = { centerprint(self.owner, "»  Help » \nmana. Impulse commands which \nchange spells and cast on the\nfly are:                     \nnextspell   - Next Spell     \nprevspell   - Previous Spell \n»  Magic » \n");
}; 

void() HelpPage20 = { centerprint(self.owner, "»  Help » \nspellmode   - Spell Mode     \ncast        - Cast selected  \n              spell          \nIn addition to these, spells \nmay also be cast by binding a\n»  Magic » \n");
}; 

void() HelpPage21 = { centerprint(self.owner, "»  Help » \na key to the number shown in \nbrackets when a spell is     \nselected or by selecting     \nweapon 8 (Thunderbolt) and   \npressing the attack button.  \n»  Magic » \n");
}; 



#define HELP_CLPAGE_START 0
#define HELP_MAX 21


void() HelpNext =
{
	if (self.help == 100)
	{
		self.help = 1;
		return;
	}

	self.help = self.help + 1;
	if(self.help < 1 || self.help > HELP_MAX)
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
  if ((self.help < 1) || (self.help > HELP_MAX ))
    self.help = 20;
};

void() PutClientInServer;
void() HelpSelectClass =
{
  local entity spot;
  local float theclass;
  if (self.help == 1)
  {
    self.help = 3+self.playerclass;
    return;
  }

  if (self.help == 2)
  {
    self.help = 18;
    return;
  }
  
  if (self.help == 3)
  {
    self.help = 16;
    return;
  }

  self.impulse = 0;
  if (self.help > 15)
  {
    sprint(self,"But you are not looking at a class page!\n");
    return;
  }

  if ((self.help >= 4) && (self.help <= 15))
    theclass = self.help - 3;

#ifdef QUAKEWORLD
  if (!CheckClassCounts(theclass)) {
    sprint(self,"Maximum percentage for that class has been reached.\n");
    return;
  }
#endif

#ifdef GAME_CTF
  if (deathmatch != MODE_CTF) {
#endif
  if (self.player_flags & PF_HAS_CLASS) self.frags = self.frags - 1;
#ifdef GAME_CTF
  }
#endif
  
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

void() HelpRefresh =
{
  if ((self.health > 19) || (self.owner.help != self.max_health))
  {
    if (self.owner.help == 100)
      HelpOnHelp();
    if (self.owner.help == 1)
      HelpPage1();
    if (self.owner.help == 2)
      HelpPage2();
    if (self.owner.help == 3)
      HelpPage3();
    if (self.owner.help == 4)
      HelpPage4();
    if (self.owner.help == 5)
      HelpPage5();
    if (self.owner.help == 6)
      HelpPage6();
    if (self.owner.help == 7)
      HelpPage7();
    if (self.owner.help == 8)
      HelpPage8();
    if (self.owner.help == 9)
      HelpPage9();
    if (self.owner.help == 10)
      HelpPage10();
    if (self.owner.help == 11)
      HelpPage11();
    if (self.owner.help == 12)
      HelpPage12();
    if (self.owner.help == 13)
      HelpPage13();
    if (self.owner.help == 14)
      HelpPage14();
    if (self.owner.help == 15)
      HelpPage15();
    if (self.owner.help == 16)
      HelpPage16();
    if (self.owner.help == 17)
      HelpPage17();
    if (self.owner.help == 18)
      HelpPage18();
    if (self.owner.help == 19)
      HelpPage19();
    if (self.owner.help == 20)
      HelpPage20();
    if (self.owner.help == 21)
      HelpPage21();

    self.max_health = self.owner.help;
    self.health = 0;
  }

  if (self.owner.help == 0)
  {
#ifdef GAME_CTF
    if (deathmatch != MODE_CTF) {
#endif
    if (self.owner.player_flags & PF_HAS_CLASS) self.owner.frags = self.owner.frags - 1;
#ifdef GAME_CTF
    }
#endif
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
	setmodel(self,"progs/blank.mdl");
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
