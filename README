# OpenTyrian 64

Port of OpenTyrian to the Nintendo 64.

## Status

For now it only builds and pass the initial loadings, but still can't display anything on the screen. Most likely an issue with how SDL (the port used) creates textures and how the game manipulates them.

Another potential issue is on loading assets, it may end up using more memory than the N64 have (the assets have 12MB). We have to check how it loads assets and change the code to only load when necessary.

## Building

- Clone the repo **with submodules**
- Download the [data files here](https://camanis.net/tyrian/tyrian21.zip) and unzip them on `/filesystem`. You should have a `/filesystem/tyrian21/[all_files_here]` at the end.
- Download [libdragon-cli](https://github.com/anacierdem/libdragon-docker) and run `libdragon make` on the root of the project.


## ORIGINAL README

OpenTyrian
================================================================================

OpenTyrian is an open-source port of the DOS game Tyrian.

Tyrian is an arcade-style vertical scrolling shooter.  The story is set
in 20,031 where you play as Trent Hawkins, a skilled fighter-pilot employed
to fight MicroSol and save the galaxy.

Tyrian features a story mode, one- and two-player arcade modes, and networked
multiplayer.

== Additional Necessary Files ==================================================

Tyrian 2.1 data files which have been released as freeware:
  https://camanis.net/tyrian/tyrian21.zip

== Keyboard Controls ===========================================================

alt-enter      -- toggle full-screen

arrow keys     -- ship movement
space          -- fire weapons
enter          -- toggle rear weapon mode
ctrl/alt       -- fire left/right sidekick

== Network Multiplayer =========================================================

Currently OpenTyrian does not have an arena; as such, networked games must be
initiated manually via the command line simultaneously by both players.

syntax:
  opentyrian --net HOSTNAME --net-player-name NAME --net-player-number NUMBER

where HOSTNAME is the IP address of your opponent, NUMBER is either 1 or 2
depending on which ship you intend to pilot, and NAME is your alias

OpenTyrian uses UDP port 1333 for multiplayer, but in most cases players will
not need to open any ports because OpenTyrian makes use of UDP hole punching.

== Links =======================================================================

project: https://github.com/opentyrian/opentyrian
irc:     ircs://irc.oftc.net/#opentyrian
forums:  https://tyrian2k.proboards.com/board/5
