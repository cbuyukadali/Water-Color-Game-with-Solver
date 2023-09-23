//Title: Watercolor Game Solver Cpp
//Author: Cemil Buyukadali
//Date_Version: 23092023_final

#include<iostream>
#include<fstream>
#include<chrono>
#include<string>
#include<vector>


//#define N 4 //Maximum number of colors hold by a tube
#define PaletMax 13 // Maximum of color palette number starting from 1 
#define MAX 15 //Maximum number of tubes

using namespace std;

//Function prototypes
void initializeGame(int(&a)[MAX][4], int& n); //initialize the game by commant prompt
void initializeGamefromFile(int(&a)[MAX][4], int& n);
void showTubes(int a[MAX][4], int n); //show tubes with colors in the game
bool countColor(int a[MAX][4], int n); //Count the number of each color in the game if all is 4? Maximum color number is assumed as PaletMax. 

int top(int a[MAX][4], int k); // top color in the k th tube.
int topIndex(int a[MAX][4], int k); //how many free space is there in k th tube? if it is 0, then the tube is full; if it is 4, then there is no color in a tube.
bool topfull(int a[MAX][4], int k); //is the tube full?

bool success(int a[MAX][4], int n); // is the game finished?
bool noMove(int a[MAX][4], int n); //if there is no move left?
bool transportColor(int(&a)[MAX][4], int n, int from, int to); //transport color from tube from+1 to to+1

void kayitYazdir(int rec[1000], int r);
void playGame(int(&a)[MAX][4], int n, string str);


//controls if k th tube is empty 
bool isTubeEmpty(int a[MAX][4], int n, int k);

//moves array: first entry from tube, second entry to tube.
void possibleMoves(int a[MAX][4], int n, int(&moves)[100][2], int& k);

//controls if the tube a and the tube b are equal.
bool isTubesEqual(int a[MAX][4], int b[MAX][4], int n);


//This function converts vector of vector to array.
void convertVec_Arr(std::vector<std::vector<int>> bVec, int(&b)[MAX][4], int n);

//This function converts  array to vector of vector. 
void convertArr_Vec(std::vector<std::vector<int>> bVec, int b[MAX][4], int n);

//sum of total different color in each tube. 
int metricTube(int a[MAX][4], int n);

//solve by bfs of possible moves with metric in each step till
bool solveGame(int a[MAX][4], int n, string str);

int main()
{
	int a[MAX][4];
	int n; //number of tube. MAX is the maximum number of tubes in the game that can be generated.

	//initialize
	//empty=0, enter different color with different number starting from 1 to 13. by changing PaletMax 

	int option = 0;


	while (1)
	{
		system("cls");
		std::cout << "What do you want to do?\nGenerate new game from keyboard (1).\nGenerate the game stored in the file named initialize.txt (2),\nExit (0)?" << std::endl;
		std::cin >> option;

		if (option == 1)
		{
			initializeGame(a, n);
			break;
		}

		if (option == 2)
		{
			initializeGamefromFile(a, n);
			break;
		}
		if (option == 0)
		{
			std::cout << "Exit from game." << std::endl;
			return -1;
		}


		std::cout << "You enter wrong number.";
		system("pause");
	}

	if (!countColor(a, n))
	{
		std::cout << "The number of color in the game is different than four.";
		system("pause");
		return -1;
	}

	string str = ""; //the string to store the movements.


	while (1)
	{
		std::cout << endl;
		std::cout << "What do you want to do? \nPlay the game (1),\nNeed the solution of the game (2), \n Exit (0)" << std::endl;
		std::cin >> option;

		if (option == 1)
		{
			system("cls");
			showTubes(a, n);
						
			playGame(a, n, str);
		}

		if (option == 2)
		{
			auto  start = std::chrono::high_resolution_clock::now();

			showTubes(a, n);

			
			if (!solveGame(a, n, str))
			{
				std::cout << "Game is not solvable." << std::endl;
			}
			auto stop = std::chrono::high_resolution_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

			unsigned long duration = dur.count();
			unsigned long microsec = duration % 1000;
			duration = duration / 1000;
			unsigned long millisec = duration % 1000;
			duration = duration / 1000;
			unsigned int sec = duration % 60;
			duration = duration / 60;
			unsigned int min = duration % 60;
			duration = duration / 60;
			int hour = duration % 60;
			duration = duration / 60;
			std::cout << endl;
			std::cout << "Total elapsed time: " << hour << " : " << min << " : " << sec << " : " << millisec << " : " << microsec << std::endl;
			break;
		}

		if (option == 0)
		{
			std::cout << "Exit from game." << std::endl;
			return 0;
		}

		std::cout << "You entered wrong number.";
		system("pause");
	}




	system("pause");
	return 0;
}

