//Implementation of Main Menu functions
#include "ConsoleSnake.h" //include all functions so we can access other functions
#include <string.h>

bool mmNeedTotalDraw;
bool mmNeedDraw;
short mmSelectedLine;
short mmLastLine;

void setupMM()
{
	mmNeedTotalDraw = true;
	mmNeedDraw = true;
	mmSelectedLine = 0;
	mmLastLine = -1;
}

void mmScrollUp()
{
	mmLastLine = mmSelectedLine;
	if(mmSelectedLine == 0)
	{
		mmSelectedLine = 3;
	}
	else
	{
		mmSelectedLine--;
	}
}
void mmScrollDown()
{
	mmLastLine = mmSelectedLine;
	if(mmSelectedLine == 3)
	{
		mmSelectedLine = 0;
	}
	else
	{
		mmSelectedLine++;
	}
}

//Lines:
//0 - Play Game
//1 - Controls
//2 - High Scores
//3 - Quit Game
void mmDoSelect()
{
	switch(mmSelectedLine)
	{
	case 0:
		setMainState(MODE_GAME_PLAY);
		setupMM();//reset the values so we redraw next time we come to the main menu
		break;
	case 1:
		setMainState(MODE_CONTROLS_MENU);
		setupMM();
		break;
	case 2:
		setMainState(MODE_HIGH_SCORES_MENU);
		setupMM();
		break;
	case 3:
		quitGame();
		break;
	}
}

void doMMUpdate()
{
	//Draw the screen
	drawMM();
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
					mmScrollUp();
					mmNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 40 ||
					inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 83)//down or s
				{
					mmScrollDown();
					mmNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 13)//enter
				{
					mmDoSelect();
				}
			}
			break;
		case MOUSE_EVENT:
			if(inputBuffer[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
			{
				if((long)inputBuffer[i].Event.MouseEvent.dwButtonState > 0)
				{
					mmScrollUp();
					mmNeedDraw = true;
				}
				else
				{
					mmScrollDown();
					mmNeedDraw = true;
				}
			}
			break;
		}
	}
}

//Lines:
//0 - Play Game
//1 - Controls
//2 - High Scores
//3 - Quit Game
void drawMM()
{
	//We want to minimize drawing. So, we ask ourselves:
	//Do we need to draw the entire screen?
	if(mmNeedTotalDraw)
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

		str = "Play Game";
		strCoord = getCenteredStringCoord(str,width,7);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Controls";
		strCoord = getCenteredStringCoord(str,width,9);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "High Scores";
		strCoord = getCenteredStringCoord(str,width,11);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = "Quit";
		strCoord = getCenteredStringCoord(str,width,13);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		mmNeedTotalDraw = false;
	}
	//Otherwise, only update what is necessary.
	if(mmNeedDraw)
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
		switch(mmLastLine)
		{
		case 0:
			str = "Play Game";
			strCoord = getCenteredStringCoord(str,width,7);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 1:
			str = "Controls";
			strCoord = getCenteredStringCoord(str,width,9);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 2:
			str = "High Scores";
			strCoord = getCenteredStringCoord(str,width,11);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 3:
			str = "Quit";
			strCoord = getCenteredStringCoord(str,width,13);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		}
		//set up text attributes
		WORD selectAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN;//white text w/ black background
		switch(mmSelectedLine)
		{
		case 0:
			str = "Play Game";
			strCoord = getCenteredStringCoord(str,width,7);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 1:
			str = "Controls";
			strCoord = getCenteredStringCoord(str,width,9);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 2:
			str = "High Scores";
			strCoord = getCenteredStringCoord(str,width,11);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 3:
			str = "Quit";
			strCoord = getCenteredStringCoord(str,width,13);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		}
	}
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
