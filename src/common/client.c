/*
 * Common/client.c, Common/client.qc 
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

#include "meta.h"

#ifdef QUAKEWORLD
#define MAX_RATE 6000
#define MAX_RATE_FIX_COMMAND "rate 6000\n"
#endif

float	intermission_running;
float	intermission_exittime;
void() IntermissionThink;
void() CheckRules;
void() GotoNextMap;
void() PlayerDeathThink;

// prototypes
void () W_WeaponFrame;
void() W_SetCurrentAmmo;
void() player_stand1;
void (vector org) spawn_tfog;
void (vector org, entity death_owner) spawn_tdeath;

float	modelindex_eyes, modelindex_player, 
		modelindex_demon, modelindex_dog, 
		modelindex_hweap, modelindex_fire,
		modelindex_hknight;

		
void() SetChangeParms =
{
};

void() SetNewParms =
{
};

void() DecodeLevelParms =
{
  local entity f;
#ifdef GAME_CTF
  // I don't know what this was
  // at all...
  //
  // think it was clanwar stuff
  /*
  if (deathmatch == MODE_CTF)
  {
    f = find(world,classname,"item_flag_team1");
    if (parm2)
    {
      f = find(world,classname,"item_flag_team1");
      TEAM1_COLOR=parm2;
      f.team = TEAM1_COLOR + 1;
    }
    if (parm2)
    {
      f = find(world,classname,"item_flag_team2");
      TEAM2_COLOR=parm2;
      f.team = TEAM2_COLOR + 1;
    }
  }
  ?*/
#endif
};

void() set_suicide_frame;

// QuakeWorld: DoRatePing -- automatic pushlatency/Rate Cap
// Was DoPush -- added serverinfo min_ping # checking -- for kicks
#ifdef QUAKEWORLD
void() DoRatePing =
{
  local float f,mp;
  local string s;

  s = infokey(self,"rate");
  f = stof(s);
  if (f > MAX_RATE)
  {
	  stuffcmd(self,MAX_RATE_FIX_COMMAND);
  }
  s = infokey(self,"ping");
  f = stof(s);
  s = infokey(world,"min_ping");
  mp = stof(s);
  if (f < mp)
  {
	  bprint(self.netname);
	  bprint(" under min_ping. BANNED.\n");
	  sprint(self,"********\nYour ping is too low for this server. BANNED.\n********\n");
	  stuffcmd(self,"disconnect\n");
	  localcmd("addip ");
	  s = infokey(self,"ip");
	  localcmd(s);
	  localcmd("\n");
	  stuffcmd(self,"disconnect\n");

  }
  if (self.option_flags & OF_AUTOPUSH)
  {
    f = -1*(f/2);
    s = ftos(f);
    sprint(self,"AutoPush: ");
    sprint(self,s);
    sprint(self,"\n");

    stuffcmd(self,"pushlatency ");
    stuffcmd(self,s);
    stuffcmd(self,"\n");
  }

};
#endif
  

// called by ClientKill and DeadThink
void() respawn =
{
	// make a copy of the dead body for appearances sake
	CopyToBodyQueue(self);
	// set default spawn parms
//	SetNewParms ();
	// respawn
#ifdef QUAKEWORLD
        DoRatePing();
#endif

   	PutClientInServer ();
};


/*
============
ClientKill

Player entered the suicide command
============
*/
void() ClientKill =
{
	bprint (self.netname);
	bprint (" suicides\n");
        self.deathtype = "suicide";
	self.modelindex = modelindex_player;
	self.invincible_finished = 0;
	self.health = -50;
	T_Damage(self,world,self,175,SH_UNKNOWN);
#ifdef QUAKEWORLD
	logfrag (self, self);
#endif
	self.frags = self.frags - 2;    // extra penalty
//	respawn ();
	self.attack_finished = time + 3;
};
/*
float(vector v) CheckSpawnPoint =
{
	return FALSE;
};
*/

