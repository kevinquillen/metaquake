/*************************************************************************
 * Metamorphism Quake Server: meta.h                                     *
 * Copyright (C) 1996-2000 by Lon Hohberger.                             *
 * Portions Copyright (C) 2011 by Patrick Baggett                        *
 *                                                                       *
 * This code is free software you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation either version 2 of the License, or (at  *
 * your option) any later version.                                       *
 *                                                                       *
 * This code is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * General Public License for more details.                              *
 *                                                                       *
 * Visit http://www.gnu.org for more information on the GPL.             *
 *                                                                       *
 *************************************************************************/
 
 #ifndef __META_META_H
 #define __META_META_H
 
/***********************\
 Global defines for Meta
\***********************/

// Classes ===============================================================
#define CL_STANDARD	0	//Standard Quake Guy
#define CL_SOLDIER	1
#define CL_SNIPER	2
#define CL_SPECIAL	3	//Specialist
#define CL_SCOUT	4
#define CL_HWEAP	5	//Heavy Weapons / Base Guard (CTF only)
#define CL_MAGE		6
#define CL_CLERIC	7
#define CL_DRUID	8
#define CL_SHOCK	9	//Shock tropper
#define CL_FIREELEM	10	//Fire Elemental
#define CL_SILENT	11	//Ninja
#define CL_LICH		12
#define CL_ASNIPER	13	//Assualt Sniper
#define CL_BERSERK	14

#define CLASS_COUNT 15 //Number of classes

//Since mage/druid morph spells (see player.c and { mage.c, druid.c }) change
//the maximum armor values of the player, use the same value in both *.c files
//by #defining them here
#define CLASS_MAGE_MAXARMOR		80
#define CLASS_DRUID_MAXARMOR	115

// Player field flags ===================================================
#define PF_FLY 1			//Player can fly
#define PF_MAGIC 2			//Player has magic
#define PF_REGEN 4			//Player regenerates when still
#define PF_SANCTUARY 8		//Player is protected by sanctuary
#define PF_SILENT 16		//Player is silent (no sounds)
#define PF_HAS_FLAG 32		//Player has flag (?) [CTF Only]
#define PF_DROPPED_FLAG 64	//Player dropped flag (?) [CTF Only]
#define PF_LOCKED 512		//?? some kind of auto loc
#define PF_NOFRAME 2048
#define PF_EYES_ONLY 4096
#define PF_CHEATER 8192
#define PF_HAS_SPECIAL 16384
#define PF_MISSILE 32768
#define PF_NO_HOOK 65536	//Player does not have a hook (e.g. Fire Elemental)
#define PF_HOOK 131072
#define PF_SPEED 262144
#define PF_DEMON 524288		//Player is currently a demon (spell)
#define PF_NO_MORPH 1048576
#define PF_DOGGIE 2097152	//Player is currently a dog (spell)
#define PF_LIGHT 4194304
#define PF_HAS_CLASS 8388608 //Player has a class (obsolete)

#define OF_NO_MISSILEAIM	1 //???
#define OF_RANDOM			2 //random class on respawn
#define OF_AUTOPUSH			4 //???
#define OF_CLASSEXEC		8 //Execute <classname>.cfg when changing classes
#define OF_CE				16 // (???)

// Save/Harm flags ==============================================
#define SH_FIRE			1
#define SH_COLD			2
#define SH_LAVA			4
#define SH_SLIME		8
#define SH_WATER		16
#define SH_FALL			32
#define SH_UNKNOWN		64
#define SH_BULLETS		128
#define SH_SNIPER		256
#define SH_ELECTRICITY	512
#define SH_EXPLOSION	1024
#define SH_PLASMA		2048
#define SH_INCINERATE	4096
#define SH_MAGIC		8192

#define SH_ILLEGAL		32768
#define SH_CRITHIT		65536

#define SH_RESET		1048576 // reset (??)


// =======================================
#define TEAM1_COLOR 12
#define TEAM2_COLOR 2

// IMPULSES ===============================================
#define IMPULSE_CLASS_START 120
#define IMPULSE_MAGIC_START 150

#define IMPULSE_CLASS_END IMPULSE_CLASS_START +  CLASS_COUNT	//First impulse that is not a class impulse

#endif

