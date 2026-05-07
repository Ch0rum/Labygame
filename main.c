#include "raylib.h"
#include <stdbool.h>
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

// ENNEMIS
typedef struct Enemy {
	float startX;
	float startY;
	float x;
	float y;
	float size;
	float startSpeed;
	float speed;
	bool movesOnX;
} Enemy;

// NIVEAUX
typedef struct Level {
	float playerStartX;
	float playerStartY;
	Rectangle exitRect;
	Rectangle keyRect;
	Rectangle doorRect;
	Rectangle *walls;
	int wallCount;
	Enemy *enemies;
	int enemyCount;

} Level;

// PROPRIETES DES MURS DE NIVEAU 1
Rectangle level1walls[] = {
	{0, 400, 175, 10},
	{250, 200, 10, 600},
	{60, 100, 1340, 10},
	{60, 100, 10, 250},
};

// PROPRIETES DES MURS DE NIVEAU 2
Rectangle level2walls[] = {
	{85, 0, 10, 550},
	{145, 0, 10, 550},
	{205, 50, 10, 550},
	{205, 50, 525, 10},
	{730, 50, 10, 450},
	{340, 500, 400, 10},
	{340, 200, 10, 300},
	{340, 200, 300, 10},
	{540, 200, 10, 150},

};

// PROPRIETES DES ENNEMIS DE NIVEAU 1
Enemy level1Enemies[] = {
	//startx, starty,   X,      Y,    size,   startspeed,  speed      movesOnX
	{110.0f, 200.0f, 110.0f, 200.0f, 20.0f,     150.0f,    150.0f,     false}
};

// PROPRIETES DES ENNEMIS DE NIVEAU 2
Enemy level2Enemies[] = {
	{300.0f, 100.0f, 300.0f, 100.0f, 20.0f,     300.0f,    300.0f,     true},
	{110.0f, 200.0f, 110.0f, 200.0f, 20.0f,     200.0f,    200.0f,     false},
	{20.0f, 400.0f, 20.0f, 400.0f, 20.0f,     300.0f,    300.0f,     true},
};

// PROPRIETES DU NIVEAU 1
Level level1 = {
	740.0f,                     // PlayerStartX
	40.0f,                      // PlayerStartY
	{100, 500, 40, 40},         // exitRect
	{700, 400, 30, 9},	    // keyRect
	{175, 400, 75, 10},	    // doorRect
	level1walls,
	sizeof(level1walls) / sizeof(level1walls[0]),
	level1Enemies,
	sizeof(level1Enemies) / sizeof(level1Enemies[0])
};

// PROPRIETES DU NIVEAU 2
Level level2 = {
	40.0f,
	30.0f,
	{100, 30, 40, 40},
	{420, 280, 30, 9},
	{95, 70, 50, 10},
	level2walls,
	sizeof(level2walls) / sizeof(level2walls[0]),
	level2Enemies,
	sizeof(level2Enemies) / sizeof(level2Enemies[0])
};

// ETAT DE LA FENETRE
typedef enum GameScreen {
	TITLE,
	PLAYING,
	WIN,
	GAMEOVER
} GameScreen;

void loadLevel(Level *currentLevel, float *playerStartX, float *playerStartY, Rectangle *exitRect, int *wallCount, Rectangle **walls, Rectangle *keyRect, Rectangle *doorRect, Enemy **enemies, int *enemyCount)
{
	*playerStartX = currentLevel->playerStartX;
	*playerStartY = currentLevel->playerStartY;
	*exitRect = currentLevel->exitRect;
	*wallCount = currentLevel->wallCount;
	*walls = currentLevel->walls;
	*keyRect = currentLevel->keyRect;
	*doorRect = currentLevel->doorRect;
	*enemies = currentLevel->enemies;
	*enemyCount = currentLevel->enemyCount;
}

