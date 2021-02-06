/*************************************************************************
 * Metamorphism Quake Server: Common/modes.c, Common/modes.qc            *
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
/******************\
 Quake Combat Modes
\******************/
#include "games/ctf/ctf.h"

// spawn flags?
float SF_ONLY_DM = 16384;
float SF_ONLY_SMEAR = 32768;

// teamplay flags (global)
float TP_ON = 1;
float TP_TEAMSAFE = 2;
float TP_SELFSAFE = 4;
float TP_TEAMLOCK = 8;
float TP_MIRRORDAMAGE = 16;

// Modes for team assignments (CTF?)
float MODE_NORMAL = 1;
float MODE_UNKNOWN = 2;	// ???!?!?!?!

// modes for samelevel/noexit
float SL_ON = 1;
float SL_EXITKILL = 2;
float SL_SAMELEVEL = 4;

