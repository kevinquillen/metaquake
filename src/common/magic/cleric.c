/*************************************************************************
 * Metamorphism Quake Server: Common/Magic/cleric.c,                     *
 *                            Common/Magic/cleric.qc                     *
 * Copyright (C) 1996-2000 by Lon Hohberger.                             *
 * Portions Copyright (C) 2011 by Patrick Baggett                        *
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
 #include "magic.h"

 
 #define AID_AMOUNT	30	//Aid offers at most this much health over maximum
 
// Spell: Sanctuary, Type: Caster or Directed
void() ClericSanct =
{
	if(!CheckMana(self, 90))
		return;
	
	//TODO: Make use of spell effects system for this?
	sound (self, CHAN_AUTO, "enforcer/enfire.wav", 1, ATTN_NORM);
	
	//Already have sanctuary, so remove it now...
	if(self.player_flags & PF_SANCTUARY)
	{
		cprint(self,"Your protection fades.");
		
		//Restore maximum mana and disable sanctuary/bright field effects
		self.max_ammo_cells = self.max_ammo_cells + 90;
		self.player_flags = self.player_flags - PF_SANCTUARY;
		self.effects = self.effects - (self.effects & EF_BRIGHTFIELD);
	}
	else //Casting Sanctuary for the first time
	{
		cprint(self,"You feel protected.");

		//Reduce maximum mana by 90 while in effect
		self.max_ammo_cells = self.max_ammo_cells - 90;
		
		#ifndef QUAKEWORLD
		self.effects = self.effects | EF_BRIGHTFIELD;
		#endif
		self.player_flags = self.player_flags | PF_SANCTUARY;
	}
	
	self.attack_finished = time + 3;
};

// Spell: Harm, Type: Area
void() BecomeExplosion;
void() Harm =
{
  local entity head;
  local float dist;

  if ((self.owner.health <= 0) || (self.owner.id_number != self.id_number))
  {
    remove(self);
    return;
  }

  head = findradius(self.owner.origin, 500);
  while(head)
  {
    if ((head.health >= 1) && (head != self.owner) && (head.takedamage == DAMAGE_AIM))
    {
      if (CanDamage(self.owner,head)) {
        if (!SameTeam(self,head))
        {
          if (head.classname == "player")
            cprint(head,"Your armor disintegrates!");
          head.armorvalue = 0;
          head.armortype = 0;
          DoDamage(head,self,self.owner,random()*5+5, SH_MAGIC);
        }
      }
    }
    head = head.chain;
  }
  self.owner.effects = self.owner.effects - EF_BRIGHTLIGHT;
  WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
  WriteByte (MSG_BROADCAST, TE_EXPLOSION);
  WriteCoord (MSG_BROADCAST, self.owner.origin_x);
  WriteCoord (MSG_BROADCAST, self.owner.origin_y);
  WriteCoord (MSG_BROADCAST, self.owner.origin_z);
  sound (self, CHAN_WEAPON, "weapons/r_exp3.wav", 1, ATTN_NORM);
  remove(self);
  return;
};

void() ClericArmorRupture =
{
  local entity harmobj;
  
  if(!CheckMana(self, 100))
	return;

  sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
  
  self.effects = self.effects | EF_BRIGHTLIGHT;
  self.attack_finished = time + 1.3; // big spell.......
  harmobj=spawn();
  harmobj.origin = self.origin;
  harmobj.owner = self;
  harmobj.nextthink = time + 1.0;
  harmobj.think = Harm;
  harmobj.netname = "spell";
  harmobj.id_number = self.id_number;
};

// Spell: Heal, Type: Caster or Directed
void() ClericHeal =
{
	if(!CheckMana(self, 25))
		return;
	
	sound (self, CHAN_VOICE, "items/r_item1.wav", 1, ATTN_NORM);

	if(self.tb_mode == 0)
	{
		cprint(self,"You feel better!");
		if(self.health < self.max_health)
		{
			self.health = self.health + 25+random()*25;
			if(self.health > self.max_health)
				self.health = self.max_health;
		}
	}
	else
	{
		if(!teamplay)
			return;
		if(!SameTeam(self, self.enemy))
			return;
	
		if(self.enemy.takedamage == DAMAGE_AIM)
		{
			sprint(self,"Healed ");
			if(self.enemy.classname == "player")
			{
				cprint(self.enemy,"You feel better!");
				sprint(self,self.enemy.netname);
			}
			else
				sprint(self,self.enemy.classname);
			sprint(self,"\n");
			
			if(self.enemy.health < self.enemy.max_health)
			{
				self.enemy.health = self.enemy.health + 25+random()*25;
				if(self.enemy.health > self.enemy.max_health)
					self.enemy.health = self.enemy.max_health;
			}
		}
		else
		{
			sprint(self,"Invalid target(");
			sprint(self,self.enemy.classname);
			sprint(self,")\n");
		}
  }

  self.attack_finished = time + 0.5;
};

// Spell: Group Heal, Type: AoE
void() GroupHeal =
{
  local entity head;

  head = findradius(self.origin, 500);
  while(head)
  {
    if ((head.health >= 1) && head.classname == "player")
    {
      if ((teamplay && (SameTeam(self.owner,head))) || (head == self.owner))
      {
        cprint(head,"A warm feeling fills your body.");
        stuffcmd(head,"bf\n");
        if (head.health < head.max_health)
        {
          head.health = head.health + 40 + random()*25; 
          if (head.health > head.max_health)
            head.health = head.max_health;
        }
        head.attack_finished = time;
        head.pain_finished = time;
      }
    }
    head = head.chain;
  }
  self.owner.effects = self.owner.effects - EF_BRIGHTLIGHT;
  remove(self);
};

void() ClericGroupHeal =
{
	local entity healobj;
  
	if(!CheckMana(self, 80))
		return;

	sound (self, CHAN_AUTO, "items/r_item2.wav", 1, ATTN_NORM);

	self.effects = self.effects | EF_BRIGHTLIGHT;
	
	/* TODO: really need a separate entity for this? */
	self.attack_finished = time + 0.6;
	healobj=spawn();
	healobj.origin = self.origin;
	healobj.owner = self;
	healobj.nextthink = time + 0.2;
	healobj.think = GroupHeal;
};

