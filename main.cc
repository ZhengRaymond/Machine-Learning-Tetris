#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <string>
#include <climits>
#include <ctime>
#include <iomanip>
#include <sstream>
// #include "GraphicsDisplay.h"
using namespace std;


// !!
// NOTE: R, C, Gen, numWeifhts, gamesPerWeight, seed changed without breaking the program!
// !!


// -0.294046    -1.3458  0.0847383   0.091462 resulted in 4200 lines
// GLOBAL variables are in caps. 
// B is our board
vector<vector<char>> B;
int R = 20, C = 10;
// H is a vector of heights of board B
// vector<int> H;

// PMap is a hashmap of a character to a vector of rotations of that piece
unordered_map<char, vector<vector<vector<int>>>> PMap;
// PChar is a vector of all our pieces ('I', 'O' ...)
vector<char> PChar;
int PNum;

int seed = 0;

int numGeneration = 10000;
int numWeights = 12;
int gamesPerWeight = 1;
const int numCoefficient = 6;


bool graphics = false;
bool play = false;
bool coefficient = false;

int main(int argc, char *argv[]){
	// Function prototypes
	void initialize();
	int runSimulation(vector<vector<char>> board, vector<double>& weights);
	void resetWeightAndScore(vector<vector<double>>& weightsForGen, vector<double>& avgScoreForWeight, int numRandom, double maxScore);
	void randomizeWeights(vector<vector<double>>& weightsForGen);

	seed = time(NULL);
	// Set PMap, PChar, PNum, B
	initialize();
	
	// command line arguments
	for(int i=1; i<argc; ++i){
		string cmdLineArgument = string(argv[i]);
		if(cmdLineArgument == "-play"){
			play = true;
		}
		else if(cmdLineArgument == "-coefficient"){
			coefficient = true;
		}
		else if(cmdLineArgument == "-graphics"){
			graphics = true;
		}
		else if(cmdLineArgument == "-seed"){
			i += 1;
			stringstream tempIss(argv[i]);
			if(tempIss >> seed){}
		}
		else if(cmdLineArgument == "-gen"){
			i += 1;
			stringstream tempIss(argv[i]);
			if(tempIss >> numGeneration){}
		}
		else if(cmdLineArgument == "-wts"){
			i += 1;
			stringstream tempIss(argv[i]);
			if(tempIss >> numWeights){}
		}
		else if(cmdLineArgument == "-gpw"){
			i += 1;
			stringstream tempIss(argv[i]);
			if(tempIss >> gamesPerWeight){}
		}

	}

	if(play){
		vector<double> ws;
		// ws = {-0.10314, -0.412885, 0.0631569, 0.330902};
		if(coefficient){
			for(int i=0; i<numCoefficient; ++i){
				cout << "Enter coefficient #" << i;
				cin >> ws[i];
				cout << endl;
				exit(0);
			}
		}
		cout << "Running\n";
		runSimulation(B, ws);
		exit(0);
	}


	vector<vector<double>> weightsForGen;
	vector<double> avgScoreForWeight;
	avgScoreForWeight.resize(numWeights);

	// Randomize weights 
	randomizeWeights(weightsForGen);

	// weightsForGen[0][24] = 1; // = {-0.10314, -0.412885, 0.0631569, 0.330902}; // {-0.294046, -1.3458, 0.0847383, 0.091462};
	for(int i=0; i<numGeneration; ++i){
		double genAvg = 0;
		double MaxAvgWtScore = INT_MIN;
		for(int j=0; j<numWeights; ++j){
			double weightAvg = 0;
			for(int k=0; k<gamesPerWeight; ++k){
				int score = runSimulation(B, weightsForGen[j]);

				weightAvg += score;
				cout << "score" << k << ": " << setw(5) << score << " ";
			}
			weightAvg /= gamesPerWeight;
			if(weightAvg > MaxAvgWtScore) MaxAvgWtScore = weightAvg;
			genAvg += weightAvg;
			cout << "Gen: " << i << " W: ";
			//Printing Weights
			for(int q=0; q<numCoefficient; ++q) cout << setw(10) << weightsForGen[j][q] << " ";
			cout << "AvgScore(of " << gamesPerWeight << "): " << weightAvg << "\n";
			// adding score for weight j
			avgScoreForWeight[j] = weightAvg;
		}
		genAvg /= numWeights;
		resetWeightAndScore(weightsForGen, avgScoreForWeight, 0, MaxAvgWtScore); // make 0 completely random
		cout << "------- Gen: " << i << " MaxAvgWtScore: " << MaxAvgWtScore << " AvgGenScore: " << genAvg << " --------\n";
		
	}
	return 0;
}

