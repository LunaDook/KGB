# Notice
This was never my project, it was originally created by @Wolfvak and at some point in the past I had forked it for whatever reason. As there is no real need for KGB as a CFW anymore (due to Luma3DS' support for booting from NAND), I'm guessing they discontinued the project. I'm keeping the repo up for archival purposes. I know for a fact that it was behind wolfvak's repo by a lot of commits though, so keep that in mind. I can not and will not provide support for it, and if you do something stupid with it I take no responisbility.

# KGB-stage2 - Modified KGB payload to boot from NAND rather than SD
Apparantly there is some issue that prevents stage2 installation from working properly. Please don't install it as stage2 right now.
# Note
This is extremely early alpha software, I'm just putting it out here in case anyone wants to check it out.
I won't provide any support whatsoever (yet) and lack of documentation is to be expected.
If you have any specific question or think it can be improved in a certain area (especially executable size) please let me know.

# How to build
This project uses [libctr9_io](https://github.com/gemarcano/libctr9_io) and assumes it's directory is pointed at by the environment path CTRARM9. This was decided to be consistent with @gemarcano's fork of uncart, which uses CTRARM9 as well.
Basically, make any modifications you want under '/source' and run 'make'. It should generate a file called 'arm9loaderhax.bin', rename it to 'kgb', xxd that file (xxd -i kgb > kgb.h) and copy kgb.h to '/stage2/source'.
Run 'make' inside 'stage2' and it should generate 'stage2.bin'. This file is your stage2 payload.

For more details on how to compile the loader fork, check out its README.

Now it supports reading and decrypting firmware files from CTRNAND, and actually doesn't mount the SD card at all.

Special thanks to a lot of people (you know who you are :3). For more details, check CREDITS.md

###### Small note
Don't worry about arm11.h, it was generated from a standard arm9loaderhax repo (delebile's one to be precise) and converted using xxd, so if you want its source go here https://github.com/delebile/arm9loaderhax/blob/master/screen_init/source/screen_init/screen_init.c
