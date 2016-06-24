<a href="https://scan.coverity.com/projects/lifish">
	<img alt="Coverity Scan Build Status"
	     src="https://scan.coverity.com/projects/5674/badge.svg"/>
</a>
## ABOUT ##
Lifish is an open source clone of the game [BOOM](http://www.factor-software.com/boom.php)
by Federico Filipponi, originally available only on MacOS. It's an arcade game with similar
mechanics as Bomberman, featuring 80 levels, 10 different enemies, bosses, powerups, 
local multiplayer and a final boss fight.

Lifish aims to be completely "backward compatible" with BOOM, while introducing new elements
and different enemies and stages. In particular, these are the differences with the
original BOOM:

* all the game sprites are (going to be) totally remade from scratch;
* the game will be 100% free without any paid content;
* several game features can be modded directly from the configuration files,
  which are plain JSON; in particular the enemies' AI and stats, the levels'
  tilesets, textures, configuration and music can be modded this way;
* a new type of wall has been added (TransparentWall) which is transparent to
  enemies' bullets but opaque to other entities;
* new powerups are planned;
* online multiplayer *may* be added in future.

Coded with [vim](http://www.vim.org/).

Lifish comes with a (Work In Progress) level editor, currently working only on Linux and MacOS: [LifishEdit](https://github.com/silverweed/lifish-edit)

### Planned release ###
Once the main work is done (read: the assets), the game will be released 
for the current platforms:

* Windows Vista or later 64 bit (if you want 32 bit, you can build it yourself)
* Linux (somewhat recent versions of `libc` and `libstdc++` are needed; to be precise,
  your distro must support `GLIBC_2.17` and `GLIBCXX_3.4.20`.)
* OS X
* Probably FreeBSD

See later for instructions on how to build the game from the sources.

## DEPENDENCIES ##

* [SFML](https://github.com/SFML/SFML) >= 2.3
* [GTK3](http://www.gtk.org/) (optional)
* [nlohmann/json](https://github.com/nlohmann/json) (included in the source)
* [mlabbe/nativefiledialog](https://github.com/mlabbe/nativefiledialog) (optional, included in the source)

### Building ###
The game can be compiled on *nix (tested on Linux, OSX and FreeBSD) via `cmake . && make`
(you need to have SFML >=2.3 installed).

On Windows, I'm able to compile the game via gcc under MinGW-w64 (I use the TDM-GCC compiler).
Avoid the Visual Studio compiler, as it lacks decent C++11 support (it can't compile nlohmann/json).

To get a working build environment, you can follow the steps on
[this guide](http://ascend4.org/Setting_up_a_MinGW-w64_build_environment) until the *MinGW-w64*
chapter (included -- you can skip the *First steps* section, as all the listed dependencies like Python etc
aren't required). Remember to `cmake -G "MSYS Makefiles" .`, otherwise cmake will probably
generate a VS makefile.
If you manage to build the game in some other way or on some other platform, let me know!

**IMPORTANT**: currently, many sprites are missing. If you see white rectangles where
sprites should be, it means that asset hasn't been created yet.

### Current Status ###

**MASSIVE REFACTOR IN PROGRESS** (see branch [refactor](https://github.com/silverweed/lifish/tree/refactor))

- [x] Levels BG / Borders display
- [x] Levels music play
- [x] Fixed Walls
- [x] Breakable Walls  
  - [x] Breakable Walls break on explosions
  - [x] Breakable Walls drop powerups
- [x] Collisions with walls
- [x] Powerups can be taken by players
- [x] Enemies  
  - [x] Enemies move  
    - [x] Soldier/Sgt.Cool AI
    - [x] ThickLizzy/etc AI
    - [x] Thing/etc AI
  - [x] Enemies shoot
  - [x] Enemies die when hit by explosions
- [x] Bosses   
  - [x] Boss collisions
  - [x] Bosses shoot
  - [x] Bosses are hit by explosions
  - [x] Bosses die correctly
  - [ ] Final Boss
- [x] Players   
  - [x] Players move
  - [x] Players drop bombs
  - [x] Players are hurt by explosions
  - [x] Players die when they should
- [x] Bombs
- [x] Coins   
  - [x] Coins can be taken
  - [x] Taking all coins causes Extra Game
- [x] Bullets
  - [x] Directional bullets use correct sprites
- [x] Hurry Up
- [x] Level-wise Victory
  - [ ] FINAL VICTORY
- [x] Game Over
- [x] Points
- [x] Extra
  - [x] Enemies become harmless in EXTRA
  - [x] Enemies drop letters in EXTRA
- [x] Side panel
- [ ] Sounds
- [ ] Music
- [ ] Graphics
  - [ ] Tilesets
    - [x] #1
    - [x] #2
    - [x] #3
    - [x] #4
    - [ ] #5
    - [ ] #6
    - [ ] #7
    - [ ] #8
  - [ ] Enemies
    - [x] #1
    - [ ] #2
    - [ ] #3
    - [x] #4
    - [ ] #5
    - [ ] #6
    - [ ] #7
    - [ ] #8
    - [ ] #9
    - [ ] #10
  - [x] Powerups
  - [ ] Bosses
  - [ ] Many other things

### LICENSE ###
Lifish is free software and its source code is licensed under the GNU GPL v3,
unless otherwise specified (in particular, the dependencies specified above
are under different licenses).
See [LICENSE.txt](https://github.com/silverweed/lifish/blob/master/LICENSE.txt)
for more information.

All the files in the `assets` directory, except those in `assets/sounds` and `assets/fonts`,
are licensed under the [Creative Commons - Attribution / Non Commercial / Share Alike](http://creativecommons.org/licenses/by-nc-sa/4.0/) License.
