#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "Trie.h"
#include "Grid.h"
#include "Level.h"
#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
using namespace std;
Trie allCommands;
string blocksFileName = "sequence.txt";
string norandomBlockSequence = "";
int norandomSequenceIndex = 0;
bool textOnly = false;
bool extraFeatureMacro = false;
vector<pair<string, string>> macroCommands;
Grid *g;
Level *currLevel = new Level0;
int startLevel = 0;

int main(int argc, char *argv[]){
	// declaring helper functions
	void ReadCommands(istream &in);

	allCommands.Insert("LEFT RIGHT DOWN DROP LEVELUP LEVELDOWN NORANDOM \
		RANDOM CLOCKWISE COUNTERCLOCKWISE I J L O T S Z RESTART HINT SEQUENCE MACRO");
	
	pair<int, int> gridDimensions(18, 11);

	// Checking command line arguments
	for(int i=1; i<argc; ++i){
		string cmdLineArgument = string(argv[i]);
		if(cmdLineArgument == "-text"){
			textOnly = true;
		}
		else if(cmdLineArgument == "-scriptfile"){
			i += 1;
			blocksFileName = argv[i];
		}
		else if(cmdLineArgument == "-seed"){
			int seed;
			i += 1;
			stringstream tempIss(argv[i]);
			if(tempIss >> seed){
				srand(seed);
			}
		}
		else if(cmdLineArgument == "-startlevel"){
			i += 1;
			stringstream tempIss(argv[i]);
			if(tempIss >> startLevel){
				for (int j = 0; j < startLevel; ++j) {
					currLevel = PlusPlus(currLevel);
				}
			}
		}
		else if(cmdLineArgument == "-extraFeatureMacro"){
			extraFeatureMacro = true;
		}
		else if(cmdLineArgument == "-extraFeatureGrid"){
			try{
				i += 1; int row, col;
				stringstream rowIss(argv[i]);
				rowIss >> row;
				i += 1;
				stringstream colIss(argv[i]);
				colIss >> col;
				if(row >=6 && col >= 6){
					gridDimensions.first = row;
					gridDimensions.second = col;
				}
			}
			catch(...){
				cout << "Usage: ./quadris -extraFeatureGrid row col (row, col >=6)" << endl;
			}
			
		}
		else{
			cout << argv[i] << " not valid command-line argument";
			cout << "Usage:\n-text (for only text display)\n";
			cout << "-seed xxx (sets the random number generator’s seed to xxx)\n";
			cout << "-scriptfile xxx (uses file xxx  as a source of blocks for level 0)\n";
			cout << "-startlevel n (Starts the game in level n)\n" ;
			cout << "-extraFeatureMacro (use extra feature macro)\n";
			cout << "-extraFeatureMacro row col (use extra feature gridDimensions)\n" << endl;
		}
	}

	g = new Grid(gridDimensions.first, gridDimensions.second);

	g->SetShapes(' ', currLevel->ChooseShape());

	g->CreateNewShape(currLevel->ChooseShape(), currLevel->CurrentLevel());

	cout << *g;

	ReadCommands(cin);

	delete currLevel;

	return 0;	
}


// Takes in a string and removes the digits at the beginning of the string
// and converts it to lower case.
void GetCommand(string &command){
	// Finding index where command actually starts
	int commandStartIndex = 0;
	for(commandStartIndex; command[commandStartIndex] != '\0'; ++commandStartIndex){
		if(command[commandStartIndex] > 57 || // If command[commandStartIndex] isn't a 
			command[commandStartIndex] < 48){ // digit, then break
			break;
		}
	}

	// Removing the digits at the beginning
	command = command.substr(commandStartIndex);

	// Convert String to upper case.
	for(int i=0; i<command.length(); ++i)
		if(command[i]<='z' && command[i]>='a')
    		command[i] = command[i]+('Z'-'z');
}