void randomizeWeights(vector<vector<double>>& weightsForGen){
	// Randomize initial weights
	weightsForGen.resize(numWeights);
	for(int i=0; i<numWeights; ++i){
		weightsForGen[i].resize(numCoefficient);
		for(int j=0; j<numCoefficient; ++j){
			weightsForGen[i][j] = ((2) * ( (double)rand() / (double)RAND_MAX ) + -1);
			// cout << weightsForGen[i][j] << " ";
		}
		// cout << "\n";
	}
}

void resetWeightAndScore(vector<vector<double>>& weightsForGen, vector<double>& avgScoreForWeight, int numRandom, double MaxAvgWtScore){
	double IndexOne = 0, IndexTwo = 1;
	if(avgScoreForWeight[IndexTwo] > avgScoreForWeight[IndexOne]){
		swap(IndexOne, IndexTwo);
	}
	// finding best two scores
	for(int i=2; i<avgScoreForWeight.size(); ++i){
		if(avgScoreForWeight[i] > avgScoreForWeight[IndexOne]){
			IndexTwo = IndexOne;
			IndexOne = i;
		}
		else if(avgScoreForWeight[i] > avgScoreForWeight[IndexTwo]){
			IndexTwo = i;
		}
	}

	
	// int biasTowardOne = (avgScoreForWeight[IndexOne] - avgScoreForWeight[IndexTwo]); 
	int biasTowardOne = 4;

	// resizing newWeights vector
	vector<vector<double>> newWeights;
	newWeights.resize(numWeights);
	for(int i=0; i<numWeights; ++i) newWeights[i].resize(numCoefficient);
	newWeights[0] = weightsForGen[IndexOne];
	newWeights[1] = weightsForGen[IndexTwo];



	// IndexOne and IndexTwo are indices of top two weights
	// printing top two weights
	cout << "--MaxOneAvgScore: " << avgScoreForWeight[IndexOne] << " MaxOneIndex: " << IndexOne << " MaxOneWeights ";
	for(int q=0; q<numCoefficient; ++q) cout << setw(10) << newWeights[0][q] << " ";
	cout << "--\n";
	cout << "--MaxTwoAvgScore: " << avgScoreForWeight[IndexTwo] << " MaxTwoIndex: " << IndexTwo << " MaxTwoWeights ";
	for(int q=0; q<numCoefficient; ++q) cout << setw(10) << newWeights[1][q] << " ";
	cout << "--\n";


	// the last numRandom are completely random
	for(int i=2; i<numWeights-numRandom; ++i){
		int pickFromWhere = rand() % (biasTowardOne);
		if(pickFromWhere == 0){ // pick coefficient of second best
			newWeights[i] = newWeights[1];
		}
		else{ // pick coefficient of first best
			newWeights[i] = newWeights[0];
		}
		// for(int j=0; j<weightsForGen[i].size(); ++j){
			
		// }
	}



	// Making the last numRandom completely random
	for(int i=numWeights-numRandom; i<numWeights; ++i){
		for(int j=0; j<numCoefficient; ++j){
			int pickFromWhere = rand() % biasTowardOne;
			if(pickFromWhere == 0){
				newWeights[i][j] = newWeights[1][j];
			}
			else{
				newWeights[i][j] = ((2) * ( (double)rand() / (double)RAND_MAX ) + -1);
			}
		}
	}


	// we mutate 1 in every 3 coefficients (3 how? numCoefficient/2 + 1)
	// one in every 3*numCoefficient are modified by large amount
	// one in every 2*numCoefficient are modified by medium amount
	// one in every 1*numCoefficient are modified by small amount
	// Mutations in coefficients 
	// keep the best anyway
	// the amount is a function of the MaxAvgWtScore
	// 
	weightsForGen[0] = newWeights[0];
	for(int i=1; i<numWeights; ++i){
		for(int j=0; j<weightsForGen[i].size(); ++j){
			// int change = rand() % 4;
			// if(change == 0) newWeights[i][j] += newWeights[i][j]*0.01; //(0.1*((2) * ( (double)rand() / (double)RAND_MAX ) + -1)) ;

			int extremeMutation = rand() % 25;//(10*(numCoefficient));
			int largeMutation = rand() % 16;//(4*(numCoefficient));
			int mediumMutation = rand() % 8;//(3*(numCoefficient));
			int smallMutation = rand() % 6; //(2*(numCoefficient));
			double changeBy = ((2) * ( (double)rand() / (double)RAND_MAX ) + -1);
			changeBy *= (1000.0/MaxAvgWtScore);
			if(extremeMutation == 0) newWeights[i][j] += (0.4) *changeBy;
			else if(largeMutation == 0) newWeights[i][j] += (0.2) *changeBy;
			else if(mediumMutation == 0) newWeights[i][j] += (0.1) *changeBy;
			else if(smallMutation == 0) newWeights[i][j] += (0.05) *changeBy;

			

			// weights can only be betwen [-1, 1]
			if(newWeights[i][j] < -1) newWeights[i][j] = -1;
			else if(newWeights[i][j] > +1) newWeights[i][j] = +1;
		}
		weightsForGen[i] = newWeights[i];
	}
}


