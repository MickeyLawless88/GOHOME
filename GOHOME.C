/*
 *     ######    #######     ##     ##  #######  ##     ## ######## #### 
 *    ##    ##  ##     ##    ##     ## ##     ## ###   ### ##       #### 
 *    ##        ##     ##    ##     ## ##     ## #### #### ##       #### 
 *    ##   #### ##     ##    ######### ##     ## ## ### ## ######    ##  
 *    ##    ##  ##     ##    ##     ## ##     ## ##     ## ##            
 *    ##    ##  ##     ##    ##     ## ##     ## ##     ## ##       #### 
 *     ######    #######     ##     ##  #######  ##     ## ######## #### 
 *
 *  
 *
 *    GoHome!  —  TSR Home Directory Shortcut
 *    --------------------------------------
 *
 *    Description:
 *
 *        GoHome is a Terminate-and-Stay-Resident (TSR) utility that
 *        instantly changes the current working directory to the
 *        configured home directory from anywhere in DOS.
 *
 *    Triggered by pressing:
 *
 *          CTRL + 7 (HOME key on numeric keypad)
 *
 *    Author:				Date:
 *
 *          Mickey W. Lawless		    February 16, 2026
 *
 *    Development Environment:
 *
 *      Compiler        : Turbo C		Operating System: MS-DOS
 *     
 *
 *  Notes:
 *      Designed for classic IBM PC–compatible systems.
 *      Lightweight, fast, and intended to remain resident.
 *
 *      To start it automatically on boot, tell autoexec.bat to execute it
 *      on startup by adding its path to autoexec.bat
 *
 *   Example: If you place it in autoexec.bat with the correct pathway such as-
 *
 *      C:\TC\GOHOME.EXE 
 *
 *   you should see something like this at startup if correctly loaded:
 *
 *	GOHOME TSR - CTRL+HOME = CD C:\
 *	Installing...
 *	Installed! Press CTRL+HOME to change to C:\
 *
 */
 
#include <dos.h>
#include <stdio.h>
#include <bios.h>

#define HOME_SCAN 0x47
#define CTRL_FLAG 0x04

void interrupt (*old_int9)();

/* These MUST be in the resident part */
char far inject_cmd[] = "cd c:\\\r";
int far inject_flag = 0;
int far inject_pos = 0;

/* Stuff a string into the keyboard buffer */
void far stuff_keyboard(char far *str) {
    unsigned int head, tail, start, end;
    int i;
    
    disable();
    
    /* Get BIOS keyboard buffer pointers */
    start = peekb(0x0040, 0x0080);
    end = peekb(0x0040, 0x0082);
    
    /* Default buffer locations if not set */
    if (start == 0) start = 0x001E;
    if (end == 0) end = 0x003E;
    
    /* Get current head position and set as new start */
    head = peekb(0x0040, 0x001A);
    
    /* Stuff characters */
    for (i = 0; str[i] != 0; i++) {
        /* Write ASCII character */
        pokeb(0x0040, head, str[i]);
        /* Write scan code (0 for stuffed keys) */
        pokeb(0x0040, head + 1, 0);
        
        /* Move to next buffer position */
        head += 2;
        if (head >= end) head = start;
    }
    
    /* Update tail pointer */
    pokeb(0x0040, 0x001C, head);
    
    enable();
}

/* Keyboard interrupt handler */
void interrupt new_int9() {
    unsigned char scan_code;
    unsigned char kbd_status;
    
    /* Read scan code from port */
    scan_code = inportb(0x60);
    
    /* Get keyboard flags from BIOS data area */
    kbd_status = peekb(0x0040, 0x0017);
    
    /* Check for CTRL+HOME */
    if (scan_code == HOME_SCAN && (kbd_status & CTRL_FLAG)) {
        /* Clear the keyboard buffer first */
        pokeb(0x0040, 0x001A, peekb(0x0040, 0x0080));
        pokeb(0x0040, 0x001C, peekb(0x0040, 0x0080));
        
        /* Stuff our command */
        stuff_keyboard(inject_cmd);
        
        /* Read the keyboard port to clear it */
        inportb(0x61);
        outportb(0x61, inportb(0x61) | 0x80);
        outportb(0x61, inportb(0x61) & 0x7F);
        
        /* Send EOI to interrupt controller */
        outportb(0x20, 0x20);
        
        return;
    }
    
    /* Call original interrupt - don't use function call, use inline asm */
    asm {
        pushf
    }
    (*old_int9)();
}

int main() {
    unsigned int res_size;
    
    printf("GOHOME TSR - CTRL+HOME = CD C:\\\n");
    printf("Installing...\n");
    
    /* Save old interrupt vector */
    old_int9 = getvect(0x09);
    
    /* Install new handler */
    setvect(0x09, new_int9);
    
    printf("Installed! Press CTRL+HOME to change to C:\\\n");
    
    /* Calculate resident size */
    res_size = (_SS + (_SP / 16) - _psp);
    
    /* Terminate and stay resident */
    keep(0, res_size);
    
    return 0;
}
