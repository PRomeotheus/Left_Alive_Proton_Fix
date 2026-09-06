# Left_Alive_Proton_Fix
Fixes crash on FMVs for this game by injecting a small DirectShow shim on the proton prefix folder for the game

LEFT ALIVE (Steam AppID 714370) crashes on Linux the moment it tries to play a
cutscene. 

`lashim.dll` is a small DirectShow shim that fixes that issue. It basically is just DLL and one modification inside the game's own Proton prefix registry.

Tested on Steam Deck and Bazzite with GE-Proton10-15 and GE-Proton10-23


## What it fixes

| Symptom | Cause |
|---|---|
| Hard crash (`EXCEPTION_ACCESS_VIOLATION`) the instant an FMV starts | The game queries the filter graph for `IDMOWrapperFilter`, gets NULL under Wine, and dereferences it |
| Game freezes when a cutscene ends or is skipped | Wine stops graph filters in list order, leaving the null renderer running and deadlocking the ASF reader's cleanup |

With the shim installed, cutscenes play without issues and gameplay is accesible. (Haven't played the full game yet to ensure no other crashes appear)
The installer touches exactly one Wine prefix — the one named by `--appid` (default 714370) or `--prefix`. Proton gives every game its own prefix under `steamapps/compatdata/<appid>`, so no other game is affected.


## Install

Install the game, force compatibility to a GE-Proton version and run the game until it crashes for the first time.
Then go to Desktop mode, decompress the Left_Alive_Proton_Fix_V1.zip file.
Close Steam completely and wait about ten seconds. `wineserver` rewrites the prefix registry when the last process in it exits, and will undo the change otherwise.

```bash
cd lashim
chmod +x install.sh
./install.sh
./install.sh --status
```

`--status` should report `active : lashim`. You should be able to launch the game normally now.

## Uninstall

```bash
./install.sh --uninstall
```

Restores the original `InprocServer32` value (saved at install time) and removes the DLL. You also need to have Steam closed before running.

## Notes and caveats

- **A Proton version change can revert the fix.**
- If you change to a different proton version or forget to set GE-Proton on the first launch, make sure to launch the game with GE-Proton first, let it crash and then follow the full install process again.

## Credits

The wrapper approach is [krkrwine](https://github.com/Alcaro/krkrwine) by Alcaro — a DLL dropped into a prefix that supplies a DirectShow piece Wine doesn't provide.

## Disclosure

The fix was found and developed using Claude
