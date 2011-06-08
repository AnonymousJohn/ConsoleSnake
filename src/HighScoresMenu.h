//This is where we define methods and datatypes specific to the high scores

//Set up what's necessary for the chart to work.
void setupHS();

//Check for any input from the user and respond to it.
void doHSUpdate();

//Draw the screen
void drawHS();

//Gets what rank this score would be if entered into the high score charts. Returns 0 if the score would not make the top 10.
short getRank(unsigned int score);

//Adds this score and name to the high score chart and saves the chart.
void putScore(unsigned int score, char* name);

//Copyright 2011 Jonathan Dahm aka "AnonymousJohn"
