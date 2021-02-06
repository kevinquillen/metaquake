/*
 * Common/world.c, Common/world.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1996-2000 by Lon Hohberger
 */
#include "meta.h"

#ifdef QUAKEWORLD
void() p_army;
#endif

void() CTF_SpawnScoreboard;

void() MetaPrecache =
{
#ifdef QUAKEWORLD
	p_army();
#endif
  // For Weapons
  precache_model("progs/v_spike.mdl");		// hook/shalrath ball
  precache_model("progs/laser.mdl");		// laser
  precache_model("progs/bolt.mdl");		// hyper laser
  precache_model("progs/s_light.spr");		// bfg/shockball?
  
  precache_sound("blob/land1.wav");		// Hook snag
  precache_sound("shalrath/attack2.wav");	// Hook
  precache_sound ("boss2/pop2.wav");		// Raiden devices
  precache_sound ("boss1/out1.wav");		// for meltdown
  precache_sound("enforcer/enfstop.wav");	// laser touch
  precache_sound("enforcer/enfire.wav");	// laser launch
  precache_sound("shambler/sboom.wav");		// Chain Lightning

  // Spell requirements
  precache_model ("progs/w_spike.mdl");
  precache_model ("progs/invulner.mdl");	// protection
  precache_model ("progs/quaddama.mdl");	// quad
  precache_model ("progs/invisibl.mdl");	// RoS
  precache_model ("progs/flame2.mdl");		// hmm
  
  precache_sound ("blob/death1.wav");
  precache_sound ("blob/hit1.wav");
  precache_sound ("doors/ddoor2.wav");

  precache_sound ("items/protect.wav");		// protection/divinity
  precache_sound ("items/protect2.wav");	// sanct/prot/divinity
  precache_sound ("items/protect3.wav");	// protection/divinity
  precache_sound ("items/inv1.wav");		// invis
  precache_sound ("items/inv2.wav");
  precache_sound ("items/inv3.wav");
  precache_sound ("items/damage.wav");		// quad damage
  precache_sound ("items/damage2.wav");
  precache_sound ("items/damage3.wav");
  precache_sound ("hknight/attack1.wav");	// Hweap
  precache_sound ("shalrath/death.wav");
  precache_sound ("shalrath/pain.wav");		// Cleric dispel evil
  precache_sound ("boss1/out1.wav"); 		// Hyper
  precache_sound ("items/r_item1.wav");		// Cleric heal
  precache_sound ("items/r_item2.wav");		// Cleric group heal
  precache_sound ("wizard/wpain.wav");		// Mage class getting hit
  precache_sound ("hknight/pain1.wav");		// Berserk pain
  precache_sound ("hknight/hit.wav");		// Actually, this is used in the fire elemental (magic/fireelem.c)
  precache_sound ("hknight/death1.wav");	// Berkserk death
  precache_sound ("hknight/grunt.wav");		// Berkserk grunt
  precache_sound ("hknight/slash1.wav");	// Berkserk swing 1
  precache_sound ("knight/sword1.wav");		// Berkserk swing 2
  precache_sound ("knight/sword2.wav");		// Berkserk swing 3
  precache_sound ("enforcer/pain1.wav");	// Heavy weapons pain sound 1
  precache_sound ("enforcer/pain2.wav");	// Heavy weapons pain sound 2
  precache_sound ("enforcer/death1.wav");	// Heavy weapons pain sound 2
  
  // for classes/morph
  precache_model ("progs/s_light.spr");		// fire elemental
  precache_model ("progs/demon.mdl");		// demon morph
  precache_model ("progs/dog.mdl");		// dog morph
  precache_model ("progs/enforcer.mdl");	// hweap
  precache_model ("progs/tarbaby.mdl"); // ensnare
  precache_model ("progs/hknight.mdl"); //berserk

  precache_sound ("demon/dhit2.wav");		// demon hit
  precache_sound ("demon/djump.wav");		// demon jump
  precache_sound ("demon/dpain1.wav");		// demon pain
  precache_sound ("dog/dpain1.wav");		// dog pain
  precache_sound ("dog/dattack1.wav");		// dog jump/attack
  precache_sound ("boss1/sight1.wav");    	// hyper boom?
  precache_sound ("boss2/idle.wav");		//Mage confusion spell

  // Special items
  precache_model ("progs/end1.mdl");		// runes
  precache_model ("progs/end2.mdl");
  precache_model ("progs/end3.mdl");
  precache_model ("progs/end4.mdl");
  precache_model ("progs/k_spike.mdl");  // blockades.
 
  // for CTF
	if(deathmatch == DMMODE_CTF)
	{
		precache_model ("progs/w_g_key.mdl");   // gold flag	
		precache_model ("progs/w_s_key.mdl");   // blue flag
		precache_model ("progs/teleport.mdl");

		precache_sound ("misc/basekey.wav");    // snag flag
		precache_sound ("boss1/sight1.wav");    // capture flag
		
		//Spawn the scoreboard entity that prints team scores
		CTF_SpawnScoreboard();
	}

  precache_sound ("misc/runekey.wav"); // scout snagging a pipebomb

	SpawnSpecials();
  
  
#ifdef QUAKEWORLD
  Spawn_NNSM();
#endif

};

//// make a body que entry for the given ent so the ent can be
//// respawned elsewhere
//void(entity ent) CopyToBodyQueue =
//{
//    bodyqueue_head.angles = ent.angles;
//    bodyqueue_head.model = ent.model;
//    bodyqueue_head.modelindex = ent.modelindex;
//    bodyqueue_head.frame = ent.frame;
//    bodyqueue_head.colormap = ent.colormap;
//    bodyqueue_head.movetype = ent.movetype;
//    bodyqueue_head.velocity = ent.velocity;
//    bodyqueue_head.flags = 0;
//    setorigin(bodyqueue_head, ent.origin);
//    setsize(bodyqueue_head, ent.mins, ent.maxs);
//    bodyqueue_head = bodyqueue_head.owner;
//};