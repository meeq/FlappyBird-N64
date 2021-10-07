# Flappy Bird for Nintendo 64

Tap A to flap your wings and fly! The hit iOS game of 2013 is now available for
your favorite obsolete 64-bit home console.

![Screenshots of menus and gameplay](./Screenshots.png?raw=true)

## Installation

[Download](./FlappyBird.z64?raw=true) or compile the ROM file and load it as
you would any other N64 ROM.

This ROM file has been tested to work on real Nintendo 64 hardware using the
[EverDrive-64 by krikzz](http://krikzz.com/). It should also work with
[64drive by retroactive](http://64drive.retroactive.be/).

This ROM file is only known to work on low-level, high-accuracy Nintendo 64
emulators such as [CEN64](https://cen64.com/) or [MAME](http://mamedev.org/)
due to the use of [libdragon](https://dragonminded.com/n64dev/libdragon/)
instead of the proprietary SDK used by official licensed Nintendo software.

## Building

In order to build you will need the following open-source software installed:

* [libdragon development toolchain](https://github.com/DragonMinded/libdragon)

Run `make` to produce the `FlappyBird.z64` ROM file from this source tree.

## License

This source code is [BSD licensed](./LICENSE) (c) 2017 Christopher Bonhage.

The graphic and sound resources from Flappy Bird (c) 2013 .Gears are used with
love, care, and respect for non-commercial purposes, but without permission.
(Please don't sue.)

Nintendo 64 is a registered trademark of Nintendo used for informational
purposes without permission.
