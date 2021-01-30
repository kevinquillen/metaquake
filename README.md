# Metamorphism Quake

Repo to preserve a great Quake mod, Metamorphism. This mod is akin to a fast paced Team Fortress style Quake mod with deathmatch and capture the flag game mode support on QuakeWorld or p2p.

## Installation

Download the latest release from the releases page. Unpack to your Quake game root directory, where you see the Id1 folder. It will look like this:

```
Quake
  - Id1
  - meta
```

Run the game with the flag `-game meta` to load the game files. You can do this in Steam by changing the game properties.

### Full Mouselook

In the game, open the Quake console with the tilde ~ key and type `+mlook` and hit enter. This will unlock mouselook so you can look up and down.

### Entering the game menu

Upon joining a game, use the Backspace key to open the Metaquake game menu. Navigate through to view simple tips and explanations, and pick your class.

### Hookshot

This mod comes bundled with the grappling hook. You can use it at your discretion by selecting the Axe weapon until you see "Axe: Hook" notification in the top left, or you can open the Quake console and type:

`bind mouse2 +hook`

This would bind the grappling hook action to your right mouse button.

## Building from source

Check out this repository to your local machine. The root Makefile has a few commands to build the project.

To build, use `make all`. The default directory for the output will be ./build/meta. After building, copy the "meta" directory in ./build to the installation path for Quake as noted above. 

## Disclaimer

I am not a master C programmer. This repository is mainly for preservation purposes and a few tweaks or bugfixes that I am able to find and rectify. I really just wanted to save and share this fantastic Quake 1 mod that I dumped many hours into in the 90s. Hopefully, it will be spotted and cared for and carried forward by folks in the GitHub/gaming community.

Thanks Lon.
