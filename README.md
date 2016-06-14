# KGB-stage2 - Modified KGB payload to boot from NAND rather than SD

# Note
This is extremely early alpha software, I'm just putting it out here in case anyone wants to check it out.
I won't provide any support whatsoever (yet) and lack of documentation is to be expected.
If you have any specific question or think it can be improved in a certain area (especially executable size) please let me know.

# How to build
This project uses [libctr9_io](https://github.com/gemarcano/libctr9_io) and assumes it's directory is pointed at by the environment path CTRARM9. This was decided to be consistent with @gemarcano's fork of uncart, which uses CTRARM9 as well.
Basically, make any modifications you want under '/source' and run 'make'. It should generate a file called 'arm9loaderhax.bin', rename it to 'kgb', xxd that file (xxd -i kgb > kgb.h) and copy kgb.h to '/loader/source'.
Run 'make' inside '/loader' and it should generate 'loader.bin'. This file is your stage2 payload.

Now it supports reading and decrypting firmware files from CTRNAND, and actually doesn't mount the SD card at all. No N3DS decryption support though, both CTRNAND and arm9loader-wise.

# Special thanks to a lot of people (you know who you are :3)

###### Small note
Don't worry about arm11.h, it was generated from a standard arm9loaderhax repo (delebile's one to be precise) and converted using xxd, so if you want its source go here https://github.com/delebile/arm9loaderhax/blob/master/screen_init/source/screen_init/screen_init.c
