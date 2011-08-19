/*
 * Common/soldier.c, Common/soldier.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 2000 by Lon Hohberger
 */


// Meta 5 -- crude-ass lame AI ;)
#ifdef QUAKEWORLD
#define AXIS_X 1 		/* Strafe */
#define AXIS_Y 0		/* Forward/back */

void() army_respawn;
void()armythink;

void() ai_drop;
void() ai_face =
{
	local vector v;
	if (!(visible(self.enemy)))
		return;
	v = normalize(self.enemy.origin - self.origin);
	self.angles = vectoangles(v);
	self.v_angle = self.angles;
	self.v_angle_x = -1 * self.v_angle_x;
};

void() ai_stand =
{
	local entity head;
	local float dist,dtmp;
	dist = 10000;
	head = findradius(self.origin + '0 0 16', 2000);
	while(head)
	{
		if (head != self)
		{
			if ((  (visible(head) && (head.health > 0)) && (head.classname == "player")))
			{
				dtmp = vlen(self.origin - head.origin);
				if (dtmp < dist)
				{
					dist = dtmp;
					self.enemy = head;
				}
			}
		}
		head = head.chain;
	}
	ai_drop();
};
/*
float sign(float f) =
{
	if (f<0)
		return 1;
	else
		return -1;
};*/

float(vector v_from, vector v_to) ai_checkmove =
{
	if (pointcontents(v_to) == CONTENT_SOLID)
		return FALSE;
	traceline(v_from,v_to,FALSE,self);
	if (trace_inwater && trace_inopen)
		return FALSE;
	if (v_to != trace_endpos)
		return FALSE;

	return TRUE;
};

void(float vel) ai_strafe =
{
	local float sv;
	local vector v1, v2;
	if (vel < 0)
		sv = -1;
	else
		sv = 1;
	if ((self.enemy.classname == "player") && (visible(self.enemy)))
	{
		ai_face();
	}

	makevectors(self.angles);
	v2 = v_right * 5 * vel;
	v1 = self.origin + v2;
	if (!ai_checkmove(self.origin + (sv * v_right * 23),v1))
		return;
	self.origin = v1;
	ai_drop();
};

float(float vel) ai_dodge =
{
	// avoid line of sight while attacking -- crude dodging.
	local vector org,dir;
	org = self.enemy.origin + '0 0 16';
	dir = aim(self.enemy, 10000);
	traceline(org,org + dir * 700,FALSE,self.enemy);
	if(trace_ent == self)
	{
		ai_strafe(vel);
		return TRUE;
	}
	return FALSE;
};

void(float vel) ai_walk =
{
	local float sv;
	local vector v1, v2;
	if (vel < 0)
		sv = -1;
	else
		sv = 1;
	if ((self.enemy.classname == "player") && (visible(self.enemy)))
	{
		ai_face();
		traceline(self.origin,self.enemy.origin,FALSE,self);
		v1 = normalize(trace_endpos - self.origin);
		v1_z = 0;
		v2 = v1 * vel * 5;
		v1 = self.origin + v2;
		makevectors(self.angles);

		// since our size is 32x32 (16x 16y from origin)
		// Trig says our greatest out is sqrt(512) -- we don't
		// need to be perfect. 23^2 = 529. Close enough.
		
		if (!ai_checkmove(self.origin + (sv * v_forward * 23),v1))
			return;
		self.origin = v1;
	} else {
		makevectors(self.angles);
		v2 = v_forward * 5 * vel;
		v1 = self.origin + v2;
		if (!ai_checkmove(self.origin + (sv * v_forward * 23),v1))
			return;
		self.origin = v1;
	}
	ai_drop();
};