int top(int a[MAX][4], int k)
{
	int temp = 0;
	for (int i = 0; i < 4; i++)
	{
		if (a[k][i] != 0)
		{
			temp = a[k][i];
			break;
		}
	}
	return temp;
}

int topIndex(int a[MAX][4], int k)
{
	int temp = 4; //before it is 3, how many free space is there in k th tube?
	for (int i = 0; i < 4; i++)
	{
		if (a[k][i] != 0)
		{
			temp = i;
			break;
		}
	}
	return temp;
}

bool topfull(int a[MAX][4], int k) //is the tube full?
{
	if (topIndex(a, k) == 0)
		return true;
	else
		return false;
}

bool success(int a[MAX][4], int n)
{
	int temp = a[0][0];
	for (int i = 0; i < n; i++)
	{
		temp = a[i][0];
		for (int j = 1; j < 4; j++)
		{
			if (temp != a[i][j]) return false;
		}
	}
	return true;
}

bool noMove(int a[MAX][4], int n)
{
	if (success(a, n))
	{
		return false;
	}
	else
	{
		// control if there is an empty tube

		for (int i = 0; i < n; i++)
		{
			if (topIndex(a, i) == 4) return false;
		}

		//control validity to move from tube A to tube B
		int tempA;
		int tempB, tempBind;
		for (int i = 0; i < n; i++)
		{
			tempA = top(a, i);
			for (int j = 0; j < n; j++)
			{
				if (i != j)
				{
					tempB = top(a, j);
					tempBind = topIndex(a, j);
					if (tempBind > 0 && tempB == tempA) return false;
				}
			}
		}
		return true;
	}
}

bool transportColor(int(&a)[MAX][4], int n, int from, int to)
{
	from = from - 1;
	to = to - 1;
	if (from == to)
		return false;

	if ((n > from && from >= 0) && (n > to && to >= 0))
	{
		if (topfull(a, to)) return false;

		int tempA = top(a, from);
		int tempAind = topIndex(a, from);

		int tempB = top(a, to);
		int tempBind = topIndex(a, to);

		if ((tempA == 0) || (tempBind == 0)) return false;
		//if tube A is empty or tube B is full, return
		if (tempB == 0)
		{
			a[from][tempAind] = 0;
			a[to][3] = tempA;

			return true;
		}
		else
		{
			if (tempA == tempB)
			{
				a[from][tempAind] = 0;
				a[to][tempBind - 1] = tempA;
				return true;
			}

		}
	}

	return false;
}

void initializeGame(int(&a)[MAX][4], int& n)
{
	std::cout << "Enter the number of tubes in the game: ";
	std::cin >> n;


	for (int i = 0; i < n; i++)
	{
		system("cls");
		std::cout << "empty=0, red=1, dark red=2, light blue=3, dark blue=4, turquaz blue=5" << std::endl;
		std::cout << "orange=6, light purple=7, dark purple=8, white=9, light grey=10, dark gray=11" << std::endl;
		std::cout << "brown=12, pink=13, yellow=14" << std::endl;
		for (int j = 0; j < 4; j++)
		{
			std::cout << i + 1 << ". tube has " << j + 1 << ". color :";
			std::cin >> a[i][j];
			std::cout << std::endl;
		}

	}


}

