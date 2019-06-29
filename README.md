# LAZARUS for YQ2

If you're here then you've stumbled across my attempt to port the Lazarus mod to Yamagi Quake II. 

For reference, this is the video I'm using as an example of when the Lazarus mod works: [https://www.youtube.com/watch?v=075EA4WPSr4](https://www.youtube.com/watch?v=075EA4WPSr4)

Many things work. Many things don't. 

Based on the maps demonstrated in the video, and known issues, here's my notes:

- Footsteps don’t work because Lazarus relied on a Windows-specific file (FMOD.dll)
- Fog not in yet because Lazarus relied on a 3DFX include file - the KMQ2 reimplementation would probably work but I haven’t done it yet
- Map EFFECTS - I think most things work but the map’s kinda confusing on what is supposed to be happening
- Map MODEL - most things seem to match the video
- Map ROTATION - teleporters not working, trigger for random thing not working, light beams on initial force field not showing up
- Map MONSTERS - “initially dead” monster not corpse on floor like in video, giant friendly tank doesn’t burst through wall like in video
- Map LRAMP - lighting changes not occurring at all
- Map PENDULUM - the block that stops the pendulum in the video just explodes - not sure which one is right
- Map REFLECT2 - reflection works great, but the doors won’t open like in the video
- Map REFLECT - works fine
- Map RAIN - works fine
- Map BLASTER - none of the wall blaster things do anything
- Map TURRETS2 - little robot thing doesn’t appear
- Map ROCKS - works fine
- Map LCRAFT - helicopter circles but never lands, ship on shore never opens up, not sure if monsters are in it. 
- Map TURRET - monster manning console is invincible and doesn’t move, can’t take control of the turret
- Map CRANE - works fine
- Map HYDE1 - floating and movement and physics work fine, lasers do not work/appear
- Map HYDE2 - does not do the transition to get to HYDE3
- Map HYDE3 - door floor trigger/button does not work
- Map TRACK1 - crashes



# Yamagi Quake II

This is the Yamagi Quake II Client, an enhanced version of id Software's Quake
II with focus on offline and coop gameplay. Both the gameplay and the graphics
are unchanged, but many bugs if the last official release were fixed and some
nice to have features like widescreen support and a modern OpenGL 3.2 renderer
were added. Unlike most other Quake II source ports Yamagi Quake II is fully 64
bit clean. It works perfectly on modern processors and operating systems. Yamagi
Quake II runs on nearly all common platforms; including FreeBSD, Linux, OpenBSD,
Windows and OS X (experimental).

This code is build upon Icculus Quake II, which itself is based on Quake II
3.21. Yamagi Quake II is released under the terms of the GPL version 2. See the
LICENSE file for further information.


## Documentation

Before asking any questions read through the documentation! The current
version can be found here: [doc/01_index.md](doc/01_index.md)


## Releases

The official releases (including Windows binaries) can be found at our
homepage: https://www.yamagi.org/quake2  
**Unsupported** preview builds for Windows can be found at
https://deponie.yamagi.org/quake2/misc/
