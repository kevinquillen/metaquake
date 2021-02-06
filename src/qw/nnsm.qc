/*
  No Non-Standard Models for quakeworld
  v0.4
*/

void() NNSM_Remind =
{
  local string ipa;
  if ((((!self.enemy) || (self.enemy == world)) || (self.enemy.classname !="player")) || (!(self.enemy.player_flags & 8192)))
  {
    remove(self);
    return;
  }

  ipa = infokey(self.enemy,"ip");
  if (ipa == self.netname)
  {
    centerprint(self.enemy,"It appears you have a non-standard\nplayer or eyes model. To play with\nno restrictions, please remedy the\nsituation and reconnect.          \n");
  } else {
    remove(self);
    return;
  }

  self.nextthink = time + 2;
  self.think = NNSM_Remind;
};

void() NNSM_Check =
{
  local string ipc, ipk;
  local entity e,a;

  ipk = infokey(world,"cheater");
  if (ipk) {
    e = find(world,classname,"player");
    while (e)
    {
      ipc = infokey(e,"ip");
      if (ipc==ipk)
      {
        localcmd("serverinfo cheater \"\"\n");
        a = spawn();
        e.player_flags = e.player_flags | 8192;
        a.netname = ipc;
        a.enemy = e;
        a.think = NNSM_Remind;
        a.nextthink = time + 2;
        a.health = 0;

        self.nextthink = time + 0.2;
        self.think = NNSM_Check;
        return;
      }
      e = find(e, classname,"player");
   }
 }

  self.nextthink = time + 0.2;
  self.think = NNSM_Check;
};

void() Spawn_NNSM =
{
  local entity e;
  e = spawn();
  e.think = NNSM_Check;
  e.nextthink = time + 0.2;
};
