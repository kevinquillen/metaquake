
void() ZapFX =
{
  local float c1,c2,c3;
  local vector v,poof;

  if (!(self.enemy.effects & EF_ZAPFX) || (self.id_number != self.enemy.id_number))
  {remove(self); return;}

  v_x = random() * 120 - 60;
  v_y = random() * 120 - 60;
  v_z = random() * 120 - 60;

  self.origin = self.enemy.origin;

  if (self.weapon == 0)
  {
     c1 = random()*250 + 2;
     c2 = random()*250 + 2;
     c3 = random()*250 + 2;
  } else 
     c1 = c2 = c3 = self.weapon;

  poof_x = v_x;
  poof_y = v_y;
  poof_z = v_z;
  particle(self.origin + poof, 0.5 * self.health * poof, c1, 20);
  poof_x = v_y;
  poof_y = v_z;
  poof_z = v_x;
  particle(self.origin + poof, 0.5 * self.health * poof, c1, 20);
  poof_x = v_z;
  poof_y = v_x;
  poof_z = v_y;
  particle(self.origin + poof, 0.5 * self.health * poof, c1, 20);

  self.nextthink = time + 0.1;
  self.think = ZapFX;
};

void(entity targ, float sp, float color) SpawnZapFX =
{
  local entity s;
  s = spawn();
  s.enemy = targ;
  s.id_number = targ.id_number;
  targ.effects = targ.effects | EF_ZAPFX;
  s.think = ZapFX;
  s.health = sp;
  s.velocity = '0 0 0';
  s.angles = '0 0 0';
  s.weapon = color;
  s.solid = SOLID_NOT;
  setsize(s,'0 0 0','0 0 0');
  s.movetype = MOVETYPE_NOCLIP;
  s.nextthink = time;
};

void() Surround =
{
  local vector spot;
  if (self.max_health != 0)
    self.weapon = self.max_health;
  else
    self.weapon = random()*255;

  makevectors(self.angles);
  self.origin = self.enemy.origin;
  particle(self.origin + v_forward * (4 * self.health) + v_up * 20, '0 0 0',
       self.weapon, 10);
  particle(self.origin + v_forward * (4 * self.health) + v_up * -20, '0 0 0',
       self.weapon, 10);
  particle(self.origin + v_forward * (4 * self.health), '0 0 0',
       self.weapon, 10);
  particle(self.origin + v_forward * (-4 * self.health) + v_up * 20, '0 0 0',
       self.weapon, 10);
  particle(self.origin + v_forward * (-4 * self.health) + v_up * -20, '0 0 0',
       self.weapon, 10);
  particle(self.origin + v_forward * (-4 * self.health), '0 0 0', 
       self.weapon, 10);

  self.health = self.health - 1;
  if (self.health <= 0)
  {
    remove(self);
    return;
  }

  self.nextthink = time + 0.1;
  self.think = Surround;
};

void(entity targ, float t, float c) SpawnSurround =
{
  local entity s;
  s = spawn();
  s.enemy = targ;
  s.think = Surround;
  s.health = t;
  s.max_health = c;
  s.velocity = '0 0 0';
  s.angles = '0 0 0';
  s.avelocity = '0 250 0';
  s.movetype = MOVETYPE_NOCLIP;
  s.nextthink = time;
};

void() BurnFX =
{
  if ((self.enemy.deadflag) || (self.enemy == world))
  {
    remove(self);
    return;
  }
  makevectors(self.enemy.v_angle);
  self.origin = self.enemy.origin + v_forward * -8 + v_up * 16;
  self.health = self.health + 1;
  if (self.health > 4)
    self.health = 2;
  self.walkframe = self.health;
  self.frame = self.health;
  self.nextthink = time + 0.1;
  self.think = BurnFX;
};  

void(entity targ) SpawnFire =
{
  local entity f;
  f = spawn();
  f.movetype = MOVETYPE_NOCLIP;
  f.enemy = targ;
  setmodel(f,"progs/s_explod.spr");
  f.think = BurnFX;
  f.nextthink = time;
};

void() WhiteThink =
{
  if ((self.enemy.deadflag) || (self.enemy == world))
  {
    remove(self);
    return;
  }
  self.origin = self.enemy.origin;
  WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
  WriteByte (MSG_BROADCAST, TE_TELEPORT);
  WriteCoord (MSG_BROADCAST, self.origin_x);
  WriteCoord (MSG_BROADCAST, self.origin_y);
  WriteCoord (MSG_BROADCAST, self.origin_z);
  self.nextthink = time + 0.5;
  self.think = WhiteThink;  
};

void(entity targ) SpawnWhite =
{
  local entity w;
  w = spawn();
  w.enemy = targ;
  w.think = WhiteThink;
  w.nextthink = time + 0.1;
};

void()	s_explode1	=	[0,		s_explode2] {};
void()	s_explode2	=	[1,		s_explode3] {};
void()	s_explode3	=	[2,		s_explode4] {};
void()	s_explode4	=	[3,		s_explode5] {};
void()	s_explode5	=	[4,		s_explode6] {};
void()	s_explode6	=	[5,		SUB_Remove] {};

void() BecomeExplosion =
{
	self.movetype = MOVETYPE_NONE;
	self.velocity = '0 0 0';
	self.touch = SUB_Null;
	setmodel (self, "progs/s_explod.spr");
	self.solid = SOLID_NOT;
	s_explode1 ();
};
