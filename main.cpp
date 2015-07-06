#include <ncurses.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <string>
#include <cstring>

#define MAX_GAMEBOARD_WIDTH 79
#define MAX_GAMEBOARD_HEIGHT 24
#define DEFAULT_GAMEBOARD_WIDTH 60
#define DEFAULT_GAMEBOARD_HEIGHT 18
#define MIN_GAMEBOARD_WIDTH 40
#define MIN_GAMEBOARD_HEIGHT 15
#define EASY_GAMEBOARD_WIDTH 70
#define EASY_GAMEBOARD_HEIGHT 20 
#define MED_GAMEBOARD_WIDTH 50
#define MED_GAMEBOARD_HEIGHT 17
#define HARD_GAMEBOARD_WIDTH 40
#define HARD_GAMEBOARD_HEIGHT 16
#define SUPERHARD_GAMEBOARD_WIDTH 30
#define SUPERHARD_GAMEBOARD_HEIGHT 15

using namespace std;

int startCurses(bool&);
void endCurses(bool);
void getPlayerName(string&);
void introScreen(int&, int&, int&, int&);
void playerSelection(char&);
void initializeGameBoard(char**, int&, int&);
void startGame(char**, int&, int&);
void setGameDifficulty(char**,int&,int&, bool&);
void viewPlayerStats();
void invalidInput(const string&);
void endGame();
void outlineScreen();
void difficultyScreen();
void continuePrevGame(); // this is temp
void destroyGameBoard(char**, int&, int&, bool&);
void printGameScreen();
void checkIfGBInit(char**, bool);
void getScreenDimensions(bool); /*
									use this function if you want to know how many rows &
									cols your terminal has.
								*/

int main(int argc, char*argv[]){

	bool cursesStarted = true;
	bool endTetris = false;
	bool gameStarted = false;
	bool gameBoardInit = false;

	int xPos = 0, yPos = 0;
	int rows = 0, cols = 0;
	int xOffset = 7, yOffset = 0;
	
	char** gameBoard;
	char* userInput = new char[10];
	char* uIHomePtr = userInput;
	
	string promptMessage = "Please Enter a Valid Selection:";
	string temp;

	userInput[0] = '\0';

	int terminalColor = startCurses(cursesStarted);

	if(terminalColor == -1){
		endCurses(cursesStarted);
		return -1;
	}

	do{
		introScreen(yPos, xPos, yOffset, xOffset);
		yPos = 14;

		if(userInput[0] == '\0'){
			yPos = 0;
			mvprintw(yPos + yOffset, xPos + xOffset, promptMessage.c_str());
		}

		else{
			introScreen(yPos, xPos, yOffset, xOffset);
			refresh();
			yPos = 0;
			mvprintw(yPos + yOffset, xPos + xOffset, promptMessage.c_str());
		}

		move(yPos + yOffset, xPos + xOffset + strlen(promptMessage.c_str()) + 1);

		wgetstr(stdscr, userInput);

		temp = userInput;

		switch(temp[0]){

			case 'P':
			case 'p':{
					gameStarted = true;
					checkIfGBInit(gameBoard, gameBoardInit);
					startGame(gameBoard, rows, cols);
					break;
				}

			case 'A':
			case 'a':{
					setGameDifficulty(gameBoard, rows, cols, gameBoardInit);
					gameBoardInit = true;
					break;
				}

			case 'V':
			case 'v':{
					viewPlayerStats();
					break;
				}

			case 'C':
			case 'c':{
					continuePrevGame();
					break;
				}

			case 'Q':
			case 'q':{
					endGame();
					destroyGameBoard(gameBoard, rows, cols, gameStarted);
					endCurses(cursesStarted);
					endTetris = true;
					break;
				}

			default:{
				invalidInput(temp);
				break;
			}

		}

		userInput = uIHomePtr;

	} while((temp[0] != 'q' || temp[0] != 'Q') && !endTetris);

	delete [] userInput; userInput = NULL;

	return 0;
}