// Runs the simulation on board board, and using weights 'weights' and returns the _total_ number of lines cleared
int runSimulation(vector<vector<char>> board, vector<double>& coefficients){
	// function prototypes
	void printBoard(const vector<vector<char>> &v);
	int dropAndRemoveClears(vector<vector<char>>& v, const int col, const char c, const int rot);
	double calculateFitness(vector<vector<char>> v, const vector<double>& coefficients, const int numCleared);
	char generatePiece();

	int totalLinesCleared = 0;
	while(1){
		bool GameNotOver = true;
		int maxRot = 0, maxRig = 0;
		double maxFitness = INT_MIN;

		char c = generatePiece();
		vector<vector<vector<int>>> pieceRotations = PMap[c];

		// Find maximum fitness
		for(int i=0; i<pieceRotations.size(); ++i){
			int pieceWidth = pieceRotations[i][0].size();
			for(int j=0; j<C-pieceWidth+1; ++j){
				vector<vector<char>> tempBoard = board;
				// rotation i, column j, piece c on tempBoard
				int numCleared = dropAndRemoveClears(tempBoard, j, c, i);
				double fitness;
				if(numCleared == -1) fitness = INT_MIN;
				else fitness = calculateFitness(tempBoard, coefficients, numCleared);
				// cout << "piece: " << c << ", rotation: " << i << ", col: " << j << ", fitness: " << fitness << endl;

				if(fitness > maxFitness){
					maxFitness = fitness;
					maxRig = j;
					maxRot = i;
				}
			}
		}
		if(play){
			// cout << "MAX: " << "piece: " << c << ", rotation: " << maxRot << ", column: " << maxRig << ", fitness: " << maxFitness << endl;
		}
		
		// make max move
		int numCleared = dropAndRemoveClears(board, maxRig, c, maxRot);
		if(play){

			printBoard(board);
			// int x;
			// cin>>x;
		}

		if(numCleared == -1){
			if(play){
				cout << "Game Over!\n";
				cout << "Lines Cleard: " << totalLinesCleared << "\n";
			}
			return totalLinesCleared;
		}
		
		totalLinesCleared += numCleared;
	}
}


