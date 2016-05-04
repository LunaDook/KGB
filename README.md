#KGB - Possibly the worst 3DS CFW the world has ever had the pleasure to witness

This is a tiny arm9loaderhax-only payload that performs a firmlaunch from a file named "/firmware".

Currently only for old3DS, although I think it can work with a new3DS if using an extremely modded firmware (decrypted arm9 section, changed arm9 entrypoint, etc.)

This is both an experiment to see how low a CFWs size can go, and as an excercise to myself, to see whether I'm able to write one from scratch (excluding sdmmc-related code, which can be worked around by including the firmware in the binary, but ugh...)

Currently, it doesn't perform any patching whatsoever, so it doesn't support signature patches (homebrew and piracy) or reboot titles or any other feature from fully-grown CFWs.

###Licensing:

All of the code is available under the GPLv2 except for certain files under the "/fatfs/" directory, which are licensed under the MIT license.
All files under the "/source/" directory (not recursively) are written by me, Wolfvak and other possible collaborators, with the exception of "start.s", which came from CakesFW.

Most of the console code (console.{c, h}) came from b1l1s' ctr project, it's pretty interesting and has a lot of stuff.

#### Special thanks to mid-kid for CakesFW and all of its derivatives, especially AuroraWright's AuReiNAND (now Luma3DS) for partly bringing my interest towards 3DS hacking

#### Also thanks to d0k3 for letting me use plenty of his stuff (sha.{c, h} and i2c.{c, h}), and to Kitlith and Docmudkipz for plenty of testing.
