# Flappy Bird for Nintendo 64

Tap A to flap your wings and fly! The hit iOS game from 2013 is now available for the hottest cartridge-based game console of 1997.

![Screenshots of Flappy Bird menus and gameplay](./Screenshots.png?raw=true)

This project is intended to be a complete example of a simple yet non-trivial game built for Nintendo 64 using the open source [LibDragon SDK](https://dragonminded.com/n64dev/libdragon/).

## Installation

[Download](./FlappyBird-v1.4.z64?raw=true) or compile the ROM file and load it as you would any other N64 ROM.

This ROM file has been tested to work on real Nintendo 64 hardware using the [EverDrive-64 by krikzz](http://krikzz.com/) and [64drive by retroactive](http://64drive.retroactive.be/).

This ROM file is also compatible with low-level, high-accuracy Nintendo 64 emulators such as [Ares](https://ares-emulator.github.io/), [CEN64](https://cen64.com/) and [MAME](http://mamedev.org/).

Due to the usage of [LibDragon](https://dragonminded.com/n64dev/libdragon/), it is an explicit non-goal of this project to suppport emulators that only implement the proprietary SDK used by official licensed Nintendo software: "If it works on a real N64 but not on an emulator, the emulator is wrong."

## Building

Run `make` to produce the `FlappyBird.z64` ROM file from this source tree.

A known-good version of [LibDragon](https://github.com/DragonMinded/libdragon) will be checked-out and built as part of the Makefile process.

If you already have a toolchain, the Makefile will respect your existing `N64_INST` environment variable. Otherwise, the toolchain will be built as part of the project.

See the [LibDragon toolchain script](https://github.com/DragonMinded/libdragon/blob/trunk/tools/build-toolchain.sh) for information on prerequisites and dependencies.

### Build Options

The Makefile will respect these environment variables, with specific effects:

* `N64_INST`: Specify where your MIPS toolchain is installed. If unspecified, the toolchain will be built in-project, which may take a while.
* `GITMODULES`: Set to `GITMODULES=0` to disable fetching a known-good commit of LibDragon, useful for experimenting with modifications and alternate versions of LibDragon.
* `V`: Set to `V=1` to enable "verbose" Make output, useful for troubleshooting.

## License

This project's source code is [BSD licensed](./LICENSE.txt?raw=true) (c) 2017-2022 Christopher Bonhage.

The graphic and sound assets are subject to the original work's copyright: [Flappy Bird (c) 2013 .Gears](https://www.dotgears.com/apps/app_flappy.html)<br />
These assets are used for non-commercial purposes with love, care, and respect, but without permission.

LibDragon is [Unlicensed public domain software](https://github.com/DragonMinded/libdragon/blob/trunk/LICENSE.md?raw=true).

"Nintendo 64" is a registered trademark of Nintendo used for informational purposes without permission.