// USING OLD FITNESS FUNCTION
double calculateFitness(vector<vector<char>> v, const vector<double>& coefficients, const int numCleared){
	// void printBoard(const vector<vector<char>> &v);
	// printBoard(v);
	// int x;
	// cin>>x;

	int totalHeight = 0;
	int maxHeight = 0;
	int numHoles = 0;
	int lastHeight;
	int heightDifferences = 0;
	vector<int> colHeights;
	for (int i = 0; i < C; i++) {
		bool startCounting = false;
		int currHeight = 0;
		for (int j = 0; j < R; j++) {
			if (v[j][i] != ' ') startCounting = true;
			if (startCounting) {
				currHeight++;
				if (v[j][i] == ' ') {
						numHoles++;
				}
			}
		}

		if (i == 0) lastHeight = currHeight;
		heightDifferences += abs(currHeight - lastHeight);
		lastHeight = currHeight;

		colHeights.push_back(currHeight);
		if (currHeight > maxHeight) {
			maxHeight = currHeight;
		}

		totalHeight += currHeight;
	}
	/*int average = totalHeight / board.width;
	double averageSquaresDiff = 0;
	for (int i = 0; i < board.width; i++) {
		averageSquaresDiff += pow(colHeights[i] - totalHeight, 2);
	}
	averageSquaresDiff /= board.width;
	double SD = sqrt(averageSquaresDiff);*/

	int numBlockades = 0;
	for (int i = 0; i < C; i++) {
		bool startCounting = false;
		for (int j = R - 1; j >= 0; j--) {
			if (v[j][i] == ' ') startCounting = true;
			if (startCounting && v[j][i] != ' ') {
				numBlockades++;
			}
		}
	}

	// for(int b=0; b<C; ++b){
	// 	int numHoles = 0;
	// 	bool seenPiece = false;
	// 	for(int a=0; a<R; ++a){
	// 		if(!seenPiece && v[a][b] != ' '){
	// 			if(b == 0){
	// 				features[0] = (R-a);
	// 			}
	// 			else if(b == C){
	// 				features[1] = R-a;
	// 			}
	// 			seenPiece = true;
	// 			continue;
	// 		}
	// 		else if(seenPiece && v[a][b] == ' '){
	// 			numHoles += 1;
	// 		}
	// 	}
	// 	// features[b+C] = numHoles; 
	// 	totalHoles += numHoles;
	// 	totalHeight += features[b];
	// 	if(features[b] > greatestHeight) greatestHeight = features[b];
	// 	if(features[b] < lowestHeight) lowestHeight = features[b];
	// }

	int firstHeight = 0, finalHeight = 0;
	for(int a=0; a<R;++a){
		if(v[a][0] != ' '){
			firstHeight = R-a;
			break;
		}
	}
	for(int a=0; a<R;++a){
		if(v[a][C-1] != ' '){
			finalHeight = R-a;
			break;
		}
	}

	// coefficients = {"heightDifferences" . "numHoles" . "maxHeight" . "numClears" };

	return coefficients[0] * heightDifferences +
		coefficients[1] * numHoles +
		coefficients[2] * maxHeight +
		coefficients[3] * numCleared +
		coefficients[4] * firstHeight + 
		coefficients[5] * finalHeight;
}


// Calculates fitness
// double calculateFitness(vector<vector<char>> v, const vector<double>& coefficients, const int numCleared){
// 	// void printBoard(const vector<vector<char>> &v);
// 	// printBoard(v);
// 	// int x;
// 	// cin>>x;

// 	// features is 10 heights, 10 numHoles, totalHeight, averageHeight, lowestHeight, greatestHeight, numCleared
// 	// changed features to 10heights, numHoles, totalHeight, avgHeight, lowestHeight, greatestheight, numCleared
// 	// changed again, now first height, last height, numHoles, totalHeight, avgHeight, lowestHeight, greatestheight, numCleared
// 	vector<double> features;
// 	features.resize(numCoefficient);

