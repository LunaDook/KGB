# KGB-stage2 - Modified KGB payload to boot from NAND rather than SD

# Note
This is extremely early alpha software, I'm just putting it out here in case anyone wants to check it out.
I won't provide any support whatsoever (yet) and lack of documentation is to be expected.
If you have any specific question or think it can be improved in a certain area (especially executable size) please let me know.

# How to build
Basically, make any modifications you want under '/source' and run 'make'. It should generate a file called 'kgb.bin', use xxd on that file (xxd -i kgb.bin > kgb.h) and copy kgb.h to '/loader/source'.
Run 'make' inside '/loader' and it should generate 'arm9loaderhax.bin'. This file is your stage2 payload.

Please note that it doesn't perform any kind of patching, and firmware reading is still done from the SD card, and doesn't support chainloading so if you want to try this out, please use it as your alternative payload.

# Special thanks to a lot of people (you know who you are :3)

###### Small note
Don't worry about arm11.h, it was generated from a standard arm9loaderhax repo (delebile's one to be precise) and converted using xxd, so if you want it's source go here https://github.com/delebile/arm9loaderhax/blob/master/screen_init/source/screen_init/screen_init.c
