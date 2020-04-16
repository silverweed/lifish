<a href="https://scan.coverity.com/projects/lifish">
	<img alt="Coverity Scan Build Status"
	     src="https://scan.coverity.com/projects/5674/badge.svg"/>
</a>

## ABOUT ##
Lifish is a work-in-progress arcade old-style game inspired by Factor Software's BOOM. 

The Lifish engine was used to create [BOOM Remake](https://silverweed.github.io/boom), a faithful remake of the original BOOM game. The code used for BOOM Remake is under the `boom` branch.

Lifish comes with a (Work In Progress) level editor, currently working only on Linux and MacOS: [LifishEdit](https://github.com/silverweed/lifish-edit). This also works for the BOOM Remake mod.

### Main game planned release ###
Lifish is currently on hiatus and I'm not sure if I'll ever finish it. However BOOM Remake is completed, so you can play it instead. These are the OS you can play it on:

* Windows 64 bit (probably >= Vista)
* Linux (somewhat recent versions of `libc` and `libstdc++` are needed; to be precise,
  your distro must support `GLIBC_2.17` and `GLIBCXX_3.4.20`.)
* OS X
* Probably BSD, but you'll have to compile it yourself.

See later for instructions on how to build the game from the sources.

## Building ##

### DEPENDENCIES ###

* [SFML](https://github.com/SFML/SFML) >= 2.4
* [nlohmann/json](https://github.com/nlohmann/json) (included in the source)

#### Linux, OS X, BSD, etc ####
The game can be compiled on *nix (tested on Linux, OSX and FreeBSD) via `cmake . && make`
(you need to have SFML >=2.4 installed).

#### Windows ####
On Windows you can either use VS2015 (or later), or a Unix-like environment like MinGW-w64 (I use the TDM-GCC compiler).

If you use the latter, you can follow the steps on
[this guide](http://ascend4.org/Setting_up_a_MinGW-w64_build_environment) until the *MinGW-w64*
chapter (included -- you can skip the *First steps* section, as all the listed dependencies like Python etc
aren't required). Remember to `cmake -G "MSYS Makefiles" .`, otherwise cmake will probably
generate a VS makefile.

### Command line parameters ###
If launched from the command line, `lifish` accepts a bunch of parameters (see `lifish -h` for details).
It also accepts an optional argument which is the path of the level JSON to use (default: `lifish.json`).

### Note about assets ###
The graphics and sounds you'll find in `assets` are placeholder. No graphic asset is even close to being final.

## LICENSE ##
See the LICENSE file for licensing information.

### Other ###

Coded with [vim](http://www.vim.org/).