// 	double totalHeight = 0.0, averageHeight = 0.0, lowestHeight = INT_MAX, greatestHeight = INT_MIN;
// 	int totalHoles = 0;
// 	for(int b=0; b<C; ++b){
// 		int numHoles = 0;
// 		bool seenPiece = false;
// 		for(int a=0; a<R; ++a){
// 			if(!seenPiece && v[a][b] != ' '){
// 				if(b == 0){
// 					features[0] = (R-a);
// 				}
// 				else if(b == C){
// 					features[1] = R-a;
// 				}
// 				seenPiece = true;
// 				continue;
// 			}
// 			else if(seenPiece && v[a][b] == ' '){
// 				numHoles += 1;
// 			}
// 		}
// 		// features[b+C] = numHoles; 
// 		totalHoles += numHoles;
// 		totalHeight += features[b];
// 		if(features[b] > greatestHeight) greatestHeight = features[b];
// 		if(features[b] < lowestHeight) lowestHeight = features[b];
// 	}
// 	averageHeight = totalHeight/C;

// 	features[2] = totalHoles;
// 	features[3] = totalHeight;
// 	features[4] = averageHeight;
// 	features[5] = lowestHeight;
// 	features[6] = greatestHeight;
// 	features[7] = numCleared;

// 	int numBlockades = 0;
// 	for (int i = 0; i < C; i++) {
// 		bool startCounting = false;
// 		for (int j = R - 1; j >= 0; j--) {
// 			if (v[j][i] == ' ') startCounting = true;
// 			if (startCounting && v[j][i] != ' ') {
// 				numBlockades++;
// 			}
// 		}
// 	}

// 	features[8] = numBlockades;

// 	double fitness = 0.0;
// 	for(int i=0; i<numCoefficient; ++i){
// 		fitness += (features[i]*coefficients[i]);
// 	}

// 	return fitness;
// }

// returns number of lines cleared, -1 if game over
int dropAndRemoveClears(vector<vector<char>>& v, const int col, const char c, const int rot){
	// function prototypes
	bool dropPieceAtCol(vector<vector<char>>& v, const int col, const char c, const int rot);
	int removeClears(vector<vector<char>>& v);

	bool gameNotOver = dropPieceAtCol(v, col, c, rot);

	if(gameNotOver){
		return removeClears(v);
	}
	else return -1;
}

// can be improved by only checking rows that the last piece was inserted in !!
// also by using something other than erase, insert.
int removeClears(vector<vector<char>>& v) {
	// function prototypes
	vector<char> getEmptyRow();

	int clears = 0;
	for(int i=0; i<R; ++i){
		bool rowFull = true;
		for(int j=0; j<C; ++j){
			if(v[i][j] == ' '){
				rowFull = false;
				break;
			}
		}
		if(rowFull){
			clears += 1;
			v.erase(v.begin() + i);
			v.insert(v.begin(), getEmptyRow());
		}
	}

	return clears;

	// Ray's clear Row
	// some bugs !!
	// int clears = 0;
	// for (int i=0; i<R; ++i){
	// 	int sum = 0;
	// 	for (int j=0; j<C; ++j){
	// 		sum += (v[i][j] == ' ' ? 0 : 1);
	// 	}
	// 	if (sum == C) {
	// 		clears++;
	// 		for (int m = i; m > 0; m--) {
	// 			v[m] = v[m - 1];
	// 		}
	// 		for (int m = 0; m < C; m++) {
	// 			v[0][m] = ' ';
	// 		}
	// 		i++;
	// 	}
	// }
	// return clears;
}

// returns a vector of empty chars of length C 
vector<char> getEmptyRow(){
	vector<char> v;
	v.resize(C);
	for(int i=0; i<C; ++i) v[i] = ' ';
	return(v);
}

// Drop piece at location (can be improved with hashmap of heights !! )
bool dropPieceAtCol(vector<vector<char>>& v, const int col, const char c, const int rot){
	// function prototypes

	
	void dropPieceAt(vector<vector<char>>& v, const int row, const int col, const char c, const int rot);
	bool canDropPieceAt(const vector<vector<char>>& v, const int row, const int col, const char c, const int rot);

	vector<vector<int>>& pieceVector = PMap[c][rot];

	for(int i=0; i<=R-pieceVector.size(); ++i){
		if(!canDropPieceAt(v, i, col, c, rot)){
			if(i == 0) return false;
			dropPieceAt(v, i-1, col, c, rot);
			return true;
		}
	}

	if(canDropPieceAt(v, R-pieceVector.size(), col, c, rot)){
		dropPieceAt(v, R-pieceVector.size(), col, c, rot);
		return true;
	}

	return false;
}

