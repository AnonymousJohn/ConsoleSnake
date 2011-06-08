// ConsoleSnake.cpp : Defines the entry point for the console application.

//For info on documentation, see "ConsoleSnake.h".

#include "ConsoleSnake.h"

bool doQuit = false;
HANDLE consHand;
HANDLE inputHand;
short mode;

//Entry point; this is where we start our game.
int _tmain(int argc, _TCHAR* argv[])
{
	//return testIt(); //for keyboard input testing
	//Get console and input handles prepared
	consHand = GetStdHandle(STD_OUTPUT_HANDLE);
	inputHand = GetStdHandle(STD_INPUT_HANDLE);
	//Do setup procedures
	setupMM();
	setupCon();
	setupHS();
	setupGame();
	setupPM();
	setupGOM();
	//Here we establish the basic state machine that tells us what screen we're looking at.
	mode = MODE_MAIN_MENU;//Start at the main menu
	while(!doQuit)
	{
		switch(mode)
		{
		case MODE_MAIN_MENU:
			//Update the main menu screen and check for input
			doMMUpdate();
			break;
		case MODE_CONTROLS_MENU:
			//Update the control menu screen and check for input
			doConUpdate();
			break;
		case MODE_HIGH_SCORES_MENU:
			//Update the high scores menu screen and check for input
			doHSUpdate();
			break;
		case MODE_GAME_PLAY:
			//Update the game screen and check for input
			doGameUpdate();
			break;
		case MODE_PAUSE_MENU:
			//Update the pause menu screen and check for input
			doPMUpdate();
			break;
		case MODE_GAME_OVER_MENU:
			//Update the gameover menu screen and check for input
			doGOMUpdate();
			break;
		}
	}
	return 0;
}

void quitGame()
{
	doQuit = true;
}

HANDLE getConsoleHandle()
{
	return consHand;
}

HANDLE getInputHandle()
{
	return inputHand;
}

void setMainState(short state)
{
	mode = state;
}

COORD getCenteredStringCoord(char* string, DWORD width, int vertPos)
{
	int len = strlen(string);
	COORD coord = {(width-len)/2, vertPos};
	return coord;
}

char* gameUIntToCString(unsigned int num)
{
	if(num == 0)
	{
		char* str0 = new char[2];
		str0[1] = '\0';
		str0[0] = '0';
		return str0;
	}
	short numLen = (short)floor(std::log10((double)num)) + 1;
	char* str = new char[numLen+1];
	str[numLen] = '\0'; //Have to end the C-String with a '\0' character.
	for(short i = numLen-1; i >= 0; i--)
	{
		char digit = (num % 10)+48;
		num = num / 10;
		str[i] = digit;
	}
	return str;
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
