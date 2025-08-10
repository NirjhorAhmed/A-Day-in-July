#include "raylib.h"
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

#define PLAYER_SPEED 4
#define BRICK_SPEED 6
#define BULLET_SPEED 5
#define MAX_UNITS 3
#define MAX_PROJECTILES 10

typedef struct
{
    Vector2 position;
    int health;
    bool isAlive;
} Character;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    bool isPlayerProjectile;
} Projectile;

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jatrabari Clash - Multi-Units");
    SetTargetFPS(60);

    Character protesters[MAX_UNITS];
    for (int i = 0; i < MAX_UNITS; i++)
    {
        protesters[i].position = (Vector2){100, 60 + i * 120};
        protesters[i].health = 10;
        protesters[i].isAlive = true;
    }
    int selectedProtester = 0;

    Character police[MAX_UNITS];
    for (int i = 0; i < MAX_UNITS; i++)
    {
        police[i].position = (Vector2){SCREEN_WIDTH - 150, 60 + i * 120};
        police[i].health = 10;
        police[i].isAlive = true;
    }

    Projectile projectiles[MAX_PROJECTILES] = {0};
    double bulletCooldown = 10.0;
    double bulletTimers[MAX_UNITS];
    for (int i = 0; i < MAX_UNITS; i++)
    {
        bulletTimers[i] = GetRandomValue(0, (int)(bulletCooldown * 60)) / 60.0;
    }

    bool gameOver = false;
    bool playerWon = false;

    while (!WindowShouldClose())
    {
        if (!gameOver)
        {
            if (IsKeyPressed(KEY_KP_1))
                selectedProtester = 0;
            if (IsKeyPressed(KEY_KP_2))
                selectedProtester = 1;
            if (IsKeyPressed(KEY_KP_3))
                selectedProtester = 2;

            Character *p = &protesters[selectedProtester];

            if (p->isAlive)
            {
                if (IsKeyDown(KEY_W) && p->position.y > 0)
                    p->position.y -= PLAYER_SPEED;
                if (IsKeyDown(KEY_S) && p->position.y < SCREEN_HEIGHT - 50)
                    p->position.y += PLAYER_SPEED;

                if (IsKeyPressed(KEY_SPACE))
                {
                    for (int i = 0; i < MAX_PROJECTILES; i++)
                    {
                        if (!projectiles[i].active)
                        {
                            projectiles[i].position = (Vector2){p->position.x + 40, p->position.y + 20};
                            projectiles[i].velocity = (Vector2){BRICK_SPEED, 0};
                            projectiles[i].active = true;
                            projectiles[i].isPlayerProjectile = true;
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_UNITS; i++)
            {
                if (!police[i].isAlive)
                    continue;
                bulletTimers[i] += GetFrameTime();
                if (bulletTimers[i] >= bulletCooldown)
                {
                    for (int j = 0; j < MAX_PROJECTILES; j++)
                    {
                        if (!projectiles[j].active)
                        {
                            projectiles[j].position = (Vector2){police[i].position.x, police[i].position.y + 20};
                            projectiles[j].velocity = (Vector2){-BULLET_SPEED, 0};
                            projectiles[j].active = true;
                            projectiles[j].isPlayerProjectile = false;
                            bulletTimers[i] = 0;
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (!projectiles[i].active)
                    continue;
                projectiles[i].position.x += projectiles[i].velocity.x;

                if (projectiles[i].position.x < 0 || projectiles[i].position.x > SCREEN_WIDTH)
                {
                    projectiles[i].active = false;
                    continue;
                }

                if (projectiles[i].isPlayerProjectile)
                {
                    for (int j = 0; j < MAX_UNITS; j++)
                    {
                        if (!police[j].isAlive)
                            continue;
                        if (CheckCollisionRecs(
                                (Rectangle){projectiles[i].position.x, projectiles[i].position.y, 10, 10},
                                (Rectangle){police[j].position.x, police[j].position.y, 50, 50}))
                        {
                            police[j].health -= 2;
                            if (police[j].health <= 0)
                                police[j].isAlive = false;
                            projectiles[i].active = false;
                            break;
                        }
                    }
                }
                else
                {
                    for (int j = 0; j < MAX_UNITS; j++)
                    {
                        if (!protesters[j].isAlive)
                            continue;
                        if (CheckCollisionRecs(
                                (Rectangle){projectiles[i].position.x, projectiles[i].position.y, 10, 10},
                                (Rectangle){protesters[j].position.x, protesters[j].position.y, 50, 50}))
                        {
                            protesters[j].health -= 2;
                            if (protesters[j].health <= 0)
                                protesters[j].isAlive = false;
                            projectiles[i].active = false;
                            break;
                        }
                    }
                }
            }

            bool allPoliceDead = true;
            bool allProtestersDead = true;
            for (int i = 0; i < MAX_UNITS; i++)
            {
                if (police[i].isAlive)
                    allPoliceDead = false;
                if (protesters[i].isAlive)
                    allProtestersDead = false;
            }
            if (allPoliceDead || allProtestersDead)
            {
                gameOver = true;
                playerWon = allPoliceDead;
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver)
        {
            for (int i = 0; i < MAX_UNITS; i++)
            {
                if (protesters[i].isAlive)
                {
                    DrawRectangleV(protesters[i].position, (Vector2){50, 50}, GREEN);
                    DrawText(TextFormat("HP: %d", protesters[i].health), protesters[i].position.x, protesters[i].position.y - 20, 10, DARKGREEN);
                }
                if (police[i].isAlive)
                {
                    DrawRectangleV(police[i].position, (Vector2){50, 50}, BLUE);
                    DrawText(TextFormat("HP: %d", police[i].health), police[i].position.x, police[i].position.y - 20, 10, DARKBLUE);
                }
            }

            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (projectiles[i].active)
                {
                    DrawRectangleV(projectiles[i].position, (Vector2){10, 10}, projectiles[i].isPlayerProjectile ? BROWN : GRAY);
                }
            }

            DrawText("Press 1/2/3 to select protester", 10, 10, 20, DARKGRAY);
        }
        else
        {
            const char *msg = playerWon ? "You Win!" : "You Lose!";
            DrawText(msg, SCREEN_WIDTH / 2 - MeasureText(msg, 40) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
            DrawText("Press R to Restart or ESC to Exit", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30, 20, DARKGRAY);

            if (IsKeyPressed(KEY_R))
            {
                for (int i = 0; i < MAX_UNITS; i++)
                {
                    protesters[i] = (Character){{100, 60 + i * 120}, 10, true};
                    police[i] = (Character){{SCREEN_WIDTH - 150, 60 + i * 120}, 10, true};
                    bulletTimers[i] = 0;
                }
                for (int i = 0; i < MAX_PROJECTILES; i++)
                {
                    projectiles[i].active = false;
                }
                gameOver = false;
                selectedProtester = 0;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// gcc -o protest protest.c -lraylib -lm