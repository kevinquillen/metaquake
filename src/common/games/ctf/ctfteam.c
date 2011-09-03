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

void(entity newbie) Assign_CTF_Team =
{
	local entity head;
	local float blues,golds;
	local float f;
	local string str;
	local string st1, st2;
   
	head = FindWorld("player");
	blues = 0;
	golds = 0;

	while (head)
	{
		if (head != newbie)
		{
			f = GetTeam(head);
			if (f == TEAM2_COLOR)
				blues = blues + 1;
			if(f == TEAM1_COLOR)
				golds = golds + 1;
		}
		head = head.goalentity;
	}

	if (golds >= blues)
		newbie.fixed_team = TEAM2_COLOR;
	else
		newbie.fixed_team = TEAM1_COLOR;

#ifdef QUAKEWORLD
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

  str = ftos(newbie.fixed_team);
  stuffcmd(newbie,"color ");
  stuffcmd(newbie,str);
  stuffcmd(newbie,"\n");
};
