//Implementation of Controls Menu functions
#include "ConsoleSnake.h" //include all functions so we can access other functions
#include <string.h>

bool conNeedTotalDraw;

void setupCon()
{
	conNeedTotalDraw = true;
}

void conDoSelect()
{
	setMainState(MODE_MAIN_MENU);
	setupCon();//reset variables so that we redraw the next time we come to this menu.
}

void doConUpdate()
{
	//Draw the screen
	drawCon();
	//Process input
    INPUT_RECORD inputBuffer[8];//We shouldn't need to process more than this at a time.
	DWORD numRecordsRead;
	 //Note: We can use PeekConsoleInput() with the same parameters to check if there is
	 //any input available; it returns immediately if there is no available input. This
	 //method waits until there is input in the console's input buffer.
	ReadConsoleInput( getInputHandle(), inputBuffer, 8, &numRecordsRead);
	 //Loop through the available input
	for(int i = 0; i < numRecordsRead; i++)
	{
		if(inputBuffer[i].EventType == KEY_EVENT)
		{
			if(inputBuffer[i].Event.KeyEvent.bKeyDown)
			{
				if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 13)//enter
				{
					conDoSelect();
				}
			}
		}
	}
}

void drawCon()
{
	//We want to minimize drawing to save CPU. So, we ask ourselves:
	//Do we need to draw the entire screen?
	if(conNeedTotalDraw)
	{
		//Draw the walls
		 //Make the walls blue
		WORD attr = BACKGROUND_BLUE | FOREGROUND_BLUE;
		 //Get the width/height of the screen
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
		DWORD width = csbi.dwSize.X;
		DWORD height = 25;//use a preset value
		 //We need to fill the screen with spaces
		COORD cornerCoord = {0,0};
		DWORD out;
		FillConsoleOutputCharacter( getConsoleHandle(), ' ', width*height, cornerCoord, &out);
		 //Draw the walls
		for(int i = 0; i < height; i++)
		{
			COORD coord = {0, i};
			//If we're at the top or bottom, we need to fill the entire row.
			if(i == 0 || i == height-1)
			{
				FillConsoleOutputAttribute( getConsoleHandle(), attr, width, coord, &out);
			}
			//Otherwise, only the two sides
			else
			{
				FillConsoleOutputAttribute( getConsoleHandle(), attr, 1, coord, &out);
				COORD coord2 = {width-1, i};
				FillConsoleOutputAttribute( getConsoleHandle(), attr, 1, coord2, &out);
				//make sure the center has the correct properties
				COORD coord3 = {1,  i};
				WORD centAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;//white text w/ black background
				FillConsoleOutputAttribute( getConsoleHandle(), centAttr, width-2, coord3, &out);
			}
		}
		//Draw the title and menu items
		DWORD numCharWritten;
		char* str = "CONSOLE SNAKE";
		COORD strCoord = getCenteredStringCoord(str,width,3);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		WORD titleAttr = FOREGROUND_RED;//red text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), titleAttr, strlen(str), strCoord, &numCharWritten);

		str = "Controls";
		strCoord = getCenteredStringCoord(str,width,5);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		titleAttr = FOREGROUND_GREEN;//green text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), titleAttr, strlen(str), strCoord, &numCharWritten);

		str = "Move Up: W, Up";
		strCoord = getCenteredStringCoord(str,width,9);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Move Down: S, Down";
		strCoord = getCenteredStringCoord(str,width,10);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Move Right: D, Right";
		strCoord = getCenteredStringCoord(str,width,11);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Move Left: A, Left";
		strCoord = getCenteredStringCoord(str,width,12);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Pause Game: P";
		strCoord = getCenteredStringCoord(str,width,13);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		

		str = "Back to Main Menu";
		strCoord = getCenteredStringCoord(str,width,16);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		strCoord.X -= 2;
		WORD selectedAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;//white text w/ cyan background
		FillConsoleOutputAttribute( getConsoleHandle(), selectedAttr, strlen(str)+4, strCoord, &numCharWritten);

		conNeedTotalDraw = false;
	}
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