void showTubes(int a[MAX][4], int n)
{
	int k = 0;
	int r = 0;
	do {
		k = std::min(r + 5, n);
		for (int j = -1; j < 4; j++)
		{
			for (int i = r; i < k; i++)
			{
				if (j == -1)
					std::cout << "\t " << i + 1 << ". Tube" << " \t";
				else
					std::cout << "\t| " << a[i][j] << " |\t\t";
			}
			std::cout << std::endl;
		}

		std::cout << std::endl;
		r += 5;
	} while (r < n);

}

void initializeGamefromFile(int(&a)[MAX][4], int& n)
{
	std::string fileName = "initialize.txt";
	std::ifstream f;
	f.open(fileName);
	if (f.fail())
	{
		std::cout << "File cannot be opened." << std::endl;
		return;
	}

	int k = 0;
	for (int i = 0; i < 100; i++)
	{
		k++;
		for (int j = 0; j < 4; j++)
		{
			f >> a[i][j];
		}

		if (f.eof())
		{
			n = k;
			break;
		}
	}
	f.close();
}

bool countColor(int a[MAX][4], int n)
{
	int count=0;
	for (int i = 1; i <= PaletMax; i++) //PaletMax is the highest color code.
	{
		count = 0;
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (a[j][k] == i)
					count++;
			}
		}
		if (count > 0 && count < 4)
		{
			std::cout << "Color " << i << " number, " << count << std::endl;
			return false;
		}
		else
		{
			if (count > 4)
			{
				std::cout << "Color " << i << " number, " << count << std::endl;
				return false;
			}
		}
	}
	return true;
}

void kayitYazdir(int kayit[1000], int r)
{
	std::cout << "Movements: " << std::endl;
	for (int i = 0; i < r; i++)
	{
		if (i % 2 == 0 && i > 0) std::cout << ", ";

		if (i % 2) std::cout << " -> ";

		std::cout << kayit[i];
	}
}

void playGame(int(&a)[MAX][4], int n, string str)
{
	int from = 0, to = 0;

	while (1)
	{
		system("cls");
		showTubes(a, n);

		if (success(a, n))
		{
			std::cout << "You win." << std::endl;

			std::cout << "Movements: " << str<< std::endl;
			
			break;
		}
		if (noMove(a, n))
		{
			std::cout << "No moves left. Game is over." << std::endl;
			break;
		}

		std::cout << "Enter the number of tube to carry from: ";
		std::cin >> from;
		std::cout << std::endl;
		std::cout << "Enter the number of tube to carry to: ";
		std::cin >> to;
		std::cout << std::endl;
		if (from > n || to > n || from < 1 || to < 1)
		{
			std::cout << "Invalid tube number!" << std::endl;
		}
		else
		{
			if (transportColor(a, n, from, to))
			{
				str += ", " + from;
				str+=" -> " + to;
			}
		}
	}
}


