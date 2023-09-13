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

* High score is not saved across resets
* Hardware reset briefly shows an exception handler screen

### Intentional omissions

* Bird sprite does not rotate
* Night sky is missing stars
* Medals do not sparkle
* Screen does not shake upon death

## Play the game

[Download](./FlappyBird-v1.4.z64?raw=true) or [compile](#build-the-rom) the ROM file and load it as you would any other N64 ROM.

This ROM file has been tested to work on real Nintendo 64 hardware using the [EverDrive-64 by krikzz](http://krikzz.com/), [64drive by retroactive](http://64drive.retroactive.be/), and [SummerCart64](https://github.com/Polprzewodnikowy/SummerCart64).

This ROM file should also be compatible with low-level, accuracy-focused Nintendo 64 emulators such as [Ares](https://ares-emulator.github.io/), [CEN64](https://cen64.com/) and [MAME](http://mamedev.org/).

**Due to the usage of [LibDragon](https://dragonminded.com/n64dev/libdragon/), it is an explicit non-goal of this project to suppport emulators.** The goal was to make a game for Nintendo 64, so tautologically:

> If it works on the real console but not on the emulator, then the emulator is insufficiently accurate.

## Build the ROM

Run `make` to produce a ROM file from this source tree.

### Dependencies

This project has been tested with the `trunk` branch of LibDragon:

```sh
git clone https://github.com/meeq/libdragon.git
cd libdragon
git checkout d74706b5962ecbbf4c9faa00075a2843eeebcabe
```

[See the LibDragon build script for information on prerequisites and dependencies.](https://github.com/DragonMinded/libdragon/blob/unstable/build.sh)

### Configuration

The Makefile can be configured using the following environment variables:

* `N64_INST` — Specify where your N64 GCC toolchain is installed.
* `V=1` — Enable "verbose" Make output; useful for troubleshooting.

### Versioning

Proper releases will be tagged as `vX.Y` where X is a major version number and Y is a minor version number.

## License

This project's source code is [BSD licensed](./LICENSE.txt?raw=true) (c) 2017-2022 Christopher Bonhage.

The graphic and sound assets are subject to the original work's copyright: [Flappy Bird (c) 2013 .Gears](https://www.dotgears.com/apps/app_flappy.html)<br />
These assets are used for non-commercial purposes with love, care, and respect, but without permission.

LibDragon is [Unlicensed public domain software](https://github.com/DragonMinded/libdragon/blob/trunk/LICENSE.md?raw=true).

"Nintendo 64" is a registered trademark of Nintendo used for informational purposes without permission.
