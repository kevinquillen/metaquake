/*
 * Common/player.c, Common/player.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */
 
#include "meta.h"
void() bubble_bob;

#define DOGFRAME_STAND_BASE		69	//Base frame of dog's standing animation (see Quake 1.06 source, dog.qc)
#define DOGFRAME_STAND_LENGTH	9	//Length of dog's standing animation

/*
==============================================================================

PLAYER

==============================================================================
*/

$cd /raid/quake/id1/models/player_4
$origin 0 -6 24
$base base              
$skin skin

//
// running
//
$frame axrun1 axrun2 axrun3 axrun4 axrun5 axrun6

$frame rockrun1 rockrun2 rockrun3 rockrun4 rockrun5 rockrun6

//
// standing
//
$frame stand1 stand2 stand3 stand4 stand5

$frame axstnd1 axstnd2 axstnd3 axstnd4 axstnd5 axstnd6
$frame axstnd7 axstnd8 axstnd9 axstnd10 axstnd11 axstnd12


//
// pain
//
$frame axpain1 axpain2 axpain3 axpain4 axpain5 axpain6

$frame pain1 pain2 pain3 pain4 pain5 pain6


//
// death
//

$frame axdeth1 axdeth2 axdeth3 axdeth4 axdeth5 axdeth6
$frame axdeth7 axdeth8 axdeth9

$frame deatha1 deatha2 deatha3 deatha4 deatha5 deatha6 deatha7 deatha8
$frame deatha9 deatha10 deatha11

$frame deathb1 deathb2 deathb3 deathb4 deathb5 deathb6 deathb7 deathb8
$frame deathb9

$frame deathc1 deathc2 deathc3 deathc4 deathc5 deathc6 deathc7 deathc8
$frame deathc9 deathc10 deathc11 deathc12 deathc13 deathc14 deathc15

$frame deathd1 deathd2 deathd3 deathd4 deathd5 deathd6 deathd7
$frame deathd8 deathd9

$frame deathe1 deathe2 deathe3 deathe4 deathe5 deathe6 deathe7
$frame deathe8 deathe9

//
// attacks
//
$frame nailatt1 nailatt2

$frame light1 light2

$frame rockatt1 rockatt2 rockatt3 rockatt4 rockatt5 rockatt6

$frame shotatt1 shotatt2 shotatt3 shotatt4 shotatt5 shotatt6

$frame axatt1 axatt2 axatt3 axatt4 axatt5 axatt6

$frame axattb1 axattb2 axattb3 axattb4 axattb5 axattb6

$frame axattc1 axattc2 axattc3 axattc4 axattc5 axattc6

$frame axattd1 axattd2 axattd3 axattd4 axattd5 axattd6


/*
==============================================================================
PLAYER
==============================================================================
*/

void() player_run;
void() player_berserk_stand;

void()  player_stand1 =[        $axstnd1,       player_stand1   ]
{
	self.weaponframe=0;

// regenerate hitpoints if standing still and in air
        if ((self.player_flags & PF_REGEN) && (self.health < self.max_health))
	  if ((pointcontents(self.origin)!=CONTENT_WATER) && (pointcontents(self.origin)!=CONTENT_LAVA) && (pointcontents(self.origin)!=CONTENT_SLIME))
          self.health = self.health + 1;

// regenerate "magic"
        if (((self.playerclass == CL_HWEAP) && (self.ammo_nails < self.max_ammo_nails)) && (self.attack_finished < time)) self.ammo_nails = self.ammo_nails + 1;

        if ((self.player_flags & PF_MAGIC) && (self.ammo_cells < self.max_ammo_cells))
          self.ammo_cells = self.ammo_cells + 1;

        if ((self.playerclass == CL_SHOCK) && (self.ammo_cells < self.max_ammo_cells))
	  self.ammo_cells = self.ammo_cells + 1;

	if (self.player_flags & PF_NOFRAME)
        {
          self.frame = 1;
          return;
        }



	if (self.velocity_x || self.velocity_y)
	{
		self.walkframe=0;
		player_run();
		return;
	}

	//Handle morphs
	if(self.player_flags & PF_DEMON)
	{
		if (self.walkframe >= 13)
			self.walkframe = 0;
		self.frame = $axrun1 + self.walkframe;
	}
	else if(self.player_flags & PF_DOGGIE)
	{
		//Need to wrap stand animation?
		if(self.walkframe >= DOGFRAME_STAND_LENGTH) 
			self.walkframe = 0;
		self.frame = DOGFRAME_STAND_BASE+self.walkframe;
	}
	else if(self.playerclass == CL_HWEAP)
	{
		if (self.walkframe >= 7)
			self.walkframe = 0;
		self.frame = $axrun1 + self.walkframe;
	}
	else if(self.playerclass == CL_FIREELEM)
	{
		if (self.walkframe > 3)
			self.walkframe = 1;
		self.frame = self.walkframe;
	}
	else if(self.playerclass == CL_BERSERK)
	{
		player_berserk_stand();
	}
	else if (self.weapon == IT_AXE)
	{
		if (self.walkframe >= 12)
			self.walkframe = 0;
		self.frame = $axstnd1 + self.walkframe;
	}
	else
	{
		if (self.walkframe >= 5)
			self.walkframe = 0;
		self.frame = $stand1 + self.walkframe;
	}
	self.walkframe = self.walkframe + 1;    
};

