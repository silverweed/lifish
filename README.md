<a href="https://scan.coverity.com/projects/lifish">
	<img alt="Coverity Scan Build Status"
	     src="https://scan.coverity.com/projects/5674/badge.svg"/>
</a>

<img src="https://silverweed.github.io/assets/img/boom/boom_screen1.png" style='height: 300px'/>

## ABOUT ##
This is the branch of Lifish that contains the code for [BOOM Remake](https://silverweed.github.io/boom), a faithful remake of the original BOOM game.

Lifish was a work-in-progress arcade game inspired by BOOM, but that project is currently in indefinite hiatus. The `boom` branch is the one that's still being occasionally updated, mostly for bugfixing, as BOOM Remake is already complete.

Lifish (and by extension, BOOM Remake) has a level editor available as a separate program, currently working only on Linux and MacOS: [LifishEdit](https://github.com/silverweed/lifish-edit). 

### Supported platforms ###
BOOM Remake is known to work on the following platforms:

* Windows 64 bit (>= 7)
* Linux 64 bit (with non-ancient versions of `libc` and `libstdc++`)
* OS X (but there are known issues with inputs for versions following Catalina)

### Binaries ###
The latest "official" binaries are available [here](https://silverweed.github.io/boom/). The latest version in this repo may contain additional features that I haven't had time to polish and package yet in the release.

## Building ##

### DEPENDENCIES ###

* [SFML](https://github.com/SFML/SFML) >= 2.5
* [nlohmann/json](https://github.com/nlohmann/json) (included in the source)

#### Linux, OS X, BSD, etc ####
The game can be compiled on *nix (tested on Linux, OSX and FreeBSD) via `cmake . && make`
(you need to have SFML >=2.5 installed).

#### Windows ####
On Windows you can use cmake + Visual Studio.

### Command line parameters ###
If launched from the command line, `lifish` accepts a bunch of parameters (see `lifish -h` for details).
It also accepts an optional argument which is the path of the level JSON to use (default: `lifish.json`).

### Note about assets ###
The original BOOM assets belong to Factor Software. I don't hold any intellectual property upon them.

## LICENSE ##
Copyright (c) silverweed, 2022

See the LICENSE file for licensing information.

## CONTRIBUTORS ##
- [silverweed](https://github.com/silverweed)
- [bl00dy1837](https://github.com/bl00dy1837)
- [DrSh4dow](https://github.com/DrSh4dow)
- [eguiraud](https://github.com/eguiraud)

### Other ###

Coded with [vim](http://www.vim.org/).

