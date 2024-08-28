#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>
#include "raygui.h"

#include "minesweeper.h" 

#define screenWidth 400 
#define screenHeight 850

int ROWS = 10;
int COLS = 10;

const int gridWidth = 360;
const int gridHeight = 320;
Vector2 gridOrigin = { 20.0f , 200.0f };

Font fontOtf;
Cell** grid;
Image logo;
Texture2D logoTexture;
Texture2D cellSprites;
Rectangle cellUnrevealedRec;
Rectangle cellRevealedRec;
Rectangle cellFlaggedRec;
Rectangle cellUncertainRec;
Rectangle cellMineRec;
Rectangle cellMineExplodedRec;
Rectangle cellFlaggedNotMineRec;
Rectangle cellNum[8];

int cellWidth, cellHeight;
GameState gameState = NONE; 
GameMode mode = EASY;
int mineSet = false;
int setMineCount = 10;
int revealedCount = 0; 
int flagCount;
bool exitWindow = false;

int main()
{
    SetRandomSeed(time(0));

    InitWindow(screenWidth, screenHeight, "Minesweeper");

    LoadAssets();    

    grid = GetGrid();
    flagCount = setMineCount;
    cellWidth = (float)(gridWidth / COLS);
    cellHeight = (float)(gridHeight / ROWS);

    SetTargetFPS(60);
    while (!WindowShouldClose() && !exitWindow) {
        if (gameState == NONE) DrawMainMenu();
        if (gameState == PLAYING) DrawPlayingGrid();   
        if (gameState == LOSE) DrawLostMenu(); 
        if (gameState == WIN) DrawWinMenu(); 
    }
    FreeGrid(grid);    

    UnloadTexture(cellSprites);

    CloseWindow();

    return 0;
}

void DrawMainMenu()
{

    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);

    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    DrawTexture(logoTexture, 54, 100, LIGHTGRAY);
    DrawTextEx(fontOtf, "MINESWEEPER", (Vector2){ 100, 106 }, 28, 1, BLACK);
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 200, 300, 50}, "PLAY")) {
        gameState = PLAYING; 
        GameInit();
    }
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 275, 300, 50}, TextFormat("Mode: %s", GameModeStr[mode]))) {
        mode = (mode + 1) % 3; 
        FreeGrid(grid);
        switch (mode) {
        case EASY: ROWS = 10, COLS = 10, setMineCount = 10;  break;
        case MEDIUM: ROWS = 16, COLS = 18, setMineCount = 40; break; 
        case DIFFICULT: ROWS = 20, COLS = 24, setMineCount = 99; break;
        default: ROWS = 10, COLS = 10, setMineCount = 10; break; 
        }
        grid = GetGrid();
        flagCount = setMineCount;
        cellWidth = gridWidth / COLS;
        cellHeight = gridHeight / ROWS;

    }
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 350, 300, 50}, "QUIT")) exitWindow = true;

    EndDrawing();
}

void DrawPlayingGrid()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mPos = GetMousePosition();
        if (CheckCollisionPointRec(mPos, (Rectangle){20.0f, 200.0f, gridWidth, gridHeight})) {
            int row = (mPos.y - gridOrigin.y) / cellHeight;
            int col = (mPos.x - gridOrigin.x) / cellWidth;
            // printf("%d, %d\n", row, col);
            // fflush(stdout);

            if (IndexIsValid(row, col)) {
                if (!mineSet) SetMines(setMineCount, row, col);
                CellReveal(row, col);
            }
        }
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mPos = GetMousePosition();
        if (CheckCollisionPointRec(mPos, (Rectangle){20.0f, 200.0f, gridWidth, gridHeight})) {
            int row = (mPos.y - gridOrigin.y) / cellHeight;
            int col = (mPos.x - gridOrigin.x) / cellWidth;
            // printf("RightClick: %d, %d\n", row, col);
            // fflush(stdout);
            if (IndexIsValid(row, col)) {
                if (!grid[row][col].isFlagged) {
                    grid[row][col].isFlagged = true;
                } else if (!grid[row][col].isUncertain) {
                    grid[row][col].isUncertain = true;
                } else {
                    grid[row][col].isFlagged = false;
                    grid[row][col].isUncertain = false;
                }
            }
        }
    }

    // Win condition
    if ((ROWS * COLS) - revealedCount == setMineCount) {
        gameState = WIN; 
    }

    BeginDrawing(); 
    ClearBackground(LIGHTGRAY);
    DrawCells();
    EndDrawing();
}

