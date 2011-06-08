//Documentation on most methods can be found in the header files. For methods that are not defined in the header,
//you can find documentation directly above the method definition in the cpp file. All method implementations
//contain comments explaining the code, where necessary.

//Main Includes
#include <Windows.h> //gives us access to windows console functions
#include <tchar.h> //gives us access to the TCHAR data type
#include <iostream> //gives us access to basic C++ I/O

#include "MainMenu.h"
#include "ControlsMenu.h"
#include "Gameplay.h"
#include "PauseMenu.h"
#include "GameOverMenu.h"
#include "HighScoresMenu.h"
#include "test.h" //only for testing purposes

//Basic Definitions
///States we can put the state machine into
#define MODE_MAIN_MENU        0
#define MODE_CONTROLS_MENU    1
#define MODE_HIGH_SCORES_MENU 2
#define MODE_GAME_PLAY        3
#define MODE_PAUSE_MENU       4
#define MODE_GAME_OVER_MENU   5

//Functions in ConsoleSnake.cpp that need to be accessible to other files
///Sets the game loop to quit next time around the main update loop.
void quitGame();
///Gets the handle to the console
HANDLE getConsoleHandle();
///Gets the handle to the input from the console
HANDLE getInputHandle();
///Sets the state of the game machine; Main Menu, Control Menu, Gameplay Screen, etc. (see Basic Definitions for a full list of states)
void setMainState(short state);
///Gets the coordinate object to use when drawing a string centered on the center of the screen.
COORD getCenteredStringCoord(char* string, DWORD width, int vertPos);
///Parses an unsigned integer into a string. YOU MUST DELETE[] THE STRING RETURNED!!!
char* gameUIntToCString(unsigned int num);

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
