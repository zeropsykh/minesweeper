typedef enum GameState {
    NONE,
    PLAYING, 
    LOSE, 
    WIN,
} GameState; 

typedef enum GameMode {
    EASY,
    MEDIUM,
    DIFFICULT,
} GameMode;

char* GameModeStr[] = {
    "Easy",
    "Medium",
    "Difficult",
};

typedef struct Cell {
    int i;
    int j;
    bool isMine;
    bool isFlagged;
    bool isUncertain;
    bool isExploded;
    bool revealed;
    int mineCount;
} Cell;

void LoadAssets();
void DrawMainMenu();
void DrawPlayingGrid();
void DrawLostMenu();
void DrawWinMenu();
void DrawCell(Cell);
void DrawCells();
void GameInit();
void SetMines(int, int, int);
int CountMines(int, int);
bool IndexIsValid(int, int);
void CellReveal(int, int);
void FloodFill(int, int);
Cell **GetGrid();
void FreeGrid(Cell**);

