//NOTE: This code was pulled straight from an MSDN Library example, with slight modifications to give me the info I need.
//This is not my code, and not really a part of the game. It is simply a way to test for keyboard input values.

#include <windows.h>
#include <stdio.h>
#include "test.h"

VOID ErrorExit(LPSTR);
VOID KeyEventProc(KEY_EVENT_RECORD); 
VOID MouseEventProc(MOUSE_EVENT_RECORD); 
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD); 
 
int testIt()
{ 
    HANDLE hStdin; 
    DWORD cNumRead, fdwMode, fdwSaveOldMode, i; 
    INPUT_RECORD irInBuf[128]; 
    int counter=0;
 
    // Get the standard input handle. 
 
    hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    if (hStdin == INVALID_HANDLE_VALUE) 
        ErrorExit("GetStdHandle"); 
 
    // Save the current input mode, to be restored on exit. 
 
    if (! GetConsoleMode(hStdin, &fdwSaveOldMode) ) 
        ErrorExit("GetConsoleMode"); 
 
    // Enable the window and mouse input events. 
 
    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT; 
    if (! SetConsoleMode(hStdin, fdwMode) ) 
        ErrorExit("SetConsoleMode"); 
 
    // Loop to read and handle the next 100 input events. 
 
    while (counter++ <= 100) 
    { 
        // Wait for the events. 
 
        if (! ReadConsoleInput( 
                hStdin,      // input buffer handle 
                irInBuf,     // buffer to read into 
                128,         // size of read buffer 
                &cNumRead) ) // number of records read 
            ErrorExit("ReadConsoleInput"); 
 
        // Dispatch the events to the appropriate handler. 
 
        for (i = 0; i < cNumRead; i++) 
        {
            switch(irInBuf[i].EventType) 
            { 
                case KEY_EVENT: // keyboard input 
                    KeyEventProc(irInBuf[i].Event.KeyEvent); 
                    break; 
 
                case MOUSE_EVENT: // mouse input 
                    MouseEventProc(irInBuf[i].Event.MouseEvent); 
                    break; 
 
                case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
                    ResizeEventProc( 
                        irInBuf[i].Event.WindowBufferSizeEvent); 
                    break; 
 
                case FOCUS_EVENT:  // disregard focus events 
 
                case MENU_EVENT:   // disregard menu events 
                    break; 
 
                default: 
                    ErrorExit("Unknown event type"); 
                    break; 
            } 
        }
    } 
 
    return 0; 
}

VOID ErrorExit (LPSTR lpszMessage) 
{ 
    fprintf(stderr, "%s\n", lpszMessage); 
    ExitProcess(0); 
}

VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
    printf("Key event: ");

    if(ker.bKeyDown)
        printf("key pressed: ");
    else printf("key released: ");
	printf("%d-%d-%d",(BYTE)(ker.uChar.AsciiChar),(BYTE)(ker.uChar.UnicodeChar),(ker.wVirtualKeyCode));
	printf("\n");
}

VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
    printf("Mouse event: ");
    
    switch(mer.dwEventFlags)
    {
        case 0:

            if(mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
            {
                printf("left button press \n");
            }
            else if(mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
            {
                printf("right button press \n");
            }
            else
            {
                printf("button press\n");
            }
            break;
        case DOUBLE_CLICK:
            printf("double click\n");
            break;
        case MOUSE_HWHEELED:
            printf("horizontal mouse wheel\n");
            break;
        case MOUSE_MOVED:
            printf("mouse moved\n");
            break;
        case MOUSE_WHEELED:
            printf("vertical mouse wheel: ");
			printf("%d:%d:%d\n",mer.dwControlKeyState, mer.dwButtonState, mer.dwEventFlags);
            break;
        default:
            printf("unknown\n");
            break;
    }
}

VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    printf("Resize event\n");
    printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
}