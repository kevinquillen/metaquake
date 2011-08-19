/*************************************************************************
 * Metamorphism Quake Server: Common/print.c, Common/print.qc            *
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

/******************************\
 sprint, bprint for portability
\******************************/
void(entity client, string s) sprint =
{
#ifdef QUAKEWORLD
  if (client.classname != "player")
    return;
  qw_sprint(client,2,s);
#else
  quake_sprint(client,s);
#endif
};

void(string s) bprint =
{ 
#ifdef QUAKEWORLD
  qw_bprint(2,s);
#else
  quake_bprint(s);
#endif
};

void(entity client, string head, string body, string ending) cmessage = #73;
void(entity client, string s) cprint =
{
  if (client.hud)
    HUD_Message(client,s);
  else
    cmessage(client,"\n\n\n\n\n\n\n\n\n",s,"\n");
};
