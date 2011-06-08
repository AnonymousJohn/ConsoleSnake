#include "ConsoleSnake.h"
#include <fstream>

//An entry into the high scores chart.
struct hs_entry
{
	unsigned int score;
	char* name; //This must be *deleted* before the hs_entry is replaced.
};

//Have we already read the high scores file?
bool hasReadHS = false;
//The list of high scores-- between program executions, this is saved in the file 'hs.bin'.
hs_entry* hsChart;
//Do we need to draw the entire screen?
bool hsNeedTotalDraw;

//Read the high scores file and parse it.
void readHS()
{
	//format: {int     short            char[]}
	//         score - length of name - name
	hsChart = new hs_entry[10];
	std::ifstream in;
	in.open("hs.bin", std::ios::in | std::ios::binary);
	if(in.bad() || in.fail())
	{
		//We couldn't open the file; this may be because it doesn't exist.
		//Make up entries. Go.
		for(int i = 0; i < 10; i++)
		{
			hsChart[i].name = new char[5];
			for(int k = 0; k < 4; k++)
			{
				hsChart[i].name[k] = '~';
			}
			hsChart[i].name[4] = '\0';
			hsChart[i].score = 0;
		}
	}
	else
	{
		//The file exists; assume, for simplicity's sake, that it
		//is properly formatted.
		for(int i = 0; i < 10; i++)
		{
			in.read((char*)(&(hsChart[i].score)),sizeof(unsigned int));
			short len;
			in.read((char*)(&len),sizeof(short));
			hsChart[i].name = new char[len+1];
			in.read(hsChart[i].name,len);
			hsChart[i].name[len] = '\0';
		}
	}
	hasReadHS = true;
}
//Save the high scores we have to the file.
void saveHS()
{
	//format: {int     short            char[]}
	//         score - length of name - name
	std::ofstream out;
	out.open("hs.bin",std::ios::out | std::ios::binary);
	for(int i = 0; i < 10; i++)
	{
		out.write((char*)(&(hsChart[i].score)), sizeof(unsigned int));
		short len = strlen(hsChart[i].name);
		out.write((char*)(&len), sizeof(short));
		out.write(hsChart[i].name, len);//note that this does not write the C-string ending.
	}
}

void setupHS()
{
	if(!hasReadHS)
	{
		readHS();
	}
	hsNeedTotalDraw = true;
}

void hsDoSelect()
{
	setMainState(MODE_MAIN_MENU);
	setupHS();//reset variables so that we redraw the next time we come to this menu.
}

void doHSUpdate()
{
	//Draw the screen
	drawHS();
	//Process input
	INPUT_RECORD inputBuffer[8];//We shouldn't need to process more than this at a time.
	DWORD numRecordsRead;
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
					hsDoSelect();
				}
			}
		}
	}
}

void drawHS()
{
	//We want to minimize drawing to save CPU. So, we ask ourselves:
	//Do we need to draw the entire screen?
	if(hsNeedTotalDraw)
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

		str = "High Scores";
		strCoord = getCenteredStringCoord(str,width,5);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		titleAttr = FOREGROUND_GREEN;//green text w/ black background
		FillConsoleOutputAttribute( getConsoleHandle(), titleAttr, strlen(str), strCoord, &numCharWritten);

		str = "Back to Main Menu";
		strCoord = getCenteredStringCoord(str,width,8);
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		strCoord.X -= 2;
		WORD selectedAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_BLUE;//white text w/ cyan background
		FillConsoleOutputAttribute( getConsoleHandle(), selectedAttr, strlen(str)+4, strCoord, &numCharWritten);

		//Write out the high scores.
		for(int i = 0; i < 10; i++)
		{
			COORD startCoord = {3, 10+i};
			SetConsoleCursorPosition( getConsoleHandle(), startCoord);
			str = gameUIntToCString((unsigned int) i+1);
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			delete[] str;
			if(i == 9)
			{
				str = " ";
			}
			else
			{
				str = "  ";
			}
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			str = gameUIntToCString(hsChart[i].score);
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			delete[] str;
			str = " - ";
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
			str = hsChart[i].name;
			WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
		}

		hsNeedTotalDraw = false;
	}
}

short getRank(unsigned int score)
{
	//loops through the high score charts until it finds one this score beats.
	for(int i = 0; i < 10; i++)
	{
		if(score >= hsChart[i].score)
		{
			return i+1;
		}
	}
	return 0;
}

void putScore(unsigned int score, char* name)
{
	//Get the ranking.
	short rank = getRank(score);
	//We don't want to do anything if the score doesn't make the chart.
	if(rank == 0)
	{
		return;
	}
	//Get rid of the string in the last entry so we don't have a memory leak.
	delete[] hsChart[9].name;
	//Work backwards to shift the entries down
	for(int i = 9; i >= rank; i--)
	{
		hsChart[i] = hsChart[i-1];
	}
	//Put the new entry in its proper space.
	hsChart[rank-1].name = name;
	hsChart[rank-1].score = score;
	//Save the high scores chart
	saveHS();
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
