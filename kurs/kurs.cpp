#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <queue>
#include "stdlib.h"

using namespace std;
int width, height, choice, matrixSize, countI = 0, countJ = 0, cell = 0, yIsOn = 1, v, heightParam;
bool levelComplete, gameOver = false, c = false, block = false, solved = false;
const int buildDirUp = 1, buildDirRight = 2, buildDirDown = 3, buildDirLeft = 4;
int x, y, exitX, exitY, buildX, buildY, buildDir, buildDirPrev, buildDir2, i, buildComplete = 0, stop = 0, exitCoords, exitCoordsSaved, score, timeForGame = 50, uScore, s, breakX, breakY;
char ratNames[10], ratScore[10], currentName[15], a, name[15], fileScore[2], fileName[15];
time_t secondsLevel, secondsLevelEnd, secondsGame, currentTime;
char strScanf[] = "%d";
char** field, ** cameFrom;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
typedef struct { char Name[15], Score[2]; } ITEM;
ITEM Unit[10];

FILE* Rating;
FILE* RatingBackup;
FILE* LastGame;

int BFS(int v, int* vis, int m, int** a, int vExit) {
	queue <int> q;
	vis[v] = 1;
	q.push(v);
	while (!q.empty() && vis[vExit - 1] == 0) {
		v = q.front();
		q.pop();
		for (int i = 0; i < m; i++) {
			if (a[v][i] == 1 && vis[i] == 0) {
				q.push(i);
				vis[i] = vis[v] + 1;
			}
		}
	}
}