/*
============
SelectSpawnPoint

Returns the entity to spawn at
============
*/
entity() SelectSpawnPoint =
{
	local   entity spot, newspot, thing;
	local   float   numspots, totalspots;
	local   float   rnum, pcount;
	local   float   rs;
	local entity spots;

// for CTF...
	local string spottype;

	numspots = 0;
	totalspots = 0;

// testinfo_player_start is only found in regioned levels
	spot = find (world, classname, "testplayerstart");
	if (spot)
		return spot;
		
// choose a info_player_deathmatch point

// ok, find all spots that don't have players nearby

	spots = world;

        spottype = "info_player_deathmatch";
      if (v_forward_x != 1333)
#ifdef GAME_CTF
  	if ((deathmatch == MODE_CTF) && (self.classname == "player"))
	{
	  if (GetTeam(self)==TEAM1_COLOR)
	    spottype = "info_player_team1";
	  else if (GetTeam(self)==TEAM2_COLOR)
	    spottype = "info_player_team2";
	  else
            spottype = "info_player_deathmatch";
	} else 
#endif
          spottype = "info_player_deathmatch";

	spot = find (world, classname, spottype);       
	if (!spot)
	{
	  spottype = "info_player_deathmatch";
	  spot = find (world, classname, spottype);       
	}

	while (spot)
	{
		totalspots = totalspots + 1;

		thing=findradius(spot.origin, 84);
		pcount=0;               
		while (thing)
		{
			if ((thing.classname == "player") && (thing.health > 0))
				pcount=pcount + 1;                      
			thing=thing.chain;      
		}
		if (pcount == 0) {
#ifdef GAME_CTF
			if ((deathmatch == MODE_CTF) && 
                            (spot.attack_finished < time) && 
                            (self.classname == "player") &&
                            (spot.classname != "info_player_deathmatch"))
			{
			  spot.attack_finished = time + 3;
			  return spot;
			}
#endif
			spot.goalentity = spots;
			spots = spot;
			numspots = numspots + 1;
		}

		// Get the next spot in the chain
		spot = find (spot, classname, spottype);                

		if ((spottype != "info_player_deathmatch") && (!spot))
		{
		  spottype = "info_player_deathmatch";
		  spot = find(spot,classname, spottype);
		}
	}
	totalspots=totalspots - 1;
	if (!numspots) {
		// ack, they are all full, just pick one at random
//		bprint ( "Ackk! All spots are full. Selecting random spawn spot\n");
		totalspots = rint((random() * totalspots));
		spot = find (world, classname, "info_player_deathmatch");       
		while (totalspots > 0) {
			totalspots = totalspots - 1;
			spot = find (spot, classname, "info_player_deathmatch");
		}
		return spot;
	}
		
// We now have the number of spots available on the map in numspots

	// Generate a random number between 1 and numspots

	numspots = numspots - 1;
	
	numspots = rint((random() * numspots ) );

	spot = spots;
	while (numspots > 0) {
		spot = spot.goalentity;
		numspots = numspots - 1;
	}
	return spot;

};
void() DecodeLevelParms;
void() PlayerDie;

void() PutClientInServer =
{
	// Code from old server inserted
	self.items = 0;
	self.weapon = IT_AXE;

	self.velocity = '0 0 0';
	
	/* Set the origin somewhere wayyyy far out.By this point, a visual copy of
	the player's body (corpse) has been left where they last died (if
	respawning). If 
	*/
	self.origin = '9999 9999 9999'; 

	ClassRespawn();

	DecodeLevelParms();
	
	self.enemy = world;

// oh, this is a hack!

	setmodel (self, "progs/eyes.mdl");
	modelindex_eyes = self.modelindex;

	setmodel (self, "progs/s_explod.spr");
	modelindex_fire = self.modelindex;

	setmodel (self, "progs/player.mdl");
	modelindex_player = self.modelindex;

	setmodel (self, "progs/demon.mdl");
	modelindex_demon = self.modelindex;

	setmodel (self, "progs/dog.mdl");
	modelindex_dog = self.modelindex;

	setmodel (self, "progs/enforcer.mdl");
	modelindex_hweap = self.modelindex;
	
	setmodel(self, "progs/hknight.mdl");
	modelindex_hknight = self.modelindex;

	if(self.playerclass == CL_FIREELEM)
		self.normalmodel = modelindex_fire;
	else if(self.playerclass == CL_BERSERK)
		self.normalmodel = modelindex_hknight;
	else if(self.playerclass == CL_HWEAP)
		self.normalmodel = modelindex_hweap;
	else
		self.normalmodel = modelindex_player;



	setsize (self, VEC_HULL_MIN, VEC_HULL_MAX);
	
  	self.view_ofs = '0 0 22';

	player_stand1 ();
	
	makevectors(self.angles);
	spawn_tfog (self.origin + v_forward*20);

//	self.weapon = W_BestWeapon();
	W_SetCurrentAmmo ();
	spawn_tdeath (self.origin, self);
	self.spawn_origin = self.origin;

};