// Run animations
//================================================

void() player_berserk_run; //prototype

void()  player_run =[   $rockrun1,      player_run      ]
{
	self.weaponframe=0;

	if(self.help)
		return;

	if(((self.playerclass == CL_HWEAP) && (self.ammo_nails < self.max_ammo_nails)) && (self.attack_finished < time))
		self.ammo_nails = self.ammo_nails + 1;

	if((self.player_flags & PF_MAGIC) && (self.ammo_cells < self.max_ammo_cells))
		self.ammo_cells = self.ammo_cells + 1;
		
	if((self.playerclass == CL_SHOCK) && (self.ammo_cells < self.max_ammo_cells))
		self.ammo_cells = self.ammo_cells + 1;

	if (self.player_flags & PF_NOFRAME)
	{
		self.frame = 1;
		return;
	}

	//Stopped moving?
	if (!self.velocity_x && !self.velocity_y)
	{
		self.walkframe=0;
		player_stand1();
		return;
	}

	if (self.player_flags & PF_DEMON)
	{
		if (self.walkframe >= 7)
			self.walkframe = 0;
		self.frame = $axstnd6 + self.walkframe;
	}
	else if (self.player_flags & PF_DOGGIE)
	{
		if (self.walkframe >= 12)
			self.walkframe = 0;
		self.frame = $axdeth8 + self.walkframe;
	}
	else if (self.playerclass == CL_HWEAP)
	{
		if (self.walkframe >= 9)
			self.walkframe = 0;
		self.frame = $axstnd7 + self.walkframe;
	}
	else if (self.playerclass == CL_FIREELEM)
	{
		if (self.walkframe > 3)
			self.walkframe = 1;
		self.frame = self.walkframe;
	}
	else if(self.playerclass == CL_BERSERK)
	{
		player_berserk_run();
	}
	else if (self.weapon == IT_AXE)
	{
		if (self.walkframe == 6)
			self.walkframe = 0;
		self.frame = $axrun1 + self.walkframe;
	}
	else
	{
		if (self.walkframe == 6)
			self.walkframe = 0;
		self.frame = self.frame + self.walkframe;
	}
	self.walkframe = self.walkframe + 1;
};

void()muzzleflash =
{
     if ((self.playerclass == CL_SNIPER) && (self.weapon == IT_SHOTGUN))
       return;

#ifdef QUAKEWORLD
     WriteByte (MSG_MULTICAST, SVC_MUZZLEFLASH);
     WriteEntity (MSG_MULTICAST, self);
     multicast (self.origin, MULTICAST_PVS);
#else
     self.effects = self.effects | EF_MUZZLEFLASH;
#endif
};


void()  player_shot1 =  [$shotatt1, player_shot2        ] {self.weaponframe=1;muzzleflash();};
void()  player_shot2 =  [$shotatt2, player_shot3        ] {self.weaponframe=2;};
void()  player_shot3 =  [$shotatt3, player_shot4        ] {self.weaponframe=3;};
void()  player_shot4 =  [$shotatt4, player_shot5        ] 
{
 self.weaponframe=4;
 if (self.playerclass == CL_SHOCK)
   self.ammo_cells = 4;
};
void()  player_shot5 =  [$shotatt5, player_shot6        ] {self.weaponframe=5;};
void()  player_shot6 =  [$shotatt6, player_run  ] {self.weaponframe=6;};

void()  player_axe1 =   [$axatt1, player_axe2   ] {self.weaponframe=1;};
void()  player_axe2 =   [$axatt2, player_axe3   ] {self.weaponframe=2;};
void()  player_axe3 =   [$axatt3, player_axe4   ] {self.weaponframe=3;W_FireAxe();};
void()  player_axe4 =   [$axatt4, player_run    ] {self.weaponframe=4;};

void()  player_axeb1 =  [$axattb1, player_axeb2 ] {self.weaponframe=5;};
void()  player_axeb2 =  [$axattb2, player_axeb3 ] {self.weaponframe=6;};
void()  player_axeb3 =  [$axattb3, player_axeb4 ] {self.weaponframe=7;W_FireAxe();};
void()  player_axeb4 =  [$axattb4, player_run   ] {self.weaponframe=8;};

void()  player_axec1 =  [$axattc1, player_axec2 ] {self.weaponframe=1;};
void()  player_axec2 =  [$axattc2, player_axec3 ] {self.weaponframe=2;};
void()  player_axec3 =  [$axattc3, player_axec4 ] {self.weaponframe=3;W_FireAxe();};
void()  player_axec4 =  [$axattc4, player_run   ] {self.weaponframe=4;};

void()  player_axed1 =  [$axattd1, player_axed2 ] {self.weaponframe=5;};
void()  player_axed2 =  [$axattd2, player_axed3 ] {self.weaponframe=6;};
void()  player_axed3 =  [$axattd3, player_axed4 ] {self.weaponframe=7;W_FireAxe();};
void()  player_axed4 =  [$axattd4, player_run   ] {self.weaponframe=8;};