// Reads in commands (from stdin/file) and calls InterpretCommands on each command
void ReadCommands(istream &in){
	// declaring helper funcitons
	int FindCommandStartIndex(string &command);
	void InterpretCommand(const string &command);
	void Restart();


	Grid nextGrid;
	
	nextGrid = (*g);

	nextGrid.TranslateWindowRight();
	cout << nextGrid;

	cout << "\n\n now original grid: \n\n";
	cout << *g;

	// string command;
	// while(in >> command){
	// 	stringstream iss(command);
	// 	int numberOfTimes;
	// 	iss >> numberOfTimes;
	// 	if(numberOfTimes < 0) // negative numbers aren't allowed
	// 		continue;
	// 	else if(numberOfTimes == 0 && command[0] != '0') // if there is no number, 
	// 		numberOfTimes += 1; // command should run once

	// 	GetCommand(command); 
	// 	// command does not have the number at the beginning now

	// 	command = allCommands.Find(command); 
	// 	// Command is now a complete command or "" 
	// 	if(command == ""){
	// 		cout << "Command not found." <<endl;
	// 		continue;
	// 	}
	// 	cout << command << endl;

	// 	// If command is sequence, then ReadCommands from file
	// 	if(command == "SEQUENCE"){
	// 		string sequenceFileName;
	// 		in >> sequenceFileName;
	// 		for(int i=0; i<numberOfTimes; ++i){
	// 			ifstream file(sequenceFileName);
	// 			ReadCommands(file);
	// 			file.close();
	// 		}
	// 		continue;
	// 	}
	// 	else if(command == "NORANDOM"){
	// 		norandomSequenceIndex = 1;
	// 		string norandomFileName;
	// 		in >> norandomFileName;
	// 		ifstream file(norandomFileName);
	// 		string s;
	// 		norandomBlockSequence = "";
	// 		while(file >> s){
	// 			norandomBlockSequence += s;
	// 		}
	// 		cout << "Blocks (in Level 3, 4) will now be in the sequence " << norandomBlockSequence << "." << endl;
	// 		file.close();
	// 		// ?? NOT RIGHT ///
	// 		// SET this and next block ... This block np, but next block?
	// 		// Restart();
	// 		// g->SetShapes(' ', currLevel->ChooseShape());
	// 		// if()
	// 		// g->CreateNewShape(currLevel->ChooseShape(), currLevel->CurrentLevel());
	// 		cout << *g;
	// 		continue;
	// 	}
	// 	else if(command == "RANDOM"){
	// 		norandomSequenceIndex = 0;
	// 		norandomBlockSequence = "";
	// 		cout << "Randomness Restored." << endl;
	// 		continue;
	// 	}
	// 	// Extra feature: Macro
	// 	else if(command == "MACRO" && extraFeatureMacro == true){
	// 		cout << "Extra feature: Macro. Usage: macroName command1 command2 ... -1\n";
	// 		string macroName;
	// 		while(1){
	// 			in >> macroName;
	// 			// convert to UPPER case
	// 			for(int i=0; i<macroName.length(); ++i)
	// 					if(macroName[i]<='z' && macroName[i]>='a')
	// 						macroName[i] = macroName[i]+('Z'-'z');

	// 			string alreadyPresent = allCommands.Find(macroName);
	// 			if(alreadyPresent == ""){
	// 				allCommands.Insert(macroName);
	// 				break;
	// 			}
	// 			else{
	// 				cout << "Macro Name " << macroName << " is already present." << endl;
	// 				cout << "Please start over" << endl;
	// 			}
	// 		}
	// 		string s;
	// 		while(in >> s){
	// 			if(s == "-1"){
	// 				break;
	// 			}
	// 			norandomBlockSequence += (s + " ");
	// 		}
	// 		pair<string, string> newMacroCommand;
	// 		newMacroCommand.first = macroName;
	// 		newMacroCommand.second = norandomBlockSequence;
	// 		// You can't delete macros
	// 		// for(int i=0; i<macroCommands.size(); ++i){
	// 		// 	if(i.first == macroName){
	// 		// 		cout << macroName << " has been overwritten" << endl;
	// 		// 		break;
	// 		// 	}
	// 		// }
	// 		cout << macroName << " now means " << newMacroCommand.second << endl;
	// 		cout << "Note: You may also use auto-complete for macros." << endl;
	// 		macroCommands.push_back(newMacroCommand);
	// 		continue;
	// 	}
	// 	else if(command == "RESTART"){
	// 		Restart();
	// 	}
	// 	else if(command == "HINT"){}
	// 	else if(extraFeatureMacro){
	// 		string completeMacroCommand = allCommands.Find(command);
	// 		for(int i=0; i<macroCommands.size(); ++i){
	// 			if(macroCommands[i].first == completeMacroCommand){
	// 				stringstream iss(macroCommands[i].second);
	// 				ReadCommands(iss);
	// 				break;
	// 			}
	// 		}
	// 	}

	// 	for(int i=0; i<numberOfTimes; ++i){ 
	// 		InterpretCommand(command);
	// 	}

	// 	// If the command is to move, then regardless of the number of times, go down by one.
	// 	if(command == "LEFT" || command == "RIGHT" || command == "DOWN" || 
	// 		command == "CLOCKWISE" || command == "COUNTERCLOCKWISE"){
	// 		currLevel->PostUpdate(*g);
	// 	}

	// 	cout << *g;
	// }
}

void Restart() {
	if(norandomSequenceIndex != 0){
		norandomSequenceIndex = 1;
	}

	g->ClearGrid();

	delete currLevel;

	currLevel = new Level0;

	for (int j = 0; j < startLevel; ++j) {
		currLevel = PlusPlus(currLevel);
	}

	g->SetShapes(' ', currLevel->ChooseShape());

	g->CreateNewShape(currLevel->ChooseShape(), currLevel->CurrentLevel());
}

// Takes in a string and performs actions on grid, based on value of string
void InterpretCommand(const string &command){	
	if(command == "LEFT"){ g->TranslateWindowLeft(); }
	else if(command == "RIGHT"){ g->TranslateWindowRight(); }
	else if(command == "DOWN"){ g->TranslateWindowDown(); }
	else if(command == "DROP"){
		bool hasRowDeleted = g->DropWindow(currLevel->CurrentLevel());

		if (hasRowDeleted == true) {
			currLevel->ResetStarCounter();
		} else {
			currLevel->IncrementCounter(*g);
		}

		bool hasCreatedNewShape = g->CreateNewShape(currLevel->ChooseShape(), currLevel->CurrentLevel());

		if (hasCreatedNewShape == false) {
			Restart();
		}
	}
	else if(command == "LEVELUP"){ currLevel = PlusPlus(currLevel); }
	else if(command == "LEVELDOWN"){ currLevel = MinusMinus(currLevel); }
	else if(command == "CLOCKWISE"){ g->RotateClockwise(); }
	else if(command == "COUNTERCLOCKWISE"){ g->RotateCounterClockwise(); }
	else if(command == "I"){ g->ChangeShape('I'); }
	else if(command == "J"){ g->ChangeShape('J'); }
	else if(command == "L"){ g->ChangeShape('L'); }
	else if(command == "O"){ g->ChangeShape('O'); }
	else if(command == "T"){ g->ChangeShape('T'); }
	else if(command == "S"){ g->ChangeShape('S'); }
	else if(command == "Z"){ g->ChangeShape('Z'); }
	else {cout << "Invalid Command." << endl; } // this should NEVER run
}