bool movePlayer(float *playerX, float *playerY, float playerSpeed, float deltaTime, float playerSize, Rectangle walls[], int wallCount, bool hasKey, Rectangle doorRect)
{
	float nextPlayerX = *playerX;
	float nextPlayerY = *playerY;

	bool blockedX = false;
	bool blockedY = false;

	if (IsKeyDown(KEY_RIGHT))
	{
		nextPlayerX = *playerX + playerSpeed * deltaTime;
	}

	if (IsKeyDown(KEY_LEFT))
	{
		nextPlayerX = *playerX - playerSpeed * deltaTime;
	}

	if (IsKeyDown(KEY_DOWN))
	{
		nextPlayerY = *playerY + playerSpeed * deltaTime;
	}

	if (IsKeyDown(KEY_UP))
	{
		nextPlayerY = *playerY - playerSpeed * deltaTime;
	}

	Rectangle testRectX = {nextPlayerX, *playerY, playerSize, playerSize};

	int i = 0;
	while (i < wallCount)
	{
		if (CheckCollisionRecs(testRectX, walls[i]))
		{
			blockedX = true;

			if (nextPlayerX > *playerX)
			{
				*playerX = walls[i].x - playerSize;
			}
			else if (nextPlayerX < *playerX)
			{
				*playerX = walls[i].x + walls[i].width;
			}

			break;
		}

		i++;
	}
	if (!hasKey && !blockedX && CheckCollisionRecs(testRectX, doorRect))
	{
		blockedX = true;

		if (nextPlayerX > *playerX)
		{
			*playerX = doorRect.x - playerSize;
		}
		else if (nextPlayerX < *playerX)
		{
			*playerX = doorRect.x + doorRect.width;
		}
	}

	if (!blockedX)
	{
		*playerX = nextPlayerX;
	}

	Rectangle testRectY = {*playerX, nextPlayerY, playerSize, playerSize};

	int j = 0;
	while (j < wallCount)
	{
		if (CheckCollisionRecs(testRectY, walls[j]))
		{
			blockedY = true;

			if (nextPlayerY > *playerY)
			{
				*playerY = walls[j].y - playerSize;
			}
			else if (nextPlayerY < *playerY)
			{
				*playerY = walls[j].y + walls[j].height;
			}

			break;
		}

		j++;
	}
	if (!hasKey && !blockedY && CheckCollisionRecs(testRectY, doorRect))
	{
		blockedY = true;

		if (nextPlayerY > *playerY)
		{
			*playerY = doorRect.y - playerSize;
		}
		else if (nextPlayerY < *playerY)
		{
			*playerY = doorRect.y + doorRect.height;
		}
	}

	if (!blockedY)
	{
		*playerY = nextPlayerY;
	}

	return blockedX || blockedY;
}



void drawHUD(float timer, float bestTime)
{
	DrawText(TextFormat("Temps : %.2fs", timer),  10, 20, 10, BLACK);
	if (bestTime < 0.0f)
	{
		DrawText("Pas encore de meilleur temps", 10, 40, 10, BLACK);
	}
	else
	{	
		DrawText(TextFormat("Meilleur temps : %.2f", bestTime), 10, 40, 10, BLACK);
	}
}

bool playerReachedExit(Rectangle playerRect, Rectangle exitRect)
{
	return CheckCollisionRecs(playerRect, exitRect);
}

void restartLevel(float *playerX, float *playerY, float playerStartX, float playerStartY, bool *win, float *timer, bool *hasKey)
{
	*playerX = playerStartX;
	*playerY = playerStartY;

	*win = false;
	*timer = 0.0f;
	*hasKey = false;
}

void resetEnemies(Enemy enemies[], int enemyCount)
{
	for (int i = 0; i < enemyCount; i++)
	{
		enemies[i].x = enemies[i].startX;
		enemies[i].y = enemies[i].startY;
		enemies[i].speed = enemies[i].startSpeed;
	}
}