void() ai_drop =
{
	local float f;
	local vector v2;

	v2 = self.origin + '0 0 -100';
	traceline(self.origin,v2,FALSE,self);
	f = trace_endpos_z;
	traceline(self.origin + '24 0 2', v2,FALSE,self);
	if (trace_endpos_z > f)
		f = trace_endpos_z;
	traceline(self.origin + '-24 0 2', v2,FALSE,self);
	if (trace_endpos_z > f)
		f = trace_endpos_z;
	traceline(self.origin + '0 24 2', v2,FALSE,self);
	if (trace_endpos_z > f)
		f = trace_endpos_z;
	traceline(self.origin + '0 -24 2', v2, FALSE, self);
	if (trace_endpos_z > f)
		f = trace_endpos_z;
	self.origin_z = f + 26;
	if (pointcontents(self.origin) == CONTENT_SOLID)
	{
		bprint("# Beetle # ");
                bprint(self.netname);
		bprint(" fell out of level\n");
		army_respawn();
	}
	if (pointcontents(self.origin) == CONTENT_LAVA)
	{
		DoDamage(self,world,world,5000,SH_LAVA);
		self.frags = self.frags - 1;
		army_respawn();
	}
		
};

void(float vel) ai_back =
{
	ai_walk(-1*vel);
};

// CRUDE god damnit, CRUDE
void(float vel) ai_run =
// run and dodge enemy's line of sight.
{
	local float f;
	f = 0.6*vel;
	if (ai_dodge(f))
		ai_walk(f);
	else 
		ai_walk(vel);
};

void(float t) ai_pain =
{
};

void(float t) ai_painforward =
{
	ai_walk(t);
};

void() ai_rocket =
{
	local float frac;
	local vector v,endp,org;

	endp = self.enemy.origin + '0 0 -12';
	org = self.origin + '0 0 16';

	frac = vlen(endp - org);
	frac = frac / 1000;

	if (self.attack_finished < time)
	{
		v = normalize((endp + (frac * self.enemy.velocity)) - org);
		self.angles = vectoangles(v);
		
		// Check for too close
		traceline(org,v,FALSE,self);
		if (vlen(org - trace_endpos) < 80)
			return;
		self.v_angle = self.angles;
		self.v_angle_x = -1 * self.v_angle_x;
		self.ammo_rockets = 1;
		W_FireRocket();
	}
};


/*
==============================================================================

SOLDIER / PLAYER

==============================================================================
*/

$cd id1/models/soldier3
$origin 0 -6 24
$base base		
$skin skin

$frame stand1 stand2 stand3 stand4 stand5 stand6 stand7 stand8

$frame death1 death2 death3 death4 death5 death6 death7 death8
$frame death9 death10

$frame deathc1 deathc2 deathc3 deathc4 deathc5 deathc6 deathc7 deathc8
$frame deathc9 deathc10 deathc11

$frame load1 load2 load3 load4 load5 load6 load7 load8 load9 load10 load11

$frame pain1 pain2 pain3 pain4 pain5 pain6

$frame painb1 painb2 painb3 painb4 painb5 painb6 painb7 painb8 painb9 painb10
$frame painb11 painb12 painb13 painb14

$frame painc1 painc2 painc3 painc4 painc5 painc6 painc7 painc8 painc9 painc10
$frame painc11 painc12 painc13

$frame run1 run2 run3 run4 run5 run6 run7 run8

$frame shoot1 shoot2 shoot3 shoot4 shoot5 shoot6 shoot7 shoot8 shoot9

$frame prowl_1 prowl_2 prowl_3 prowl_4 prowl_5 prowl_6 prowl_7 prowl_8
$frame prowl_9 prowl_10 prowl_11 prowl_12 prowl_13 prowl_14 prowl_15 prowl_16
$frame prowl_17 prowl_18 prowl_19 prowl_20 prowl_21 prowl_22 prowl_23 prowl_24


/*
==============================================================================
SOLDIER CODE
==============================================================================
*/
/*
void() kill_cfb =
{
	local entity e;
	e = find (world,classname,"chromatic fireball");
	while (e) 
	{
		if (visible(e))
		{
			self.enemy = e;
			ai_face();
			DruidCall();
			self.attack_finished = time + 0.7;
			return;
		}
		e = find(e,classname,"chromatic fireball");
	}
};
*/

