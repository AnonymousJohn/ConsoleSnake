//This is where we define methods and datatypes specific to the Settings Menu screen

//Setting values
#define SPEED_SLOW   0
#define SPEED_MEDIUM 1
#define SPEED_FAST   2
#define SPEED_CURVED 3

//Set up what's necessary for the settings menu to work.
void setupSM();

//Check for any input from the user and respond to it.
void doSMUpdate();

//Draw the screen
void drawSM();

//Gets the setting defining speed of the snake
short getSpeedSetting();

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
