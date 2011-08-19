/*************************************************************************
 * Metamorphism Quake Server: Common/motd.c, Common/motd.qc              *
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
#include "meta.h"

/* MOTD: Message of the Day */

void() WelcomeMessage;

void() MOTD3 =
{
  if (!deathmatch)
  {
    remove(self);
    return;
  }

  WelcomeMessage();
//  centerprint(self.owner,"Game setting display will\nbe here soon...\nno bots\n");

  self.nextthink = time + 2;
  self.think = SUB_Remove;
};

/*
Mail of the Day
*/

#ifdef GAME_CTF
void(entity newbie) Assign_CTF_Team;
#endif

#ifdef QUAKEWORLD
void() QWForward =
{
  local string fw;
  fw = infokey(world,"forward");
  if (fw!="")
  {
    sprint(self.owner,"*****\nThis server is not accepting connections.\n");
    sprint(self.owner,"Auto-forwarding: ");
    sprint(self.owner,fw);
    sprint(self.owner,"\n*****\n");
    stuffcmd(self.owner,fw);
  }
};
#endif

void() MOTD2 =
{
  local string teamname;

  sprint(self.owner,"Type 'meta' at console for server information.\n");
  sprint(self.owner,"Type 'commands' at console for a list of commands.\n");
  WelcomeMessage();
  
  self.option_flags = self.option_flags | OF_AUTOPUSH | OF_CLASSEXEC;

#ifdef GAME_CTF
  if (deathmatch == MODE_CTF)
  {

    Assign_CTF_Team(self.owner);
    bprint(self.owner.netname);
    bprint(" joins the ");
    bprint_teamcolor(self.owner.fixed_team);
    bprint(" team!\n");
    teamname = TeamToColor(self.owner.fixed_team);

#ifdef QUAKEWORLD
    if (infokey(world,"clanwar")!="1") {
      stuffcmd(self.owner,"team ");
      stuffcmd(self.owner,teamname);
      stuffcmd(self.owner,"\n");
    }
#endif

  }
#endif // GAME_CTF

#ifdef QUAKEWORLD
  QWForward();
#endif

  self.owner.invincible_finished = time + 5;

  self.think = MOTD3;
  self.nextthink = time + 1.7;

};

void(entity monkey) S_MetaInfo;

/*
	Adds an alias given a name and numerical value. Originally,
	Meta hardcoded these impulse values in the string making
	modification/maintenance a nightmare.
*/
void(entity client, string cmd, float value) AddAlias =
{
	local string s;
	
	s = ftos(value);

	/* Effectively the same as: printf("alias %s \"impulse %d\"\n", cmd, (int)value) */
	
	stuffcmd(client, "alias ");
	stuffcmd(client, cmd);
	stuffcmd(client, " \"impulse ");
	stuffcmd(client, s);
	stuffcmd(client, "\"\n");

	
};

