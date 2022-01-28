# Flappy Bird for Nintendo 64

Tap A to flap your wings and fly! The hit iOS game from 2013 is now available on N64!

![Screenshots of Flappy Bird menus and gameplay](./Screenshots.png?raw=true)

 Navigate a bird through a series of gaps and try not to hit the walls! This simple, one-button game is so easy to play that everyone will want a turn to see how far they can go!

## Overview

This project is intended to be a complete example of a simple yet non-trivial game built for Nintendo 64 using the open source [LibDragon SDK](https://dragonminded.com/n64dev/libdragon/).

This project is considered "done", and is no longer being actively developed. It may receive occasional maintenance updates to ensure the code remains compatible with upstream changes in LibDragon.

### Features

* Faithful recreation of the original iOS game
* N64 RDP-accelerated graphics rendering
* N64 RSP-accelerated audio mixing
* 60FPS gameplay at 320x240 resolution
* Parallax background scrolling
* Rumble Pak support

### Known issues

* Bird sprites do not rotate
* High score is not saved across resets
* Hardware reset briefly shows an exception handler screen

## Play the game

[Download](./FlappyBird-v1.4.z64?raw=true) or [compile](#build-the-rom) the ROM file and load it as you would any other N64 ROM.

This ROM file has been tested to work on real Nintendo 64 hardware using the [EverDrive-64 by krikzz](http://krikzz.com/) and [64drive by retroactive](http://64drive.retroactive.be/).

This ROM file should also be compatible with low-level, accuracy-focused Nintendo 64 emulators such as [Ares](https://ares-emulator.github.io/), [CEN64](https://cen64.com/) and [MAME](http://mamedev.org/).

**Due to the usage of [LibDragon](https://dragonminded.com/n64dev/libdragon/), it is an explicit non-goal of this project to suppport emulators.** The goal was to make a game for Nintendo 64, so tautologically:

> If it works on a real console but not on the emulator, then the emulator is insufficiently accurate.

## Build the ROM

Run `make` to produce a ROM file from this source tree. The filename of the ROM is derived from the `ROM_VERSION` variable, which will reference the current Git tag or commit hash. If the project has uncommitted changes, the ROM will be marked `-dirty` (indicating that it is not suitable for release).

A known-good version of [LibDragon](https://github.com/DragonMinded/libdragon) will be checked-out as a Git submodule and built as part of the Makefile process.

If you already have a toolchain, the Makefile will respect your existing `N64_INST` environment variable. Otherwise, the toolchain will be built as part of the project.

[See the LibDragon toolchain script for information on prerequisites and dependencies.](https://github.com/DragonMinded/libdragon/blob/trunk/tools/build-toolchain.sh)

### Configuration

The Makefile can be configured using the following environment variables:

* `N64_INST` — Specify where your MIPS toolchain is installed. If unspecified, the toolchain will be built in-project, which may take a while.
* `V=1` — Enable "verbose" Make output; useful for troubleshooting.
* `GITMODULES=0` — Disable updating Git submodules when building; useful for experimenting with modifications to and alternate versions of LibDragon.

## License

This project's source code is [BSD licensed](./LICENSE.txt?raw=true) (c) 2017-2022 Christopher Bonhage.

The graphic and sound assets are subject to the original work's copyright: [Flappy Bird (c) 2013 .Gears](https://www.dotgears.com/apps/app_flappy.html)<br />
These assets are used for non-commercial purposes with love, care, and respect, but without permission.

LibDragon is [Unlicensed public domain software](https://github.com/DragonMinded/libdragon/blob/trunk/LICENSE.md?raw=true).

"Nintendo 64" is a registered trademark of Nintendo used for informational purposes without permission.
