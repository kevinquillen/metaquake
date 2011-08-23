/*
 * Common/Weapons/rl.c, Common/Weapons/rl.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

#include "meta.h"
void() player_rocket1;
/*
==============================================================================
ROCKETS
==============================================================================
*/
void()muzzleflash;

void() GoForward =
{
  local vector v;
  v = self.angles;
  v_x = -1 * v_x;
  makevectors(v);
  self.velocity = v_forward * 1000;
  self.nextthink = time + 4;
  self.think = SUB_Remove;
};

void() T_MissileTouch =
{
	local float     damg;

	if ((other == self.owner) && (self.owner != world))
		return;         // don't explode on owner

	if (self.voided) {
		return;
	}
	self.voided = 1;

	if (pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}

	damg = 85 + random()*25;
	
	if ((other.health) && (self.health != 1)) // shalrath ball
	{
		other.deathtype = "rocket";
		DoDamage (other, self, self.owner, damg, SH_EXPLOSION);
	}

	// don't do radius damage to the other, because all the damage
	// was done in the impact


	// use variable radius damage -- so it doesn't do more from
	// groundpound than normal
        if (self.health != 1)
 	  T_RadiusDamage (self, self.owner, damg, other, SH_EXPLOSION);
        else
	  T_RadiusDamage (self, self.owner, 50, world, SH_EXPLOSION); // shalrath ball

//  sound (self, CHAN_WEAPON, "weapons/r_exp3.wav", 1, ATTN_NORM);
	self.origin = self.origin - 8 * normalize(self.velocity);

	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte (MSG_MULTICAST, TE_EXPLOSION);
	WriteCoord (MSG_MULTICAST, self.origin_x);
	WriteCoord (MSG_MULTICAST, self.origin_y);
	WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
  	  multicast (self.origin, MULTICAST_PHS);
#endif

	BecomeExplosion();
};

void() ShalHome =
{
  if ((self.enemy == self.owner) || (self.enemy.takedamage != DAMAGE_AIM))
  {
    self.enemy = self.owner;
    self.owner = world;
  }
  
  if (self.ltime < time - 3.5)
  {
     self.think = SUB_Null;
     self.movetype = MOVETYPE_BOUNCE;
     self.velocity = self.velocity + '0 0 100';
     return;
  }
  if (self.enemy == world)
  {
    self.think = SUB_Null; 
    return;
  }
  if (self.enemy.takedamage != DAMAGE_AIM)
    return;

  if (self.enemy.health < 1)
  {
    self.enemy = world;
    return;
  }
  self.velocity = normalize((self.enemy.origin + '0 0 10') - self.origin) * 325;

  self.nextthink = time + 0.1;
  self.think = ShalHome;  
};

/*
================
W_FireRocket
================
*/
void() W_FireRocket =
{
	if (self.classname == "player")
	{
		if(self.playerclass != CL_HWEAP)
			player_rocket1();
		else
			player_hw_att1();
	}
	self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
	
	sound (self, CHAN_WEAPON, "weapons/sgun1.wav", 1, ATTN_NORM);

#ifdef QUAKEWORLD
	if (self.classname == "player")
	{
		msg_entity = self;
		WriteByte (MSG_ONE, SVC_SMALLKICK);
	}
#else
        self.punchangle_x = -2;
#endif

	newmis = spawn ();
	newmis.owner = self;
	newmis.movetype = MOVETYPE_FLYMISSILE;
	newmis.solid = SOLID_BBOX;
		
// set newmis speed     

	makevectors (self.v_angle);
        
        newmis.netname = "rocket";
	if (self.option_flags & OF_NO_MISSILEAIM)
        {
	  newmis.velocity = v_forward * 1000;
        } else {
          newmis.velocity = aim(self, 1000);
	  newmis.velocity = newmis.velocity * 1000;
	}
	newmis.angles = vectoangles(newmis.velocity);

	if (self.rl_mode == 2)
	{
          newmis.netname = "missile";
 	  newmis.velocity = v_forward * 100;
          newmis.velocity_x = newmis.velocity_x + (random()*220 - 110);
          newmis.velocity_y = newmis.velocity_y + (random()*220 - 110);
          newmis.velocity_z = newmis.velocity_z + (random()*80 - 40);
	}

	
	newmis.touch = T_MissileTouch;
	newmis.voided = 0;
	
// set newmis duration
	newmis.health = self.rl_mode;
	newmis.nextthink = time + 5;
	newmis.think = SUB_Remove;
	newmis.classname = "rocket";

	self.attack_finished = time + 1;
	if (self.rl_mode == 2)
	{
	  self.attack_finished = time + 0.6;
          newmis.think = GoForward;
	  newmis.nextthink = time + 0.3 + random() * 0.5;
	}

	if (self.rl_mode == 1) {
          newmis.saves = SH_MAGIC;
          newmis.netname = "Shalrath Ball";
          newmis.velocity = v_forward * 320;
          self.attack_finished = time + 0.8;
          setmodel (newmis, "progs/v_spike.mdl");
          newmis.think = ShalHome;
          newmis.nextthink = time + 0.6;
          newmis.ltime = time;
          newmis.enemy = self.enemy;
        } else
	  setmodel (newmis, "progs/missile.mdl");
	setsize (newmis, '0 0 0', '0 0 0');             
	setorigin (newmis, self.origin + v_forward*8 + '0 0 16');
	if (self.rl_mode == 1)
          newmis.avelocity = '300 300 300';
	newmis.effects = 0;

};