//solve by bfs of possible moves with metric in each step till
bool solveGame(int a[MAX][4], int n, string str)
{
	std::vector< std::pair< long long int, std::vector<std::vector<int> > > > bVec;

	std::vector<std::pair<long long int, string>> kayitVec;
	kayitVec.push_back(make_pair(0, str));


	// determine which tubes are empty at start.
	int Empty[2] = { -1, -1 };
	int EmptyIndex = 0;
	for (int i = 0; i < n; i++)
	{
		if (isTubeEmpty(a, n, i))
		{
			Empty[EmptyIndex] = i + 1;
			EmptyIndex++;
		}
	}


	//start with inserting the game to a vector.
	std::vector<std::vector<int>> tempVecX(MAX, std::vector<int>(4));

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempVecX[i][j] = a[i][j];
		}
	}

	bVec.push_back(make_pair(0, tempVecX));

	int steps = 0; //hold the index of each movement
	int moves[100][2]; // possible move matrix, first column from, second column to
	int kontrol = 0; // kontrol index for successful matrix to finish the game 
	int stepsTemp;

	int tempB[MAX][4];
	int tempX[MAX][4];
	int tempY[MAX][4];
	int moveIndex = 0;

	bool oneEmptyTube[2] = { true, true };
	EmptyIndex = 0;


	int minMetric = 10000, minMetricstep = 0;

	std::vector<int> aVec;
	std::vector<std::vector<int>> aVecVec;

	bool flagNotExist = false;

	while (1)
	{
		for (int m = kontrol; m <= steps; m++)
		{
			convertVec_Arr(bVec[m].second, tempX, n);
			if (success(tempX, n))
			{
				std::cout << "\n" << "You win." << std::endl;
				std::cout << "Movements: " << kayitVec[m].second << std::endl;
				return true;
			}
		}


		stepsTemp = steps;


		for (int m = kontrol; m <= stepsTemp; m++)
		{

			convertVec_Arr(bVec[m].second, tempX, n);
			possibleMoves(tempX, n, moves, moveIndex);


			for (int i = 0; i < moveIndex; i++)
			{
				for (int tt = 0; tt < n; tt++)
				{
					for (int j = 0; j < 4; j++)
					{
						tempB[tt][j] = tempX[tt][j];
					}
				}
				if ((moves[i][1] == Empty[0] && oneEmptyTube[0]))
				{

				}
				else
				{
					if (moves[i][1] == Empty[1] && oneEmptyTube[1])
					{

					}
					else
					{
						if (transportColor(tempB, n, moves[i][0], moves[i][1]))
						{
							if (metricTube(tempB, n) <= metricTube(tempX, n) + 1) // To transport a color to empty tube we add plus one.
							{

								flagNotExist = true;
								for (int r = 0; r <= steps; r++)
								{
									convertVec_Arr(bVec[r].second, tempY, n);
									if (isTubesEqual(tempB, tempY, n))
									{
										flagNotExist = false;
										break;
									}
								}

								if (flagNotExist)
								{
									steps++;
									if (minMetric > metricTube(tempB, n))
									{
										minMetric = metricTube(tempB, n);
										minMetricstep = steps;
									}

									for (int i = 0; i < n; i++)
									{
										for (int j = 0; j < 4; j++)
										{
											aVec.push_back(tempB[i][j]);
										}
										aVecVec.push_back(aVec);
										aVec.clear();
									}
									bVec.push_back(make_pair(steps, aVecVec));
									aVecVec.clear();

									kayitVec.push_back(make_pair(steps, kayitVec[m].second + ", " + std::to_string(moves[i][0]) + "->" + std::to_string(moves[i][1])));

								}
							}
						}
					}

				}
			}
		}

		if (steps == stepsTemp) //control if there is no movemnt occured.
		{

			if (EmptyIndex == 2) return false; //if there is no empty tube or movement, then the game with inserted tube is not solvable and return false.

			oneEmptyTube[EmptyIndex] = false; //open any empty tube if any. 
			EmptyIndex++;
		}
		else
		{
			kontrol = stepsTemp + 1;

			system("cls");
			convertVec_Arr(bVec[minMetricstep].second, tempX, n);
			showTubes(tempX, n);

			std::cout << "Minimum metric " << minMetric << std::endl;
			std::cout << "Movements: " << kayitVec[minMetricstep].second << std::endl;
			std::cout << "Step  " << steps << ". " << std::endl;

			//carry the tube with minimum metric to solveGame function with movemnts if the number of move is more than 1000. 
			//Recursively if the carried minimal tube is not solvable, continue till new minimal tube is generated. 
			//If the carried minimal tube has obtained solution, all carried tube returns true and game is solved.   
			if (steps > 1000 && solveGame(tempX, n, kayitVec[minMetricstep].second)) return true;


			system("cls");
			convertVec_Arr(bVec[minMetricstep].second, tempX, n);
			showTubes(tempX, n);
			std::cout << "Minimum metric " << minMetric << std::endl;
			std::cout << "Movements: " << kayitVec[minMetricstep].second << std::endl;
			std::cout << "Step  " << steps << ". " << std::endl;
		}


	}
}