void DrawCells()
{
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            DrawCell(grid[i][j]); 
        }
    }
}

void DrawLostMenu()
{
    char *lostText = "You have LOST!!";
    Vector2 textSize = MeasureTextEx(fontOtf, lostText, 32, 2);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 22);

    BeginDrawing();

    DrawCells();
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(LIGHTGRAY, 0.85f));
    DrawTexture(logoTexture, 54, 100, LIGHTGRAY);
    DrawTextEx(fontOtf, "MINESWEEPER", (Vector2){ 100, 106 }, 28, 1, BLACK);
    DrawTextEx(fontOtf, lostText, (Vector2){ (screenWidth/2) - textSize.x/2, 200.0f }, 32, 2, BLACK);

    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 275, 300, 50}, "TRY AGAIN!!")) {
        gameState = PLAYING;
        GameInit();
    }
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 350, 300, 50}, "MAIN MENU")) gameState = NONE;
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 425, 300, 50}, "QUIT")) exitWindow = true;

    EndDrawing();
}

void DrawWinMenu()
{
    char *winText = "Congrats, You have WON!!";
    Vector2 textSize = MeasureTextEx(fontOtf, winText, 20, 2); 
    GuiSetStyle(DEFAULT, TEXT_SIZE, 22);

    BeginDrawing();

    DrawCells();
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(LIGHTGRAY, 0.85f));
    DrawTexture(logoTexture, 54, 100, LIGHTGRAY);
    DrawTextEx(fontOtf, "MINESWEEPER", (Vector2){ 100, 106 }, 28, 1, BLACK);
    DrawTextEx(fontOtf, winText, (Vector2){ (screenWidth/2) - textSize.x/2, 200.0f }, 20, 2, BLACK);
    
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 275, 300, 50}, "PLAY AGAIN")) {
        gameState = PLAYING;
        GameInit();
    }
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 350, 300, 50}, "MAIN MENU")) gameState = NONE;
    if (GuiButton((Rectangle){ (screenWidth/2) - 150, 425, 300, 50}, "QUIT")) CloseWindow();

    EndDrawing();
}

void DrawCell(Cell cell)
{
    Rectangle dest = {(cell.j * cellWidth) + gridOrigin.x, (cell.i * cellHeight) + gridOrigin.y, cellWidth, cellHeight};
    Vector2 origin = {0.0f, 0.0f};

    if (cell.revealed) {
        if (cell.isMine) {
            if (cell.isExploded) DrawTexturePro(cellSprites, cellMineExplodedRec, dest, origin, 0.0f, WHITE);
            else DrawTexturePro(cellSprites, cellMineRec, dest, origin, 0.0f, WHITE); 
        } else {
            if (!cell.mineCount) DrawTexturePro(cellSprites, cellRevealedRec, dest, origin, 0.0f, WHITE); 
            else DrawTexturePro(cellSprites, cellNum[cell.mineCount - 1], dest, origin, 0.0f, WHITE);
        }
    } else if (cell.isFlagged) {
        if (cell.isUncertain) DrawTexturePro(cellSprites, cellUncertainRec, dest, origin, 0.0f, WHITE);   
        else DrawTexturePro(cellSprites, cellFlaggedRec, dest, origin, 0.0f, WHITE);   
    } else {
        DrawTexturePro(cellSprites, cellUnrevealedRec, dest, origin, 0.0f, WHITE);
    }
}

