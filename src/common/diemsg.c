/*************************************************************************
 * Metamorphism Quake Server: Common/diemsg.c, Common/diemsg.qc          *
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
 
void(entity targ, entity attacker, entity inflictor, float dam, float dtype) DeathMsg =
{
	local string dmsg;
	local string smsg;
	local float r;
  

	if(dtype == SH_RESET)
	{
		return;
	}

	if (dtype == SH_ILLEGAL)
	{
		bprint(targ.netname);
		bprint(" tried to break the rules.\n");
		return;
	}

// Section 1: Team Play
	if(SameTeam(targ,attacker) && (targ != attacker) && (teamplay & TP_ON))
	{
		if(deathmatch != DMMODE_CTF)
			attacker.frags = attacker.frags - 1;
			
		//Let everyone know what a great teammate 'attacker' is
		
	      
		r = random();
		if(r < 0.25)
		{
			bprint(attacker.netname);
			bprint(" should really check his fire.\n");
		}
		else if(r < 0.5)
		{
			bprint(attacker.netname);
			bprint(" mowed down a teammate.\n");
		}
		else if(r < 0.75)
		{
			bprint(targ.netname);
			bprint(" didn't think ");
			bprint(attacker.netname);
			bprint("'s fire was that friendly.\n");
		}
		else
		{
			bprint(attacker.netname);
			bprint(" decided to help the other team instead.\n");
		}
		
		return;
	}

	
	// section 1.5: Sex determination
	#ifdef QUAKEWORLD
	smsg = infokey(targ,"sex");
	if(smsg == "f")
		smsg = "her ";
	else if(smsg == "n")
		smsg = "its ";
	else
		smsg = "his ";
	#else
	smsg="his ";
	#endif

	// Section 2: Damage names
	// Special color character: AEIOU -> ¡≈…œ’
	if ((dtype & (SH_SNIPER|SH_BULLETS|SH_UNKNOWN|SH_EXPLOSION|SH_MAGIC))!=0)
	{
		if(dam <= 20)
			dmsg = "decimated";
		else if(dam <= 40)
			dmsg = "devastated";
		else if(dam <= 80)
			dmsg = "maimed";
		else if(dam <= 120)
			dmsg = "CR…PPL≈D"; //crippled
		else if(dam <= 160)
			dmsg = "M’T…L¡T≈D";	//mutilated
		else if(dam <= 200)
			dmsg = "ÖD…SM≈MB≈R≈DÖ";	//dismembered
		else if(dam <= 250)
			dmsg = "ÖD≈MœL…SH≈DÖ";	//demolished
		else if(dam <= 325)
			dmsg = "Ö™M¡SS¡CR≈D™Ö";	//massacred
		else if(dam <= 450)
			dmsg = "Ö™P’LV≈R…Z≈D™Ö"; //pulverized
		else if(dam <= 650)
			dmsg = "Ö™Ö≈XT…RP¡T≈DÖ™Ö"; //extirpated
		else
			dmsg = "Ö™Ö¡NN…H…L¡T≈DÖ™Ö"; //annihilated
	}
	else if((dtype & (SH_FIRE|SH_PLASMA))!=0)
	{
		if(dam <= 40)
			dmsg = "torched";
		else if(dam <= 80)
			dmsg = "cooked";
		else if(dam <= 110)
			dmsg = "fried";
		else if(dam <= 140)
			dmsg = "S≈¡R≈D"; //seared
		else if(dam <= 180)
			dmsg = "CH¡RR≈D"; //charred
		else if(dam <= 250)
			dmsg = "Ö…MMœL¡T≈DÖ"; //immolated
		else if(dam <= 340)
			dmsg = "Ö™V¡PœR…Z≈D™Ö"; //vaporized
		else if(dam <= 800)
			dmsg = "Ö™…NC…N≈R¡T≈D™Ö"; //incenerated
		else if(dam <= 2000)
			dmsg = "Ö™ÖCR≈M¡T≈DÖ™Ö"; //cremated
		else
			dmsg = "Ö™ÖD…S…NT≈GR¡T≈DÖ™Ö"; //disintegrated
	}
	else if ((dtype & (SH_ELECTRICITY))!=0)
	{
		if (dam < 15)
			dmsg = "tingled";
		else if(dam <= 35)
			dmsg = "jolted";
		else if(dam <= 75)
			dmsg = "shocked";
		else if(dam <= 120)
			dmsg = "≈L≈CTR…F…≈D"; //electrified
		else if(dam <= 200)
			dmsg = "Ö≈L≈CTRœCUT≈DÖ"; //electricuted
		else if(dam <= 300)
			dmsg = "Ö…œN…Z≈DÖ"; //ionized
		else if(dam <= 400)
			dmsg = "Ö™¡TœM…Z≈D™Ö"; //atomized
		else if(dam <= 600)
			dmsg = "Ö™…RR¡D…¡T≈D™Ö"; //irradiated
		else
			dmsg = "Ö™ÖL¡S≈DÖ™Ö"; //lased
	}
	else if ((dtype & (SH_LAVA|SH_SLIME))!=0)
	{
		if (dam <= 15)
			dmsg = "blemished";
		else if(dam <= 35)
			dmsg = "scortched";
		else if(dam <= 80)
			dmsg = "burned";
		else if(dam <= 150)
			dmsg = "M≈LT≈D"; //melted
		else if(dam <= 250)
			dmsg = "ÖCœRRœD≈DÖ"; //corroded
		else if(dam <= 450)
			dmsg = "Ö™SC¡RR≈D™Ö"; //scarred
		else
			dmsg = "Ö™L…Q’…F…≈D™Ö"; //liquified
	}
	else if ((dtype & (SH_COLD))!=0)
	{
		if(dam <= 20)
			dmsg = "cooled";
		else if(dam <= 60)
			dmsg = "chilled";
		else if(dam <= 100)
			dmsg = "frosted";
		else if(dam <= 180)
			dmsg = "FRœZ≈N"; //frozen
		else if(dam <= 250)
			dmsg = "ÖST…FF≈N≈DÖ"; //stiffened
		else if(dam <= 400)
			dmsg = "Ö™SœL…D…F…≈D™Ö"; //soldified
		else
			dmsg = "Ö™ÖSH¡TT≈R≈DÖ™Ö"; //shattered
	}

	//Fall damage
	if(dtype & SH_FALL)
	{
		if(dam <= 20)
			dmsg = "battered";
		else if(dam <= 50)
			dmsg = "flattened";
		else if(dam <= 100)
			dmsg = "broken in half";
		else if(dam <= 170)
			dmsg = "SPL¡TT≈R≈D";
	}

	if(dtype & SH_WATER)
	{
		bprint(targ.netname);
		
		if (deathmatch != DMMODE_CTF)
			targ.frags = targ.frags - 1;
		
		//Silly fire elementals...
		if(targ.playerclass == CL_FIREELEM)
		{
			if(random() < 0.5)
				bprint(" really shouldn't play in the water.\n");
			else
				bprint(" forgot he was made of fire.\n");
		}
		else
		{			
			if(random() < 0.5)
				bprint(" sleeps with the fishes.\n");
			else
				bprint(" forgot to come up for air.\n");
		}
			
		return;
	}
	
	if(dtype & SH_CRITHIT)
	{
		bprint(attacker.netname);
		bprint(" scores a CRITICAL HIT!\n");
	}

	//Attacker is a dog
	if(attacker.classname == "player" && (attacker.player_flags & PF_DOGGIE))
	{
		r = random();
		
		bprint(targ.netname);
		
		if(r < 0.25)
		{
			bprint(" became ");
			bprint(attacker.netname);
			bprint("'s Milk-Bone.\n");
		}
		else if(r < 0.50)
		{
			bprint(" was shredded by ");
			bprint(attacker.netname);
			bprint("'s fangs.\n");
		}
		else if(r < 0.75)
		{
			bprint("'s kneecaps became ");
			bprint(attacker.netname);
			bprint("'s frisbee.\n");
		}
		else
		{
			bprint(" got a double helping of ");
			bprint(attacker.netname);
			bprint("'s puppy love.\n");
		}
		return;
	}
	
	
	bprint(targ.netname);
	bprint(" was ");
	bprint(dmsg);
	
	// Section 3: attacker = targ (suicide)
	if((attacker == targ) && (inflictor == world))
	{
		if(deathmatch != DMMODE_CTF)
			targ.frags = targ.frags - 1;
		bprint(".\n");
		return;
	}

	if((attacker.classname == "player") || (attacker.flags & FL_MONSTER))
		bprint(" by ");

	if((attacker == targ) || (attacker == world))
	{
		if(deathmatch != DMMODE_CTF)
			targ.frags = targ.frags - 1;

		if (inflictor == attacker) // falling, etc
		{
			if (dtype == SH_FALL)
			{
				bprint(smsg);
				bprint("fall.\n");
				return;
			}

			if (dtype == SH_ELECTRICITY)
			{
				bprint(smsg);
				bprint("discharge.\n");
				return;
			}

		}

		if (attacker == world) // slime, etc
		{
			bprint(" by Nature.\n");
			return;
		}

		bprint(smsg);
		
		if (attacker.items & IT_QUAD)
			bprint("Quad ");
		bprint(inflictor.netname);
		bprint(".\n");
		return;
	} //end when attacker == { target, world }

	// Section 5: Unknown kill (explodbox?)
	if((attacker.classname != "player") && (!(attacker.flags & FL_MONSTER)))
	{
		if (attacker != world)
		{
			if ((attacker.classname == "func_plat") || (attacker.classname == "func_train"))
			{
				bprint(" by ");
				bprint(attacker.netname);
				attacker.deathtype = ftos(attacker.frags);
				bprint(" (");
				bprint(attacker.deathtype);
				bprint(" frags)");
			}
		}
      
		bprint("!\n");
		return;
	}

	// Section 6: normal kill
	//====================================
	
	//TODO: wtf chromatic fireball? this is killable?
	if((targ.netname != "chromatic fireball") && (deathmatch != DMMODE_CTF))
	{
		attacker.frags = attacker.frags + 1;
		attacker.deaths = attacker.deaths + 1;
	}
	
	if (targ.netname != "chromatic fireball" && (deathmatch != DMMODE_CTF))
	{
		attacker.frags = attacker.frags + 1; 
	}

	bprint(attacker.netname);
	bprint("'s ");
	if(attacker.items & IT_QUAD)
		bprint("Quad ");

	if(inflictor != attacker) // projectile
	{
		bprint(inflictor.netname);
		bprint(".\n");
		return;
	}

	if (attacker.weapon == IT_AXE)
	{
		if(attacker.playerclass == CL_FIREELEM)
			bprint("burning touch");
		else if(attacker.playerclass == CL_BERSERK)
		{
			if(random() < 0.5)
				bprint("raging sword swing");
			else
				bprint("razor blade");
		}
		else
			bprint("axe blow");
	}

	if (attacker.weapon == IT_SHOTGUN)
	{
		if (attacker.playerclass == CL_SNIPER)
		{
			if(attacker.sg_mode == 0)
				bprint("snipe");
			else
				bprint("AutoSnipe(tm)");
		}
		else
			bprint("shotgun");
	}
  
	if(attacker.weapon == IT_SUPER_SHOTGUN)
	{
		if(attacker.ssg_mode == 0)
			bprint("double buckshot");
		else
			bprint("sawed off boomstick");
	}

	if(attacker.weapon == IT_SUPER_NAILGUN)
	{
		if (attacker.playerclass == CL_HWEAP)
		{
			if(attacker.sng_mode == 2)
				bprint("siege");
			else if(attacker.sng_mode == 3)
				bprint("assault");
		}
	}

	if (attacker.weapon == IT_LIGHTNING)
	{
		if(attacker.playerclass == CL_DRUID)
		{
			if(attacker.player_flags & PF_DEMON)
				bprint("demon claws");
			else
				bprint("chain lightning");
		}
		else
			bprint("lightning");
	}
	bprint(".\n");
};


#ifdef QUAKEWORLD
#ifdef SPREEMSG
void(entity e) SpreeMsg =
{
	if (e.deaths == 7)
	{
		qw_bprint(PRINT_HIGH,e.netname);
		qw_bprint(PRINT_HIGH," is on a killing spree!\n");
	}
	if (e.deaths == 14)
	{
		qw_bprint(PRINT_HIGH,e.netname);
		qw_bprint(PRINT_HIGH," is berzerk!\n");
	}
	if (e.deaths == 21)
	{
		qw_bprint(PRINT_HIGH,e.netname);
		qw_bprint(PRINT_HIGH," is immortal!\n");
	}
	if (e.deaths == 28)
	{
		qw_bprint(PRINT_HIGH,e.netname);
		qw_bprint(PRINT_HIGH," is a MONKEY!\n");
	}
	if (e.deaths == 35)
	{
		qw_bprint(PRINT_HIGH,e.netname);
		qw_bprint(PRINT_HIGH," is DEATH INCARNATE!\n");
	}
};
#endif
#endif


