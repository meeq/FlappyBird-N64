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
* High score save support using EEPROM 4K
* Rumble Pak support

### Intentional omissions

* Night sky is missing stars
* Screen does not shake upon death

## Play the game

[Download](https://github.com/meeq/flappybird-n64/releases) or [compile](#build-the-rom) the ROM file and load it as you would any other N64 ROM.

This ROM file has been tested to work on real Nintendo 64 hardware using the [EverDrive-64 by krikzz](http://krikzz.com/), [64drive by retroactive](http://64drive.retroactive.be/), and [SummerCart64](https://github.com/Polprzewodnikowy/SummerCart64).

This ROM file should also be compatible with low-level, accuracy-focused Nintendo 64 emulators such as [Ares](https://ares-emulator.github.io/), [gopher64](https://github.com/gopher64/gopher64) and [MAME](http://mamedev.org/).

**Due to the usage of [LibDragon](https://dragonminded.com/n64dev/libdragon/), it is an explicit non-goal of this project to suppport emulators.** The goal was to make a game for Nintendo 64, so tautologically:

> If it works on the real console but not on the emulator, then the emulator is insufficiently accurate.

## Build the ROM

### Using mise (recommended)

This project uses [mise](https://mise.jdx.dev/) to manage the development environment and build tasks. Once mise is installed, run:

```sh
mise run setup
```

This will automatically initialize git submodules, build the N64 toolchain, and install LibDragon.

#### Available tasks

| Task | Alias | Description |
|------|-------|-------------|
| `mise run setup` | | Set up the complete development environment |
| `mise run build` | `mise run b` | Build the ROM |
| `mise run clean` | `mise run c` | Clean build artifacts |
| `mise run ares` | | Build and run the ROM in Ares emulator |

Additional lower-level tasks (usually run automatically by `setup`):

| Task | Description |
|------|-------------|
| `mise run submodules` | Initialize and update git submodules |
| `mise run toolchain` | Build the N64 GCC cross-compiler toolchain + GDB |
| `mise run libdragon` | Build and install LibDragon into the toolchain |

### Using make directly

Run `make` to produce a ROM file from this source tree.

#### Dependencies

This project has been tested with the `trunk` branch of LibDragon:

```sh
git submodule update --init --recursive
```

Run the [`libdragon/tools/build-toolchain.sh`](https://github.com/DragonMinded/libdragon/blob/trunk/tools/build-toolchain.sh) and [`libdragon/tools/build-gdb.sh`](https://github.com/DragonMinded/libdragon/blob/trunk/tools/build-gdb.sh) scripts to prepare a toolchain for LibDragon.

Then run the [`libdragon/build.sh`](https://github.com/DragonMinded/libdragon/blob/trunk/build.sh) script to install LibDragon in the toolchain.

#### Configuration

The Makefile can be configured using the following environment variables:

* `N64_INST` — Specify where your N64 GCC toolchain is installed.
* `V=1` — Enable "verbose" Make output; useful for troubleshooting.

### Versioning

Proper releases will be tagged as `vX.Y` where X is a major version number and Y is a minor version number.

## License

This project's source code is [BSD licensed](./LICENSE.txt?raw=true) (c) 2017-2022 Christopher Bonhage.

The graphic and sound assets are subject to the original work's copyright: [Flappy Bird (c) 2013 .Gears](https://www.dotgears.com/apps/app_flappy.html)<br />
These assets are used for non-commercial purposes with love, care, and respect, but without permission.

The [at01 font by GrafxKid](https://grafxkid.itch.io/at01) is [Public Domain (CC0 1.0)](https://creativecommons.org/public-domain/cc0/).

LibDragon is [Unlicensed public domain software](https://github.com/DragonMinded/libdragon/blob/trunk/LICENSE.md?raw=true).

"Nintendo 64" is a registered trademark of Nintendo used for informational purposes without permission.