void() army_fire;

void()	army_stand1	=[	$stand1,	armythink	] {ai_stand();};/*i
void()	army_stand2	=[	$stand2,	army_stand3	] {};
void()	army_stand3	=[	$stand3,	army_stand4	] {};
void()	army_stand4	=[	$stand4,	army_stand5	] {};
void()	army_stand5	=[	$stand5,	army_stand6	] {ai_stand();};
void()	army_stand6	=[	$stand6,	army_stand7	] {};
void()	army_stand7	=[	$stand7,	army_stand8	] {};
void()	army_stand8	=[	$stand8,	armythink	] {};
*/

void()	army_walk1	=[	$prowl_1,	army_walk2	] {
if (random() < 0.2)
	sound (self, CHAN_VOICE, "soldier/idle.wav", 1, ATTN_IDLE);
ai_walk(7);};
void()	army_walk2	=[	$prowl_2,	army_walk3	] {};
void()	army_walk3	=[	$prowl_3,	army_walk4	] {ai_walk(7);};
void()	army_walk4	=[	$prowl_4,	army_walk5	] {};
void()	army_walk5	=[	$prowl_5,	army_walk6	] {ai_walk(7);};
void()	army_walk6	=[	$prowl_6,	army_walk7	] {};
void()	army_walk7	=[	$prowl_7,	army_walk8	] {ai_walk(7);};
void()	army_walk8	=[	$prowl_8,	army_walk9	] {};
void()	army_walk9	=[	$prowl_9,	army_walk10	] {ai_walk(7);};
void()	army_walk10	=[	$prowl_10,	army_walk11	] {};
void()	army_walk11	=[	$prowl_11,	army_walk12	] {ai_walk(7);};
void()	army_walk12	=[	$prowl_12,	army_walk13	] {};
void()	army_walk13	=[	$prowl_13,	army_walk14	] {ai_walk(7);};
void()	army_walk14	=[	$prowl_14,	army_walk15	] {};
void()	army_walk15	=[	$prowl_15,	army_walk16	] {ai_walk(7);};
void()	army_walk16	=[	$prowl_16,	army_walk17	] {};
void()	army_walk17	=[	$prowl_17,	army_walk18	] {ai_walk(7);};
void()	army_walk18	=[	$prowl_18,	army_walk19	] {};
void()	army_walk19	=[	$prowl_19,	army_walk20	] {ai_walk(7);};
void()	army_walk20	=[	$prowl_20,	army_walk21	] {};
void()	army_walk21	=[	$prowl_21,	army_walk22	] {ai_walk(7);};
void()	army_walk22	=[	$prowl_22,	army_walk23	] {};
void()	army_walk23	=[	$prowl_23,	army_walk24	] {ai_walk(7);};
void()	army_walk24	=[	$prowl_24,	armythink	] {};

void()	army_run1	=[	$run1,		army_run2	] {
if (random() < 0.2)
	sound (self, CHAN_VOICE, "soldier/idle.wav", 1, ATTN_IDLE);
ai_run(5);};
void()	army_run2	=[	$run2,		army_run3	] {ai_run(7);};
void()	army_run3	=[	$run3,		army_run4	] {ai_run(7);};
void()	army_run4	=[	$run4,		army_run5	] {ai_run(7);};
void()	army_run5	=[	$run5,		army_run6	] {ai_run(7);};
void()	army_run6	=[	$run6,		army_run7	] {ai_run(7);};
void()	army_run7	=[	$run7,		army_run8	] {ai_run(7);};
void()	army_run8	=[	$run8,		armythink	] {ai_run(7);};

void()	army_atk1	=[	$shoot1,	army_atk2	] {ai_run(7);};
void()	army_atk2	=[	$shoot2,	army_atk3	] {ai_strafe(-5);};
void()	army_atk3	=[	$shoot3,	army_atk4	] {ai_strafe(-5); ai_dodge(5);};
void()	army_atk4	=[	$shoot4,	army_atk5	] {ai_dodge(5);};

