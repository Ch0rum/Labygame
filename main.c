#include "raylib.h"
#include <stdbool.h>

void drawHUD(bool win, int wallCount, float timer)
{
	DrawText("Fleches : bouger", 50, 50, 10, BLACK);
	DrawText("R : recommencer", 50, 70, 10, BLACK);
	DrawText(TextFormat("Murs : %i", wallCount),  50, 90, 10, BLACK);
	DrawText(TextFormat("Temps : %.2f", timer),  50, 110, 10, BLACK);
	if (win)
 	{
	    DrawText("Victoire", 300, 250, 30, BLACK);
	}
}

bool playerHitsWall(Rectangle playerRect, Rectangle walls[], int wallCount)
{
    int i = 0;

    while (i < wallCount)
    {
        if (CheckCollisionRecs(playerRect, walls[i]))
        {
            return true;
        }

        i++;
    }

    return false;
}

bool playerReachedExit(Rectangle playerRect, Rectangle exitRect)
{
	    return CheckCollisionRecs(playerRect, exitRect);
}

void restartLevel(int *playerX, int *playerY, int playerStartX, int playerStartY, bool *win, float *timer)
{
	    *playerX = playerStartX;
	    *playerY = playerStartY;
            *win = false;
	    *timer = 0.0f;
}

int main(void)
{

    int playerStartX = 340;
    int playerStartY = 280;

    int playerX = playerStartX;
    int playerY = playerStartY; 

    int playerSize = 40;
    int playerSpeed = 1;

    int exitX = 100;
    int exitY = 500;
    int exitSizeX = 55;
    int exitSizeY = 55;  

    bool win = false;

    Rectangle walls[] = {
	        {250, 150, 10, 450},
		{0, 250, 175, 10},
		{250, 250, 175, 10},
		{425, 75, 10, 400}
    };

    int wallCount = sizeof(walls) / sizeof(walls[0]);
    
    InitWindow(800, 600, "Mon premier jeu");
    SetTargetFPS(60);
    float timer = 0.0f;
    while (!WindowShouldClose())
    {
	int oldPlayerX = playerX;
	int oldPlayerY = playerY;
	if (!win)
	{
		timer = timer + GetFrameTime();	
    		if (IsKeyDown(KEY_RIGHT))
		{
			playerX = playerX + playerSpeed;
		}
		if (IsKeyDown(KEY_DOWN))
		{
			playerY = playerY + playerSpeed;
		}
    		if (IsKeyDown(KEY_LEFT))
		{
			playerX = playerX - playerSpeed;
		}
    		if (IsKeyDown(KEY_UP))
		{
			playerY = playerY - playerSpeed;
		}

	}
	if (IsKeyPressed(KEY_R))
	{
		restartLevel(&playerX, &playerY, playerStartX, playerStartY, &win, &timer);
	}
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
	Rectangle exitRect = {exitX, exitY, exitSizeX, exitSizeY};
	win = playerReachedExit(playerRect, exitRect);
	Color playerColor;
	bool blocked = playerHitsWall(playerRect, walls, wallCount);

	if (blocked)
	{
	    playerColor = GREEN;
	    playerX = oldPlayerX;
	    playerY = oldPlayerY;
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
        DrawRectangle(exitX, exitY, exitSizeX, exitSizeY, YELLOW);
        DrawRectangle(playerX, playerY, playerSize, playerSize, playerColor);
	drawHUD(win, wallCount, timer); 
  	EndDrawing();
    }
    CloseWindow();
    return 0;
}