// Spell: Drain, Type: Directed
void() Drain =
{
	//Drain a point of health directly
	self.enemy.health = self.enemy.health - 1;
	
	//If they reach 1 or less, call DoDamage() so the caster gets a
	//kill.
	if(self.enemy.health <= 1)
		DoDamage(self.enemy, self, self.owner, 5, SH_MAGIC); // armor no matter

	//Take the drained health and give it to the other player
	if(self.owner.health < self.owner.max_health)
		self.owner.health = self.owner.health + 1;    

	//If either the caster or the target dies, or the spell duration runs out, then remove
	if(((self.enemy.deadflag) || (self.health <= 1)) || (self.owner.deadflag))
	{
		remove(self);
		return;
	}

	//Next frame in 0.1 secs
	self.health = self.health - 1;
	self.nextthink = time + 0.1;
	self.think = Drain; 
};  

void() ClericDrain =
{
	local entity drainobj;
	
	if(CheckMana(self, 40))
		return;

	sound (self, CHAN_AUTO, "enforcer/enfire.wav", 1, ATTN_NORM);

	if (!(self.enemy.takedamage == DAMAGE_AIM))
	{
		sprint(self,"Invalid target!\n");
		return;
	}

	//Give a resistance check of 10%
	if(CheckResist(self.enemy, 0.10))
	{
		//Resisted the spell
		ResistMsg(self.enemy, self);
		return;
	}
	
	drainobj = spawn();
	drainobj.owner = self;
	drainobj.think = Drain;

	if(self.super_damage_finished > time) //Quad does improve damage
		drainobj.health = 40;
	else
		drainobj.health = 20;
  
	drainobj.nextthink = time + 0.1;
	drainobj.enemy = self.enemy;
#ifndef QUAKEWORLD
	SpawnSurround(self.enemy, 30, 65);
#endif
	//Change caster's target so s/he can't just spam drain spells
	//without at least targeting the other player.
	self.enemy = world;
	self.attack_finished = time + 0.5;
	drainobj.netname = "drain spell";
};

