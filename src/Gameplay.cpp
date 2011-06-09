//Implementation of Gameplay functions
#include "ConsoleSnake.h" //include all functions so we can access other functions
#include <string.h>
#include <ctime>
#include <cmath>

#define DIR_NORTH 0
#define DIR_EAST  1
#define DIR_SOUTH 2
#define DIR_WEST  3

//A structure representing a segment of the snake
//struct snake_segment;//We declare it here so we can use it in the struct itself.
struct snake_segment
{
	int Xpos;
	int Ypos;
	BYTE locationOfNextSeg;//this is simply for the drawing mechanism, so that it does not have to redraw the entire screen every time
	snake_segment* nextSeg;
};

//The snake is comprised of multiple 'snake_segment's, with the head of the snake
//being the first segment and the tail being the last one. You traverse the segments
//from head to tail
snake_segment* gameHead = NULL;
//Length of the snake; used during updating to find the segment of the snake to cut off.
short gameSnakeLength = 0;
//Direction the snake is currently facing
BYTE gameSnakeDirection = DIR_NORTH;
//The last time we did an update; used to help keep the game from overclocking.
DWORD gameLastTick;
//Do we need to redraw the entire screen?
bool gameNeedTotalDraw;
//Score of the game
unsigned int gameScore;
//Number of apples we've eaten so far
unsigned short gameNumApples;
//Location of the target apple.
COORD* gameAppleLoc = NULL;


//Delete the given snake segment (and all segments that follow it).
void gameDeleteSnakeSeg(snake_segment* seg)
{
	//Because we don't want any memory leaks, we need
	//to make sure we've deleted the other snake segments
	//attached to this one.

	//We've reached the end of the snake if the segment
	//given is null.
	if(seg == NULL)
	{
		return;
	}
	//Delete the chain attached to this segment (if there is one) first,
	gameDeleteSnakeSeg(seg->nextSeg);
	//then delete the segment given to us.
	delete seg;
}
//Creates a new snake. Does not delete the current snake! gameHead must be NULL for this to work properly.
void gameCreateNewSnake()
{
	//We want the user to start with 5 snake segments.
	//We also want the snake to start as a dot and extend from its starting position.
	//The simplist way to do this is to have all the snake segments start in the
	//same position and let the game logic take its normal course; this will automatically
	//extend the snake out to its full length while appearing to start as a dot.

	//Get the width of the screen
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
	DWORD width = csbi.dwSize.X;
	DWORD height = 25;//use a constant

	//Start with the tail and build backwards.
	for(int i = 0; i < 5; i++)
	{
		snake_segment* last = gameHead;
		gameHead = new snake_segment();
		gameHead->nextSeg = last;
		gameHead->locationOfNextSeg = DIR_SOUTH;
		//We want the snake to start in the center of the screen.
		gameHead->Xpos = width/2;
		gameHead->Ypos = height/2;
	}
}
//Sets the direction of the snake
void gameSetDirection(BYTE dir)
{
	gameSnakeDirection = dir;
}
//Pauses the game
void gamePause()
{
	//When we pause, we're really bringing up a menu. To keep things nice, neat and organized,
	//I'll put that menu in its own state in the main state machine and put the code into a
	//different header/cpp file.

	gameNeedTotalDraw = true;//we don't want to reset the game just yet; we want to be able to return to it.
	setMainState(MODE_PAUSE_MENU);
}
//Called when we hit ourselves/a wall
void gameOver()
{
	//You see what I did with the name there? Yeah. Me too. *ahem*
	//Like the pause menu, the game over menu is its own state in the state machine.
	//This is to keep things nice and organized.
	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
	DWORD width = csbi.dwSize.X;
	DWORD height = 25;//use a preset value
	//Display 'GAME OVER' for a couple seconds before proceeding to the game over screen.
	
	DWORD numCharWritten;
	char* str = "GAME OVER";
	COORD strCoord = getCenteredStringCoord(str,width,height/2);
	SetConsoleCursorPosition( getConsoleHandle(), strCoord);
	WriteFile( getConsoleHandle(), str, strlen(str), &numCharWritten, NULL);
	Sleep(3000);

	gameNeedTotalDraw = true;//We don't want to reset the game just yet; we want to hold onto the score.
	setMainState(MODE_GAME_OVER_MENU);
}
//Places a new apple on the field, replacing the old one if there was one.
void gameMakeNewApple()
{
	//Place a new apple randomly on the field, making sure not to place it on the snake or the walls.
	
	//Get the width of the screen
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
	DWORD width = csbi.dwSize.X - 2;
	DWORD height = 25 - 2;//use a constant

	while(true)
	{
		//Generate a random position
		int x = (rand() % width) + 1;
		int y = (rand() % height) + 1;
		//Check if we've chosen a location on top of a snake segment. If so, we need to retry.
		bool onTopOfSnake = false;
		snake_segment* seg = gameHead;
		while(seg != NULL)
		{
			if(seg->Xpos == x && seg->Ypos == y)
			{
				//We're on top of a snake segment.
				onTopOfSnake = true;
				break;
			}
			seg = seg->nextSeg;
		}
		if(onTopOfSnake)
		{
			//retry.
			continue;
		}
		//We weren't on top of a snake. Set the apple's position
		if(gameAppleLoc != NULL)
		{
			delete gameAppleLoc;
		}
		gameAppleLoc = new COORD();
		gameAppleLoc->X = x;
		gameAppleLoc->Y = y;
		break;
	}
}