void()	army_atk5	=[	$shoot5,	army_atk6	] {
	local float f;
	local vector v;

	ai_strafe(5); ai_dodge(-5);
	ai_face();
	// Cast spells.
	local float f;
	local vector v;
	self.ammo_cells = 100;
	
	
	f = vlen(self.enemy.origin - self.origin);

	if ((self.enemy.items & (IT_QUAD | IT_INVISIBILITY)) && (self.attack_finished < time)) {
		DruidDMagic();
	}

	if (self.attack_finished < time)
	{
		if ((f>=300) && (f<=500))
		{
			// Lead Target with psistorm?
			v = normalize((self.enemy.origin + (0.5 * self.enemy.velocity)) - self.origin);	
			self.angles = vectoangles(v);
			self.v_angle = self.angles;
			self.v_angle_x = -1 * self.v_angle_x;
  			DruidPsionicStorm();
		}
		else
		{
			ai_face();
			if (self.enemy.health <= 50)
				army_fire();
			else 
  				DruidCall();
		}
	}

};


void()	army_atk6	=[	$shoot6,	army_atk7	] {ai_strafe(5); ai_walk(-2); ai_face();
};
void()	army_atk7	=[	$shoot7,	army_atk8	] {
	ai_strafe(5);
};
void()	army_atk8	=[	$shoot8,	army_atk9	] {ai_face();
};
void()	army_atk9	=[	$shoot9,	armythink	] {ai_face();
	self.ammo_cells = 35;
if(self.attack_finished < time)
{
	DruidCall();
}


};


void()	army_pain1	=[	$pain1,		army_pain2	] {ai_dodge(-5);};
void()	army_pain2	=[	$pain2,		army_pain3	] {ai_dodge(-5);
self.ammo_cells = 5;
DruidWind();
ai_dodge(5);
};
void()	army_pain3	=[	$pain3,		army_pain4	] {ai_dodge(5);};
void()	army_pain4	=[	$pain4,		army_pain5	] {ai_run(7);};
void()	army_pain5	=[	$pain5,		army_pain6	] {
self.ammo_cells = 5;
DruidWind();
};
void()	army_pain6	=[	$pain6,		armythink	] {};

void()	army_painb1	=[	$painb1,	army_painb2	] {self.ammo_cells = 5;
DruidWind();
};
void()	army_painb2	=[	$painb2,	army_painb3	] {ai_painforward(10);
ai_face();
self.ammo_cells = 35;
if (self.attack_finished < time)
	DruidCall();

};
void()	army_painb3	=[	$painb3,	army_painb4	] {ai_painforward(9);};
void()	army_painb4	=[	$painb4,	army_painb5	] {ai_dodge(-5);
};
void()	army_painb5	=[	$painb5,	army_painb6	] {ai_dodge(-5);};
void()	army_painb6	=[	$painb6,	army_painb7	] {ai_dodge(5);};
void()	army_painb7	=[	$painb7,	army_painb8	] {ai_dodge(5);};
void()	army_painb8	=[	$painb8,	army_painb9	] {
	ai_run(-7);
};
void()	army_painb9	=[	$painb9,	army_painb10] {ai_run(-7);};
void()	army_painb10=[	$painb10,	army_painb11] {
};
void()	army_painb11=[	$painb11,	army_painb12] {};
void()	army_painb12=[	$painb12,	army_painb13] {ai_pain(2);
self.ammo_cells = 5;
DruidWind();
};
void()	army_painb13=[	$painb13,	army_painb14] {
self.ammo_rockets = 1;
ai_rocket();
};
void()	army_painb14=[	$painb14,	armythink	] {
};