int startCurses(bool& startMode){
	if(!startMode){
	}
	else{
		initscr(); // initialize terminal in curses mode
		raw(); /* 
					raw() and cbreak() both disable line buffering, however, raw() passes
					control characters (CTRL - C) to the program with out generating a 
					signal while cbreak() does generate a signal to the program
				*/
		echo(); // displays user-input characters to the screen
		keypad(stdscr, TRUE); // enables function keys (F1, F2), arrow keys, and more
		startMode = true;
	}

	try{

		if(!has_colors()){
			throw -1;
		}
		else{
			/*start_color(); // start color in terminal
			init_pair(1, COLOR_RED, COLOR_BLACK);

			attron(COLOR_PAIR(A_REVERSE));
			attroff(COLOR_PAIR(A_REVERSE));*/
		}

	}

	catch(int colorNotSupported){
		mvprintw(LINES-1/2, COLS-1/2, "ERROR: UBUNTU 14.04 LTS DOES NOT SUPPORT COLOR FOR THE NCURSES STDSCR WINDOW");
		return - 1;
	}
	return 0;
}
void endCurses(bool hasCursesStarted){
	if(hasCursesStarted && !isendwin()){
		getch();
		endwin(); // endcurses
	}
	else{
		if(hasCursesStarted)
			printf("%s\n", "ERROR: Cannot End Curses Because It Has Not Already Been Initialized.");
		else
			printf("%s\n", "ERROR: Cannot End Curses Because It Has Already Ended.");
	}

}


void introScreen(int& yPos, int& xPos, int& yOffset, int& xOffset){
	int tY = 0, tX = 0, tYO = 0, tXO = 0;
	xPos = 0, yPos = 0, yOffset = 0, xOffset = 0;
	string welcomeMessage = "Welcome To Tetris";
	bool menuDisplayed = false;

	for(int i = 0; i < COLS - 1; i++){
		mvprintw(yPos, xPos++, "*");
	}

	yPos++;
	xPos = 0;

	mvprintw(yPos, xPos, "*");

	xPos = ((COLS - 1 ) / 2) - strlen(welcomeMessage.c_str());
	
	mvprintw(yPos, xPos, welcomeMessage.c_str());

	xPos = COLS - 1;

	mvprintw(yPos, xPos, "*");

	yPos++;
	xPos = 0;

	for(int i = 0; i < COLS - 1; i++){
		mvprintw(yPos, xPos++, "*");
	}

	yPos++;
	xPos = 0;

	xOffset = ((COLS - 1) / 2 ) - 20;
	yOffset = ((LINES - 1) / 2) - 20;

	for(int i = 0; i < LINES - 1; i++){
		if(yPos == (LINES - 1)/2 && !menuDisplayed){
			
			mvprintw(yOffset, xPos, "*");
			mvprintw(yOffset, xPos + xOffset, "MAIN MENU");
			mvprintw(yOffset, xPos + COLS - 1, "*");

			for(int i = 0; i < 2; i++){
				mvprintw(++yOffset, xPos, "*");
				mvprintw(yOffset, xPos + COLS - 1, "*");
			}

			mvprintw(++yOffset, xPos, "*");
			mvprintw(yOffset, xPos + xOffset, "1) Press (p) To Play Tetris");
			mvprintw(yOffset, xPos + COLS - 1, "*");

			for(int i = 0; i < 2; i++){
				mvprintw(++yOffset, xPos, "*");
				mvprintw(yOffset, xPos + COLS - 1, "*");
			}

			mvprintw(++yOffset, xPos, "*");
			mvprintw(yOffset, xPos + xOffset, "2) Press (v) To View The Top Ten Player Scores");
			mvprintw(yOffset, xPos + COLS -1, "*");

			for(int i = 0; i < 2; i++){
				mvprintw(++yOffset, xPos, "*");
				mvprintw(yOffset, xPos + COLS - 1, "*");
			}

			mvprintw(++yOffset,xPos, "*");
			mvprintw(yOffset, xPos + xOffset, "3) Press (a) To Adjust The Game Difficulty");
			mvprintw(yOffset, xPos + COLS - 1, "*");

			for(int i = 0; i < 2; i++){
				mvprintw(++yOffset, xPos, "*");
				mvprintw(yOffset, xPos + COLS - 1, "*");
			}

			mvprintw(++yOffset, xPos, "*");
			mvprintw(yOffset, xPos + xOffset, "4) Press (c) To Continue Previous Game");
			mvprintw(yOffset, xPos + COLS - 1, "*");

			for(int i = 0; i < 2; i++){
				mvprintw(++yOffset, xPos, "*");
				mvprintw(yOffset, xPos + COLS - 1, "*");
			}
			
			mvprintw(++yOffset, xPos, "*");
			mvprintw(yOffset, xPos + xOffset, "5) Press (q) To Leave Tetris");
			mvprintw(yOffset, xPos + COLS - 1, "*");
			
			tYO = yOffset + 3;
			tXO = xOffset;
			tY = yPos;
			tX = xPos;
			
			menuDisplayed = true;
			
		}
		else{
			mvprintw(yPos, xPos, "*");
			xPos = COLS - 1;
			mvprintw(yPos, xPos, "*");
			yPos++;
			xPos = 0;
		}

	}

	yPos = LINES - 1;

	for(int i = 0; i < COLS - 1; i++){
		mvprintw(yPos, xPos++, "*");
	}

	refresh();

	yPos = tY; xPos = tX;
	yOffset= tYO; xOffset = tXO;

}
void initializeGameBoard(char** gameBoard, int& rows, int& cols){

	if(rows < MIN_GAMEBOARD_HEIGHT || rows > MAX_GAMEBOARD_HEIGHT)
		rows = DEFAULT_GAMEBOARD_HEIGHT;

	if(cols < MIN_GAMEBOARD_WIDTH || cols > MAX_GAMEBOARD_WIDTH)
		cols = DEFAULT_GAMEBOARD_WIDTH;

	gameBoard = new char*[rows];

	for (int i = 0; i < rows; ++i){
		gameBoard[i] = new char [cols];
		
		for(int j = 0; j < cols; j++){
			gameBoard[i][j] = 'e'; // indicate that this current coordinate is empty
		}
	}
}

