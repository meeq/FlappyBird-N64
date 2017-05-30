The source code and resources to build this ROM file are included in this file
as a bzipped tarball starting at offset 9C800 (641024 decimal).

On Unix systems this can be extracted using the following commands:

dd bs=1 skip=641024 if=FlappyBird.z64 of=FlappyBird-src.tar.bz
tar -xf FlappyBird-src.tar.bz

You will also need a mips64 GCC toolchain and libdragon:

https://github.com/DragonMinded/libdragon
