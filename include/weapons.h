/*************************************************************************
 * Metamorphism Quake Server: weapons.h,                                 *
 * Copyright (C) 2011 by Patrick Baggett                                 *
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

#ifndef __META_WEAPONS_H
#define __META_WEAPONS_H

 /*
	This file defines symbolic constants for the <weapon>_mode. For example,
	ssg_mode = SSG_NORMAL means normal
 */

//Axe (axe_mode) 
#define AXE_NORMAL	0
#define AXE_HOOK	1
 
//Rocket launcher (rl_mode)
#define RL_NORMAL 0
#define RL_SHALRATH 1
#define RL_DUMBFIRE 2
#define NUM_RL_MODES 3

#endif
