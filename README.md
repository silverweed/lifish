## DEPENDENCIES ##

* [SFML](https://github.com/SFML/SFML)
* [nlohmann/json](https://github.com/nlohmann/json)

### Building ###
The game can be compiled on *nix (tested on Linux and OSX) via `cmake . && make`
(you need to have SFML >=2 installed).

On Windows, I'm able to compile the game via gcc under MinGW-w64 (I use the TDM-GCC compiler).
Avoid the Visual Studio compiler, as it lacks decent C++11 support (it can't compile nlohmann/json).

To get a working build environment, you can follow the steps on
[this guide](http://ascend4.org/Setting_up_a_MinGW-w64_build_environment) until the *MinGW-w64*
chapter (included -- you can skip the *First steps* section, as all the listed dependencies like Python etc
aren't required).
If you manage to build the game in some other way or on some other platform, let me know!

### Current Status ###

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
- [ ] Game Over
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
    - [ ] #4
    - [ ] #5
    - [ ] #6
    - [ ] #7
    - [ ] #8
  - [ ] Enemies
    - [ ] #1
    - [ ] #2
    - [ ] #3
    - [ ] #4
    - [ ] #5
    - [ ] #6
    - [ ] #7
    - [ ] #8
    - [ ] #9
    - [ ] #10
  - [ ] Powerups
  - [ ] Bosses
  - [ ] Many other things