void()  player_fe_axe1 = [0, player_fe_axe2] {self.weaponframe=1;};
void()  player_fe_axe2 = [0, player_fe_axe3] {self.weaponframe=2;};
void()  player_fe_axe3 = [0, player_fe_axe4] {self.weaponframe=3;W_FireAxe();};
void()  player_fe_axe4 = [0, player_run    ] {self.weaponframe=4;};


//******************************
// New framing (weapon/magic)
//******************************

// Player hook frames
void()  player_hook1 =   [$axatt1, player_hook2   ] {self.weaponframe=1;};
void()  player_hook2 =   [$axatt2, player_hook3   ] {self.weaponframe=2;W_FireHook();};
void()  player_hook3 =   [$axatt3, player_hook3   ] 
{
  self.weaponframe=3;
  if ((!self.button0) || (!(self.player_flags & PF_HOOK)))
    {player_run ();return;}
};

// Player casting frames
void()  player_cast_ax1 =       [       $axdeth1,       player_cast_ax2 ] {};
void()  player_cast_ax2 =       [       $axdeth1,       player_cast_ax3 ] {};
void()  player_cast_ax3 =       [       $axdeth1,       player_cast_ax4 ] {};
void()  player_cast_ax4 =       [       $axdeth1,       player_stand1   ] {};

void()  player_cast_gun1 =       [       $deathd1,      player_cast_gun2] {};
void()  player_cast_gun2 =       [       $deathd1,      player_cast_gun3] {};
void()  player_cast_gun3 =       [       $deathd1,      player_cast_gun4] {};
void()  player_cast_gun4 =       [       $deathd1,      player_stand1   ] {};

//********************************************

//============================================================================

void() player_nail1   =[$nailatt1, player_nail2  ] 
{
	muzzleflash();

	if (!self.button0 || intermission_running || self.impulse)
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;
	SuperDamageSound();
	if (((self.sng_mode == 0) && (self.weapon == IT_SUPER_NAILGUN)) ||
            (self.weapon == IT_NAILGUN))
	  W_FireSpikes (4);
	else
	  W_FireLaser();
	self.attack_finished = time + 0.2;
};

void() player_nail2   =[$nailatt2, player_nail1  ]
{
	muzzleflash();

	if (!self.button0 || intermission_running || self.impulse)
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;
	SuperDamageSound();
	if (((self.sng_mode == 0) && (self.weapon == IT_SUPER_NAILGUN)) ||
            (self.weapon == IT_NAILGUN))
	  W_FireSpikes (-4);
	else
	  W_FireLaser();
	self.attack_finished = time + 0.2;
};

//============================================================================

void() player_light1   =[$light1, player_light2  ] 
{
	muzzleflash();

	if (!self.button0 || intermission_running)
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 5)
		self.weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	self.attack_finished = time + 0.4;
};
void() player_light2   =[$light2, player_light1  ]
{
	muzzleflash();

	if (!self.button0 || intermission_running)
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 5)
		self.weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	self.attack_finished = time + 0.4;
};

//============================================================================


void() player_rocket1   =[$rockatt1, player_rocket2  ] {self.weaponframe=1;
	muzzleflash();};
void() player_rocket2   =[$rockatt2, player_rocket3  ] {self.weaponframe=2;};
void() player_rocket3   =[$rockatt3, player_rocket4  ] {self.weaponframe=3;};
void() player_rocket4   =[$rockatt4, player_rocket5  ] {self.weaponframe=4;};
void() player_rocket5   =[$rockatt5, player_rocket6  ] {self.weaponframe=5;};
void() player_rocket6   =[$rockatt6, player_run  ] {self.weaponframe=6;};
void(float num_bubbles) DeathBubbles;

void() PainSound =
{
local float             rs;

	if (self.health < 0)
		return;

	if (damage_attacker.classname == "teledeath")
	{
		sound (self, CHAN_VOICE, "player/teledth1.wav", 1, ATTN_NONE);
		return;
	}

// water pain sounds
	if (self.watertype == CONTENT_WATER && self.waterlevel == 3)
	{
		DeathBubbles(1);
		if (random() > 0.5)
			sound (self, CHAN_VOICE, "player/drown1.wav", 1, ATTN_NORM);
		else
			sound (self, CHAN_VOICE, "player/drown2.wav", 1, ATTN_NORM);
		return;
	}

// slime pain sounds
	if (self.watertype == CONTENT_SLIME)
	{
// FIX ME       put in some steam here
		if (random() > 0.5)
			sound (self, CHAN_VOICE, "player/lburn1.wav", 1, ATTN_NORM);
		else
			sound (self, CHAN_VOICE, "player/lburn2.wav", 1, ATTN_NORM);
		return;
	}

	if (self.watertype == CONTENT_LAVA)
	{
		if (random() > 0.5)
			sound (self, CHAN_VOICE, "player/lburn1.wav", 1, ATTN_NORM);
		else
			sound (self, CHAN_VOICE, "player/lburn2.wav", 1, ATTN_NORM);
		return;
	}

	if (self.pain_finished > time)
	{
		self.axhitme = 0;
		return;
	}
	self.pain_finished = time + 0.5;

// don't make multiple pain sounds right after each other

// ax pain sound
	if (self.axhitme == 1)
	{
		self.axhitme = 0;
		sound (self, CHAN_VOICE, "player/axhit1.wav", 1, ATTN_NORM);
		return;
	}
	

	

	if(self.playerclass == CL_MAGE) //Mages get a Scrag pain sound.
	{
		self.noise = "wizard/wpain.wav";
	}
	else if(self.playerclass == CL_BERSERK) //Berserk gets a manly sound
	{
		self.noise = "hknight/pain1.wav";
	}
	else //Not class-specific
	{
		if(self.player_flags & PF_DEMON)
			self.noise = "demon/dpain1.wav";
		else if(self.player_flags & PF_DOGGIE)
			self.noise = "dog/dpain1.wav";
		else
		{
			rs = rint((random() * 5) + 1);
			
			if(rs == 1)
				self.noise = "player/pain1.wav";
			else if (rs == 2)
				self.noise = "player/pain2.wav";
			else if (rs == 3)
				self.noise = "player/pain3.wav";
			else if (rs == 4)
				self.noise = "player/pain4.wav";
			else if (rs == 5)
				self.noise = "player/pain5.wav";
			else
				self.noise = "player/pain6.wav";
		}
	}

	sound (self, CHAN_VOICE, self.noise, 1, ATTN_NORM);
	return;
};