void PreSetup() {
	system("cls");
	printf("Do you wanna play on saved maps or create new ones?\n");
	printf(" +---+                 +---+\n");
	printf(" | 1 | - Random maps   | 2 | - Saved maps\n");
	printf(" +---+                 +---+\n");
	scanf("%d", &choice);

	while (choice != 1 && choice != 2) {
		system("cls");
		printf("Do you wanna play on random maps or saved ones?\n");
		printf(" +---+                 +---+\n");
		printf(" | 1 | - Random maps   | 2 | - Saved maps\n");
		printf(" +---+                 +---+\n");
		scanf("%d", &choice);
	}
	if (choice == 1) {
		system("cls");
		printf("Enter width if a Labirinth you would like to play\nIt should be odd; <= 17; and >= 3:\n");
		scanf("%d", &width);
		while (width % 2 == 0 || width < 3 || width > 17) {
			system("cls");
			printf("WRONG SIZE!!\nEnter width if a Labirinth you would like to play\nIt should be odd; <=17; and >= 3:\n");
			scanf("%d", &width);
		}
		system("cls");
		if (width == 13 || width == 15) heightParam = 7;
		else if (width == 17) heightParam = 11;
		else heightParam = 3;
		printf("Enter height if a Labirinth you would like to play\nIt should be odd; %d <= height <= %d:\n", heightParam, width);
		scanf("%d", &height);
		while (height % 2 == 0 || height < heightParam || height > width) {
			system("cls");
			printf("WRONG SIZE!!\nEnter height if a Labirinth you would like to play\nIt should be odd; %d <= height <= %d:\n", heightParam, width);
			scanf("%d", &height);
		}
	}

	width = width * 2 + 3;
	height = height * 2 + 3;
	field = (char**)malloc((width) * sizeof(char*));
	cameFrom = (char**)malloc((width) * sizeof(char*));
	for (int i = 0; i < width; i++) {
		if (field) field[i] = (char*)malloc((height) * sizeof(char));
		if (cameFrom) cameFrom[i] = (char*)malloc((height) * sizeof(char));
	}
	secondsGame = time(NULL);
}
//при запуске
void Setup() {
	if (choice == 1) {
		free(field);
		free(cameFrom);
		field = (char**)malloc((width) * sizeof(char*));
		cameFrom = (char**)malloc((width) * sizeof(char*));
		for (int i = 0; i < width; i++) {
			field[i] = (char*)malloc(height * sizeof(char));
			cameFrom[i] = (char*)malloc(height * sizeof(char));
		}

		levelComplete = false;
		x = width / 2;
		y = height / 2;
		buildX = x;
		buildY = y;
		stop = 0;
		buildComplete = 0;
		secondsLevel = time(NULL);

		srand(time(0));
		exitCoordsSaved = exitCoords = rand() % ((width - 3) + (height - 3)) + 1;

		for (int i = 0; i < width; i++) {
			*(*(field + 0) + i) = static_cast<char>(32);
			*(*(field + 1) + i) = static_cast<char>(219);
		}
		*(*(field + 1) + 0) = static_cast<char>(32);
		*(*(field + 1) + (width - 1)) = static_cast<char>(32);

		for (int i = 2; i < height - 2; i++) {
			for (int j = 0; j < width; j++) {
				*(*(cameFrom + i) + j) = static_cast<char>(32);
				if (j == 1 || j == width - 2)
					*(*(field + i) + j) = static_cast<char>(219);
				else if (j == 0 || j == width - 1)
					*(*(field + i) + j) = static_cast<char>(32);
				else if (i == y && j == x)
					*(*(field + i) + j) = static_cast<char>(32);
				else if (i == exitY && j == exitX)
					*(*(field + i) + j) = static_cast<char>(69);
				else {
					cout << static_cast<char>(219);
					*(*(field + i) + j) = static_cast<char>(219);
				}
			}
		}

		for (int i = 0; i < width; i++) {
			*(*(field + (height - 2)) + i) = static_cast<char>(219);
			*(*(field + (height - 1)) + i) = static_cast<char>(32);
		}
		*(*(field + (height - 2)) + 0) = static_cast<char>(32);
		*(*(field + (height - 2)) + (width - 1)) = static_cast<char>(32);
		//-------------------------
		if (exitCoords <= ((width - 3) / 2)) {
			*(*(field + 1) + (exitCoords * 2)) = static_cast<char>(32);
			exitX = exitCoords * 2;
			exitY = 0;
		}
		else if (exitCoords <= ((width - 3) / 2) + ((height - 3) / 2)) {
			exitCoords -= ((width - 3) / 2);
			*(*(field + (exitCoords * 2)) + (width - 2)) = static_cast<char>(32);
			exitX = width - 1;
			exitY = exitCoords * 2;
		}
		else if (exitCoords <= (width - 3 + ((height - 3) / 2))) {
			exitCoords -= (((width - 3) / 2) + ((height - 3) / 2));
			*(*(field + (height - 2)) + (exitCoords * 2)) = static_cast<char>(32);
			exitX = exitCoords * 2;
			exitY = height - 1;
		}
		else if (exitCoords <= ((width - 3) + (height - 3))) {
			exitCoords -= ((width - 3) + ((height - 3) / 2));
			*(*(field + (exitCoords * 2)) + 1) = static_cast<char>(32);
			exitX = 0;
			exitY = exitCoords * 2;
		}
		//-------------------------

		while (buildComplete != 1) {
			i++;
			srand(time(0) - i);
			buildDir = rand() % 4;

			switch (buildDir) {
				//верх
			case 0:
				//если верх недоступен
				if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
					srand(time(0) - i);
					buildDir2 = rand() % 2;
					//если низ недоступен
					if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
						//если лево недоступно
						if (*(*(field + buildY) + (buildX - 2)) == static_cast<char>(32)) {
							//если право недоступно
							if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
								stop = 1;
							}
							//иначе построить право
							else {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
						}
						// если право недоступно построить лево
						else if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
							buildX = buildX - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirRight;
						}
						// иначе либо право либо лево
						else {
							if (buildDir2 == 0) {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
							else {
								buildX = buildX - 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirRight;
							}
						}
					}
					// Если низ все таки доступен, но недоступно лево
					else if (*(*(field + buildY) + (buildX - 2)) == static_cast<char>(32)) {
						//если право недоступно
						if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
							buildY = buildY + 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirUp;
						}
						// иначе либо право либо низ
						else {
							if (buildDir2 == 0) {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
							else {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
						}
					}
					// низ доступен, недоступно право
					// либо низ, либо лево
					else {
						if (buildDir2 == 0) {
							buildX = buildX - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirRight;
						}
						else {
							buildY = buildY + 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirUp;
						}
					}
				}

				//иначе построить путь наверх
				else {
					buildY = buildY - 2;
					*(*(field + buildY) + buildX) = static_cast<char>(32);
					*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
					*(*(cameFrom + buildY) + buildX) = buildDirDown;
				}
				break;
				//-------
				//право
			case 1:
				//если право недоступно
				if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
					srand(time(0) - i);
					buildDir2 = rand() % 2;
					//если лево недоступно
					if (*(*(field + buildY) + (buildX - 2)) == static_cast<char>(32)) {
						//если верх недоступен
						if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
							//если низ недоступен
							if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
								stop = 1;
							}
							//иначе построить низ
							else {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
						}
						// если низ недоступен построить верх
						else if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
							buildY = buildY - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirDown;
						}
						// иначе либо низ либо верх
						else {
							if (buildDir2 == 0) {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
							else {
								buildY = buildY - 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirDown;
							}
						}
					}
					// Если лево все таки доступно, но недоступен верх
					else if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
						//если низ недоступен
						if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
							buildX = buildX - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirRight;
						}
						// иначе либо низ либо лево
						else {
							if (buildDir2 == 0) {
								buildX = buildX - 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirRight;
							}
							else {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
						}
					}
					// лево доступно, недоступен низ
					// либо лево, либо верх
					else {
						if (buildDir2 == 0) {
							buildX = buildX - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirRight;
						}
						else {
							buildY = buildY - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirDown;
						}
					}
				}
				else {
					buildX = buildX + 2;
					*(*(field + buildY) + buildX) = static_cast<char>(32);
					*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
					*(*(cameFrom + buildY) + buildX) = buildDirLeft;
				}
				break;
				//-------
				//низ
			case 2:
				//если низ недоступен
				if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
					srand(time(0) - i);
					buildDir2 = rand() % 2;
					//если верх недоступен
					if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
						//если лево недоступно
						if (*(*(field + buildY) + (buildX - 2)) == static_cast<char>(32)) {
							//если право недоступно
							if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
								stop = 1;
							}
							//иначе построить право
							else {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
						}
						// если право недоступно построить лево
						else if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
							buildX = buildX - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirRight;
						}
						// иначе либо право либо лево
						else {
							if (buildDir2 == 0) {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
							else {
								buildX = buildX - 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirRight;
							}
						}
					}
					// Если верх все таки доступен, но недоступно лево
					else if (*(*(field + buildY) + (buildX - 2)) == static_cast<char>(32)) {
						//если право недоступно
						if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
							buildY = buildY - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirDown;
						}
						// иначе либо право либо верх
						else {
							if (buildDir2 == 0) {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
							else {
								buildY = buildY - 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirDown;
							}
						}
					}
					// верх доступен, недоступно право
					// либо верх, либо лево
					else {
						if (buildDir2 == 0) {
							buildX = buildX - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirRight;
						}
						else {
							buildY = buildY - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirDown;
						}
					}
				}
				else {
					buildY = buildY + 2;
					*(*(field + buildY) + buildX) = static_cast<char>(32);
					*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
					*(*(cameFrom + buildY) + buildX) = buildDirUp;
				}
				break;

				//-------
				//лево
			case 3:
				//если лево недоступно
				if (*(*(field + buildY) + (buildX - 2)) == static_cast<char>(32)) {
					srand(time(0) - i);
					buildDir2 = rand() % 2;
					//если право недоступно
					if (*(*(field + buildY) + (buildX + 2)) == static_cast<char>(32)) {
						//если верх недоступен
						if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
							//если низ недоступен
							if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
								stop = 1;
							}
							// иначе построить низ
							else {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
						}
						// если низ недоступен построить верх
						else if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
							buildY = buildY - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirDown;
						}
						// иначе либо низ либо верх
						else {
							if (buildDir2 == 0) {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
							else {
								buildY = buildY - 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirDown;
							}
						}
					}
					// Если право все таки доступно, но недоступен верх
					else if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(32)) {
						//если низ недоступен
						if (*(*(field + (buildY + 2)) + buildX) == static_cast<char>(32)) {
							buildX = buildX + 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirLeft;
						}
						// иначе либо низ либо право
						else {
							if (buildDir2 == 0) {
								buildX = buildX + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirLeft;
							}
							else {
								buildY = buildY + 2;
								*(*(field + buildY) + buildX) = static_cast<char>(32);
								*(*(field + (buildY - 1)) + buildX) = static_cast<char>(32);
								*(*(cameFrom + buildY) + buildX) = buildDirUp;
							}
						}
					}
					// право доступно, недоступен низ
					// либо право, либо верх
					else {
						if (buildDir2 == 0) {
							buildX = buildX + 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + buildY) + (buildX - 1)) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirLeft;
						}
						else {
							buildY = buildY - 2;
							*(*(field + buildY) + buildX) = static_cast<char>(32);
							*(*(field + (buildY + 1)) + buildX) = static_cast<char>(32);
							*(*(cameFrom + buildY) + buildX) = buildDirDown;
						}
					}
				}
				else {
					buildX = buildX - 2;
					*(*(field + buildY) + buildX) = static_cast<char>(32);
					*(*(field + buildY) + (buildX + 1)) = static_cast<char>(32);
					*(*(cameFrom + buildY) + buildX) = buildDirRight;
				}
				break;
			default:
				break;
			}

			if (stop == 1) {
				//если пришёл сверху
				if (*(*(cameFrom + buildY) + buildX) == buildDirUp)
					buildY = buildY - 2;
				//если пришёл справа
				else if (*(*(cameFrom + buildY) + buildX) == buildDirRight)
					buildX = buildX + 2;
				//если пришёл снизу
				else if (*(*(cameFrom + buildY) + buildX) == buildDirDown)
					buildY = buildY + 2;
				//если пришёл слева
				else
					buildX = buildX - 2;
				if (*(*(field + (buildY - 2)) + buildX) == static_cast<char>(219) || *(*(field + (buildY + 2)) + buildX) == static_cast<char>(219) || *(*(field + buildY) + (buildX - 2)) == static_cast<char>(219) || *(*(field + buildY) + (buildX + 2)) == static_cast<char>(219))
					stop = 0;
			}
			if (buildX == (width / 2) && buildY == (height / 2))
				buildComplete = 1;
		}

		for (int k = 0; k < (((width - 3) / 2 + (height - 3) / 2) / 2); k++) {
			srand(time(0) + k);
			breakX = rand() % ((width - 3) / 2) + 1;
			breakY = rand() % (((height - 3) / 2) - 1) + 1;
			*(*(field + (breakY * 2 + 1)) + (breakX * 2)) = static_cast<char>(32);
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (i == y && j == x)
					*(*(field + i) + j) = static_cast<char>(48);
				if (i == exitY && j == exitX)
					*(*(field + i) + j) = static_cast<char>(69);
			}
		}
	}
	if (choice == 2) {
		free(field);
		levelComplete = false;

		std::ifstream SavedLabyrints("SavedLabyrints.txt");
		if (!SavedLabyrints.is_open()) {
			std::cerr << "Can't open input file!" << std::endl;
			exit(1);
		}

		SavedLabyrints >> width >> height >> exitCoords;
		if (SavedLabyrints.bad()) {
			std::cerr << "Error while reading file!" << std::endl;
			exit(1);
		}

		if (!height || !width || !exitCoords) {
			std::cerr << "Wrong data!" << std::endl;
			exit(1);
		}

		matrixSize = width * height;
		int** D;
		D = (int**)malloc(matrixSize * sizeof(int*));
		for (int i = 0; i < matrixSize; i++) {
			D[i] = (int*)malloc(matrixSize * sizeof(int));
			for (int j = 0; j < matrixSize; j++) {
				D[i][j] = 0;
			}
		}

		for (int i = 0; i < matrixSize; i++) {
			for (int j = 0; j < matrixSize; j++) {
				SavedLabyrints >> D[i][j];
				if (SavedLabyrints.bad()) {
					std::cerr << "Error while reading file!" << std::endl;
					exit(1);
				}
			}
		}

		width = width * 2 + 3;
		height = height * 2 + 3;
		x = width / 2;
		y = height / 2;
		field = (char**)malloc((width) * sizeof(char*));
		for (int i = 0; i < width; i++) {
			field[i] = (char*)malloc(height * sizeof(char));
		}

		for (int i = 0; i < width; i++) {
			*(*(field + 0) + i) = static_cast<char>(32);
			*(*(field + 1) + i) = static_cast<char>(219);
		}
		*(*(field + 1) + 0) = static_cast<char>(32);
		*(*(field + 1) + (width - 1)) = static_cast<char>(32);

		for (int i = 2; i < height - 2; i++) {
			for (int j = 0; j < width; j++) {
				if (j == 1 || j == width - 2)
					*(*(field + i) + j) = static_cast<char>(219);
				else if (j == 0 || j == width - 1)
					*(*(field + i) + j) = static_cast<char>(32);
				else if (i == y && j == x)
					*(*(field + i) + j) = static_cast<char>(32);
				else if (i == exitY && j == exitX)
					*(*(field + i) + j) = static_cast<char>(69);
				else {
					*(*(field + i) + j) = static_cast<char>(219);
				}
			}
		}

		for (int i = 0; i < width; i++) {
			*(*(field + (height - 2)) + i) = static_cast<char>(219);
			*(*(field + (height - 1)) + i) = static_cast<char>(32);
		}
		*(*(field + (height - 2)) + 0) = static_cast<char>(32);
		*(*(field + (height - 2)) + (width - 1)) = static_cast<char>(32);
		//-------------------------
		if (exitCoords <= ((width - 3) / 2)) {
			*(*(field + 1) + (exitCoords * 2)) = static_cast<char>(32);
			exitX = exitCoords * 2;
			exitY = 0;
		}
		else if (exitCoords <= ((width - 3) / 2) + ((height - 3) / 2)) {
			exitCoords -= ((width - 3) / 2);
			*(*(field + (exitCoords * 2)) + (width - 2)) = static_cast<char>(32);
			exitX = width - 1;
			exitY = exitCoords * 2;
		}
		else if (exitCoords <= (width - 3 + ((height - 3) / 2))) {
			exitCoords -= (((width - 3) / 2) + ((height - 3) / 2));
			*(*(field + (height - 2)) + (exitCoords * 2)) = static_cast<char>(32);
			exitX = exitCoords * 2;
			exitY = height - 1;
		}
		else if (exitCoords <= ((width - 3) + (height - 3))) {
			exitCoords -= ((width - 3) + ((height - 3) / 2));
			*(*(field + (exitCoords * 2)) + 1) = static_cast<char>(32);
			exitX = 0;
			exitY = exitCoords * 2;
		}



		for (int i = 2; i < height - 2; i += 2) {
			for (int j = 2; j < width - 2; j += 2) {
				if (D[cell][cell + 1] == 1) {
					field[i][j + 1] = static_cast<char>(32);
					field[i][j + 2] = static_cast<char>(32);
					field[i][j] = static_cast<char>(32);
				}
				if (D[cell][cell + ((width - 3) / 2)] == 1) {
					field[i + 1][j] = static_cast<char>(32);
					field[i + 2][j] = static_cast<char>(32);
					field[i][j] = static_cast<char>(32);
				}
				cell++;
			}
		}
		field[height / 2][width / 2] = static_cast<char>(48);
		field[exitY][exitX] = static_cast<char>(69);

		cell = 0;
		choice = 1;
		SavedLabyrints.close();
		free(D);
	}
	system("cls");
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cout << *(*(field + i) + j);
		}
		cout << endl;
	}

	matrixSize = ((height - 3) / 2) * ((width - 3) / 2);

	printf(" Previous level completed in %lld seconds", secondsLevelEnd);
	printf("\n Levels completed - %d", score);
	printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
	printf("\n\n 'M' - return to  Main Menu");
	printf("\n\n 'T' - save labyrinth");
	printf("\n\n 'Y' - show path threw labyrinth");
}