/*
void() SineThink;
void() FireSnake;

void() TagReCenter =
{
  local entity r;
  local entity temp;

  if (((self.health == 3) && (self.ltime < time)) || (self.owner.health <= 0))
  {
    remove(self);
    return;
  }

  self.origin = self.enemy.origin;

  if ((self.ltime < time) && (self.health != 3))
  {
    r = spawn();
    sound (self.owner,CHAN_WEAPON, "weapons/shotgn2.wav", 1, ATTN_NORM); 

    temp = self;
    self = self.owner;
    muzzleflash();
    player_rocket1();
    makevectors (self.v_angle);
    self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
    self = temp;

    FireSnake();

    self.ltime = time + 0.3;
    self.health = self.health + 1;
    if (self.health == 3)
      self.ltime = time;
  } 

  self.owner.attack_finished = time + 2;
  self.nextthink = time + 0.01;
  self.think = TagReCenter;
};

void() T_TagTouch =
{
  if (other == self.owner)
    return;

  if (other.takedamage != DAMAGE_AIM)
  {
    self.owner.attack_finished = time + 0.2;
    remove(self);
    return;
  }
  self.id_number = other.id_number;
  self.enemy = other;
  self.think = TagReCenter;
  self.nextthink = time + 0.01; 
  
};

void() W_FireTag =
{
  local entity tag;
  tag = spawn();
  tag.owner = self;

#ifdef QUAKEWORLD
  sound (self, CHAN_WEAPON, "weapons/ax1.wav", 1, ATTN_NORM);
  setmodel(tag,"progs/bottle.mdl");
#else
  sound (self, CHAN_WEAPON, "weapons/spike2.wav", 1, ATTN_NORM);
  setmodel(tag,"progs/w_spike.mdl");
#endif

  tag.solid = SOLID_TRIGGER;
  tag.movetype = MOVETYPE_FLYMISSILE;
  setsize(tag,'0 0 0','0 0 0');
  tag.origin = self.origin + '0 0 16';
  makevectors (self.v_angle);
  tag.velocity = v_forward * 1000;
  tag.angles = vectoangles(tag.velocity);
#ifdef QUAKEWORLD
  tag.avelocity = '200 0 0';
#endif

  tag.touch = T_TagTouch;
  self.attack_finished = time + 2;
  tag.health = 0;
};

void() SineThink =
{
  if (self.ltime < (time - 5))
  {
    remove(self);
    return;
  }

  if (self.health <= -120)
    self.playerclass = 1;
  if (self.health >= 120)
    self.playerclass = -1;

  self.health = self.health + 40*self.playerclass;

  if (self.max_health <= -120)
    self.weapon = 1;
  if (self.max_health >= 120)
    self.weapon = -1;

  makevectors(self.v_angle);
  self.max_health = self.max_health + 30*self.weapon;

  if (self.enemy != world)
  {
    self.spawn_origin = normalize(self.enemy.origin - self.origin);
  }
      
  self.velocity = self.spawn_origin * 350 + v_right * self.health + v_up * self.max_health;
  self.angles = vectoangles(self.velocity);
  self.nextthink = time + 0.1;
  self.think = SineThink;
};

void() FireSnake =
{
  local entity m;
  m = spawn();
  m.enemy = self.enemy;
  m.owner = self;
  makevectors(self.owner.v_angle);
  m.origin = self.owner.origin + v_forward * 16 + '0 0 16';
  m.solid = SOLID_BBOX;
  m.movetype = MOVETYPE_FLYMISSILE;
  setmodel(m,"progs/missile.mdl");
  setsize(m,'0 0 0','0 0 0');
  m.v_angle = self.v_angle;
  m.spawn_origin = v_forward;
  m.velocity = v_forward;
  m.netname = "drunk missile";
  m.angles = vectoangles(m.velocity);
  m.touch = T_MissileTouch;
  m.think = SineThink;
  m.nextthink = time + 0.3;
  m.ltime = time;
};

*/
/*
void() FlakExplode =
{
	if ((other == self.owner) && (self.owner != world))
		return;         // don't explode on owner

	if (self.voided) {
		return;
	}
	self.voided = 1;

	if (pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}

 	T_RadiusDamage (self, self.owner, 100, other, SH_EXPLOSION);

	self.origin = self.origin - 8 * normalize(self.velocity);

	WriteByte (MSG_MULTICAST, SVC_TEMPENTITY);
	WriteByte (MSG_MULTICAST, TE_EXPLOSION);
	WriteCoord (MSG_MULTICAST, self.origin_x);
	WriteCoord (MSG_MULTICAST, self.origin_y);
	WriteCoord (MSG_MULTICAST, self.origin_z);
#ifdef QUAKEWORLD
  	multicast (self.origin, MULTICAST_PHS);
#endif

	remove(self);
};

void() FlakStop =
{ 
  self.touch = SUB_Null;
  self.velocity = '0 0 0';
};

void() W_FireFlak =
{
  local entity rpg;
  if (self.ammo_rockets < 1)
  { return; }
  self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
  rpg = spawn();
  rpg.owner = self;
  rpg.netname = "flak burst";

  setmodel(rpg,"progs/missile.mdl");
  rpg.solid = SOLID_BBOX;
  rpg.movetype = MOVETYPE_FLYMISSILE;
  setsize(rpg,'0 0 0','0 0 0');
  rpg.origin = self.origin + '0 0 16';
  makevectors (self.v_angle);
  rpg.spawn_origin = v_forward;
  rpg.velocity = v_forward * 750;
  rpg.angles = vectoangles(rpg.velocity);
  rpg.touch = FlakStop;
  self.attack_finished = time + 1.0;
  rpg.effects = EF_RED;
  rpg.think = FlakExplode;
  rpg.nextthink = time + 0.6;
  sound (self, CHAN_WEAPON, "weapons/sgun1.wav", 1, ATTN_NORM);
  
};
*/
/*
//===========
void() GoBoom;
void() W_BlowIt =
// Kill all pipebombs within range...
{
  local entity head;

  head = find (world,classname,"tagrocket"); // Search
  while(head)
  {
    if((head.classname == "tagrocket") && (head.owner == self))
    {
      head.think = GoBoom;
      head.nextthink = time + 0.1;
    }
    head = find(head,classname,"tagrocket"); // Check next object
  }
};




void() GoBoom =
{
  makevectors(self.owner.v_angle);
  self.velocity = v_forward * 1000;
  if (self.velocity_z < 0) self.velocity_z = 0;
  self.angles = vectoangles(self.velocity);
  self.touch = GrenadeExplode;
  self.netname = "bottle rocket";
  self.solid = SOLID_BBOX;
  self.nextthink = time + 2;
  self.think = SUB_Remove;
};


void() T_Touch = 
{
  local entity y;
  if (other == self.owner)
    return;
  if (self.voided)
    return;
  self.voided = 1;
  if(other != world)
    return;

  y = spawn();
  y.movetype = MOVETYPE_FLYMISSILE;
  setmodel(y,"progs/missile.mdl");
  setorigin(y,self.origin + '0 0 10');
  y.owner = self.owner;
  y.angles = '90 0 0';
  y.avelocity = '0 200 0';
  setsize(y,'0 0 0','0 0 0');
  y.solid = SOLID_NOT;
  y.nextthink = time + 20;
  y.netname = "tagrocket";
  y.classname = "tagrocket";
  y.think = SUB_Remove;
  remove(self);
};


void() W_FireIt =
{
  local entity r;

  if (!CheckCount(5000,"tagrocket",4))
  {
    cprint(self,"4 bottle rockets already out!");
    return;
  }


  if (self.ammo_rockets < 1)
  { return; }
  self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
  r = spawn();
  r.owner = self;
  setmodel(r,"progs/missile.mdl");
  r.solid = SOLID_BBOX;
  r.movetype = MOVETYPE_BOUNCE;
  r.angles = '90 0 0';
  r.origin = self.origin + '0 0 16';
  r.velocity = aim(self,1000) * 600 + '0 0 200';
  setsize(r,'0 0 0','0 0 0');
  self.attack_finished = time + 0.4;
  r.touch = T_Touch;
  r.think = SUB_Remove;
  r.nextthink = time + 3;
  r.health = 1;
  r.netname = "tagrocket";
  r.classname = "tagrocket";
};
*/


void() W_FireRL =
{
  if (self.playerclass == CL_SPECIAL)
  {
    return;
  }
  if (self.rl_mode <= 2)
    W_FireRocket();
};