void() ClericDivinity =
{
  local entity divobj;
  
  if(!CheckMana(self, 170))
	return;
	
	sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);
	
	self.effects = self.effects | EF_BRIGHTLIGHT;
	self.attack_finished = time + 1.1;

	cprint(self,"God touches your nose!");
	stuffcmd(self,"bf\n");
	
	//Give a few seconds of invulnerability
	self.pain_finished = time;
	if(self.invincible_finished < time)
	{
		self.items = self.items | IT_INVULNERABILITY;
		self.invincible_time = 1;
		self.invincible_finished = time + 3;
	}

	
};

void() HolyMissileTouch =
{
	local float damg;
	
	//Don't explode on caster
	if(other == self.owner)
		return;
	
	if(pointcontents(self.origin) == CONTENT_SKY)
	{
		remove(self);
		return;
	}

	sound(self,CHAN_VOICE,"weapons/r_exp3.wav",1,ATTN_NORM);
	
	//Deal some base damage to target...
	DoDamage(other, self, self.owner, 25 + random()*20, SH_COLD);

	//Then deal some damage via explosion
	T_RadiusDamage(self, self.owner, 75, self.owner, SH_COLD);
	BecomeExplosion();
};

void() ClericHolyMissile =
{
	local entity holymissile;
	if(!CheckMana(self, 20))
		return;

	sound (self, CHAN_AUTO, "items/inv1.wav", 1, ATTN_NORM);

	holymissile = spawn();
	holymissile.owner = self;
	holymissile.movetype = MOVETYPE_FLYMISSILE;
	holymissile.solid = SOLID_BBOX;
	holymissile.classname = "magic_good";
	holymissile.velocity = aim(self, 1000) * 1000;
	holymissile.angles = vectoangles(holymissile.velocity);
	holymissile.touch = HolyMissileTouch;
	holymissile.nextthink = time + 5;
	holymissile.think = SUB_Remove;
	setmodel (holymissile, "progs/w_spike.mdl");
	setsize(holymissile,'0 0 0','0 0 0');
	setorigin (holymissile, self.origin + v_forward*8 + '0 0 16');
	self.attack_finished = time + 1.0;
	holymissile.netname = "purification";
};

// Spell: Magical Vestment, Type: Caster or Directed
void() ClericMagicalVestment =
{
	local entity targ;
 
	//Need 25 mana to cast
	if(!CheckMana(self, 25))
		return;

	sound(self, CHAN_VOICE, "items/itembk2.wav", 1, ATTN_NORM);

	if (self.tb_mode == 0)
	{
		cprint(self,"Your armor becomes stronger!\n");
		targ = self;
	}
	else
	{
		if (self.enemy.takedamage == DAMAGE_AIM && SameTeam(self, self.enemy)) 
		{
			sprint(self,"Armored ");
			if(self.enemy.classname == "player")
			{
				cprint(self.enemy,"Your armor becomes stronger!\n");
				sprint(self,self.enemy.netname);
			}
			else
				sprint(self,self.enemy.classname);

			sprint(self,"\n");

			targ = self.enemy;
    
		}
		else
			sprint(self,"Invalid target!\n");
	}
   
	if(targ)
	{
		//Clear all armor types
		targ.items = targ.items - (targ.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3));
		
		if(targ.armortype == 0.8) //Add red armor if they have it
		{
			targ.items = targ.items | IT_ARMOR3;
			targ.armorvalue = targ.armorvalue + 10;
		}
		else if(targ.armortype == 0.6) //Change yellow armor to red
		{
			targ.items = targ.items | IT_ARMOR3;
			targ.armortype = 0.8;
		}
		else if(targ.armortype == 0.3) //Change green armor to yellow
		{
			targ.items = targ.items | IT_ARMOR2;
			targ.armortype = 0.6;
		}
		else if(targ.armortype == 0)
		{
			targ.items = targ.items | IT_ARMOR1;
			targ.armortype = 0.3;
			targ.armorvalue = 10;
		}
		
		//Clamp armor values to maximum
		if (targ.armorvalue > targ.max_arm) 
			targ.armorvalue = targ.max_arm;

		//Ensure that if player can't wear armor, then it doesn't show an armor icon
		if(targ.armorvalue == 0)
			targ.items = targ.items - (targ.items & (IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3));
	} //end of if(targ)

	self.attack_finished = time + 0.3;
};