void() player_demon_jump1;
void() player_dog_jump1;
void() PlayerJump =
{
	local vector start, end;

	if (self.flags & FL_WATERJUMP)
		return;

        if ((self.player_flags & PF_FLY) && (!(self.player_flags & PF_DEMON)))
        {
          self.velocity_z = 220;
          return;
        }
	
	if (self.waterlevel >= 2)
	{

#ifndef QUAKEWORLD
		if (self.watertype == CONTENT_WATER) 
                        self.velocity_z = 100;
		else if (self.watertype == CONTENT_SLIME)
			self.velocity_z = 80;
		else
			self.velocity_z = 50;
#endif

// play swiming sound
		if (self.swim_flag < time)
		{
			self.swim_flag = time + 1;
			if (random() < 0.5)
				sound (self, CHAN_BODY, "misc/water1.wav", 1, ATTN_NORM);
			else
				sound (self, CHAN_BODY, "misc/water2.wav", 1, ATTN_NORM);
		}

		return;
	}

	if (!(self.flags & FL_ONGROUND))
		return;

	if ( !(self.flags & FL_JUMPRELEASED) )
		return;         // don't pogo stick

	self.flags = self.flags - (self.flags & FL_JUMPRELEASED);       
	self.button2 = 0;

// player jumping sound
        if (self.player_flags & PF_DEMON)
        {
           player_demon_jump1();
           return;
        }
        if (self.player_flags & PF_DOGGIE)
        {
           player_dog_jump1();
           return;
        }

	if (!(self.player_flags & PF_FLY))
	{
		if(self.playerclass == CL_BERSERK)
			sound(self, CHAN_BODY, "hknight/grunt.wav", 1, ATTN_NORM);
		else
			sound(self, CHAN_BODY, "player/plyrjmp8.wav", 1, ATTN_NORM);
	}
        self.velocity_z = self.velocity_z + (500 - self.weight);
};


/*
===========
WaterMove

============
*/
.float	dmgtime;

void() WaterMove =
{
	if((self.movetype == MOVETYPE_NOCLIP) || (self.movetype == MOVETYPE_FLY))
		return;
      
	if (self.health < 0)
		return;

	if(self.waterlevel != 3)
	{
		//Can't breathe underwater, so maybe play a gasping sound
		if(!(self.saves & SH_WATER))
		{
			if((self.air_finished < time) && (!(self.player_flags & PF_FLY)))
				sound (self, CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM);
			else if ((self.air_finished < time + 9) && (!(self.player_flags & PF_FLY)))
				sound (self, CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM);
		}
			
		self.air_finished = time + 12;
		self.dmg = 2;
	}
	else if(self.air_finished < time && !(self.saves & SH_WATER)) //out of air -> drown!
	{
		if(self.pain_finished < time)
		{
			self.dmg = self.dmg + 2;
			if (self.dmg > 12)
				self.dmg = 12;
			
			DoDamage(self, world, world, self.dmg, SH_WATER);
			
			//Take more drowning damage in a second
			self.pain_finished = time + 1;
		}
	}
	
	if (!self.waterlevel)
	{
		if (self.flags & FL_INWATER)
		{	
			// play leave water sound
		        
			sound (self, CHAN_BODY, "misc/outwater.wav", 1, ATTN_NORM);
			self.flags = self.flags - FL_INWATER;
		}
		return;
	}
	
	//In lava, but do not have a protection against lava
	if (self.watertype == CONTENT_LAVA && !(self.saves & SH_LAVA))
	{
		// do damage
		if (self.dmgtime < time)
		{
			if (self.radsuit_finished > time)
				self.dmgtime = time + 1;
			else
				self.dmgtime = time + 0.2;

			DoDamage (self, world, world, 10*self.waterlevel, SH_LAVA);
		}
	}
	else if (self.watertype == CONTENT_SLIME && !(self.saves & SH_SLIME))
	{
		// do damage
		if (self.dmgtime < time && self.radsuit_finished < time)
		{
			self.dmgtime = time + 1;
			DoDamage (self, world, world, 4*self.waterlevel, SH_SLIME);
		}
	}
	
	if ( !(self.flags & FL_INWATER) )
	{	

// player enter water sound

		if (self.watertype == CONTENT_LAVA)
			sound (self, CHAN_BODY, "player/inlava.wav", 1, ATTN_NORM);
		if (self.watertype == CONTENT_WATER)
			sound (self, CHAN_BODY, "player/inh2o.wav", 1, ATTN_NORM);
		if (self.watertype == CONTENT_SLIME)
			sound (self, CHAN_BODY, "player/slimbrn2.wav", 1, ATTN_NORM);

		self.flags = self.flags + FL_INWATER;
		self.dmgtime = 0;
	}
	
        if (! (self.player_flags & PF_FLY))
          if (! (self.flags & FL_WATERJUMP) )
		self.velocity = self.velocity - 0.8*self.waterlevel*frametime*self.velocity;
};



