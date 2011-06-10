//Implementation of Main Menu functions
#include "ConsoleSnake.h"
#include <fstream>

//Do we need to draw the entire screen?
bool smNeedTotalDraw;
//Do we need to draw part of the screen?
bool smNeedDraw;
//The current selected line
short smSelectedLine;
//The previously selected line (for drawing purposes)
short smLastLine;
//Have we read the settings file yet?
bool smHaveReadSettings = false;

//Settings
short speedSetting;


//Reads the settings file, 'settings.bin'
void readSettings()
{
	//Order:
	//byte - speed;
	std::ifstream in;
	in.open("settings.bin", std::ios::in | std::ios::binary);
	if(in.bad() || in.fail())
	{
		//We couldn't open the file; this may be because it doesn't exist.
		//Use default values
		speedSetting = SPEED_CURVED;
	}
	else
	{
		//The file exists; assume, for simplicity's sake, that it
		//is properly formatted.
		char speedTemp;
		in.read(&speedTemp, sizeof(char));
		speedSetting = speedTemp;
	}
	smHaveReadSettings = true;
}
//Saves the settings file, 'settings.bin'
void saveSettings()
{
	//Order:
	//byte - speed;
	std::ofstream out;
	out.open("settings.bin",std::ios::out | std::ios::binary);

	char speedTemp = (char) speedSetting;
	out.write( &speedTemp, sizeof(char));
}
//Does an action when the user hits 'enter'
void smDoSelect()
{
	//SELECTABLES:
	//0 - Back to Main Menu
	switch(smSelectedLine)
	{
	case 0:
		setMainState(MODE_MAIN_MENU);
		setupSM();//reset variables so that we redraw the next time we come to this menu.
		break;
	}
}
//Changes a setting when the user hits left or right
  //The parameter is true if the user clicked the left key, false if the user clicked the right key
void smDoSetChange(bool left)
{
	//SETTINGS:
	//1 - Speed
	switch(smSelectedLine)
	{
	case 1://Speed
		if(left)
		{
			if(speedSetting == SPEED_SLOW)
			{
				speedSetting = SPEED_CURVED;
			}
			else
			{
				speedSetting--;
			}
		}
		else
		{
			if(speedSetting == SPEED_CURVED)
			{
				speedSetting = SPEED_SLOW;
			}
			else
			{
				speedSetting++;
			}
		}
		smNeedDraw = true;
		saveSettings();
		break;
	}
}
void smScrollUp()
{
	smLastLine = smSelectedLine;
	if(smSelectedLine == 0)
	{
		smSelectedLine = 1;
	}
	else
	{
		smSelectedLine--;
	}
}
void smScrollDown()
{
	smLastLine = smSelectedLine;
	if(smSelectedLine == 1)
	{
		smSelectedLine = 0;
	}
	else
	{
		smSelectedLine++;
	}
}

void setupSM()
{
	if(!smHaveReadSettings)
	{
		readSettings();
	}
	smNeedTotalDraw = true;
	smNeedDraw = true;
	smSelectedLine = 0;
	smLastLine = -1;
}

void doSMUpdate()
{
	//Draw the screen
	drawSM();
	//Process input
    INPUT_RECORD inputBuffer[8];//We shouldn't need to process more than this at a time.
	DWORD numRecordsRead;
	 //We can use PeekConsoleInput() with the same parameters to check if there is
	 //any input available; it returns immediately if there is no available input.
	 //We don't need to here, though.
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
					smScrollUp();
					smNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 40 ||
					inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 83)//down or s
				{
					smScrollDown();
					smNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 37 ||
					inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 65)//left or a
				{
					smDoSetChange(true);
					smNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 39 ||
					inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 68)//right or d
				{
					smDoSetChange(false);
					smNeedDraw = true;
				}
				else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 13)//enter
				{
					smDoSelect();
				}
			}
			break;
		case MOUSE_EVENT:
			if(inputBuffer[i].Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
			{
				if((long)inputBuffer[i].Event.MouseEvent.dwButtonState > 0)
				{
					smScrollUp();
					smNeedDraw = true;
				}
				else
				{
					smScrollDown();
					smNeedDraw = true;
				}
			}
			break;
		}
	}
}