void setupGame()
{
	//Delete the previous snake, and create a new one.
	gameDeleteSnakeSeg( gameHead);
	gameHead = NULL;
	gameCreateNewSnake();
	gameSnakeLength = 5;
	gameSnakeDirection = DIR_NORTH;
	gameLastTick = GetTickCount();
	gameNeedTotalDraw = true;
	gameScore = 0;
	gameNumApples = 0;
	gameMakeNewApple();
}

void doGameUpdate()
{
	//We don't want to update every time. Wait for a certain amount of time.
	int diff = 250;//Default to 250
	switch( getSpeedSetting())
	{
	case SPEED_SLOW:
		break;//Leave at 250
	case SPEED_MEDIUM:
		diff = 150;//Take the speed up a notch
		break;
	case SPEED_FAST:
		diff = 50;//Full speed ahead!
		break;
	case SPEED_CURVED:
		//Formula time!
		diff = (int)(200*pow(1.25, -gameNumApples) + 50);
		break;
	}
	DWORD elapsedTime = GetTickCount() - gameLastTick;
	if( elapsedTime >= diff)
	{
		//Process input
		INPUT_RECORD inputBuffer[64];//We shouldn't need to process more than this at a time.
		DWORD numRecordsRead;
		 //Check if we have any input to handle
		PeekConsoleInput( getInputHandle(), inputBuffer, 64, &numRecordsRead);
		if(numRecordsRead > (DWORD)0)
		{
			//There was input to process; let's get it.
			ReadConsoleInput( getInputHandle(), inputBuffer, 64, &numRecordsRead);
			for(int i = 0; i < numRecordsRead; i++)
			{
				//Controls (reminder):
				// W/Up    - Face up (north)
				// D/Right - Face right (east)
				// S/Down  - Face down (south)
				// A/Left  - Face left (west)
				// P       - Pause game (brings up a pause menu)
				if(inputBuffer[i].EventType == KEY_EVENT)
				{
					if(inputBuffer[i].Event.KeyEvent.bKeyDown)
					{
						if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 38 ||
							inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 87)//up or w
						{
							gameSetDirection(DIR_NORTH);
						}
						else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 39 ||
							inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 68)//right or d
						{
							gameSetDirection(DIR_EAST);
						}
						else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 40 ||
							inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 83)//down or s
						{
							gameSetDirection(DIR_SOUTH);
						}
						else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 37 ||
							inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 65)//left or a
						{
							gameSetDirection(DIR_WEST);
						}
						else if(inputBuffer[i].Event.KeyEvent.wVirtualKeyCode == 80)//p
						{
							gamePause();
						}
					}//end if(was the key pressed?)
				}//end if(was it a key event?)
			}//end for (loop through input)
		}//end if (do we have input?)

		//Do game logic
		 //First we update the snake's head's location and lop off the tail.
		  //Update the head
		snake_segment* newHead = new snake_segment();
		if(gameSnakeDirection == DIR_NORTH)
		{
			newHead->Xpos = gameHead->Xpos;
			newHead->Ypos = gameHead->Ypos - 1;
			newHead->locationOfNextSeg = DIR_SOUTH;
		}
		else if(gameSnakeDirection == DIR_SOUTH)
		{
			newHead->Xpos = gameHead->Xpos;
			newHead->Ypos = gameHead->Ypos + 1;
			newHead->locationOfNextSeg = DIR_NORTH;
		}
		else if(gameSnakeDirection == DIR_EAST)
		{
			newHead->Xpos = gameHead->Xpos + 1;
			newHead->Ypos = gameHead->Ypos;
			newHead->locationOfNextSeg = DIR_WEST;
		}
		else if(gameSnakeDirection == DIR_WEST)
		{
			newHead->Xpos = gameHead->Xpos - 1;
			newHead->Ypos = gameHead->Ypos;
			newHead->locationOfNextSeg = DIR_EAST;
		}
		newHead->nextSeg = gameHead;
		gameHead = newHead;
		  //Lop off the tail
		   //Considering we now have an extra segment, this
		   //should land us on the second-to-last segment.
		snake_segment* seg = gameHead;
		for(int i = 1; i < gameSnakeLength; i++)
		{
			seg = seg->nextSeg;
		}
		gameDeleteSnakeSeg(seg->nextSeg);
		seg->nextSeg = NULL;//Just to keep from having a dangling pointer.
		 //Next, we check to see if we've hit the apple.
		if(gameHead->Xpos == gameAppleLoc->X && gameHead->Ypos == gameAppleLoc->Y)
		{
			//We got the apple! Now we need to increase size and make a new apple.
			gameSnakeLength++; //We don't need to physically add another segment; the normal move logic will do this for us.
			gameMakeNewApple();
			//Add to the score, depending on the speed
			switch( getSpeedSetting())
			{
			case SPEED_SLOW:
				gameScore += 25;
				break;
			case SPEED_MEDIUM:
				gameScore += 50;
				break;
			case SPEED_FAST:
				gameScore += 100;
				break;
			case SPEED_CURVED:
				//Formula time! The score will curve with the speed.
				double x = 200*pow(1.25, -gameNumApples) + 50;
				gameScore += (int)(0.00125 * pow(x,2) - 0.75 * x + 134.375);
				break;
			}
			gameNumApples++;
		}
		 //If we didn't hit the apple, we could've hit the walls or ourself.
		else
		{
			//Wall check
			 //Get the width of the screen
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo( getConsoleHandle(), &csbi );
			DWORD width = csbi.dwSize.X;
			DWORD height = 25;//use a constant
			if(gameHead->Xpos == 0 || gameHead->Xpos == width-1 || 
				gameHead->Ypos == 0 || gameHead->Ypos == height-1)
			{
				//Oops! We ran into the wall. Game over!
				gameOver();
			}
			else{
				//Self check
				snake_segment* seg = gameHead->nextSeg;
				bool wasCollision = false;
				while(seg != NULL)
				{
					if(seg->Xpos == gameHead->Xpos && seg->Ypos == gameHead->Ypos)
					{
						//We've collided with ourself. Oops! Game over.
						wasCollision = true;
						gameOver();
					}
					seg = seg->nextSeg;
				}
			}
		}
		//Update screen
		drawGame();

		gameLastTick = GetTickCount();
	}
}