Cell **GetGrid()
{
    Cell **grid = (Cell**)malloc(ROWS * sizeof(Cell*));
    // printf("\nAllocated\n");
    // for (int i = 0; i < ROWS; ++i) {
    //     printf("%u\n", &grid[i]);
    // }

    for (int i = 0; i < ROWS; ++i) 
        grid[i] = (Cell*)malloc(COLS * sizeof(Cell));
    return grid; 
}

void FreeGrid(Cell **grid)
{
    // printf("\nFreeing\n");
    // for (int i = 0; i < ROWS; ++i) {
    //     printf("%u\n", &grid[i]);
    // }

    for (int i = 0; i < ROWS; ++i) free(grid[i]);
    free(grid);
}

void GameInit()
{
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            grid[i][j] = (Cell) {
                .i = i, 
                .j = j, 
                .isMine = false,
                .isFlagged = false,
                .isUncertain = false,
                .isExploded = false,
                .revealed = false,
                .mineCount = 0,
            }; 
        }
    }

    mineSet = false;
    gameState = PLAYING; 
    revealedCount = 0;
}

bool IndexIsValid(int i, int j) 
{
    return (i >= 0 && i < ROWS) && (j >= 0 && j < COLS);
}

void CellReveal(int i, int j)
{
    if (grid[i][j].isFlagged) return;
    if (grid[i][j].isMine) {
        grid[i][j].revealed = true;
        grid[i][j].isExploded = true;
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (grid[i][j].isMine) grid[i][j].revealed = true;
            }
        }
        gameState = LOSE;
        return;
    }
    if (grid[i][j].mineCount == 0) FloodFill(i, j);
    grid[i][j].revealed = true; 
}

void SetMines(int mineCount, int startCellrow, int startCellCol)
{
    while (mineCount != 0) {
        int i = GetRandomValue(0, ROWS-1); 
        int j = GetRandomValue(0, COLS-1);

        if (!grid[i][j].isMine && i != startCellrow && j != startCellCol) {
            grid[i][j].isMine = true;  
            mineCount -= 1;
        }
    } 

    mineSet = true;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (!grid[i][j].isMine) 
                grid[i][j].mineCount = CountMines(i, j);
        }
    } 
}

int CountMines(int row, int col)
{
    int count = 0;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (i == 0 && j == 0) continue;
            int x = row + i;
            int y = col + j;
            if (!IndexIsValid(x, y)) continue;        
            if (grid[x][y].isMine) count++; 
        }
    } 
    return count;
}

void FloodFill(int row, int col)
{
    if (!IndexIsValid(row, col)) return;
    if (grid[row][col].revealed) return;
    if (grid[row][col].isMine) return;
    
    grid[row][col].revealed = true;
    if (grid[row][col].mineCount == 0) {
        FloodFill(row - 1, col);
        FloodFill(row + 1, col);
        FloodFill(row, col - 1);
        FloodFill(row, col + 1);
    }
}

void LoadAssets()
{
    fontOtf = LoadFont("assets/mine-sweeper.otf");
    GuiSetFont(fontOtf);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);

    logo = LoadImage("assets/logo.png");
    ImageResize(&logo, 40, 40); 
    logoTexture = LoadTextureFromImage(logo); 
    cellSprites = LoadTexture("assets/sprites.png");
    

    cellUnrevealedRec = (Rectangle){ 0.0f, 0.0f, 16.0f , 16.0f }; 
    cellRevealedRec = (Rectangle){ 16.0f, 0.0f, 16.0f , 16.0f }; 
    cellFlaggedRec = (Rectangle){ 32.0f, 0.0f, 16.0f , 16.0f };
    cellUncertainRec = (Rectangle){ 48.0f, 0.0f, 16.0f , 16.0f };
    cellMineRec = (Rectangle){ 80.0f, 0.0f, 16.0f , 16.0f };
    cellMineExplodedRec = (Rectangle){ 96.0f, 0.0f, 16.0f , 16.0f };
    cellFlaggedNotMineRec = (Rectangle){ 112.0f, 0.0f, 16.0f , 16.0f };

    for (int i = 0; i < 8; ++i) {
        cellNum[i] = (Rectangle){ i * 16.0f, 16.0f, 16.0f, 16.0f };
    }
}