// Считывание действий пользователя
void Input() {
	if (_kbhit()) {
		switch (_getch()) {
		case 'a':
			if ((timeForGame - (currentTime - secondsGame)) > 0 && !block) {
				if (*(*(field + y) + (x - 1)) == static_cast<char>(32) || *(*(field + y) + (x - 1)) == static_cast<char>(31)) {
					*(*(field + y) + x) = static_cast<char>(32);
					*(*(field + y) + (x - 1)) = static_cast<char>(32);
					x = x - 2;
					*(*(field + y) + x) = static_cast<char>(48);
				}

				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (*(*(field + i) + j) == static_cast<char>(31)) {
							SetConsoleTextAttribute(hConsole, 4);
							cout << *(*(field + i) + j);
							SetConsoleTextAttribute(hConsole, 15);
						}
						else cout << *(*(field + i) + j);
					}
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");
			}
			else if (block && a == static_cast<char>(50)) {
				system("cls");
				for (i = 9; i >= 0; i--) {
					printf("%s - %s\n", Unit[i].Name, Unit[i].Score);
				}
				printf("\n\n 'M' - return to  Main Menu");
			}
			else if (block && a == static_cast<char>(51)) {
				system("cls");
				LastGame = fopen("LastGame.txt", "r");
				fscanf(LastGame, "%s%s", fileScore, fileName);
				printf(" Last game result:\n %s - %s", fileScore, fileName);
				fclose(LastGame);
				printf("\n\n 'M' - return to  Main Menu");
			}
			else {
				system("cls");
				printf("Game over\n");
				printf("Enter your name (3-15 symbols) > ");
				scanf("%s", &currentName);
				while (strlen(currentName) > 15 || strlen(currentName) < 3) {
					system("cls");
					printf("Name is too short or too long, try another (3-15 symbols)  > \n");
					scanf("%s", &currentName);
				}
				LastGame = fopen("LastGame.txt", "w");
				*fileScore = score + '0';
				fprintf(LastGame, "%s\n%s", fileScore, currentName);
				fclose(LastGame);
				system("cls");
				printf("Press 'M' to continue");
			}
			break;
			//----------------
		case 'd':
			if ((timeForGame - (currentTime - secondsGame)) > 0 && !block) {
				if (*(*(field + y) + (x + 1)) == static_cast<char>(32) || *(*(field + y) + (x + 1)) == static_cast<char>(31)) {
					*(*(field + y) + x) = static_cast<char>(32);
					*(*(field + y) + (x + 1)) = static_cast<char>(32);
					x = x + 2;
					*(*(field + y) + x) = static_cast<char>(48);
				}

				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (*(*(field + i) + j) == static_cast<char>(31)) {
							SetConsoleTextAttribute(hConsole, 4);
							cout << *(*(field + i) + j);
							SetConsoleTextAttribute(hConsole, 15);
						}
						else cout << *(*(field + i) + j);
					}
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");
			}
			else if (block && a == static_cast<char>(50)) {
				system("cls");
				for (i = 9; i >= 0; i--) {
					printf("%s - %s\n", Unit[i].Name, Unit[i].Score);
				}
				printf("\n\n 'M' - return to  Main Menu");
			}
			else if (block && a == static_cast<char>(51)) {
				system("cls");
				LastGame = fopen("LastGame.txt", "r");
				fscanf(LastGame, "%s%s", fileScore, fileName);
				printf(" Last game result:\n %s - %s", fileScore, fileName);
				fclose(LastGame);
				printf("\n\n 'M' - return to  Main Menu");
			}
			else {
				system("cls");
				printf("Game over\n");
				printf("Enter your name (3-15 symbols) > ");
				scanf("%s", &currentName);
				while (strlen(currentName) > 15 || strlen(currentName) < 3) {
					system("cls");
					printf("Name is too short or too long, try another (3-15 symbols)  > \n");
					scanf("%s", &currentName);
				}
				LastGame = fopen("LastGame.txt", "w");
				*fileScore = score + '0';
				fprintf(LastGame, "%s\n%s", fileScore, currentName);
				fclose(LastGame);
				system("cls");
				printf("Press 'M' to continue");
			}
			break;
			//----------------
		case 'w':
			if ((timeForGame - (currentTime - secondsGame)) > 0 && !block) {
				if (*(*(field + (y - 1)) + x) == static_cast<char>(32) || *(*(field + (y - 1)) + x) == static_cast<char>(31)) {
					*(*(field + y) + x) = static_cast<char>(32);
					*(*(field + (y - 1)) + x) = static_cast<char>(32);
					y = y - 2;
					*(*(field + y) + x) = static_cast<char>(48);
				}

				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (*(*(field + i) + j) == static_cast<char>(31)) {
							SetConsoleTextAttribute(hConsole, 4);
							cout << *(*(field + i) + j);
							SetConsoleTextAttribute(hConsole, 15);
						}
						else cout << *(*(field + i) + j);
					}
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");
			}
			else if (block && a == static_cast<char>(50)) {
				system("cls");
				for (i = 9; i >= 0; i--) {
					printf("%s - %s\n", Unit[i].Name, Unit[i].Score);
				}
				printf("\n\n 'M' - return to  Main Menu");
			}
			else if (block && a == static_cast<char>(51)) {
				system("cls");
				LastGame = fopen("LastGame.txt", "r");
				fscanf(LastGame, "%s%s", fileScore, fileName);
				printf(" Last game result:\n %s - %s", fileScore, fileName);
				fclose(LastGame);
				printf("\n\n 'M' - return to  Main Menu");
			}
			else {
				system("cls");
				printf("Game over\n");
				printf("Enter your name (3-15 symbols) > ");
				scanf("%s", &currentName);
				while (strlen(currentName) > 15 || strlen(currentName) < 3) {
					system("cls");
					printf("Name is too short or too long, try another (3-15 symbols)  > \n");
					scanf("%s", &currentName);
				}
				LastGame = fopen("LastGame.txt", "w");
				*fileScore = score + '0';
				fprintf(LastGame, "%s\n%s", fileScore, currentName);
				fclose(LastGame);
				system("cls");
				printf("Press 'M' to continue");
			}
			break;
			//-----------------------
		case 's':
			if ((timeForGame - (currentTime - secondsGame)) > 0 && !block) {
				if (*(*(field + (y + 1)) + x) == static_cast<char>(32) || *(*(field + (y + 1)) + x) == static_cast<char>(31)) {
					*(*(field + y) + x) = static_cast<char>(32);
					*(*(field + (y + 1)) + x) = static_cast<char>(32);
					y = y + 2;
					*(*(field + y) + x) = static_cast<char>(48);
				}

				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (*(*(field + i) + j) == static_cast<char>(31)) {
							SetConsoleTextAttribute(hConsole, 4);
							cout << *(*(field + i) + j);
							SetConsoleTextAttribute(hConsole, 15);
						}
						else cout << *(*(field + i) + j);
					}
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");
			}
			else if (block && a == static_cast<char>(50)) {
				system("cls");
				for (i = 9; i >= 0; i--) {
					printf("%s - %s\n", Unit[i].Name, Unit[i].Score);
				}
				printf("\n\n 'M' - return to  Main Menu");
			}
			else if (block && a == static_cast<char>(51)) {
				system("cls");
				LastGame = fopen("LastGame.txt", "r");
				fscanf(LastGame, "%s%s", fileScore, fileName);
				printf(" Last game result:\n %s - %s", fileScore, fileName);
				fclose(LastGame);
				printf("\n\n 'M' - return to  Main Menu");
			}
			else {
				system("cls");
				printf("Game over\n");
				printf("Enter your name (3-15 symbols) > ");
				scanf("%s", &currentName);
				while (strlen(currentName) > 15 || strlen(currentName) < 3) {
					system("cls");
					printf("Name is too short or too long, try another (3-15 symbols)  > \n");
					scanf("%s", &currentName);
				}
				LastGame = fopen("LastGame.txt", "w");
				*fileScore = score + '0';
				fprintf(LastGame, "%s\n%s", fileScore, currentName);
				fclose(LastGame);
				system("cls");
				printf("Press 'M' to continue");
			}
			break;


		case 't':
			if (yIsOn % 2 != 0) {
				// матрица смежности
				int** D;
				D = (int**)malloc(matrixSize * sizeof(int*));
				for (int i = 0; i < matrixSize; i++) {
					D[i] = (int*)malloc(matrixSize * sizeof(int));
					for (int j = 0; j < matrixSize; j++) {
						D[i][j] = 0;
					}
				}
				for (int i = 2; i < height - 2; i += 2) {
					for (int j = 2; j < width - 2; j += 2) {
						if (field[i][j + 1] == static_cast<char>(32) && field[i][j + 2] != static_cast<char>(69)) {
							D[cell][cell + 1] = 1;
							D[cell + 1][cell] = 1;
						}
						if (field[i + 1][j] == static_cast<char>(32) && field[i + 2][j] != static_cast<char>(69)) {
							D[cell][cell + ((width - 3) / 2)] = 1;
							D[cell + ((width - 3) / 2)][cell] = 1;
						}
						cell++;
					}
				}
				// обход
				v = (((width - 3) / 2) * ((y / 2) - 1)) + (x / 2) - 1;// начинаем поиск пути от расположения игрока

				int vExit; // находим ячеёку с выходом
				if (exitX == 0) vExit = ((width - 3) / 2) * ((exitY / 2) - 1) + 1;
				else if (exitX == width - 1) vExit = (exitY / 2) * ((width - 3) / 2);
				else if (exitY == 0) vExit = (exitX / 2);
				else vExit = ((width - 3) / 2) * (((height - 3) / 2) - 1) + (exitX / 2);

				int* vis;
				vis = (int*)malloc(matrixSize * sizeof(int));
				for (int i = 0; i < matrixSize; i++) {
					vis[i] = 0;
				}

				BFS(v, vis, matrixSize, D, vExit);

				// запись в файл

				std::ofstream SavedLabyrintsOut("SavedLabyrints.txt");
				if (!SavedLabyrintsOut.is_open()) {
					std::cerr << "Can't open output file!" << std::endl;
					exit(1);
				}

				SavedLabyrintsOut << ((width - 3) / 2) << ' ' << ((height - 3) / 2) << ' ' << exitCoordsSaved << '\n';
				for (int i = 0; i < matrixSize; i++) {
					for (int j = 0; j < matrixSize; j++) {
						SavedLabyrintsOut << D[i][j] << ((j < matrixSize - 1) ? ' ' : '\n');;
						if (SavedLabyrintsOut.bad()) {
							std::cerr << "Error while writing data!" << std::endl;
							exit(1);
						}
					}
				}
				SavedLabyrintsOut << "\n";
				for (int i = 0; i < matrixSize; i++)
					SavedLabyrintsOut << setfill(' ') << setw(3) << i << "|";
				SavedLabyrintsOut << "\n";
				for (int i = 0; i < matrixSize; i++)
					SavedLabyrintsOut << setfill(' ') << setw(3) << vis[i] << "|";
				SavedLabyrintsOut.close();

				//
				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (*(*(field + i) + j) == static_cast<char>(31)) {
							SetConsoleTextAttribute(hConsole, 4);
							cout << *(*(field + i) + j);
							SetConsoleTextAttribute(hConsole, 15);
						}
						else cout << *(*(field + i) + j);
					}
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");

				cell = 0;

				free(vis);
				free(D);
			}
			else {
				printf("\n TURN OFF 'path threw labyrinth' TO SAVE THIS MAP");

			}
			break;


		case 'y':
			yIsOn++;
			if (yIsOn % 2 == 0) {
				int** D;
				D = (int**)malloc(matrixSize * sizeof(int*));
				for (int i = 0; i < matrixSize; i++) {
					D[i] = (int*)malloc(matrixSize * sizeof(int));
					for (int j = 0; j < matrixSize; j++) {
						D[i][j] = 0;
					}
				}

				for (int i = 2; i < height - 2; i += 2) {
					for (int j = 2; j < width - 2; j += 2) {
						if (field[i][j + 1] == static_cast<char>(32) && field[i][j + 2] != static_cast<char>(69)) {
							D[cell][cell + 1] = 1;
							D[cell + 1][cell] = 1;
						}
						if (field[i + 1][j] == static_cast<char>(32) && field[i + 2][j] != static_cast<char>(69)) {
							D[cell][cell + ((width - 3) / 2)] = 1;
							D[cell + ((width - 3) / 2)][cell] = 1;
						}
						cell++;
					}
				}
				cell = 0;


				v = (((width - 3) / 2) * ((y / 2) - 1)) + (x / 2) - 1;// начинаем поиск пути от расположения игрока

				int vExit; // находим ячеёку с выходом
				if (exitX == 0) vExit = ((width - 3) / 2) * ((exitY / 2) - 1) + 1;
				else if (exitX == width - 1) vExit = (exitY / 2) * ((width - 3) / 2);
				else if (exitY == 0) vExit = (exitX / 2);
				else vExit = ((width - 3) / 2) * (((height - 3) / 2) - 1) + (exitX / 2);

				int* vis;
				vis = (int*)malloc(matrixSize * sizeof(int));
				for (int i = 0; i < matrixSize; i++) {
					vis[i] = 0;
				}

				BFS(v, vis, matrixSize, D, vExit);

				// Находим ячейку около выхода и продвигаемся от неё к месту, где находится игрок
				//Построение пути
				int i, j;
				printf("\n\n\n");
				while ((v + 1) != vExit) {
					j = ((vExit - 1) % ((width - 3) / 2)) * 2 + 2;
					i = ((vExit - 1) / ((width - 3) / 2)) * 2 + 2;
					//если от нас справа ближе и справа есть проход
					if (vis[vExit - 1] == (vis[vExit] + 1) && field[i][j + 1] == static_cast<char>(32) && field[i][j + 2] != static_cast<char>(69)) {
						field[i][j] = static_cast<char>(31);
						field[i][j + 1] = static_cast<char>(31);
						j += 2;
						vExit += 1;
						printf("built");
					}
					// слева
					else if (vis[vExit - 1] == (vis[vExit - 2] + 1) && field[i][j - 1] == static_cast<char>(32) && field[i][j - 2] != static_cast<char>(69)) {
						field[i][j] = static_cast<char>(31);
						field[i][j - 1] = static_cast<char>(31);
						j -= 2;
						vExit -= 1;
						printf("built");
					}
					//сверху
					else if (vis[vExit - 1] == (vis[(vExit - 1) - ((width - 3) / 2)] + 1) && field[i - 1][j] == static_cast<char>(32) && field[i - 2][j] != static_cast<char>(69)) {
						field[i][j] = static_cast<char>(31);
						field[i - 1][j] = static_cast<char>(31);
						i -= 2;
						vExit = vExit - ((width - 3) / 2);
						printf("built");
					}
					//снизу
					else if (vis[vExit - 1] == (vis[(vExit - 1) + ((width - 3) / 2)] + 1) && field[i + 1][j] == static_cast<char>(32) && field[i + 2][j] != static_cast<char>(69)) {
						field[i][j] = static_cast<char>(31);
						field[i + 1][j] = static_cast<char>(31);
						i += 2;
						vExit = vExit + ((width - 3) / 2);
						printf("built");
					}
				}

				//---

				//вывод на экран
				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (*(*(field + i) + j) == static_cast<char>(31)) {
							SetConsoleTextAttribute(hConsole, 4);
							cout << *(*(field + i) + j);
							SetConsoleTextAttribute(hConsole, 15);
						}
						else cout << *(*(field + i) + j);
					}
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");

				free(vis);
				free(D);
			}//нарисовать путь
			else {
				for (int i = 2; i < height - 2; i++) {
					for (int j = 2; j < width - 2; j++)
						if (*(*(field + i) + j) == static_cast<char>(31)) *(*(field + i) + j) = static_cast<char>(32);
				}
				system("cls");
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++)
						cout << *(*(field + i) + j);
					cout << endl;
				}
				printf(" Previous level completed in %lld seconds", secondsLevelEnd);
				printf("\n Levels completed - %d", score);
				printf("\n Time left - %lld", (timeForGame - (currentTime - secondsGame)));
				printf("\n\n 'M' - return to  Main Menu");
				printf("\n\n 'T' - save labyrinth");
				printf("\n\n 'Y' - show path threw labyrinth");
			}//убрать путь
			break;


		case 'm':
			block = false;
			a = static_cast<char>(48);

			//если счёт больше минимального в файле - начать перезапись
			// полностью перезаписать файл имеющимся массивом
			Rating = fopen("Rating.txt", "r+");
			for (i = 0; i < 10; i++) {
				fscanf(Rating, "%s%s", Unit[i].Score, Unit[i].Name);
				uScore = *Unit[i].Score - '0';
				if (uScore < score && i != 0) {
					strcpy(Unit[i - 1].Score, Unit[i].Score);
					*Unit[i].Score = score + '0';

					strcpy(name, Unit[i - 1].Name);
					strcpy(Unit[i - 1].Name, Unit[i].Name);
					strcpy(Unit[i].Name, name);
				}
				else if (uScore < score && i == 0) {
					*Unit[i].Score = score + '0';
					strcpy(Unit[i].Name, currentName);

				}
			}
			fclose(Rating);
			Rating = fopen("Rating.txt", "w");
			for (i = 0; i < 10; i++)
				fprintf(Rating, "%s\n%s\n", Unit[i].Score, Unit[i].Name);
			fclose(Rating);

			system("cls");
			printf("   ##       ####   #####   ##  ##  #####   ######  ##  ## ###### ##  ##  ##  ##   ##### \n");
			printf("   ##      ##  ##  ##  ##   ####   ##  ##    ##    ##  ##   ##   ##  ##  ##  ##  ##     \n");
			printf("   ##      ######  ####      ##    ####      ##    ### ##   ##   ######  ##  ##   ####  \n");
			printf("   ##      ##  ##  ##  ##   ##     ## ##     ##    ## ###   ##   ##  ##  ##  ##      ## \n");
			printf("   ######  ##  ##  #####   ##      ##  ##  ######  ##  ##   ##   ##  ##   ####   #####  \n");
			printf("\n\n");
			printf(" +---+                 +---+                      +---+\n");
			printf(" | 1 | - New game      | 2 | - Top 10 runs        | 3 | - Last game result\n");
			printf(" +---+                 +---+                      +---+\n");
			printf("\n - ");

			scanf("%s", &a);
			if (strlen(&a) > 1)
				a = static_cast<char>(55);


			while (a != static_cast<char>(49) && a != static_cast<char>(50) && a != static_cast<char>(51)) {
				system("cls");
				printf("   ##       ####   #####   ##  ##  #####   ######  ##  ## ###### ##  ##  ##  ##   ##### \n");
				printf("   ##      ##  ##  ##  ##   ####   ##  ##    ##    ##  ##   ##   ##  ##  ##  ##  ##     \n");
				printf("   ##      ######  ####      ##    ####      ##    ### ##   ##   ######  ##  ##   ####  \n");
				printf("   ##      ##  ##  ##  ##   ##     ## ##     ##    ## ###   ##   ##  ##  ##  ##      ## \n");
				printf("   ######  ##  ##  #####   ##      ##  ##  ######  ##  ##   ##   ##  ##   ####   #####  \n");
				printf("\n\n");
				printf(" +---+                 +---+                      +---+\n");
				printf(" | 1 | - New game      | 2 | - Top 10 runs        | 3 | - Last game result\n");
				printf(" +---+                 +---+                      +---+\n");
				printf("\nWrong command");
				printf("\n - ");
				a = static_cast<char>(50);
				scanf("%s", &a);
				if (strlen(&a) > 1) {
					a = static_cast<char>(55);
				}
			}
			if (a == static_cast<char>(49)) {
				a = static_cast<char>(48);
				secondsLevelEnd = 0;
				score = 0;
				PreSetup();
				secondsGame = time(NULL);
				Setup();
			}

			if (a == static_cast<char>(50)) {
				block = true;
				system("cls");
				Rating = fopen("Rating.txt", "r");
				for (i = 0; i < 10; i++) {
					fscanf(Rating, "%s%s", Unit[i].Score, Unit[i].Name);
				}
				for (i = 9; i >= 0; i--) {
					printf("%s - %s\n", Unit[i].Name, Unit[i].Score);
				}
				fclose(Rating);
				printf("\n\n 'M' - return to  Main Menu");
			}

			if (a == static_cast<char>(51)) {
				block = true;
				system("cls");
				LastGame = fopen("LastGame.txt", "r");
				fscanf(LastGame, "%s%s", fileScore, fileName);
				printf(" Last game result:\n %s - %s", fileScore, fileName);
				fclose(LastGame);
				printf("\n\n 'M' - return to  Main Menu");
			}

			break;
		case 'x':
			levelComplete = true;
			break;
		}

	}
}

//Логика игры
void Logic() {
	currentTime = time(NULL);
	if (x == exitX && y == exitY) {
		levelComplete = true;
		secondsLevelEnd = time(NULL);
		secondsLevelEnd -= secondsLevel;
		timeForGame += 2;
		score++;
		yIsOn = 1;
		Setup();
	}
}


int main() {
	PreSetup();
	Setup();
	while (c != 1) {
		Input();
		Logic();
	}
	return 0;
}