// Returns true if piece can be dropped in row, col
bool canDropPieceAt(const vector<vector<char>>& v, const int row, const int col, const char c, const int rot){
	vector<vector<int>>& pieceVector = PMap[c][rot];

	for(int i=0; i<pieceVector.size(); ++i){
		for(int j=0; j<pieceVector[i].size(); ++j){
			if(pieceVector[i][j] && v[row+i][col+j] != ' ') return false;
		}
	}
	return true;
}

// Drops piece at location row, col with character pieceChar, rotation rot
void dropPieceAt(vector<vector<char>>& v, const int row, const int col, const char c, const int rot){
	vector<vector<int>>& pieceVector = PMap[c][rot];

	for(int i=0; i<pieceVector.size(); ++i){
		for(int j=0; j<pieceVector[i].size(); ++j){
			if(pieceVector[i][j]) v[row+i][col+j] = c;
		}
	}
}

// A function to initialize global variables
void initialize(){
	// functions used here
	void reserveSpace(vector<vector<char>>&);
	void setPieces();

	// Reserve space for B
	reserveSpace(B);

	// reserving column space for H
	H.resize(C);
	// Initializing Heights
	for(int i=0; i<C; ++i) H[i] = 0;

	// Set pieces used
	setPieces();

	// Setting the seed
	// seed = time(NULL);
	srand(seed);
}

// Reserves R rows and C cols for v and sets them to ' '
void reserveSpace(vector<vector<char>>& v){
	v.resize(R);
	for(int i=0; i<R; ++i){
		v[i].resize(C);
		for(int j=0; j<C; ++j){
			v[i][j] = ' ';
		}
	}
}

// Sets PNum, PChar, PMap
void setPieces(){
	PChar = {'O', 'I', 'J', 'L', 'S', 'T', 'Z'};
	PNum = PChar.size();

	// Setting Piece Maps
	PMap['O'] = {
		{
			{1,1},
			{1,1}
		}
	};

	PMap['I'] = {
		{
			{1},
			{1},
			{1},
			{1}
		},
		{
			{1,1,1,1}
		}
	};

	PMap['J'] = {
		{
			{0,1},
			{0,1},
			{1,1}
		},
		{
			{1,0,0},
			{1,1,1},
		},
		{
			{1,1},
			{1,0},
			{1,0}
		},
		{
			{1,1,1},
			{0,0,1},
		}
	};

	PMap['L'] = {
		{
			{1,0},
			{1,0},
			{1,1}
		},
		{
			{0,0,1},
			{1,1,1},
		},
		{
			{1,1},
			{0,1},
			{0,1}
		},
		{
			{1,1,1},
			{1,0,0},
		}
	};

	PMap['S'] = {
		{
			{0,1,1},
			{1,1,0}
		},
		{
			{1,0},
			{1,1},
			{0,1}
		}
	};

	PMap['T'] = {
		{
			{0,1,0},
			{1,1,1}
		},
		{
			{1,0},
			{1,1},
			{1,0}
		},
		{
			{1,1,1},
			{0,1,0}
		},
		{
			{0,1},
			{1,1},
			{0,1}
		}
	};

	PMap['Z'] = {
		{
			{1,1,0},
			{0,1,1}
		},
		{
			{0,1},
			{1,1},
			{1,0}
		}
	};
}

// Generates a random piece from PChar
char generatePiece(){
	return (PChar[rand() % PNum]);
}

// Prints the board v
void printBoard(const vector<vector<char>> &v){
	for(int i=0; i<C+2; ++i) cout << "-";
	cout << endl;
	for (int i = 0; i<R; i++) {
		cout << "|";
		for (int j = 0; j<C; j++) {
			cout << v[i][j];
		}
		cout << "|" << endl;
	}
	for(int i=0; i<C+2; ++i) cout << "-";
	cout << endl;
}