void() CheckWaterJump =
{
	local vector start, end;

// check for a jump-out-of-water
	makevectors (self.angles);
	start = self.origin;
	start_z = start_z + 8; 
	v_forward_z = 0;
	normalize(v_forward);
	end = start + v_forward*24;
	traceline (start, end, TRUE, self);
	if (trace_fraction < 1)
	{       // solid at waist
		start_z = start_z + self.maxs_z - 8;
		end = start + v_forward*24;
		self.movedir = trace_plane_normal * -50;
		traceline (start, end, TRUE, self);
		if (trace_fraction == 1)
		{       // open at eye level
			self.flags = self.flags | FL_WATERJUMP;
			self.velocity_z = 325;
			self.flags = self.flags - (self.flags & FL_JUMPRELEASED);
//			self.teleport_time = time + 2;  // safety net
			return;
		}
	}
};

/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void() PlayerPreThink =
{
	local   float   mspeed, aspeed;
	local   float   r;

	if (intermission_running)
	{
		IntermissionThink ();   // otherwise a button could be missed between
		return;                                 // the think tics
	}

	if (self.view_ofs == '0 0 0')
		return;         // intermission or finale

	makevectors (self.v_angle);             // is this still used

        self.deathtype = "";

	CheckRules();
	WaterMove();
	Magic_CheckExpired(self);


#ifndef QUAKEWORLD
	if (self.waterlevel == 2)
		CheckWaterJump ();
#endif


	if (self.deadflag >= DEAD_DEAD)
	{
		PlayerDeathThink ();
		return;
	}
	
	if (self.deadflag == DEAD_DYING)
		return; // dying, so do nothing

	if (self.button2)
	{
		PlayerJump ();
	}
	else
		self.flags = self.flags | FL_JUMPRELEASED;

// teleporters can force a non-moving pause time        
	if (time < self.pausetime)
		self.velocity = '0 0 0';

	if(time > self.attack_finished && self.currentammo == 0 && self.weapon != IT_AXE)
	{
		self.weapon = W_BestWeapon ();
		W_SetCurrentAmmo ();
	}
};
	