void() ClericCheckTarg =
{
  local string st;
  sprint(self,"-> Target Status: ");
  if ((self.enemy.classname != "player") || (self.enemy == world))
  {
    sprint(self,"Invalid Target!\n");
    return;
  }

  sprint(self,self.enemy.netname);
  sprint(self,"\n-> ");
  st = ftos(self.enemy.health);
  sprint(self,st);
  sprint(self,"/");
  st = ftos(self.enemy.max_health);
  sprint(self,st);
  sprint(self," hp\n-> ");
  st = ftos(self.enemy.armorvalue);
  sprint(self,st);
  sprint(self,"/");
  st = ftos(self.enemy.max_arm);
  sprint(self,st);
  if (self.enemy.armortype == 0.3)
    sprint(self," Green\n");
  else if (self.enemy.armortype == 0.6)
    sprint(self," Yellow\n");
  else if (self.enemy.armortype == 0.8)
    sprint(self," Red\n");
  else
    sprint(self," [None]\n");

};


void() ClericDispelEvil =
{
	local entity e;
	
	if(!CheckMana(self, 35))
		return;

	sound(self, CHAN_AUTO, "enforcer/enfire.wav", 1, ATTN_NORM);

	if(self.enemy.takedamage != DAMAGE_AIM)
	{
		sprint(self,"Casting Failed!\n");
		self.attack_finished = time + 1.0;
		return;
	}

	if(!(self.enemy.player_flags & PF_HAS_SPECIAL))
	{
		sprint(self,self.enemy.netname);
		sprint(self, " has no special item!\n");
		self.attack_finished = time + 1.0;
		return;
	}

	//Base 50% chance to resist
	if(CheckResist(self.enemy, 0.5))
		return;

	//Drop their rune
	self.enemy.player_flags = self.enemy.player_flags - (self.enemy.player_flags & PF_HAS_SPECIAL);
	sound(self.enemy, CHAN_AUTO, "shalrath/pain.wav", 1, ATTN_NORM);
	cprint(self.enemy, "Your rune flies out of your pack!");
	
	//Let caster know it succeeded
	sprint(self, self.enemy.netname);
	sprint(self, "'s rune flies out of their pack!\n");
	#ifndef QUAKEWORLD
	SpawnSurround(self.enemy, 10, 252);
	#endif
	self.enemy = world;
	self.attack_finished = time + 1.0;
};

