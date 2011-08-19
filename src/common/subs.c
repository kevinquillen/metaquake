/*
 * Common/subs.c, Common/subs.qc
 *   Copyright 1996 by id Software
 *   Portions Copyright 1999-2000 by Lon Hohberger
 */


/* Disabled */
void(entity e, string str) temp_unavail =
{
  sprint(e,"*** ");
  sprint(e,str);
  sprint(e," has been temporarily disabled\n");
  e.attack_finished = time + 0.2;
};


/*
=============
visible

returns 1 if the entity is visible to self, even if not infront ()
=============
*/
float (entity targ) visible =
{
	local vector	spot1, spot2;
	
	spot1 = self.origin + self.view_ofs;
	spot2 = targ.origin + targ.view_ofs;
	traceline (spot1, spot2, TRUE, self);	// see through other monsters
	
	if (trace_inopen && trace_inwater)
		return FALSE;			// sight line crossed contents

	if (trace_fraction == 1)
		return TRUE;
	return FALSE;
};


/*
=============
infront

returns 1 if the entity is in front (in sight) of self
=============
*/
float(entity targ) infront =
{
	local vector	vec;
	local float		dot;
	
	makevectors (self.angles);
	vec = normalize (targ.origin - self.origin);
	dot = vec * v_forward;
	
	if ( dot > 0.3)
	{
		return TRUE;
	}
	return FALSE;
};

/*
 Unfortunately, id didn't provide for finding an entire list of something
 in the world w/o a fixed radius, so I created one...
 Pretty simple, however, it is quite odd setting up a linked list with
 arbitrary pointers...
*/
entity(string theclass) FindWorld =
{
  local entity head, poof;
  head = poof = find (world,classname,theclass);
  while (poof)
  {
    poof.goalentity = find(poof,classname,theclass);
    poof = poof.goalentity;
  }
  return head;
};

entity(float radius) ClosestTarget =
{
  local entity head, found;
  found = world;
  head = findradius(self.origin, radius);
  local float dist, dtmp;
  local float comp;
  dist = radius + 50;
  while (head) {
    if (((((head.takedamage == DAMAGE_AIM) && (head.health > 0)) &&
          (head != self)) && (head != self.owner)) && (head.owner != self.owner))
    {
      comp = SameTeam(head,self.owner);
      if ((((!comp) && (teamplay & TP_ON)) || 
          (!(teamplay & TP_ON))) && visible(head))
      {
        dtmp = fabs(vlen(self.origin - head.origin));
        if (dtmp < dist)
        {
          dist = dtmp;
          found = head;
        }

      }
    }
    head = head.chain;
  }
  return found;
};