void startGame(char** gameBoard, int& rows, int& cols){
	
	erase(); // unlike clear(), erase fills the terminal with blank spaces
	outlineScreen();
	getch();
	
}
void setGameDifficulty(char** gameBoard, int& rows, int& cols, bool& gbHasBeenInit){

	int yPos = 0, xPos = 0;

	yPos = ((LINES - 1)/2) - 8;

	bool hasTetrisStarted = true;

	
	char* userInput = new char[10];
	bool validInput = false;

	string msg = "Enter difficulty level here: ";
	string option;

	clear();

	

	do{
		outlineScreen();

		difficultyScreen();

		xPos = ((COLS - 1)/2) - strlen(msg.c_str()) + 9;

		mvprintw(yPos, xPos, msg.c_str());

		xPos = ((COLS - 1)/2) + 9;

		move(yPos, xPos);

		wgetstr(stdscr, userInput);

		option = userInput;

		if(gbHasBeenInit){
			destroyGameBoard(gameBoard, rows, cols, hasTetrisStarted); // true = game has started
			gbHasBeenInit = false;
		}
		switch(option[0]){
			case 'e':
			case 'E':
				rows = EASY_GAMEBOARD_HEIGHT;
				cols = EASY_GAMEBOARD_WIDTH;
				initializeGameBoard(gameBoard, rows, cols);
				gbHasBeenInit = true;
				validInput = true;
				break;
			case 'm':
			case 'M':
				rows = MED_GAMEBOARD_HEIGHT;
				cols = MED_GAMEBOARD_WIDTH;
				initializeGameBoard(gameBoard, rows, cols);
				gbHasBeenInit = true;
				validInput = true;
				break;

			case 'h':
			case 'H':
				rows = HARD_GAMEBOARD_HEIGHT;
				cols = HARD_GAMEBOARD_WIDTH;
				initializeGameBoard(gameBoard, rows, cols);
				gbHasBeenInit = true;
				validInput = true;
				break;

			case 's':
			case 'S':
				rows = SUPERHARD_GAMEBOARD_HEIGHT;
				cols = SUPERHARD_GAMEBOARD_WIDTH;
				initializeGameBoard(gameBoard, rows, cols);
				gbHasBeenInit = true;
				validInput = true;
				break;

			default:
				invalidInput(option);
				break;
		}
	} while(!validInput);

	delete [] userInput;

	clear();
}