/*
  Display the banner, do all the aliasing, and bind appropriate keys.
*/
void() MOTD1 =
{
	local entity pl;
	
	S_MetaInfo(self.owner);
	cdate(self.owner);
	WelcomeMessage();

	//Player reference, so my fingers don't fall off when typing these
	pl = self.owner;
	
	// Aliases -- Class Changing 
	//=========================================================
	AddAlias(self.owner, "standard",IMPULSE_CLASS_START + CL_STANDARD);
	AddAlias(self.owner, "scout",	IMPULSE_CLASS_START + CL_SCOUT);
	AddAlias(self.owner, "sniper",	IMPULSE_CLASS_START + CL_SNIPER);
	AddAlias(self.owner, "soldier", IMPULSE_CLASS_START + CL_SOLDIER);
	AddAlias(self.owner, "hweap",	IMPULSE_CLASS_START + CL_HWEAP);
	AddAlias(self.owner, "special", IMPULSE_CLASS_START + CL_SPECIAL);
	AddAlias(self.owner, "berserk",	IMPULSE_CLASS_START + CL_BERSERK);
	
	AddAlias(self.owner, "mage",	IMPULSE_CLASS_START + CL_MAGE);
	AddAlias(self.owner, "cleric",	IMPULSE_CLASS_START + CL_CLERIC);
	AddAlias(self.owner, "druid",	IMPULSE_CLASS_START + CL_DRUID);
	
	AddAlias(self.owner, "fireelem",IMPULSE_CLASS_START + CL_FIREELEM);
	AddAlias(self.owner, "shock",	IMPULSE_CLASS_START + CL_SHOCK);
	AddAlias(self.owner, "ninja",	IMPULSE_CLASS_START + CL_SILENT);
	AddAlias(self.owner, "lich",	IMPULSE_CLASS_START + CL_LICH);
	AddAlias(self.owner, "asniper",	IMPULSE_CLASS_START + CL_ASNIPER);

// Special aliases...
  stuffcmd(self.owner,"alias light \"impulse 98\"\n");
  stuffcmd(self.owner,"alias +hook \"impulse 90\"\n");
  stuffcmd(self.owner,"alias -hook \"impulse 91\"\n");
  stuffcmd(self.owner,"alias +jet \"impulse 92\"\n");
  stuffcmd(self.owner,"alias -jet \"impulse 93\"\n");
  stuffcmd(self.owner,"alias blowpipe \"impulse 94\"\n");
  stuffcmd(self.owner,"alias mortar \"impulse 108\"\n");
  stuffcmd(self.owner,"alias autopush \"impulse 111\"\n");
  stuffcmd(self.owner,"alias classexec \"impulse 95\"\n");  
  stuffcmd(self.owner,"alias droprune \"impulse 96\"\n");
  stuffcmd(self.owner,"alias dropflag \"impulse 97\"\n");
  stuffcmd(self.owner,"alias aim \"impulse 109\"\n");
  stuffcmd(self.owner,"alias flag \"impulse 110\"\n");
  stuffcmd(self.owner,"alias create \"impulse 113\"\n");
  stuffcmd(self.owner,"alias accept \"impulse 114\"\n");
  stuffcmd(self.owner,"alias teams \"impulse 115\"\n");
  stuffcmd(self.owner,"alias disband \"impulse 117\"\n");
  stuffcmd(self.owner,"alias gorandom \"impulse 106\"\n");
  stuffcmd(self.owner,"alias changeteam \"impulse 116\"\n");
  stuffcmd(self.owner,"alias hr \"impulse 118\"\n");
  stuffcmd(self.owner,"alias hl \"impulse 119\"\n");


// spell commands
  stuffcmd(self.owner,"alias prevspell \"impulse 160\"\n");
  stuffcmd(self.owner,"alias nextspell \"impulse 161\"\n");
  stuffcmd(self.owner,"alias spellmode \"impulse 162\"\n");
  stuffcmd(self.owner,"alias cast \"impulse 163\"\n");


// HUD commands
  stuffcmd(self.owner,"alias hud_off \"impulse 102\"\n");
  stuffcmd(self.owner,"alias hud_on \"impulse 103\"\n");
  stuffcmd(self.owner,"alias hud_up \"impulse 104\"\n");
  stuffcmd(self.owner,"alias hud_down \"impulse 105\"\n");

// Special bindings...
  stuffcmd(self.owner,"bind BACKSPACE \"impulse 99\"\n");
  stuffcmd(self.owner,"alias commands \"impulse 99\"\n");
  stuffcmd(self.owner,"alias meta \"impulse 112\"\n");

  stuffcmd(self.owner,"bind l light\n");
  stuffcmd(self.owner,"bind [ prevspell\n");
  stuffcmd(self.owner,"bind ] nextspell\n");
  stuffcmd(self.owner,"bind p spellmode\n");
  stuffcmd(self.owner,"bind ' cast\n");

  self.think = MOTD2;
  self.nextthink = time + 1.7;
};

/*
  The Mail of the Day procedure. This, again, is an entity. However, since it
  has no user interaction, it only thinks every 1.7 seconds.
  In this function, the user has just entered the game. We start him as a
  STANDARD, with no class chosen (hence the "self.has_class = 0" line)
*/
void() MetaMOTD =
{
  local entity mail;
  self.option_flags = OF_CLASSEXEC;
  self.playerclass = CL_STANDARD;

  mail = spawn();
  mail.owner = self;
  mail.think = MOTD1;
  mail.nextthink = time;                // do mail of the day RIGHT gold...
};

