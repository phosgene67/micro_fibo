// #include "raylib.h"

// const int ROWS = 4;
// const int COLS = 4;

// const int CELL_SIZE = 80;
// const int MAZE_X = 100;
// const int MAZE_Y = 50;

// struct Cell
// {
//     bool top;
//     bool right;
//     bool bottom;
//     bool left;
// };

// Cell maze[ROWS][COLS];

// void DrawCell(const Cell& cell, int row, int col)
// {
//     int x = MAZE_X + col * CELL_SIZE;
//     int y = MAZE_Y + row * CELL_SIZE;

//     if (cell.top)
//         DrawLine(x, y, x + CELL_SIZE, y, BLACK);

//     if (cell.right)
//         DrawLine(x + CELL_SIZE, y,
//                  x + CELL_SIZE, y + CELL_SIZE, BLACK);

//     if (cell.bottom)
//         DrawLine(x, y + CELL_SIZE,
//                  x + CELL_SIZE, y + CELL_SIZE, BLACK);

//     if (cell.left)
//         DrawLine(x, y, x, y + CELL_SIZE, BLACK);
// }

// void DrawMaze()
// {
//     for (int row = 0; row < ROWS; row++)
//     {
//         for (int col = 0; col < COLS; col++)
//         {
//             DrawCell(maze[row][col], row, col);
//         }
//     }
// }

// int main()
// {
//     InitWindow(800, 600, "Maze Solver");

//     SetTargetFPS(60);

//     // Example: initialize all walls
//     for (int row = 0; row < ROWS; row++)
//     {
//         for (int col = 0; col < COLS; col++)
//         {
//             maze[row][col] = {
//                 true,   // top
//                 true,   // right
//                 true,   // bottom
//                 true    // left
//             };
//         }
//     }

//     while (!WindowShouldClose())
//     {
//         BeginDrawing();

//         ClearBackground(RAYWHITE);

//         DrawMaze();

//         EndDrawing();
//     }

//     CloseWindow();

//     return 0;
// }