void viewPlayerStats(){

}
void endGame(){
	//endCurses(true);	
	return;
}
void invalidInput(const string& userInput){
	int xPos = 0, yPos = 0;

	clear();
	string pre = "ERROR: \'";
	string post = "\' is not a valid selection";
	string error = pre + userInput + post;

	outlineScreen();

	yPos = (LINES - 1) / 2;
	xPos = ((COLS - 1) / 2) - strlen(error.c_str());


	mvprintw(yPos, xPos , error.c_str());
	mvprintw(++yPos, xPos, "Please Enter A Valid Selection This Time.");
	mvprintw(++yPos, xPos, "Press Any Key To Return To The Main Menu...");
	getch();
	clear();
	return;
}
void outlineScreen(){
	
	int xPos = 0, yPos = 0;

	for(int i = 0; i < COLS - 1 ; i++){
		mvprintw(yPos, xPos++, "*");
	}

	yPos++;
	xPos = 0;

	for(int j = 0; j < LINES - 1; j++){
		mvprintw(yPos, xPos, "*");
		xPos = COLS - 1;
		mvprintw(yPos++, xPos, "*");
		xPos = 0;
	}

	yPos = LINES - 1;
	xPos = 0;

	for(int i = 0; i < COLS - 1 ; i++){
		mvprintw(yPos, xPos++, "*");
	}
}
void difficultyScreen(){
	int yPos = 0, xPos = 0;

	yPos = ((LINES - 1)/2) - 20;
	xPos = ((COLS - 1)/2) - 20;

	mvprintw(yPos, xPos, "Set Difficulty");
	mvprintw(yPos + 2, xPos, "1) Press (e) For Easy");
	mvprintw(yPos + 4, xPos, "2) Press (m) For Medium");
	mvprintw(yPos + 6, xPos, "3) Press (h) For Hard");
	mvprintw(yPos + 8, xPos, "4) Press (s) For Super Hard");
}

void continuePrevGame(){

}
void destroyGameBoard(char** gameBoard, int& rows, int& cols, bool& hasGameStarted){
	
	try{
		if(!hasGameStarted){
			throw - 1;
		}
		else{
			for(int i = 0; i < rows; i++){
				delete [] gameBoard[i];
				delete [] gameBoard;
			}
		}
	}
	catch(int PREVENT_DOUBLE_FREE_CORRUPTION){
		return;
	}
}
void checkIfGBInit(char** gameBoard, bool gbInit){
	try{
		if(!gbInit){
			throw - 1;
		}
	}
	catch(int PREVENT_MEMORY_LEAK){

		gameBoard = new char*[DEFAULT_GAMEBOARD_HEIGHT];

		for (int i = 0; i < DEFAULT_GAMEBOARD_HEIGHT; ++i){
			gameBoard[i] = new char [DEFAULT_GAMEBOARD_WIDTH];
			
			for(int j = 0; j < DEFAULT_GAMEBOARD_WIDTH; j++){
				gameBoard[i][j] = 'e'; // indicate that this current coordinate is empty
			}
		}
	}
}
void printGameScreen(){

}
void getScreenDimensions(bool cursesStarted){
	int rows = 0, cols = 0;
	if (!cursesStarted){
		mvprintw(DEFAULT_GAMEBOARD_HEIGHT, DEFAULT_GAMEBOARD_WIDTH, "ERROR: curses has not been started.");
		return;
	}
	else{
		getmaxyx(stdscr, rows, cols);
		mvprintw((rows - 1)/2, ((cols - 1)/2) - 20, "This screen has %d rows and %d columns", rows, cols);
		timeout(3*1000);
		getch();
		refresh();
		clear();
	}
}