//sum of total different color in each tube. 
int metricTube(int a[MAX][4], int n)
{
	int metric = 0;
	int tempMetric = 0;
	int tempIndex = 0;
	int tempColor = 0;
	//How many different colors in a tube. If the tube is empty, the number is zero. 
	for (int i = 0; i < n; i++)
	{
		tempIndex = topIndex(a, i);
		if (tempIndex < 4)
		{
			if (tempIndex == 3) tempMetric = 1;

			if (tempIndex == 2)
			{
				if (a[i][2] != a[i][3])
				{
					tempMetric = 2;
				}
				else
				{
					tempMetric = 1;
				}
			}

			if (tempIndex == 1)
			{
				if (a[i][2] == a[i][3] && a[i][1] == a[i][3])
				{
					tempMetric = 1;
				}
				else
				{
					tempMetric = 1;
					tempColor = top(a, i);
					for (int j = 2; j < 4; j++)
					{
						if (tempColor != a[i][j])
						{
							tempMetric++;
						}
					}
					if (a[i][2] == a[i][3]) tempMetric--;
				}
			}

			if (tempIndex == 0)
			{
				if (a[i][2] == a[i][3] && a[i][1] == a[i][3] && a[i][0] == a[i][1])
				{
					tempMetric = 1;
				}
				else
				{
					if (a[i][0] == a[i][1] && a[i][1] == a[i][2])
					{
						tempMetric = 1;
					}
					else
					{
						tempMetric = 1;
						tempColor = top(a, i);
						for (int j = 1; j < 3; j++)
						{
							if (tempColor != a[i][j])
							{
								tempMetric++;
							}
						}
						if (a[i][1] == a[i][2]) tempMetric--;
					}

					bool flagMetric = true;
					for (int r = 0; r < 3; r++)
					{
						if (a[i][r] == a[i][3]) flagMetric = false;
					}
					if (flagMetric) tempMetric++;
				}
			}

			metric += tempMetric;
		}
	}
	return metric;
}

//controls if k th tube is empty 
bool isTubeEmpty(int a[MAX][4], int n, int k)
{
	if (k >= n) return false;

	for (int j = 0; j < 4; j++)
	{
		if (a[k][j] != 0) return false;
	}

	return true;
}


//moves array: first entry from tube, second entry to tube.

void possibleMoves(int a[MAX][4], int n, int(&moves)[100][2], int& k)
{
	int tempA;
	k = 0;

	for (int i = 0; i < n; i++)
	{
		if (a[i][3] != 0)
		{
			if (a[i][0] == a[i][1] && a[i][1] == a[i][2] && a[i][2] == a[i][3])
			{

			}
			else
			{
				tempA = top(a, i);
				for (int j = 0; j < n; j++)
				{
					if (i != j && !topfull(a, j))
					{
						if (top(a, j) == tempA || topIndex(a, j) == 4)
						{
							moves[k][0] = i + 1;
							moves[k][1] = j + 1;
							k++;
						}
					}
				}
			}
		}
	}
}

//controls if the tube a and the tube b are equal.
bool isTubesEqual(int a[MAX][4], int b[MAX][4], int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (a[i][j] != b[i][j]) return false;
		}
	}
	return true;
}


//This function converts vector of vector to array.
void convertVec_Arr(std::vector<std::vector<int>> bVec, int(&b)[MAX][4], int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			b[i][j] = bVec[i][j];
		}
	}
}

//This function converts  array to vector of vector. 
void convertArr_Vec(std::vector<std::vector<int>> bVec, int b[MAX][4], int n)
{
	vector<int> a;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			a.push_back(b[i][j]);
		}
		bVec.push_back(a);
		a.clear();
	}
}


/*
initialize.txt example
 0 0 0 0
1 2 3 4
5 3 4 6
7 8 4 9
3 6 9 10
9 10 1 10
6 10 1 11
4 6 9 8
11 8 5 11
1 11 5 12
12 3 12 13
13 5 12 2
13 2 7 13
7 8 2 7
0 0 0 0
*/