void()  player_pain1 =  [       $pain1, player_pain2    ] {PainSound();self.weaponframe=0;};
void()  player_pain2 =  [       $pain2, player_pain3    ] {};
void()  player_pain3 =  [       $pain3, player_pain4    ] {};
void()  player_pain4 =  [       $pain4, player_pain5    ] {};
void()  player_pain5 =  [       $pain5, player_pain6    ] {};
void()  player_pain6 =  [       $pain6, player_run      ] {};

void()  player_axpain1 =        [       $axpain1,       player_axpain2  ] {PainSound();self.weaponframe=0;};
void()  player_axpain2 =        [       $axpain2,       player_axpain3  ] {};
void()  player_axpain3 =        [       $axpain3,       player_axpain4  ] {};
void()  player_axpain4 =        [       $axpain4,       player_axpain5  ] {};
void()  player_axpain5 =        [       $axpain5,       player_axpain6  ] {};
void()  player_axpain6 =        [       $axpain6,       player_run      ] {};

// ****** old code

void() player_hw_pain1 = [$deathc10, player_hw_pain2] {PainSound();self.weaponframe=0;};
void() player_hw_pain2 = [$deathc11, player_hw_pain3] {};
void() player_hw_pain3 = [$deathc12, player_hw_pain4] {};
void() player_hw_pain4 = [$deathc13, player_hw_pain5] {};
void() player_hw_pain5 = [$deathc14, player_hw_pain6] {};
void() player_hw_pain6 = [$deathc15, player_run] {};


void(entity attacker, float damage) player_berserk_pain;

void(entity attacker, float damage) player_pain =
{
	if (self.weaponframe)
		return;

	if (self.invisible_finished > time)
		return;         // eyes don't have pain frames

	if (self.playerclass == CL_FIREELEM)
          return;

	if (self.playerclass == CL_HWEAP)
	{ 
	  player_hw_pain1();
	  return;
	}
	
	if(self.playerclass == CL_BERSERK)
	{
		player_berserk_pain(attacker, damage);
		return;
	}


	if (self.weapon == IT_AXE)
		player_axpain1 ();
	else
		player_pain1 ();
};

void() player_diea1;
void() player_dieb1;
void() player_diec1;
void() player_died1;
void() player_diee1;
void() player_die_ax1;

void() DeathBubblesSpawn =
{
local entity    bubble;
	if (self.owner.waterlevel != 3)
		return;
	bubble = spawn();
	setmodel (bubble, "progs/s_bubble.spr");
	setorigin (bubble, self.owner.origin + '0 0 24');
	bubble.movetype = MOVETYPE_NOCLIP;
	bubble.solid = SOLID_NOT;
	bubble.velocity = '0 0 15';
	bubble.nextthink = time + 0.5;
	bubble.think = bubble_bob;
	bubble.classname = "bubble";
	bubble.frame = 0;
	bubble.cnt = 0;
	setsize (bubble, '-8 -8 -8', '8 8 8');
	self.nextthink = time + 0.1;
	self.think = DeathBubblesSpawn;
	self.air_finished = self.air_finished + 1;
	if (self.air_finished >= self.bubble_count)
		remove(self);
};

void(float num_bubbles) DeathBubbles =
{
local entity    bubble_spawner;
	
	bubble_spawner = spawn();
	setorigin (bubble_spawner, self.origin);
	bubble_spawner.movetype = MOVETYPE_NONE;
	bubble_spawner.solid = SOLID_NOT;
	bubble_spawner.nextthink = time + 0.1;
	bubble_spawner.think = DeathBubblesSpawn;
	bubble_spawner.air_finished = 0;
	bubble_spawner.owner = self;
	bubble_spawner.bubble_count = num_bubbles;
	return;
};


