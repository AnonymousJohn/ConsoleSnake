//Implementation of Pause Menu functions
#include "ConsoleSnake.h" //include all functions so we can access other functions
#include <string.h>

bool pmNeedTotalDraw;
bool pmNeedDraw;
short pmSelectedLine;
short pmLastLine;

void setupPM()
{
	pmNeedTotalDraw = true;
	pmNeedDraw = true;
	pmSelectedLine = 0;
	pmLastLine = -1;
}

void pmScrollUp()
{
	pmLastLine = pmSelectedLine;
	if(pmSelectedLine == 0)
	{
		pmSelectedLine = 1;
	}
	else
	{
		pmSelectedLine--;
	}
}
void pmScrollDown()
{
	pmLastLine = pmSelectedLine;
	if(pmSelectedLine == 1)
	{
		pmSelectedLine = 0;
	}
	else
	{
		pmSelectedLine++;
	}
}

//Lines
//0 - Return to Game
//1 - Quit Game
void pmDoSelect()
{
	switch(pmSelectedLine)
	{
	case 0:
		setMainState(MODE_GAME_PLAY);
		setupPM();
		break;
	case 1:
		setMainState(MODE_MAIN_MENU);
		setupGame();//reset the game so the next time they play, it's a new game.
		setupPM();
		break;
	}
}

void doPMUpdate()
{
	//Draw the screen
	drawPM();
	//Process input
    INPUT_RECORD inputBuffer[8];//We shouldn't need to process more than this at a time.
	DWORD numRecordsRead;
	 //We can use PeekConsoleInput() with the same parameters to check if there is
	 //any input available; it returns immediately if there is no available input.
	ReadConsoleInput( getInputHandle(), inputBuffer, 8, &numRecordsRead);
	 //Loop through the available input
	for(int i = 0; i < numRecordsRead; i++)
	{
		switch(inputBuffer[i].EventType)
		{
		case KEY_EVENT:
			if(inputBuffer[i].Event.KeyEvent.bKeyDown)
			{
				if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 38 ||
					inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 87)//up or w
				{
					pmScrollUp();
					pmNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 40 ||
					inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 83)//down or s
				{
					pmScrollDown();
					pmNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 13)//enter
				{
					pmDoSelect();
					return;
				}
			}
			break;
		case MOUSE_EVENT:
			if(inputBuffer[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
			{
				if((long)inputBuffer[i].Event.MouseEvent.dwButtonState > 0)
				{
					pmScrollUp();
					pmNeedDraw = true;
				}
				else
				{
					pmScrollDown();
					pmNeedDraw = true;
				}
			}
			break;
		}
	}
}

void drawPM()
{
	//We want to minimize drawing, so we ask ourselves:
	//Do we need to draw the entire thing?
	if(pmNeedTotalDraw)
	{
		//Draw the walls
		 //Make the walls red
		WORD attr = BACKGROUND_RED | FOREGROUND_RED;
		 //Get the width/height of the screen
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
		DWORD width = csbi.dwSize.X;
		DWORD height = 25;//use a preset value
		//COORD cornerCoord = {width/3,5};
		DWORD out;
		 //Draw the walls
		for(int i = 5; i < height-5; i++)
		{
			COORD coord = {width/3, i};
			//We need to fill the screen with spaces
			FillConsoleOutputCharacter( getConsoleHandle(), ' ', width/3, coord, &out);
			//If we're at the top or bottom, we need to fill the entire row.
			if(i == 5 || i == height-6)
			{
				FillConsoleOutputAttribute( getConsoleHandle(), attr, width/3+1, coord, &out);
			}
			//Otherwise, only the two sides
			else
			{
				FillConsoleOutputAttribute( getConsoleHandle(), attr, 1, coord, &out);
				COORD coord2 = {(width*2)/3-1, i};
				FillConsoleOutputAttribute( getConsoleHandle(), attr, 1, coord2, &out);
				//make sure the center has the correct properties
				COORD coord3 = {width/3+1,  i};
				WORD centAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;//white text w/ black background
				FillConsoleOutputAttribute( getConsoleHandle(), centAttr, width/3-1, coord3, &out);
			}
		}
		//Draw the title and menu items
		DWORD numCharWritten;
		char* str = "GAME PAUSED";
		COORD strCoord = getCenteredStringCoord(str,width,7);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		WORD titleAttr = FOREGROUND_GREEN;//green text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), titleAttr, strlen(str), strCoord, &numCharWritten);

		str = "Resume Game";
		strCoord = getCenteredStringCoord(str,width,11);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Quit Game";
		strCoord = getCenteredStringCoord(str,width,13);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		pmNeedTotalDraw = false;
	}
	//Or do we only need to update the highlights?
	if(pmNeedDraw)
	{
		//reset the highlighting of the now selected and previously selected lines
		//get width of the screen
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
		DWORD width = csbi.dwSize.X;
		//set up necessary variables
		char* str;
		COORD strCoord;
		DWORD numCharWritten;
		//set up text attributes
		WORD unselectAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;//white text w/ black background
		switch(pmLastLine)
		{
		case 0:
			str = "Resume Game";
			strCoord = getCenteredStringCoord(str,width,11);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 1:
			str = "Quit Game";
			strCoord = getCenteredStringCoord(str,width,13);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		}
		//set up text attributes
		WORD selectAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN;//white text w/ cyan background
		switch(pmSelectedLine)
		{
		case 0:
			str = "Resume Game";
			strCoord = getCenteredStringCoord(str,width,11);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 1:
			str = "Quit Game";
			strCoord = getCenteredStringCoord(str,width,13);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		}
	}
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