void drawGame()
{
	//We want to minimize drawing as much as possible; so we ask ourselves:
	//Do we need to draw the entire screen?
	if(gameNeedTotalDraw)
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
		//Draw the apple
		if(gameAppleLoc != NULL)
		{
			WORD appleAttr = FOREGROUND_RED | BACKGROUND_RED; //completely red.
			FillConsoleOutputAttribute( getConsoleHandle(), appleAttr, 1, *gameAppleLoc, &out);
		}
		//Draw the entire snake
		WORD snakeAttr = FOREGROUND_GREEN | BACKGROUND_GREEN; //completely green.
		snake_segment* seg = gameHead;
		while(seg != NULL)
		{
			COORD* segCoord = new COORD();
			segCoord->X = seg->Xpos;
			segCoord->Y = seg->Ypos;
			FillConsoleOutputAttribute( getConsoleHandle(), snakeAttr, 1, *segCoord, &out);
			delete segCoord;
			seg = seg->nextSeg;
		}
		//Draw the score
		char* str = "Score: ";
		COORD strCoord = {1, height-1};
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &out, NULL);
		str = gameUIntToCString(gameScore);
		WriteFile( getConsoleHandle(), str, strlen(str), &out, NULL);
		delete[] str;

		gameNeedTotalDraw = false;
	}
	else
	{
		DWORD out;
		//Draw the apple
		if(gameAppleLoc != NULL)
		{
			WORD appleAttr = FOREGROUND_RED | BACKGROUND_RED; //completely red.
			FillConsoleOutputAttribute( getConsoleHandle(), appleAttr, 1, *gameAppleLoc, &out);
		}
		//Draw the snake; but really, erase where the tail was previously, and draw where the head is now.
		WORD normAttr = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;//white text w/ black background
		 //Get the coordinate of the last tail position
		snake_segment* seg = gameHead;
		while(seg->nextSeg != NULL)
		{
			seg = seg->nextSeg;
		}
		COORD tailCoord;
		switch(seg->locationOfNextSeg)
		{
		case DIR_NORTH:
			tailCoord.X = seg->Xpos;
			tailCoord.Y = seg->Ypos-1;
			break;
		case DIR_EAST:
			tailCoord.X = seg->Xpos+1;
			tailCoord.Y = seg->Ypos;
			break;
		case DIR_SOUTH:
			tailCoord.X = seg->Xpos;
			tailCoord.Y = seg->Ypos+1;
			break;
		case DIR_WEST:
			tailCoord.X = seg->Xpos-1;
			tailCoord.Y = seg->Ypos;
			break;
		}
		FillConsoleOutputAttribute( getConsoleHandle(), normAttr, 1, tailCoord, &out);
		 //Get the coordinate of the new head position
		COORD headCoord = { gameHead->Xpos, gameHead->Ypos};
		WORD snakeAttr = FOREGROUND_GREEN | BACKGROUND_GREEN; //completely green.
		FillConsoleOutputAttribute( getConsoleHandle(), snakeAttr, 1, headCoord, &out);
		//Draw the score
		//Draw the score
		char* str = "Score: ";
		COORD strCoord = {1, 25-1};
		SetConsoleCursorPosition( getConsoleHandle(), strCoord);
		WriteFile( getConsoleHandle(), str, strlen(str), &out, NULL);
		str = gameUIntToCString(gameScore);
		WriteFile( getConsoleHandle(), str, strlen(str), &out, NULL);
		delete[] str;
	}
}

unsigned int getGameScore()
{
	return gameScore;
}

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