void() DeathSound =
{
local float             rs;

	// water death sounds
	if (self.waterlevel == 3)
	{
		DeathBubbles(5);
		sound (self, CHAN_VOICE, "player/h2odeath.wav", 1, ATTN_NONE);
		return;
	}
	
	if(self.playerclass == CL_BERSERK)
	{
		self.noise = "hknight/death1.wav";
	}
	else
	{
		rs = rint ((random() * 4) + 1);
		if (rs == 1)
			self.noise = "player/death1.wav";
		if (rs == 2)
			self.noise = "player/death2.wav";
		if (rs == 3)
			self.noise = "player/death3.wav";
		if (rs == 4)
			self.noise = "player/death4.wav";
		if (rs == 5)
			self.noise = "player/death5.wav";
	}
	
	sound (self, CHAN_VOICE, self.noise, 1, ATTN_NONE);
	return;
};


void() PlayerDead =
{
	self.nextthink = -1;
// allow respawn after a certain time
	self.deadflag = DEAD_DEAD;
};

vector() VelocityForDamage =
{
	local vector v;
/*
	if (vlen(damage_inflictor.velocity)>0)
	{
		v = 0.5 * damage_inflictor.velocity;
		v = v + (25 * normalize(self.origin-damage_inflictor.origin));
		v_z = 100 + 240 * random();
		v_x = v_x + (200 * crandom());
		v_y = v_y + (200 * crandom());
		//dprint ("Velocity gib\n");                
	}
	else
	{

		v_x = 100 * crandom();
		v_y = 100 * crandom();
		v_z = 200 + 100 * random();
	}

	v_x = 100 * crandom();
	v_y = 100 * crandom();
	v_z = 200 + 100 * random();

	if (dm > -50)
	{
	//      dprint ("level 1\n");
		v = v * 0.7;
	}
	else if (dm > -200)
	{
	//      dprint ("level 3\n");
		v = v * 2;
	}
	else
		v = v * 10;
*/
	
	// Velocity calculated by damage ;)
	v = normalize((self.origin + '0 0 32') - self.spawn_origin) * 4 * self.max_health;
	v_x = v_x + 300 * random() - 150;
	v_y = v_y + 300 * random() - 150;
	v_z = v_z + random() * 300;
	return v;
};


void(string gibname) ThrowGib =
{
	local   entity new;

	new = spawn();
	new.origin = self.origin;
	setmodel (new, gibname);
	setsize (new, '0 0 0', '0 0 0');
	new.velocity = VelocityForDamage();
	new.movetype = MOVETYPE_BOUNCE;
	new.solid = SOLID_NOT;
	new.avelocity_x = random()*400;
	new.avelocity_y = random()*400;
	new.avelocity_z = random()*400;
	new.think = SUB_Remove;
	new.ltime = time;
	new.nextthink = time + 10 + random()*10;
	new.frame = 0;
	new.flags = 0;
};

void(string gibname) ThrowHead =
{
	setmodel (self, gibname);
	self.frame = 0;
	self.nextthink = -1;
	self.movetype = MOVETYPE_TOSS;
	self.takedamage = DAMAGE_NO;
	self.solid = SOLID_NOT;
	self.view_ofs = '0 0 8';
	setsize (self, '-16 -16 0', '16 16 56');
	self.velocity = VelocityForDamage ();
//        self.velocity = '0 0 100'; 
	self.origin_z = self.origin_z - 24;
	self.flags = self.flags - (self.flags & FL_ONGROUND);
	self.avelocity = crandom() * '0 600 0';
};


void() GibPlayer =
{
	ThrowHead ("progs/h_player.mdl");
	ThrowGib ("progs/gib1.mdl");
	ThrowGib ("progs/gib2.mdl");
	ThrowGib ("progs/gib3.mdl");

	self.deadflag = DEAD_DEAD;

	if (damage_attacker.classname == "teledeath")
	{
		sound (self, CHAN_VOICE, "player/teledth1.wav", 1, ATTN_NONE);
		return;
	}

	if (damage_attacker.classname == "teledeath2")
	{
		sound (self, CHAN_VOICE, "player/teledth1.wav", 1, ATTN_NONE);
		return;
	}
		
	if (random() < 0.5)
		sound (self, CHAN_VOICE, "player/gib.wav", 1, ATTN_NONE);
	else
		sound (self, CHAN_VOICE, "player/udeath.wav", 1, ATTN_NONE);
};

void() PlayerDie =
{
	local   float   i;
	local	string	s;	

        if (deathmatch == 2)
        {
          self.items = self.items - (self.items & IT_KEY1);
          self.items = self.items - (self.items & IT_KEY2);
        }

	DropBackpack();

	self.items = 0;
	self.player_flags = (self.player_flags & (PF_HAS_CLASS | PF_CHEATER));// | PF_UNLOCKED));
	

	self.attack_finished = time + 3;
	self.invisible_finished = 0;    // don't die as eyes
	self.invincible_finished = 0;
	self.super_damage_finished = 0;
	self.radsuit_finished = 0;
	
	if(self.playerclass != CL_BERSERK)
	{
		self.modelindex = modelindex_player;    // don't use eyes
		self.normalmodel = modelindex_player;
	}
	
	self.weaponmodel="";
	self.view_ofs = '0 0 -8';
	self.deadflag = DEAD_DYING;
	self.solid = SOLID_NOT;
	self.flags = self.flags - (self.flags & FL_ONGROUND);
	
	if(self.playerclass != CL_BERSERK)
	{
		self.movetype = MOVETYPE_TOSS;
		if (self.velocity_z < 10)
			self.velocity_z = self.velocity_z + random()*300;
	}
	
	if (self.health < -40)
	{
		GibPlayer ();
		return;
	}

	DeathSound();
	
	self.angles_x = 0;
	self.angles_z = 0;

	if(self.playerclass == CL_BERSERK)
	{
		player_berserk_die();
		return;
	}
	
	if(self.weapon == IT_AXE)
	{
		player_die_ax1 ();
		return;
	}

	
	i = cvar("temp1");
	if (!i)
		i = 1 + floor(random()*6);
	
	if (i == 1)
		player_diea1();
	else if (i == 2)
		player_dieb1();
	else if (i == 3)
		player_diec1();
	else if (i == 4)
		player_died1();
	else
		player_diee1();

};