//Gets the string to display for a setting. You do not need to delete the string returned.
char* getSettingString(short settingID)
{
	//SETTINGS:
	//1 - Speed
	switch(settingID)
	{
	case 1://Speed
		switch(speedSetting)
		{
		case SPEED_SLOW:
			return "Slow";
		case SPEED_MEDIUM:
			return "Medium";
		case SPEED_FAST:
			return "Fast";
		case SPEED_CURVED:
			return "Curved";
		}
		break;
	}
}

void drawSM()
{
	//SELECTABLES:
	//0 - Back to Main Menu
	//SETTINGS:
	//1 - Speed

	//We want to minimize drawing. So, we ask ourselves:
	//Do we need to draw the entire screen?
	if(smNeedTotalDraw)
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

		str = "Settings";
		strCoord = getCenteredStringCoord(str,width,7);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		titleAttr = FOREGROUND_GREEN;//green text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), titleAttr, strlen(str), strCoord, &numCharWritten);

		str = "Back to Main Menu";
		strCoord = getCenteredStringCoord(str,width,9);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		WORD labelAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;//white text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), labelAttr, strlen(str), strCoord, &numCharWritten);

		str = "Speed:";
		strCoord = getCenteredStringCoord(str,width,12);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);

		str = getSettingString(1);//Speed
		strCoord = getCenteredStringCoord(str,width,13);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		WORD settingAttr = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;//yellow text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), settingAttr, strlen(str), strCoord, &numCharWritten);

		smNeedTotalDraw = false;
	}
	//Do we need to otherwise update the screen?
	if(smNeedDraw)
	{
		//Reset the highlighting of the now selected and previously selected lines
		 //Get width of the screen
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
		DWORD width = csbi.dwSize.X;
		 //Set up necessary variables
		char* str;
		COORD strCoord;
		DWORD numCharWritten;
		 //Set up text attributes
		WORD unselectAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;//white text w/ black background
		WORD unselectSettingAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;//yellow text w/ black background
		WORD selectAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN;//white text w/ cyan background
		WORD selectSettingAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN;//yellow text w/ cyan background
		
		switch(smLastLine)
		{
		case 0://Back to Main Menu
			str = "Back to Main Menu";
			strCoord = getCenteredStringCoord(str,width,9);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), unselectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 1://Speed
			//With settings, we need to get rid of the arrows on either side as well as the highlighting
			str = getSettingString(1);//Speed
			strCoord = getCenteredStringCoord(str,width,13);
			strCoord.X -= 4;
			COORD lnCoord = {1,13};
			FillConsoleOutputAttribute( getConsoleHandle(), unselectSettingAttr, width-2, lnCoord, &numCharWritten);
			short strLen = strlen(str);
			 //Write spaces over the arrows
			str = " ";
			strCoord.X += 2;
			SetConsoleCursorPosition( getConsoleHandle(), strCoord);
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			strCoord.X += 2+strLen+1;
			SetConsoleCursorPosition( getConsoleHandle(), strCoord);
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			break;
		}
		
		switch(smSelectedLine)
		{
		case 0://Back to Main Menu
			str = "Back to Main Menu";
			strCoord = getCenteredStringCoord(str,width,9);
			strCoord.X = strCoord.X - 2;
			FillConsoleOutputAttribute( getConsoleHandle(), selectAttr, strlen(str)+4, strCoord, &numCharWritten);
			break;
		case 1://Speed
			//With settings, we need to add the arrows on either side as well as the highlighting
			str = getSettingString(1);//Speed
			strCoord = getCenteredStringCoord(str,width,13);
			 //Rewrite the string, in case it changed
			  //Make sure that the last setting isn't on the screen
			COORD lnCoord = {0,13};
			FillConsoleOutputCharacter( getConsoleHandle(), ' ', width, lnCoord, &numCharWritten);
			SetConsoleCursorPosition( getConsoleHandle(), strCoord);
			  //Write the string
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			short strLen = strlen(str);
			 //Write arrows
			str = "\256";
			strCoord.X -= 2;
			SetConsoleCursorPosition( getConsoleHandle(), strCoord);
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			str = "\257";
			strCoord.X += 2 + strLen + 1;
			SetConsoleCursorPosition( getConsoleHandle(), strCoord);
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			 //Do the highlighting
			strCoord.X -= (1+strLen+4);
			FillConsoleOutputAttribute( getConsoleHandle(), selectSettingAttr, strLen+8, strCoord, &numCharWritten);
			break;
		}

		smNeedDraw = false;
	}
}

short getSpeedSetting()
{
	return speedSetting;
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