/*
================
CheckPowerups

Check for turning off powerups
================
*/
void() CheckPowerups =
{
	if (self.health <= 0)
		return;

	CheckTeam();	// for teamlock, and (QW-CTF) crossdressing

	if (self.player_flags & PF_NOFRAME)
	  return;

	//Fire elemental has special logic for lava/water
	if (self.playerclass == CL_FIREELEM)
	{
		//The fire elemental takes massive damage from water but heals faster in lava.
		if(self.watertype == CONTENT_WATER)
			DoDamage(self, self, world, 3, SH_WATER);
		else if(self.watertype == CONTENT_LAVA)
		{
			self.health = self.health + 1;
			if (self.health > self.max_health)
				self.health = self.max_health;
		}
	}


// Model handling (semi new)
	if (self.player_flags & PF_DEMON)
	{
		if (self.normalmodel != modelindex_demon)
			self.normalmodel = modelindex_demon;
	}
	if (self.player_flags & PF_DOGGIE)
	{
		if (self.normalmodel != modelindex_dog)
			self.normalmodel = modelindex_dog;
	}

	if (self.player_flags & PF_NO_MORPH)
	{
		self.normalmodel = modelindex_player;
		self.player_flags = self.player_flags - PF_NO_MORPH;
	}



// ************
        if (self.pausetime > time)
          self.maxspeed = 0;
        else
          self.maxspeed = 340;

// invisibility
	if (self.invisible_finished)
	{
// sound and screen flash when items starts to run out
		if (self.invisible_sound < time)
		{
			sound (self, CHAN_AUTO, "items/inv3.wav", 0.5, ATTN_IDLE);
			self.invisible_sound = time + ((random() * 3) + 1);
		}


		if (self.invisible_finished < time + 3)
		{
			if (self.invisible_time == 1)
			{
				sprint (self,  "Ring of Shadows magic is fading\n");
				stuffcmd (self, "bf\n");
				sound (self, CHAN_AUTO, "items/inv2.wav", 1, ATTN_NORM);
				self.invisible_time = time + 1;
			}
			
			if (self.invisible_time < time)
			{
				self.invisible_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self.invisible_finished < time)
		{       // just stopped
			self.items = self.items - IT_INVISIBILITY;
			self.invisible_finished = 0;
			self.invisible_time = 0;
		}
		
	// use the eyes
		self.frame = 0;
		self.modelindex = modelindex_eyes;
	}
	else
//	{
//		if ((self.playerclass == CL_SNIPER) && (self.weapon == IT_SHOTGUN))
//		  if (self.velocity=='0 0 0')
//		    self.modelindex = modelindex_eyes;
//               else
		  self.modelindex = self.normalmodel;    // don't use eyes
//	}

// Sniper special: invisible when sniper rifle used and standing still
	if ((self.playerclass == CL_SNIPER) && (self.flags & FL_ONGROUND))
        {
          if (((self.weapon == IT_SHOTGUN) && (self.velocity=='0 0 0')) && (self.sg_mode == 0))
	  {
            self.modelindex = modelindex_eyes;
	  }

        }

	
// Druid combine...
	if (self.player_flags & PF_EYES_ONLY)
	{
	  self.modelindex = modelindex_eyes;
        }


// invincibility
	if (self.invincible_finished)
	{
// sound and screen flash when items starts to run out
		if (self.invincible_finished < time + 3)
		{
			if (self.invincible_time == 1)
			{
				sprint (self,  "Protection is almost burned out\n");
				stuffcmd (self, "bf\n");
				sound (self, CHAN_AUTO, "items/protect2.wav", 1, ATTN_NORM);
				self.invincible_time = time + 1;
			}
			
			if (self.invincible_time < time)
			{
				self.invincible_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}
		
		if (self.invincible_finished < time)
		{       // just stopped
			self.items = self.items - IT_INVULNERABILITY;
			self.invincible_time = 0;
			self.invincible_finished = 0;
		}
		if (self.invincible_finished > time)
		{
#ifndef QUAKEWORLD
			self.effects = self.effects | EF_DIMLIGHT;
#else
			self.effects = self.effects | EF_RED;
#endif
		}
		else
		{
#ifndef QUAKEWORLD
			self.effects = self.effects - (self.effects & EF_DIMLIGHT);
#else
			self.effects = self.effects - (self.effects & EF_RED);
#endif
		}
	}

// super damage
	if (self.super_damage_finished)
	{

// sound and screen flash when items starts to run out

		if (self.super_damage_finished < time + 3)
		{
			if (self.super_time == 1)
			{
				if (deathmatch == 4)
					sprint (self,  "OctaPower is wearing off\n");
				else
					sprint (self,  "Quad Damage is wearing off\n");
				stuffcmd (self, "bf\n");
				sound (self, CHAN_AUTO, "items/damage2.wav", 1, ATTN_NORM);
				self.super_time = time + 1;
			}         
			
			if (self.super_time < time)
			{
				self.super_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self.super_damage_finished < time)
		{       // just stopped
			self.items = self.items - IT_QUAD;
			if (deathmatch == 4)
			{
				self.ammo_cells = 255;
				self.armorvalue = 1;
				self.armortype = 0.8;
				self.health = 100;
			}
			self.super_damage_finished = 0;
			self.super_time = 0;
		}
		if (self.super_damage_finished > time)
		{
#ifndef QUAKEWORLD
			self.effects = self.effects | EF_DIMLIGHT;
#else
			self.effects = self.effects | EF_BLUE;
#endif
		}
		else
		{
#ifndef QUAKEWORLD
			self.effects = self.effects - (self.effects & EF_DIMLIGHT);
#else
			self.effects = self.effects - (self.effects & EF_BLUE);
#endif
		}
	}       

// suit 
	if (self.radsuit_finished)
	{
		self.air_finished = time + 12;          // don't drown

// sound and screen flash when items starts to run out
		if (self.radsuit_finished < time + 3)
		{
			if (self.rad_time == 1)
			{
				sprint (self,  "Air supply in Biosuit expiring\n");
				stuffcmd (self, "bf\n");
				sound (self, CHAN_AUTO, "items/suit2.wav", 1, ATTN_NORM);
				self.rad_time = time + 1;
			}
			
			if (self.rad_time < time)
			{
				self.rad_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self.radsuit_finished < time)
		{       // just stopped
			self.items = self.items - IT_SUIT;
			self.rad_time = 0;
			self.radsuit_finished = 0;
		}
	}       

};


/*
================
PlayerPostThink

Called every frame after physics are run
================
*/
void() PlayerPostThink =
{
	local   float   mspeed, aspeed;
	local   float   r;

//dprint ("post think\n");
	if (self.view_ofs == '0 0 0')
		return;         // intermission or finale
	if (self.deadflag)
		return;

// check to see if player landed and play landing sound 
	if ((self.jump_flag < -300) && (self.flags & FL_ONGROUND) )
	{
		if (self.watertype == CONTENT_WATER)
			sound (self, CHAN_BODY, "player/h2ojump.wav", 1, ATTN_NORM);
		else if (self.jump_flag < -650)
		{
                        r = -0.1*(self.jump_flag + (700 - self.weight));
			self.deathtype = "falling";
                        self.jump_flag = 0;
                        self.velocity_z = 0;
			DoDamage (self, self, self, r, SH_FALL);
			if (!(self.player_flags & PF_FLY)) 
			  sound (self, CHAN_VOICE, "player/land2.wav", 1, ATTN_NORM);
		}
		else
			sound (self, CHAN_VOICE, "player/land.wav", 1, ATTN_NORM);
	}

	self.jump_flag = self.velocity_z;

	CheckPowerups ();

	W_WeaponFrame ();

};


/*
===========
ClientConnect

called when a player connects to a server
============
*/
//void() Assign_CTF_Team;
void() MetaMOTD;
void() ClientConnect =
{
        local string str;
	bprint ( self.netname);
	bprint ( " joined the ");
	if (deathmatch == 1)
	{
		bprint("Deathmatch\n");
	}
#ifdef GAME_CTF
	if (deathmatch == 2)
	{
		bprint("CTF game\n");
	}
#endif

#ifdef QUAKEWORLD
        str = infokey(self,"ip");
#endif

	if ((!deathmatch) || (coop))
	{
          bprint("No single player/cooperative modes\navailable.\n");
	  stuffcmd(self,"disconnect\n");
	}

	self.wait = time + 30;

        self.player_flags = 0;
	self.option_flags = OF_AUTOPUSH;
	self.fixed_team = 255;
        self.teament = world;
	self.handicap = 1.0;

	MetaMOTD();

// a client connecting during an intermission can cause problems
	if (intermission_running)
		GotoNextMap ();
};


/*
===========
ClientDisconnect

called when a player disconnects from a server
============
*/
void() ClientDisconnect =
{
	// let everyone else know
	self.fixed_team = 255;
	self.team = 0;
	self.id_number = 0;
#ifdef GAME_CTF
        self.player_flags = self.player_flags - (self.player_flags & (PF_HAS_SPECIAL|PF_HAS_FLAG));
#else
        self.player_flags = self.player_flags - (self.player_flags & PF_HAS_SPECIAL);
#endif
	bprint ( self.netname);
		bprint ( " left the game with ");
		bprint ( ftos(self.frags));
		bprint ( " frags\n");
        self.classname = "unknown";
        self.health = 0;
        self.solid = SOLID_NOT;
	sound (self, CHAN_BODY, "player/tornoff2.wav", 1, ATTN_NONE);
	set_suicide_frame ();
};