void() set_suicide_frame =
{       // used by klill command and diconnect command
	if (self.model != "progs/player.mdl")
		return; // allready gibbed
	self.frame = $deatha11;
	self.solid = SOLID_NOT;
	self.movetype = MOVETYPE_TOSS;
	self.deadflag = DEAD_DEAD;
	self.nextthink = -1;
};


void()  player_diea1    =       [       $deatha1,       player_diea2    ] {};
void()  player_diea2    =       [       $deatha2,       player_diea3    ] {};
void()  player_diea3    =       [       $deatha3,       player_diea4    ] {};
void()  player_diea4    =       [       $deatha4,       player_diea5    ] {};
void()  player_diea5    =       [       $deatha5,       player_diea6    ] {};
void()  player_diea6    =       [       $deatha6,       player_diea7    ] {};
void()  player_diea7    =       [       $deatha7,       player_diea8    ] {};
void()  player_diea8    =       [       $deatha8,       player_diea9    ] {};
void()  player_diea9    =       [       $deatha9,       player_diea10   ] {};
void()  player_diea10   =       [       $deatha10,      player_diea11   ] {};
void()  player_diea11   =       [       $deatha11,      player_diea11 ] {PlayerDead();};

void()  player_dieb1    =       [       $deathb1,       player_dieb2    ] {};
void()  player_dieb2    =       [       $deathb2,       player_dieb3    ] {};
void()  player_dieb3    =       [       $deathb3,       player_dieb4    ] {};
void()  player_dieb4    =       [       $deathb4,       player_dieb5    ] {};
void()  player_dieb5    =       [       $deathb5,       player_dieb6    ] {};
void()  player_dieb6    =       [       $deathb6,       player_dieb7    ] {};
void()  player_dieb7    =       [       $deathb7,       player_dieb8    ] {};
void()  player_dieb8    =       [       $deathb8,       player_dieb9    ] {};
void()  player_dieb9    =       [       $deathb9,       player_dieb9    ] {PlayerDead();};

void()  player_diec1    =       [       $deathc1,       player_diec2    ] {};
void()  player_diec2    =       [       $deathc2,       player_diec3    ] {};
void()  player_diec3    =       [       $deathc3,       player_diec4    ] {};
void()  player_diec4    =       [       $deathc4,       player_diec5    ] {};
void()  player_diec5    =       [       $deathc5,       player_diec6    ] {};
void()  player_diec6    =       [       $deathc6,       player_diec7    ] {};
void()  player_diec7    =       [       $deathc7,       player_diec8    ] {};
void()  player_diec8    =       [       $deathc8,       player_diec9    ] {};
void()  player_diec9    =       [       $deathc9,       player_diec10   ] {};
void()  player_diec10   =       [       $deathc10,      player_diec11   ] {};
void()  player_diec11   =       [       $deathc11,      player_diec12   ] {};
void()  player_diec12   =       [       $deathc12,      player_diec13   ] {};
void()  player_diec13   =       [       $deathc13,      player_diec14   ] {};
void()  player_diec14   =       [       $deathc14,      player_diec15   ] {};
void()  player_diec15   =       [       $deathc15,      player_diec15 ] {PlayerDead();};

void()  player_died1    =       [       $deathd1,       player_died2    ] {};
void()  player_died2    =       [       $deathd2,       player_died3    ] {};
void()  player_died3    =       [       $deathd3,       player_died4    ] {};
void()  player_died4    =       [       $deathd4,       player_died5    ] {};
void()  player_died5    =       [       $deathd5,       player_died6    ] {};
void()  player_died6    =       [       $deathd6,       player_died7    ] {};
void()  player_died7    =       [       $deathd7,       player_died8    ] {};
void()  player_died8    =       [       $deathd8,       player_died9    ] {};
void()  player_died9    =       [       $deathd9,       player_died9    ] {PlayerDead();};

void()  player_diee1    =       [       $deathe1,       player_diee2    ] {};
void()  player_diee2    =       [       $deathe2,       player_diee3    ] {};
void()  player_diee3    =       [       $deathe3,       player_diee4    ] {};
void()  player_diee4    =       [       $deathe4,       player_diee5    ] {};
void()  player_diee5    =       [       $deathe5,       player_diee6    ] {};
void()  player_diee6    =       [       $deathe6,       player_diee7    ] {};
void()  player_diee7    =       [       $deathe7,       player_diee8    ] {};
void()  player_diee8    =       [       $deathe8,       player_diee9    ] {};
void()  player_diee9    =       [       $deathe9,       player_diee9    ] {PlayerDead();};

