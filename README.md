# GOHOME

GOHOME is a small DOS Terminate-and-Stay-Resident (TSR) utility. Written In TURBO-C

## Automatic Loading at Boot

To load GOHOME into memory automatically when the system boots, add its full path to `AUTOEXEC.BAT`.

Example:

Example: If you place it in autoexec.bat with the correct pathway such as-
 
   # C:\TC\BIN\GOHOME.EXE 
 
    you should see something like this at startup if correctly loaded:
   
    GOHOME TSR - CTRL+HOME = CD C:\
    Installing...
    Installed! Press CTRL+HOME to change to C:\
 