int main(void)
{
	// NIVEAU
	Level levels[] = {level1, level2};
	int levelCount = sizeof(levels) / sizeof(levels[0]);
	int currentLevelIndex = 0;
	Level *currentLevel = &levels[currentLevelIndex];

	// SELECTION DU NIVEAU
	int selectedLevelIndex = 0;

	// JOUEUR
	float playerStartX;
	float playerStartY;
	float playerSpeed = 200.0f;
	float playerSize = 40.0f;

	// SORTIE
	Rectangle exitRect;

	// ETAT DU JEU
	bool win = false;
	GameScreen currentScreen = TITLE;
	float timer = 0.0f;
	float bestTime = -1.0f;

	// MURS
	Rectangle *walls;
	int wallCount;

	// CLE
	Rectangle keyRect;
	bool hasKey = false;

	// PORTE
	Rectangle doorRect;

	// ENNEMI
	Enemy *enemies;
	int enemyCount;


	// CHARGER UN NIVEAU
	loadLevel(currentLevel, &playerStartX, &playerStartY, &exitRect, &wallCount, &walls, &keyRect, &doorRect, &enemies, &enemyCount);

	float playerX = playerStartX;
	float playerY = playerStartY;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mon premier jeu");
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		bool blocked = false;
		// ECRAN TITRE
		if (currentScreen == TITLE)
		{
			if (IsKeyPressed(KEY_UP))
			{
				if (selectedLevelIndex > 0)
				{
					selectedLevelIndex--;
				}
			}

			if (IsKeyPressed(KEY_DOWN))
			{
				if (selectedLevelIndex < levelCount - 1)
				{
					selectedLevelIndex++;
				}
			}

			if (IsKeyPressed(KEY_ENTER))
			{
				currentLevelIndex = selectedLevelIndex;
				currentLevel = &levels[currentLevelIndex];

				loadLevel(currentLevel,
						&playerStartX,
						&playerStartY,
						&exitRect,
						&wallCount,
						&walls,
						&keyRect,
						&doorRect,
						&enemies,
						&enemyCount);

				playerX = playerStartX;
				playerY = playerStartY;
				win = false;
				timer = 0.0f;
				hasKey = false;
				resetEnemies(enemies, enemyCount);
				currentScreen = PLAYING;
				continue;
			}

			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("SYWO", 530, 40, 50, BLACK);
			DrawText("Fleches : Déplacements", 20, 20, 15, BLACK);
			DrawText("Entrée : Sélection du niveau", 20, 40, 15, BLACK);
			DrawText("R : Recommencer le niveau", 20, 60, 15, BLACK);
			DrawText("M : Retour au menu", 20, 80, 15, BLACK);

			for (int i = 0; i < levelCount; i++)
			{
				if (i == selectedLevelIndex)
				{
					DrawText(TextFormat("> Niveau %i", i + 1), 450, 150 + i * 30, 25, BLACK);
				}
				else
				{
					DrawText(TextFormat("  Niveau %i", i + 1), 450, 150 + i * 30, 25, DARKGRAY);
				}
			}

			EndDrawing();
			continue;
		}
		if (currentScreen != TITLE && IsKeyPressed(KEY_M))
		{
			selectedLevelIndex = currentLevelIndex;
			currentScreen = TITLE;
			continue;
		}		

		// ECRAN DE JEU
		if (currentScreen == PLAYING)
		{
			timer = timer + deltaTime;
			blocked = movePlayer(&playerX, &playerY, playerSpeed, deltaTime, playerSize, walls, wallCount, hasKey, doorRect);
		}
		if (currentScreen == PLAYING && IsKeyPressed(KEY_R))
		{
			restartLevel(&playerX, &playerY, playerStartX, playerStartY, &win, &timer, &hasKey);
			resetEnemies(enemies, enemyCount);
			blocked = false;
		}

		// ECRAN DE VICTOIRE
		if (currentScreen == WIN)
		{
			if (IsKeyPressed(KEY_ENTER))
			{
				if (currentLevelIndex + 1 < levelCount)
				{
					currentLevelIndex++;
					currentLevel = &levels[currentLevelIndex];

					loadLevel(currentLevel,
							&playerStartX,
							&playerStartY,
							&exitRect,
							&wallCount,
							&walls,
							&keyRect,
							&doorRect,
							&enemies,
							&enemyCount);

					playerX = playerStartX;
					playerY = playerStartY;
					win = false;
					timer = 0.0f;
					hasKey = false;
					resetEnemies(enemies, enemyCount);
					currentScreen = PLAYING;
					continue;
				}
			}

			if (IsKeyPressed(KEY_R))
			{
				restartLevel(&playerX, &playerY, playerStartX, playerStartY, &win, &timer, &hasKey);

				resetEnemies(enemies, enemyCount);
				currentScreen = PLAYING;
				continue;
			}

			if (IsKeyPressed(KEY_M))
			{
				selectedLevelIndex = currentLevelIndex;
				currentScreen = TITLE;
				continue;
			}

			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("VICTOIRE !", 480, 250, 40, BLACK);
			DrawText(TextFormat("Temps : %.2fs", timer), 510, 290, 25, RED);
			if (currentLevelIndex + 1 < levelCount)
			{
				DrawText("ENTREE : Niveau suivant", 460, 350, 25, DARKGRAY);
			}
			else
			{
				DrawText("Dernier niveau termine", 460, 350, 25, DARKGRAY);
			}
			DrawText("R : Recommencer le niveau", 460, 380, 25, DARKGRAY);
			DrawText("M : Retour au menu", 460, 410, 25, DARKGRAY);
			EndDrawing();
			continue;
		}

		// ECRAN DE DEFAITE
		if (currentScreen == GAMEOVER)
		{
			if (IsKeyPressed(KEY_R))
			{
				restartLevel(&playerX, &playerY, playerStartX, playerStartY, &win, &timer, &hasKey);

				resetEnemies(enemies, enemyCount);
				currentScreen = PLAYING;
				continue;
			}

			if (IsKeyPressed(KEY_M))
			{
				selectedLevelIndex = currentLevelIndex;
				currentScreen = TITLE;
				continue;
			}

			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("DEFAITE !", 480, 250, 40, BLACK);
			DrawText("R : Recommencer le niveau", 460, 320, 25, DARKGRAY);
			DrawText("M : Retour au menu", 460, 355, 25, DARKGRAY);
			EndDrawing();
			continue;
		}
		// LIMITE DE DEPLACEMENT DU JOUEUR A 800x600
		if (playerX < 0)
		{
			playerX = 0;
		}
		if (playerY < 0)
		{
			playerY = 0;
		}
		if (playerX > SCREEN_WIDTH - playerSize)
		{
			playerX = SCREEN_WIDTH - playerSize;
		}
		if (playerY > SCREEN_HEIGHT - playerSize)
		{
			playerY = SCREEN_HEIGHT - playerSize;
		}

		Rectangle playerRect = {playerX, playerY, playerSize, playerSize};

		// BOUCLE DES ENNEMIS
		for (int i = 0; i < enemyCount; i++)
		{
			if (enemies[i].movesOnX)
			{
				enemies[i].x = enemies[i].x + enemies[i].speed * deltaTime;

				if (enemies[i].x < 0)
				{
					enemies[i].x = 0;
					enemies[i].speed = -enemies[i].speed;
				}

				if (enemies[i].x > SCREEN_WIDTH - enemies[i].size)
				{
					enemies[i].x = SCREEN_WIDTH - enemies[i].size;
					enemies[i].speed = -enemies[i].speed;
				}
			}
			else
			{
				enemies[i].y = enemies[i].y + enemies[i].speed * deltaTime;

				if (enemies[i].y < 0)
				{
					enemies[i].y = 0;
					enemies[i].speed = -enemies[i].speed;
				}

				if (enemies[i].y > SCREEN_HEIGHT - enemies[i].size)
				{
					enemies[i].y = SCREEN_HEIGHT - enemies[i].size;
					enemies[i].speed = -enemies[i].speed;
				}
			}
		}

		bool hitEnemy = false;
		for (int i = 0; i < enemyCount; i++)
		{
			Rectangle enemyRect = {enemies[i].x, enemies[i].y, enemies[i].size,
				enemies[i].size};
			if (CheckCollisionRecs(playerRect, enemyRect))
			{
				hitEnemy = true;
			}
		}

		// COLLISION ENNEMI/JOUEUR
		if (hitEnemy)
		{
			resetEnemies(enemies, enemyCount);
			currentScreen = GAMEOVER;
		}


		if (currentScreen == PLAYING)
		{
			win = playerReachedExit(playerRect, exitRect);
			if (win)
			{
				currentScreen = WIN;
				if (bestTime < 0.0f || timer < bestTime)
				{
					bestTime = timer;
				}
				continue;
			}
		}
		if (!hasKey && CheckCollisionRecs(playerRect, keyRect))
		{
			hasKey = true;
		}

		Color playerColor;
		if (blocked)
		{
			playerColor = GREEN;
		}
		else
		{
			playerColor = RED;
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		for (int i = 0; i < wallCount; i++)
		{
			DrawRectangleRec(walls[i], BLUE);
		}
		for (int i = 0; i < enemyCount; i++)
		{		
			Rectangle enemyRect = {enemies[i].x, enemies[i].y, enemies[i].size, enemies[i].size};
			DrawRectangleRec(enemyRect, PURPLE);
		}
		DrawRectangleRec(exitRect, YELLOW);
		DrawRectangleRec(playerRect, playerColor);
		drawHUD(timer, bestTime);

		if (!hasKey)
		{
			DrawRectangleRec(doorRect, BROWN);
			DrawRectangleRec(keyRect, GOLD);
		}
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
