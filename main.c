#include "raylib.h"
#include <stdbool.h>

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
	{250, 400, 10, 200},
	{60, 100, 740, 10},
	{60, 100, 10, 250},
	{250, 160, 10, 240}
};

// PROPRIETES DES MURS DE NIVEAU 2
Rectangle level2walls[] = {
	{50, 250, 10, 100},
	{150, 200, 10, 100},
	{600, 50, 10, 500},
	{60, 100, 10, 250},
	{150, 260, 10, 340}
};

// PROPRIETES DES ENNEMIS DE NIVEAU 1
Enemy level1Enemies[] = {
	//startx, starty,   X,      Y,    size,   startspeed,  speed      movesOnX
	{300.0f, 530.0f, 300.0f, 530.0f, 20.0f,     200.0f,    200.0f,     true},
	{100.0f, 200.0f, 100.0f, 200.0f, 20.0f,     150.0f,    150.0f,     false}
};

// PROPRIETES DES ENNEMIS DE NIVEAU 2
Enemy level2Enemies[] = {
	//startx, starty,   X,      Y,    size,   startspeed,  speed      movesOnX
	{200.0f, 230.0f, 200.0f, 230.0f, 20.0f,     300.0f,    300.0f,     false},
	{100.0f, 200.0f, 100.0f, 200.0f, 20.0f,     250.0f,    250.0f,     false}
};

// PROPRIETES DU NIVEAU 1
Level level1 = {
	740.0f,
	40.0f,
	{100, 500, 55, 55},
	{750, 550, 30, 9},
	{175, 400, 75, 10},
	level1walls,
	sizeof(level1walls) / sizeof(level1walls[0]),
	level1Enemies,
	sizeof(level1Enemies) / sizeof(level1Enemies[0])
};

// PROPRIETES DU NIVEAU 2
Level level2 = {
	500.0f,
	200.0f,
	{600, 300, 55, 55},
	{150, 250, 30, 9},
	{35, 200, 75, 10},
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



void drawHUD(bool win, int wallCount, float timer, float bestTime)
{
	DrawText("Fleches : bouger", 50, 50, 10, BLACK);
	DrawText("R : recommencer", 50, 70, 10, BLACK);
	DrawText(TextFormat("Murs : %i", wallCount),  50, 90, 10, BLACK);
	DrawText(TextFormat("Temps : %.2f", timer),  50, 110, 10, BLACK);
	if (win)
	{
		DrawText("Victoire", 300, 250, 30, BLACK);
	}
	if (bestTime < 0.0f)
	{
		DrawText("Pas encore de meilleur temps", 50, 130, 10, BLACK);
	}
	else
	{	
		DrawText(TextFormat("Meilleur temps : %.2f", bestTime), 50, 130, 10, BLACK);
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

int main(void)
{
	// NIVEAU
	Level levels[] = {level1, level2};
	int levelCount = sizeof(levels) / sizeof(levels[0]);
	int currentLevelIndex = 0;
	Level *currentLevel = &levels[currentLevelIndex];

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

	// NIVEAU 1
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

	InitWindow(800, 600, "Mon premier jeu");
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		bool blocked = false;
		// ECRAN TITRE
		if (currentScreen == TITLE)
		{
			if (IsKeyPressed(KEY_ENTER))
			{
				currentScreen = PLAYING;
				continue;
			}
			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("MON PREMIER JEU", 250, 220, 30, BLACK);
			DrawText("Appuie sur ENTREE pour jouer", 220, 280, 20, DARKGRAY);
			EndDrawing();
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
			for (int i = 0; i < enemyCount; i++)
			{
				enemies[i].x = enemies[i].startX;
				enemies[i].y = enemies[i].startY;
				enemies[i].speed = enemies[i].startSpeed;
			}
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
					currentScreen = PLAYING;
					continue;
				}
			}
			if (IsKeyPressed(KEY_R))
			{
				restartLevel(&playerX, &playerY, playerStartX, playerStartY, &win, &timer, &hasKey);

				for (int i = 0; i < enemyCount; i++)
				{
					enemies[i].x = enemies[i].startX;
					enemies[i].y = enemies[i].startY;
					enemies[i].speed = enemies[i].startSpeed;
				}
				currentScreen = PLAYING;
				continue;
			}
			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("VICTOIRE !", 300, 220, 30, BLACK);
			DrawText(TextFormat("Temps : %.2f", timer), 300, 270, 20, DARKGRAY);
			DrawText("Appuie sur R pour rejouer", 240, 320, 20, DARKGRAY);
			EndDrawing();
			continue;
		}

		// ECRAN DE DEFAITE
		if (currentScreen == GAMEOVER)
		{
			if (IsKeyPressed(KEY_R))
			{
				restartLevel(&playerX, &playerY, playerStartX, playerStartY, &win, 										&timer, &hasKey);

				for (int i = 0; i < enemyCount; i++)
				{
					enemies[i].x = enemies[i].startX;
					enemies[i].y = enemies[i].startY;
					enemies[i].speed = enemies[i].startSpeed;
				}
				currentScreen = PLAYING;
				continue;
			}
			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("DEFAITE !", 300, 220, 30, BLACK);
			DrawText("Appuie sur R pour rejouer", 240, 320, 20, DARKGRAY);
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
		if (playerX > 800 - playerSize)
		{
			playerX = 800 - playerSize;
		}
		if (playerY > 600 - playerSize)
		{
			playerY = 600 - playerSize;
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

				if (enemies[i].x > 800 - enemies[i].size)
				{
					enemies[i].x = 800 - enemies[i].size;
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

				if (enemies[i].y > 600 - enemies[i].size)
				{
					enemies[i].y = 600 - enemies[i].size;
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
			for (int i = 0; i < enemyCount; i++)
			{
				enemies[i].x = enemies[i].startX;
				enemies[i].y = enemies[i].startY;
				enemies[i].speed = enemies[i].startSpeed;
			}
			playerRect = (Rectangle){playerX, playerY, playerSize, playerSize};
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
		drawHUD(win, wallCount, timer, bestTime);

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
