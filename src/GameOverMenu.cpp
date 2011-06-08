#include "ConsoleSnake.h"

void setupGOM()
{
	//There's not a whole lot to set up here.
}

void doGOMUpdate()
{
	if(getRank(getGameScore()) == 0)
	{
		//We didn't place in the top 10, so no need to show this menu.
		setupGame();
		setMainState(MODE_MAIN_MENU);
		return;
	}
	//Draw the screen (this also sets the cursor in its proper position)
	drawGOM();
	//Set the console to allow input via typing
	DWORD normalInputMode;//Back up the old input mode so we can restore it when we're done.
	GetConsoleMode( getConsoleHandle(), &normalInputMode);
	DWORD newInputMode = ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT;
	SetConsoleMode( getConsoleHandle(), newInputMode);
	//Wait for input
	char* name = new char[21];
	DWORD numChars;
	ReadFile( getInputHandle(), name, 20, &numChars, NULL);
	name[numChars] = '\0';
	//Add the score to the chart
	putScore(getGameScore(), name);
	//Restore the previous input mode
	SetConsoleMode( getConsoleHandle(), normalInputMode);
	//Go back the the main menu.
	setupGame();
	setMainState(MODE_MAIN_MENU);
}

void drawGOM()
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
		COORD coord = {width/10, i};
		//We need to fill the screen with spaces
		FillConsoleOutputCharacter( getConsoleHandle(), ' ', (width*8)/10, coord, &out);
		//If we're at the top or bottom, we need to fill the entire row.
		if(i == 5 || i == height-6)
		{
			FillConsoleOutputAttribute( getConsoleHandle(), attr, (width*8)/10, coord, &out);
		}
		//Otherwise, only the two sides
		else
		{
			FillConsoleOutputAttribute( getConsoleHandle(), attr, 1, coord, &out);
			COORD coord2 = {(width/10)*9-1, i};
			FillConsoleOutputAttribute( getConsoleHandle(), attr, 1, coord2, &out);
			//make sure the center has the correct properties
			COORD coord3 = {width/10+1,  i};
			WORD centAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;//white text w/ black background
			FillConsoleOutputAttribute( getConsoleHandle(), centAttr, (width/10)*8-2, coord3, &out);
		}
	}
	//Draw the title and menu items
	DWORD numCharWritten;
	char* str = "GAME OVER";
	COORD strCoord = getCenteredStringCoord(str,width,7);
	SetConsoleCursorPosition( getConsoleHandle(), strCoord);
	WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
	WORD titleAttr = FOREGROUND_GREEN;//green text w/ black background
	FillConsoleOutputAttribute( getConsoleHandle(), titleAttr, strlen(str), strCoord, &numCharWritten);

	str = "Congratulations! You made the high score chart!";
	strCoord = getCenteredStringCoord(str,width,9);
	SetConsoleCursorPosition( getConsoleHandle(), strCoord);
	WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

	str = "Enter your name (max. 20 characters) and press \'Enter\'.";
	strCoord = getCenteredStringCoord(str,width,10);
	SetConsoleCursorPosition( getConsoleHandle(), strCoord);
	WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

	COORD cursCoord = {width/3+3, 13};
	SetConsoleCursorPosition( getConsoleHandle(), cursCoord);
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