void()	army_painc1	=[	$painc1,	army_painc2	] {self.ammo_rockets = 1; ai_rocket(); };
void()	army_painc2	=[	$painc2,	army_painc3	] {};
void()	army_painc3	=[	$painc3,	army_painc4	] {};
void()	army_painc4	=[	$painc4,	army_painc5	] {};
void()	army_painc5	=[	$painc5,	army_painc6	] {ai_painforward(1);};
void()	army_painc6	=[	$painc6,	army_painc7	] {ai_painforward(1);};
void()	army_painc7	=[	$painc7,	army_painc8	] {
self.ammo_cells = 5;
DruidWind();};
void()	army_painc8	=[	$painc8,	army_painc9	] {ai_pain(1);};
void()	army_painc9	=[	$painc9,	army_painc10] {ai_painforward(4);};
void()	army_painc10=[	$painc10,	army_painc11] {ai_painforward(3);};
void()	army_painc11=[	$painc11,	army_painc12] {ai_painforward(6);
self.ammo_cells = 5;
DruidWind();};
void()	army_painc12=[	$painc12,	army_painc13] {ai_painforward(8);
self.ammo_cells = 5;
DruidWind();};
void()	army_painc13=[	$painc13,	armythink] {
self.ammo_cells = 35;
ai_face();
if (self.attack_finished < time)
	DruidCall();		// Fuck'em up.
};

void(entity attacker, float damage)	army_pain =
{
	local float r;
	
	if (self.pain_finished > time)
		return;

	r = random();

	if (r < 0.2)
	{
		self.pain_finished = time + 0.6;
		army_pain1 ();
		sound (self, CHAN_VOICE, "soldier/pain1.wav", 1, ATTN_NORM);
	}
	else if (r < 0.6)
	{
		self.pain_finished = time + 1.4;
		army_painb1 ();
		sound (self, CHAN_VOICE, "soldier/pain2.wav", 1, ATTN_NORM);
	}
	else
	{
		self.pain_finished = time + 1.3;
		army_painc1 ();
		sound (self, CHAN_VOICE, "soldier/pain2.wav", 1, ATTN_NORM);
	}
};


void() army_fire =
{
	local	vector	dir;
	
	ai_face();
	
	sound (self, CHAN_WEAPON, "soldier/sattck1.wav", 1, ATTN_NORM);	

	dir = normalize (self.enemy.origin - self.origin);
	
	FireBullets (5, dir, '0.1 0.1 0', 8, SH_BULLETS);
};


void()	army_die1	=[	$death1,	army_die2	] {self.solid = SOLID_NOT;};
void()	army_die2	=[	$death2,	army_die3	] {};
void()	army_die3	=[	$death3,	army_die4	]
{};
void()	army_die4	=[	$death4,	army_die5	] {};
void()	army_die5	=[	$death5,	army_die6	] {};
void()	army_die6	=[	$death6,	army_die7	] {};
void()	army_die7	=[	$death7,	army_die8	] {};
void()	army_die8	=[	$death8,	army_die9	] {};
void()	army_die9	=[	$death9,	army_die10	] {};
void()	army_die10	=[	$death10,	army_die10	] {
self.think = army_respawn;
self.nextthink = time + 1;
};

void()	army_cdie1	=[	$deathc1,	army_cdie2	] {self.solid = SOLID_NOT; };
void()	army_cdie2	=[	$deathc2,	army_cdie3	] {ai_back(5);};
void()	army_cdie3	=[	$deathc3,	army_cdie4	]
{ai_back(4);};
void()	army_cdie4	=[	$deathc4,	army_cdie5	] {ai_back(13);};
void()	army_cdie5	=[	$deathc5,	army_cdie6	] {ai_back(3);};
void()	army_cdie6	=[	$deathc6,	army_cdie7	] {ai_back(4);};
void()	army_cdie7	=[	$deathc7,	army_cdie8	] {};
void()	army_cdie8	=[	$deathc8,	army_cdie9	] {};
void()	army_cdie9	=[	$deathc9,	army_cdie10	] {};
void()	army_cdie10	=[	$deathc10,	army_cdie11	] {};
void()	army_cdie11	=[	$deathc11,	army_cdie11	] {
self.think = army_respawn;
self.nextthink = time + 1;

};

void() army_free =
// prevent bot from sticking to players? :)
{
	if (other.classname == "player")
		ai_back(5);
};