void()  player_die_ax1  =       [       $axdeth1,       player_die_ax2  ] {};
void()  player_die_ax2  =       [       $axdeth2,       player_die_ax3  ] {};
void()  player_die_ax3  =       [       $axdeth3,       player_die_ax4  ] {};
void()  player_die_ax4  =       [       $axdeth4,       player_die_ax5  ] {};
void()  player_die_ax5  =       [       $axdeth5,       player_die_ax6  ] {};
void()  player_die_ax6  =       [       $axdeth6,       player_die_ax7  ] {};
void()  player_die_ax7  =       [       $axdeth7,       player_die_ax8  ] {};
void()  player_die_ax8  =       [       $axdeth8,       player_die_ax9  ] {};
void()  player_die_ax9  =       [       $axdeth9,       player_die_ax9  ] {PlayerDead();};

void (vector org) spawn_tfog;

void() spell_become_demon =
{
	spawn_tfog(self.origin);
	self.player_flags = self.player_flags | PF_DEMON;

	if(self.health > self.max_health)
		self.health = self.max_health;

	self.max_health = self.max_health * 2;
	self.health = self.health * 2;
	self.max_arm = 0;
	self.armorvalue = 0;
	self.armortype = 0;
	self.view_ofs = '0 0 4';
	stuffcmd(self,"bf\n");
	self.weaponmodel = "";
	cprint(self,"You are now a demon.");
};

void() end_demon =
{
	spawn_tfog(self.origin);
	self.player_flags = self.player_flags - (self.player_flags & PF_DEMON);
	self.player_flags = self.player_flags | PF_NO_MORPH;
	self.max_health = self.max_health / 2;
	self.health = floor(self.health / 3);
  
	//Integer division by 3 can result in 0 values, which screws up the camera
	//and leaves the player in a weird dead-alive state. Don't do that.
	if(self.health == 0)
		self.health = 1;
	
	self.max_arm = CLASS_DRUID_MAXARMOR;
	self.view_ofs = '0 0 22';
	stuffcmd(self,"bf\n");
  
	cprint(self,"You are now human.");
};

void(float side) Player_Demon_Melee =
{
  local vector org;
  sound (self, CHAN_WEAPON, "weapons/ax1.wav", 1, ATTN_NORM);
  makevectors (self.v_angle);
  org = self.origin + '0 0 4';
  traceline (org, org + v_forward*72, FALSE, self);
  if (trace_fraction == 1.0)
    return;

  org = trace_endpos - v_forward * 4;
  if (trace_ent.takedamage)
  {
    sound (self, CHAN_WEAPON, "demon/dhit2.wav", 1, ATTN_NORM);
    T_Damage(trace_ent, self, self, 20+20*random(), SH_UNKNOWN);
  }
};

void()  player_demon_atta1 = [$deatha5, player_demon_atta2 ] {self.attack_finished = time + 1.4;};
void()  player_demon_atta2 = [$deatha6, player_demon_atta3 ] {};
void()  player_demon_atta3 = [$deatha7, player_demon_atta4 ] {self.punchangle_y = -5; };
void()  player_demon_atta4 = [$deatha8, player_demon_atta5 ] {Player_Demon_Melee(160);};
void()  player_demon_atta5 = [$deatha9, player_demon_atta6 ] {Player_Demon_Melee(120);};
void()  player_demon_atta6 = [$deatha10, player_demon_atta7 ] {Player_Demon_Melee(80);};
void()  player_demon_atta7 = [$deatha11, player_demon_atta8 ] {};
void()  player_demon_atta8 = [$deathb1, player_demon_atta9 ] {};              
void()  player_demon_atta9 = [$deathb2, player_demon_atta10 ] {self.punchangle_y = 5; };
void()  player_demon_atta10 = [$deathb3, player_demon_atta11 ] {Player_Demon_Melee(-160); };
void()  player_demon_atta11 = [$deathb4, player_demon_atta12 ] {Player_Demon_Melee(-120); };
void()  player_demon_atta12 = [$deathb5, player_demon_atta13 ] {Player_Demon_Melee(-80); };
void()  player_demon_atta13 = [$deathb6, player_demon_atta14 ] {};
void()  player_demon_atta14 = [$deathb7, player_demon_atta15 ] {};
void()  player_demon_atta15 = [$deathb8, player_stand1 ] {};

void() Player_Demon_JumpTouch =
{
	if (other.takedamage)
	{
		T_Damage(other, self, self, 25+25*random(), SH_UNKNOWN);
		self.touch = SUB_Null;
	}
};

void() player_demon_jump2 =[     $axstnd11,  player_demon_jump2  ]
{
  if (self.walkframe >= 12)
    self.walkframe = 11;
  self.frame = $axstnd11 + self.walkframe;
  self.walkframe = self.walkframe + 1;
  if (self.flags & FL_ONGROUND)
  {
    self.touch = SUB_Null;
    player_stand1();
  }
};

void() player_demon_jump1 =[    $axstnd11, player_demon_jump2 ]
{
  makevectors (self.v_angle);
  self.attack_finished = time + 0.5;
  self.touch = Player_Demon_JumpTouch;
  self.origin_z = self.origin_z + 1;
  self.velocity = v_forward * 500 + '0 0 250';
  sound (self, CHAN_BODY, "demon/djump.wav", 1, ATTN_NORM);
  self.walkframe = 1;
};

