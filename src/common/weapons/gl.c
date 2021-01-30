/*
 * Common/Weapons/gl.c, Common/Weapons/gl.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */

 #include "meta.h"

float() crandom;
void() player_rocket1;
void() player_hw_att1;
void(float radius, float d_p, entity ignore) T_PlasmaDamage;

void(entity bomb, entity attacker, float rad, entity ignore, float damagetype) T_RadiusDamage;


void() GrenadeExplode =
{
	// use variable damage...
	T_RadiusDamage (self, self.owner, 90+random()*30, world, SH_EXPLOSION);

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

void() GrenadeTouch =
{

	if ((other == self.owner) && (self.ltime < time - 0.2))
		return;         // don't explode on owner
	

        if (self.voided)
          return;
    
        self.voided = 1;

	if ((other.takedamage == DAMAGE_AIM) && (self.classname != "pipebomb"))
	{
		GrenadeExplode();
		return;
	}
	sound (self, CHAN_WEAPON, "weapons/bounce.wav", 1, ATTN_NORM);  // bounce sound
	if (self.velocity == '0 0 0')
		self.avelocity = '0 0 0';
};


void() MortarThink =
{
  self.angles = vectoangles(self.velocity);
  self.nextthink = time + 0.1;
  self.think = MortarThink;
};

// Mortar =================================================================

void() W_FireMortar =
{
  local entity missile;
  if (!self.flags & FL_ONGROUND)
    return;
  player_rocket1();
  self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
  sound (self, CHAN_WEAPON, "weapons/grenade.wav", 1, ATTN_NORM);
  missile = spawn();
  missile.owner = self;
  missile.movetype = MOVETYPE_BOUNCE;
  missile.solid = SOLID_BBOX;
  missile.netname = "mortar";
  missile.classname = "mortar";
  makevectors (self.v_angle);
  if (self.v_angle_x)
  {
    missile.velocity = v_forward*200*self.spell + v_up * 100 + crandom()*v_right*10 + crandom()*v_up*10;
    if (missile.velocity_z < 50)
      missile.velocity_z = 50;
    missile.velocity_z = missile.velocity_z + 50*self.spell;
  }
  else
    {
       missile.velocity = aim(self, 10000);
       missile.velocity = missile.velocity * 700;
       missile.velocity_z = 350;
    }

  missile.angles = vectoangles(missile.velocity);
  missile.think = MortarThink;
  missile.nextthink = time + 0.1;

  missile.touch = GrenadeExplode;
#ifdef QUAKEWORLD
  missile.effects = EF_RED;
#else
  missile.effects = EF_DIMLIGHT;
#endif
  setmodel (missile, "progs/missile.mdl");
  setsize (missile, '0 0 0', '0 0 0');     
  setorigin (missile, self.origin);
  self.attack_finished = time + 0.4;
  self.pausetime = time + 1;
};

// Pipe bombs =============================================================
void() W_BlowPipeBombs =
// Kill all pipebombs within range...
{
  local entity head;
  local string num;

  head = findradius (self.origin, 5000); // Search
  while(head) 
  {
    if((head.classname == "pipebomb") && (head.owner == self))
    {
      head.think = GrenadeExplode;
      head.nextthink = time + 0.1;
    }
    head = head.chain; // Check next object
  }
};

void() MineTouch =
{
  local float f;
  f = SameTeam(other,self.owner);
  if ((f) && (other == self.owner))
    f = 0;
  if ((other == world) || f)
    return;

  if ((other.playerclass == CL_SCOUT) || (other.playerclass == CL_SILENT) || (other.playerclass == CL_CLERIC))
    return;
  self.think = GrenadeExplode;
  self.nextthink = time;                // blow up now...
};

void() MineArm =
{
  if (pointcontents(self.origin)==CONTENT_WATER)
  {
    remove(self);
    return;
  }
  self.netname = "land mine";
  cprint(self.owner,"Mine armed!");
  self.solid = SOLID_TRIGGER;
  self.movetype = MOVETYPE_FLYMISSILE;
  self.origin_z = self.origin_z + 2;
  self.velocity = '0 0 0';
  self.avelocity = '0 400 0';
 
  setsize(self,'-30 -30 -30','30 30 30');
  self.touch = MineTouch;
};

/*
================
W_FireGrenade
================
*/
void() W_FireGrenade =
{
	local float maxmines;
	if ((self.gl_mode == 1) && (!CheckCount(5000,"pipebomb",4)))
        {
          cprint(self,"4 pipebombs already out!");
	  return;
        }
		
	if(self.playerclass == CL_SOLDIER || self.playerclass == CL_SPECIAL)
		maxmines = 7;
	else
		maxmines = 5;
		
	if ((self.gl_mode == 2) && (!CheckCount(5000,"landmine",maxmines)))
        {
          cprint(self,"Too many mines already out!");
	  return;
        }

	if (self.playerclass != CL_HWEAP)
          player_rocket1();
	else
	  player_hw_att1();
	self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
	
	sound (self, CHAN_WEAPON, "weapons/grenade.wav", 1, ATTN_NORM);

#ifdef QUAKEWORLD
	msg_entity = self;
	WriteByte (MSG_ONE, SVC_SMALLKICK);
#else
        self.punchangle_x = -2;
#endif

	newmis = spawn ();
	newmis.voided=0;
	newmis.owner = self;
	newmis.movetype = MOVETYPE_BOUNCE;
	newmis.solid = SOLID_BBOX;
	newmis.ltime = time;

        if (self.gl_mode == 0) {
//          self.netname = "pineapple";
          newmis.netname = "pineapple";
          self.attack_finished = time + 0.6;
	  newmis.classname = "grenade";
	} else if (self.gl_mode == 1) {
          self.attack_finished = time + 1.0;
          newmis.netname = "pipe bomb";
          newmis.classname = "pipebomb";
        } else {
          self.attack_finished = time + 1.3;
          newmis.classname = "landmine";
        }
         	
// set newmis speed     

	makevectors (self.v_angle);

	if (self.v_angle_x)
		newmis.velocity = v_forward*600 + v_up * 200 + crandom()*v_right*10 + crandom()*v_up*10;
	else
	{
		newmis.velocity = aim(self, 10000);
		newmis.velocity = newmis.velocity * 600;
		newmis.velocity_z = 200;
	}

	newmis.avelocity = '300 300 300';

	newmis.angles = vectoangles(newmis.velocity);
	
        if (self.gl_mode != 2)
	  newmis.touch = GrenadeTouch;
	else
          newmis.touch = MineArm;

// set newmis duration
	if (self.gl_mode == 0)
	  newmis.nextthink = time + 2.5;
        else
          newmis.nextthink = time + 180;  // 3 min and pipebomb go boom

	newmis.think = GrenadeExplode;

	if (self.gl_mode != 2) {
          setmodel (newmis, "progs/grenade.mdl");
        } else {
          setmodel (newmis, "progs/v_spike.mdl");
        }
	setsize (newmis, '0 0 0', '0 0 0');             
	setorigin (newmis, self.origin);
};

// Vortex Cannon ===========================================================
// "follow itself"
void() TurnThink =
{
  local vector v;
  local float sc;
  if (((self.enemy == world) || (!self.enemy)) || (self.enemy.velocity == '0 0 0'))
  { 
    self.think = SUB_Null;
    return;
  }
  makevectors(self.enemy.angles);
  v = self.enemy.origin + (self.health)*v_right*(self.player_flags);
  sc = vlen(v - self.origin)*(self.player_flags);
  self.velocity = normalize(v - self.origin) * sc;
  self.angles = vectoangles(self.velocity);
  self.think = TurnThink;
  self.nextthink = time + 0.1;	// minor change
};
  
void() StopNow =
{
  self.velocity = '0 0 0';
  self.nextthink = time;
  self.think = SUB_Remove;
};  

void() W_FireVortex =
{
  local entity s, m1, m2;
  if (self.ammo_rockets < 3)
  {
    self.gl_mode = 0;
    sprint(self,"GL: Standard (Auto-selected)\n");
    return;
  }

  player_rocket1();
  self.currentammo = self.ammo_rockets = self.ammo_rockets - 3;

  s = spawn();
  s.movetype = MOVETYPE_FLYMISSILE;

  sound (self, CHAN_WEAPON, "weapons/spike2.wav", 1, ATTN_NORM);
  s.owner = self;
  s.classname = "ambient";
  s.origin = self.origin + '0 0 16';
  makevectors(self.v_angle);
  s.angles = vectoangles(v_forward);
  setsize(s,'0 0 0','0 0 0');  
  s.solid = SOLID_BBOX;
  s.avelocity = '0 0 400';
  s.velocity = v_forward * 650;
  s.touch = StopNow;

  m1 = spawn();
  m2 = spawn();
  m1.player_flags = m2.player_flags = 14;
  setmodel(m1,"progs/missile.mdl");
  setmodel(m2,"progs/missile.mdl");
  m1.movetype = m2.movetype = MOVETYPE_FLYMISSILE;
  m1.solid = m2.solid = SOLID_BBOX;
  m1.enemy = m2.enemy = s;
  m1.owner = self;
  m2.owner = self;
  m1.netname = "North Wind";
  m2.netname = "South Wind";
  
  setsize(m1,'0 0 0','0 0 0');
  setsize(m2,'0 0 0','0 0 0');
  m1.origin = self.origin + '0 0 16' + v_right * 6;
  m2.origin = self.origin + '0 0 16' + v_right * -6;

  m1.velocity = v_forward * 400;
  m2.velocity = v_forward * 400;
  m1.angles = vectoangles(m1.velocity);
  m2.angles = vectoangles(m2.velocity);

  m1.think = TurnThink;
  m2.think = TurnThink;
  m1.nextthink = m2.nextthink = time + 0.1;
  m1.health = 1;
  m2.health = -1;

  m1.ltime = m2.ltime = time;
  m1.touch = GrenadeExplode;
  m2.touch = GrenadeExplode;
  self.attack_finished = time + 1.7;
};



void() W_FireGL =
{
  if (self.gl_mode <= 2)
    W_FireGrenade();
  if (self.gl_mode == 3)
    W_FireMortar();
  if (self.gl_mode == 4)
    if (self.ammo_rockets < 3)
    {
      self.gl_mode = 3;
      W_FireMortar();
    } else
      W_FireVortex();
    /* Timebombs are gone. No more cheese.
  if (self.gl_mode == 5)
    if (self.ammo_rockets < 45)
    {
      self.gl_mode = 3;
      W_FireMortar();
    } else {
      self.player_flags = self.player_flags | PF_ARMED;
      cprint(self,"Armed, Impulse xxx to Set\n");
    }
    */
};