void() army_die =
{
// check for gib
	self.nextthink = time + 10;

// regular death
	sound (self, CHAN_VOICE, "soldier/death1.wav", 1, ATTN_NORM);
	if (random() < 0.5)
		army_die1 ();
	else
		army_cdie1 ();
};


/*QUAKED monster_army (1 0 0) (-16 -16 -24) (16 16 40) Ambush
*/
void() army_respawn;
void() p_army =
{	
	precache_model ("progs/soldier.mdl");
	precache_model ("progs/h_guard.mdl");
	precache_model ("progs/gib1.mdl");
	precache_model ("progs/gib2.mdl");
	precache_model ("progs/gib3.mdl");

	precache_sound ("soldier/death1.wav");
	precache_sound ("soldier/idle.wav");
	precache_sound ("soldier/pain1.wav");
	precache_sound ("soldier/pain2.wav");
	precache_sound ("soldier/sattck1.wav");
	precache_sound ("soldier/sight1.wav");

	precache_sound ("player/udeath.wav");		// gib death
	
	precache_sound ("misc/talk.wav");

};

void() armythink =
{
	self.think = armythink;
	if (self.enemy.health <= 0)
		self.enemy = world;
	if (self.enemy.classname!="player")
	{
		self.health = self.health + 1;
		if (self.health > self.max_health)
			self.health = self.max_health;
		army_stand1();
		return;
	} else {
		ai_stand();
		if ((random() * 9) < 3)
		{
			army_run1();
			return;
		} else {
			army_atk1();
			return;
		}
	}
	self.nextthink = time + 0.5;

};

void() create_army =
{
	local entity e;
        e = find(world,classname,"Énhumaî…ÃÒ‘");
        while(e) { return; }
        bprint("# Alpha # Summoned Énhumaî…ÃÒ‘\n");
	e = spawn();
        e.netname = "Énhumaî…ÃÒ‘";
        e.classname = "Énhumaî…ÃÒ‘";
	e.think = army_respawn;
	e.nextthink = time + 1;
	e.frags = 0;
	sound(self,CHAN_VOICE,"misc/talk.wav",1,ATTN_NONE);
	bprint(e.netname);
        bprint("º ");
	bprint(self.netname);
	bprint(" is a big meanie.\n");

};

void() army_respawn =
{
	local entity spot,new;
	local string str;
	new = spawn();
	new.solid = SOLID_BBOX;
	new.movetype = MOVETYPE_FLYMISSILE;
	new.takedamage = DAMAGE_AIM;

	setmodel (new, "progs/soldier.mdl");
	sound(self,CHAN_VOICE,"misc/talk.wav",1,ATTN_NONE);

	setsize (new, '-16 -16 -24', '16 16 24');
	new.frags = self.frags;
	new.health = 150;
	new.max_health = 150;
	new.max_arm = 100;	// for devious clerics
	new.weapon = IT_SHOTGUN;
	new.saves = SH_ELECTRICITY | SH_BULLETS | SH_MAGIC;
	new.harms = SH_SNIPER;
	new.flags = FL_MONSTER;
	new.option_flags = OF_NO_MISSILEAIM;

	new.th_stand = army_stand1;
	new.th_walk = army_walk1;
	new.th_run = army_run1;
	new.th_missile = army_atk1;
	new.th_pain = army_pain;
	new.th_die = army_die;
	new.touch = army_free;

	spot = SelectSpawnPoint();
	new.origin = spot.origin + '0 0 25';
	new.angles = spot.angles;
        new.classname = "Énhumaî…ÃÒ‘";
	spawn_tfog(new.origin);
	spawn_tdeath(new.origin,new);
	new.netname = self.netname;
	bprint(new.netname);
	str = ftos(new.frags);
        bprint("º ");
	bprint(str);
        bprint(" Æòáç¨ó©\n");

	new.think = armythink;
	new.nextthink = time + 0.5;
	remove(self);
};

#endif //QUAKEWORLD