/*
  DOGGIE MODE
*/
void() Player_Dog_JumpTouch =
{
  if (other.takedamage)
  {
    T_Damage(other, self, self, 25+10*random(), SH_UNKNOWN);
    self.touch = SUB_Null;
  }
};

void() spell_become_doggie =
{
  spawn_tfog(self.origin);
  self.player_flags = self.player_flags | PF_DOGGIE;
  if (self.health > self.max_health)
    self.health = self.max_health;
  self.max_health = self.max_health * 2;
  self.health = self.health * 2;
  self.max_arm = 0;
  self.armorvalue = 0;
  self.armortype = 0;
  self.view_ofs = '0 0 1';
  stuffcmd(self,"bf\n");
  self.weaponmodel = "";
//  stuffcmd(self,"r_drawviewmodel 0\n");
  cprint(self,"You are now a dog.");
};

void() end_dog =
{
  spawn_tfog(self.origin);
  self.player_flags = self.player_flags - (self.player_flags & PF_DOGGIE);
  self.player_flags = self.player_flags | PF_NO_MORPH;
  self.max_health = self.max_health / 2;
  self.health = self.health / 2;
  self.max_arm = CLASS_MAGE_MAXARMOR;
  self.view_ofs = '0 0 22';
  stuffcmd(self,"bf\n");
//  stuffcmd(self,"r_drawviewmodel 1\n");
  cprint(self,"You are now human.");
};

void() player_dog_jump2 =[     $deathb3,  player_dog_jump2  ]
{
  if (self.walkframe >= 10)
    self.walkframe = 9;
  self.frame = $deathb3 + self.walkframe;
  self.walkframe = self.walkframe + 1;
  if (self.flags & FL_ONGROUND)
  {
    self.touch = SUB_Null;
    player_stand1();
  }
};

void() player_dog_jump1 =[    $deathb3, player_dog_jump2 ]
{
  makevectors (self.v_angle);
  self.touch = Player_Dog_JumpTouch;
  self.origin_z = self.origin_z + 1;
  self.velocity = v_forward * 600 + '0 0 150';
  if (self.velocity_z > 600)
    self.velocity_z = 600;
  sound (self, CHAN_BODY, "dog/dattack1.wav", 1, ATTN_NORM);
  self.walkframe = 1;
};

/* 
  Heavy weapons framing (enforcer)
*/
void() player_hw_att1 = [$pain1, player_hw_att2] {self.weaponframe = 1;};
void() player_hw_att2 = [$pain2, player_hw_att3] {self.weaponframe = 2;};
void() player_hw_att3 = [$pain3, player_hw_att4] {self.weaponframe = 3;};
void() player_hw_att4 = [$pain4, player_hw_att5] {self.weaponframe = 4;};
void() player_hw_att5 = [$pain5, player_run    ] {self.weaponframe = 5;};

void() player_hw_attb1 = [$pain1, player_hw_attb2] {self.weaponframe = 1;
muzzleflash();
};
void() player_hw_attb2 = [$pain2, player_hw_attb3] {self.weaponframe = 2;};
void() player_hw_attb3 = [$pain3, player_run     ] {self.weaponframe = 3;};

void() player_hw_light1   =[$pain3, player_hw_light2  ] 
{
	muzzleflash();

	if (!self.button0)
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 5)
		self.weaponframe = 1;
	SuperDamageSound();
        W_FireLightning();
        self.attack_finished = time + 0.2;
};
void() player_hw_light2   =[$pain2, player_hw_light1  ]
{
	muzzleflash();

	if (!self.button0)
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 5)
		self.weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	self.attack_finished = time + 0.2;
};

void() player_hw_nail1   =[$pain3, player_hw_nail2  ] 
{
	muzzleflash();

        if ((!self.button0) || (self.currentammo < 1))
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;
	SuperDamageSound();
        if (((self.sng_mode == 0) && (self.weapon == IT_SUPER_NAILGUN))
           || (self.weapon == IT_NAILGUN)){
          W_FireSpikes (4);
          self.attack_finished = time + 0.2; }
        else if (self.sng_mode >= 2) // seige cannon
	{
	  if (!(self.flags & FL_ONGROUND))
          { player_run();  return; }
          W_FireCannon();
        }
	else {
          W_FireLaser();
          self.attack_finished = time + 0.1; }
};

void() player_hw_nail2   =[$pain2, player_hw_nail1  ]
{
	muzzleflash();

        if ((!self.button0) || (self.currentammo < 1))
		{player_run ();return;}
	self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;
	SuperDamageSound();
        if (((self.sng_mode == 0) && (self.weapon == IT_SUPER_NAILGUN))
           || (self.weapon == IT_NAILGUN)){
          W_FireSpikes (-4);
          self.attack_finished = time + 0.3; }
        else if (self.sng_mode == 2) // seige cannon
	{
	  if (!(self.flags & FL_ONGROUND))
          { player_run();  return; }
          W_FireCannon();
        }
        else {
          W_FireLaser();
          self.attack_finished = time + 0.1; }
};