void() ClericAid =
{
	local entity e;

	if(!CheckMana(self, 90))
		return;
	
	sound(self, CHAN_VOICE, "items/r_item1.wav", 1, ATTN_NORM);

	if(self.tb_mode == CASTMODE_SELF)
	{
		if(self.health < self.max_health + AID_AMOUNT)
		{
			cprint(self,"Your God grants you aid!");
			self.health = self.health + AID_AMOUNT;
			
			//Don't exceed maximum health
			if(self.health > self.max_health + AID_AMOUNT)
				self.health = self.max_health + AID_AMOUNT;
		}
		else //Already have aid
			cprint(self, "You already are strong!");
	}
	else
	{
		if(!teamplay)
			return;
		if(!SameTeam(self, self.enemy))
			return;
	
		if(self.enemy.takedamage == DAMAGE_AIM)
		{
			sprint(self,"Aided: ");
			if(self.enemy.classname == "player")
			{
				cprint(self.enemy,"You feel healthier!");
				sprint(self,self.enemy.netname);
			}
			else
				sprint(self,self.enemy.classname);
			sprint(self,"\n");
			
			//Only give aid when lower than the maximum "Aid" will do.
			if(self.enemy.health < self.enemy.max_health + AID_AMOUNT)
			{
				//Give them health, then ensure that we didn't exceed the limmit
				self.enemy.health = self.enemy.health + AID_AMOUNT;
				if(self.enemy.health > self.enemy.max_health + AID_AMOUNT)
					self.enemy.health = self.enemy.max_health + AID_AMOUNT;
			}
		}
		else
		{
			sprint(self,"Invalid target(");
			sprint(self,self.enemy.classname);
			sprint(self,")\n");
		}
	}

	self.attack_finished = time + 0.5;
};

void() ClericLock =
{
	// Not a spell, really
	if (self.player_flags & PF_LOCKED)
	{
		self.player_flags = self.player_flags - PF_LOCKED;
		cprint(self,"Target lost!\n");
	} else {
		self.player_flags = self.player_flags | PF_LOCKED;
	}
	sprint(self,"Concentrating on ");
	sprint(self,self.enemy.netname);
	sprint(self,"\n");
	self.attack_finished = time + 0.1;
};

// Spell casting: by selection.
void() ClericSpellName =
{
	sprint(self,"Cleric: ");
	if(self.spell == CLERIC_SPELL_SANCT)
		sprint(self,"Sanctuary\n");
	else if(self.spell == CLERIC_SPELL_RUPTURE)
		sprint(self,"Rupture Armor\n");
	else if(self.spell == CLERIC_SPELL_HEAL)
		sprint(self,"Heal\n");
	else if(self.spell == CLERIC_SPELL_GRHEAL)
		sprint(self,"Group Heal\n");
	else if(self.spell == CLERIC_SPELL_DRAIN)
		sprint(self,"Drain\n");
	else if(self.spell == CLERIC_SPELL_DIVINITY)
		sprint(self,"Divinity\n");
	else if(self.spell == CLERIC_SPELL_HOLYMISS)
		sprint(self,"Holy Missile\n");
	else if(self.spell == CLERIC_SPELL_MVESTMENT)
		sprint(self,"Magical Vestment\n");
	else if(self.spell == CLERIC_SPELL_CHECKTARG)
		sprint(self,"Target Status\n");
	else if(self.spell == CLERIC_SPELL_DISPELEVIL)
		sprint(self,"Dispel Evil\n");
	else if(self.spell == CLERIC_SPELL_AID)
		sprint(self,"Aid\n");
	else if(self.spell == CLERIC_SPELL_LOCK)
		sprint(self,"Concentration\n");
};

void() ClericCast =
{
	if(self.spell == CLERIC_SPELL_SANCT)
		ClericSanct();
	else if(self.spell == CLERIC_SPELL_RUPTURE)
		ClericArmorRupture();
	else if(self.spell == CLERIC_SPELL_HEAL)
		ClericHeal();
	else if(self.spell == CLERIC_SPELL_GRHEAL)
		ClericGroupHeal();
	else if(self.spell == CLERIC_SPELL_DRAIN)
		ClericDrain();
	else if(self.spell == CLERIC_SPELL_DIVINITY)
		ClericDivinity();
	else if(self.spell == CLERIC_SPELL_HOLYMISS)
		ClericHolyMissile();
	else if(self.spell == CLERIC_SPELL_MVESTMENT)
		ClericMagicalVestment();
	else if(self.spell == CLERIC_SPELL_CHECKTARG)
		ClericCheckTarg();
	else if(self.spell == CLERIC_SPELL_DISPELEVIL)
		ClericDispelEvil();
	else if(self.spell ==  CLERIC_SPELL_AID)
		ClericAid();
  if (self.spell == CLERIC_SPELL_LOCK)
    ClericLock();
};
