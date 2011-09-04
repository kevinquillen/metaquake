/*************************************************************************
 * Metamorphism Quake Server: common/games/ctf/ctfteam.c,                *
 *                            common/games/ctf/ctfteam.qc                *
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
/*
	The basic idea of this is to count up the number of each players, and
	
*/
void(entity newbie) CTF_AssignTeam =
{
	local entity head;
	local float t1,t2;
	local float f;
	local float reassign_colors;
	local string str;
	local string st1, st2;
   
	head = FindWorld("player");
	t1 = 0;
	t2 = 0;
	reassign_colors = 0;
	

	while (head)
	{
		if (head != newbie)
		{
			f = GetTeam(head);
			if (f == TEAM2_COLOR)
				t2 = t2 + 1;
			if(f == TEAM1_COLOR)
				t1 = t1 + 1;
		}
		head = head.goalentity;
	}
	
	/*
		When comparing entity.team (pants color of player) to a named constant, Quake
		does some really silly things. First, the "color" command takes between 0 and 13,
		but what shows up server-side is actually 1-14, so it is necessary to subtract 1
		when comparing to a constant like TEAM1_COLOR. However, relative comparisions like
		if(e1.team == e2.team) are safe because both numbers are "wrong" in the same way.
	*/

	if (t1 >= t2)
	{
		newbie.fixed_team = TEAM2_COLOR;
		if(newbie.team - 1 != TEAM2_COLOR)
			reassign_colors = 1;		
	}
	else
	{
		newbie.fixed_team = TEAM1_COLOR;
		if(newbie.team - 1 != TEAM1_COLOR)
			reassign_colors = 1;
	}

#ifdef QUAKEWORLD
	reassign_colors = 1;
  if (infokey(world,"clanwar")=="1")
  {
    st1 = infokey(world,"clan1");
    st2 = infokey(world,"clan2");
    str = infokey(newbie,"team");
    if (str==st1)
    {
      newbie.fixed_team = TEAM1_COLOR;
    } else if (str==st2)
    {
      newbie.fixed_team = TEAM2_COLOR;
    } else {
      sprint(newbie,"*******\nServer-named teams, please type one of the following:\n");
      sprint(newbie,"team "); sprint(newbie,st1);
      sprint(newbie,"\n");
      sprint(newbie,"team "); sprint(newbie,st2);
      sprint(newbie,"\n\nand reconnect.\n*******\n");
      stuffcmd(newbie,"disconnect \n");
      stuffcmd(newbie,"disconnect \n");
    }
  }
#endif

	//Modify player's colors if they don't match the team they joined
	if(reassign_colors)
	{
		str = ftos(newbie.fixed_team);
		stuffcmd(newbie,"color ");
		stuffcmd(newbie,str);
		stuffcmd(newbie,"\n");
	}
};

void() Scoreboard_Think =
{
	local float team1pts, team2pts;
	local float winner, loser;
	local float minscore, maxscore;
	local float f;
	local string str;
	local entity head;
	
	team1pts = 0;
	team2pts = 0;
	
	head = FindWorld("player");
	

	while(head)
	{
		f = GetTeam(head);
		if(f == TEAM2_COLOR)
			team2pts = team2pts + head.frags;
		if(f == TEAM1_COLOR)
			team1pts = team1pts + head.frags;
		head = head.goalentity;
	}

	bprint("TEAM SCORES: ");
	if(team1pts == team2pts)
	{
		str = ftos(team1pts);
		bprint("The teams are tied with ");
		bprint(str);
		bprint(" points!\n");
	}
	else
	{
		if(team1pts > team2pts)
		{
			winner = TEAM1_COLOR;
			loser = TEAM2_COLOR;
			minscore = team2pts;
			maxscore = team1pts;
		}
		else
		{
			winner = TEAM2_COLOR;
			loser = TEAM1_COLOR;
			minscore = team1pts;
			maxscore = team2pts;
		}
		bprint_teamcolor(winner);
		bprint(" leads ");
		
		str = ftos(maxscore);
		bprint(str);
		bprint(" to ");
		str = ftos(minscore);
		bprint(str);
		bprint("!\n");
	}
	
	self.nextthink = time + 300;
};

void() CTF_SpawnScoreboard =
{
	local entity scoreboard;
	
	scoreboard = spawn();
	scoreboard.think =  Scoreboard_Think;
	scoreboard.nextthink = time + 300;
};

