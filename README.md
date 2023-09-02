# SAMPHack
Unfinished SA:MP cheat with thread-safe rendering, likely not gonna be updated.

# Features
- Own hooking library;
- Thread-safe rendering (doesn't use exclusive lock for writing, one-line change);
- Partially reverse-engineered player class, and reverse-engineered player iteration;
- Ability to execute code in game thread, that runs every frame;
- Many SA-MP utility functions (W2S, get local player - or other players by ID, player name by ID, killfeed stuff, ...) and skeletons for hooks.
- Partially implemented ESP and fully implemented perfect skin changer.

# Disadvantages
Built for the very specific version of gta_sa and SA:MP I used.
