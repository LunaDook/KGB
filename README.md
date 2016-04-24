#KGB - Possibly the worst 3DS CFW the world has ever had the pleasure to witness

This is a tiny arm9loaderhax-only payload that performs a firmlaunch from a file named "/firmware".
Currently only for old3DS, although it can work with a new3DS' if using an extremely modded firmware (decrypted arm9 section, changed arm9 entrypoint, etc.)
This is both an experiment to see how low a CFW's size can go, and as an excercise to myself, to see whether I'm able to write one from scratch (excluding sdmmc-related code), which can be worked around by including it in the binary, but ugh...

Currently, it doesn't perform any patching whatsoever, so it doesn't support signature patches (homebrew and piracy) or reboot titles or any other feature from fully-grown CFWs.

###Licensing:

All of the code is available under the GPLv2 except for certain files under the "/fatfs/" directory, which are licensed under the MIT license.
All files under the "/source/" directory (not recursively) are written by me (Wolfvak) and other possible collaborators.

#### Special thanks to mid-kid for CakesFW and all of its derivatives, especially AuroraWright's AuReiNAND (now Luma3DS) for bringing my interest towards 3DS hacking.
