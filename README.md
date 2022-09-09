# Metamorphism Quake

Repository to preserve a great Quake mod, Metamorphism. This mod is akin to a fast paced Team Fortress style Quake mod with deathmatch and capture the flag game mode support on QuakeWorld or p2p.

## Installation

Download the latest release from the releases page. Unpack to your Quake game root directory, where you see the Id1 folder. It will look like this:

```
Quake
  - Id1
  - meta
```

Run the game with the flag `-game meta` to load the game files. You can do this in Steam by changing the game properties, or by appending it on your server start command.

### In game help

To enter the in game help menu, hit Backspace. Navigate the menu options with the 1, 2, and 3 keys.

To select a class, you can also type its alias into the console:

- soldier
- sniper
- special
- scout
- hweap
- mage
- cleric
- druid
- shock
- fireelem
- ninja
- lich
- asniper
- berserk

### Current Classes

Pick from any of the following classes in Meta with their own abilities:

- Soldier
- Sniper
- Demolition
- Scout
- Heavy Weapons
- Mage
- Cleric
- Druid
- Shock Trooper
- Fire Elemental
- Ninja
- Lich
- Assault Sniper
- Berserker

#### Spells

Note that some classes use magic only based attacks and no weapons. Spells can be cast in a few 'modes' you can cycle through:

- Directed
- Area
- Self

You may want to bind a key to `nextspell` or `prevspell` so you can cycle through them quickly.

### Full Mouselook

In the game, open the Quake console with the tilde ~ key and type `+mlook` and hit enter. This will unlock mouselook so you can look up and down.

### Add crosshair

To show a crosshair on your HUD, open the Quake console and enter:

`crosshair 1`

### Entering the game menu

Upon joining a game, use the Backspace key to open the Metaquake game menu. Navigate through to view simple tips and explanations, and pick your class.

### Grappling Hook

This mod comes bundled with the grappling hook. You can use it at your discretion by selecting the Axe weapon until you see "Axe: Hook" notification in the top left, or you can open the Quake console and alias it:

#### ORIGINAL QUAKE 1

This is the bind alias for the original release of Quake:

`bind mouse2 +hook`

#### QUAKE 1 REMASTER (2021)

The Quake 1 remaster from NightDive Studios needs to bind like this.

```
bind mouse_right "-hook; bind mouse_right +hook"
alias +hook "impulse 90"
alias -hook "impulse 91"
```

The above would bind the grappling hook action to your right mouse button. Upon releasing the mouse right button, the hook will release.

You can still fire your weapon as you grapple through the air.

### Capture the Flag mode

To set capture the flag mode, either set launch options of deathmatch 2 or enter deathmatch 2 in the console and reload the map.

### MVDSV Server Example

With nQuake installed, you can run meta on a server like so:

1. Add the 'meta' game dir to the nquakesv directory
2. Inside the meta directory, upload qwprogs.dat file from the latest release.
3. Add a server.cfg, pwd.cfg, and mvdsv.cfg file.

server.cfg:

```
exec mvdsv.cfg
exec pwd.cfg

map e2m2
set deathmatch 2
```

pwd.cfg

```
rcon_password YOURPASSWORD
```

mvdsv.cfg

```
sv_hashpasswords      0         // hash account passwords (0 = off, 1 = on)
sv_crypt_rcon         0         // rcon encryption (0 = off, 1 = on)
sv_timestamplen       60        // time (seconds) during rcon command which encryption is valid
sv_mapcheck           0
```

4. With that in place, start your Quake server (from the nquakesv root):

```
./mvdsv -port 28501 +gamedir meta
```

This would start a mvdsv server on port 28501, and autoload the server.cfg file. The map would be set to e2m2, with the game mode set as CTF (set it to 1 for deathmatch).

Using rcon, you can send commands to the server after entering your password in the Quake console in-game. Check the link in this document for a list of all commands.

Example:

```
rcon_password "YOURPASSWORD"
rcon map e1m1
rcon deathmatch 2
```

If you are hosting the server, make sure you are accepting UDP traffic on ports 26000-29000. Additionally, if you are hosting it from your home computer, you need to enable port forwarding on your router.

# Building from source

Check out this repository to your local machine. The root Makefile has a few commands to build the project.

To build, use `make progs.dat` for Quake or `make qwprogs.dat` for QuakeWorld. The default directory for the output will be ./build/meta. After building, copy the "meta" directory in ./build to the installation path for Quake as noted above.

## Testing

This build was tested under the following:

- Built/compiled on Ubuntu 20 & qccx
- Quake (Steam on Linux)
- Amazon EC2 t2.medium with mvdsv (nQuake) loading and serving the game with QuakeWorld
- GLQuake and GLQuakeWorld is *UNTESTED*

Additional game engines (QuakeSpasm, FTE, ezQuake) are completely untested. This is meant specifically for vanilla Quake/QuakeWorld, and was tested so far only on mvdsv server.

I recommend grabbing an IDE like [CLion](https://www.jetbrains.com/clion/) if you plan on grokking/editing the codebase.

## Known Issues

- The game client will currently crash on player respawn in QuakeWorld if the resolution is set above 1024x. Do not use anything higher.
- You must supply the game PAK0/PAK1 files in the Id1 directory for the server to run it.
- You must supply the [base.pcx skin](http://ftp.gwdg.de/pub/misc/ftp.idsoftware.com/idstuff/quakeworld/skins/skinbase.zip) file under 'meta/skins' on the server so players can connect.
- rcon must be unencrypted in mvdsv.cfg otherwise the server will not recognize the password at all from the client.
- sv_mapcheck must be set to 0 or basic game maps may fail to load for players (this may be an nQuake/mvdsv quirk)
- The .qc files in /qw do not understand certain constants defined in meta.h and had to be replaced with their actual value

## Helpful Resources

- [FTEQCC Manual](https://icculus.org/~marco/quakec/fteqcc_manual.txt)
- [QuakeWorld Server Console Commands](http://www.joz3d.net/html/qwsconsole.html)

# Disclaimer

I am not a master C programmer. This repository is mainly for preservation purposes and a few tweaks or bugfixes that I am able to find and rectify. I really just wanted to save and share this fantastic Quake 1 mod that I dumped many hours into in the 90s. 

Hopefully it will be spotted and cared for and carried forward by folks in the GitHub/gaming community. I hear Quake 1 mods are still going strong in 2021! That is incredible, so please fork or provide updates and keep this repo informed of any servers hosting Meta!

Thanks Lon & Patrick!
