#include<iostream> 
#include<raylib.h>
using namespace std ; 
const int row =15;
const int col=15;
const int maze_x=50;
const int maze_y=50;
const int cellsize=50;

enum Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

struct Cell
{
    bool top;
    bool right;
    bool bottom;
    bool left;
    bool wall;
    bool isFinishBox;
    bool start;
};

struct sensor
{
    bool RW_detected;
    bool LW_detected; 
    bool FW_detected; 
    bool RearW_detected; 
};

Cell maze[row][col];
bool visited[row][col];  // Track visited cells for green marking
bool shortestPath[row][col];  // Purple cells show the Dijkstra race route

class robot {
    public:
        int robotRow;
        int robotCol;
        Direction robotDirection;
        sensor robotSensor;
        
        void robot_init();
        void robot_movement();
        void sensor();
        void algo_hunter();
        
    private:
        bool canMove(int nextRow, int nextCol);
};

// Initialize robot at start position
void robot::robot_init() {
    robotRow = 14;
    robotCol = 0;
    robotDirection = UP;
    
    // Initialize visited array
    for(int r = 0; r < row; r++) {
        for(int c = 0; c < col; c++) {
            visited[r][c] = false;
        }
    }
    
    // Mark start position as visited
    visited[robotRow][robotCol] = true;
    
    cout << "Robot initialized at (" << robotRow << ", " << robotCol << ")" << endl;
}

// Check if robot can move to next position
bool robot::canMove(int nextRow, int nextCol) {
    // Check boundaries
    if(nextRow < 0 || nextRow >= row || nextCol < 0 || nextCol >= col) {
        return false;
    }
    
    // Check if next cell is a wall
    if(maze[nextRow][nextCol].wall) {
        return false;
    }
    
    return true;
}

void robot::sensor() {
    robotSensor.FW_detected = false;
    robotSensor.RW_detected = false;
    robotSensor.LW_detected = false;
    robotSensor.RearW_detected = false;
    
    switch(robotDirection) {
        
        case UP:  
            // Front = check cell above (row - 1)
            if(!canMove(robotRow - 1, robotCol)) {
                robotSensor.FW_detected = true;
            }
            // Right = check cell to the right (col + 1)
            if(!canMove(robotRow, robotCol + 1)) {
                robotSensor.RW_detected = true;
            }
            // Left = check cell to the left (col - 1)
            if(!canMove(robotRow, robotCol - 1)) {
                robotSensor.LW_detected = true;
            }
            // Rear = check cell below (row + 1)
            if(!canMove(robotRow + 1, robotCol)) {
                robotSensor.RearW_detected = true;
            }
            break;
            
        case DOWN:  // Robot facing DOWN
            // Front = check cell below
            if(!canMove(robotRow + 1, robotCol)) {
                robotSensor.FW_detected = true;
            }
            // Right = check cell to the left
            if(!canMove(robotRow, robotCol - 1)) {
                robotSensor.RW_detected = true;
            }
            // Left = check cell to the right
            if(!canMove(robotRow, robotCol + 1)) {
                robotSensor.LW_detected = true;
            }
            // Rear = check cell above
            if(!canMove(robotRow - 1, robotCol)) {
                robotSensor.RearW_detected = true;
            }
            break;
            
        case LEFT:  // Robot facing LEFT
            // Front = check cell to the left
            if(!canMove(robotRow, robotCol - 1)) {
                robotSensor.FW_detected = true;
            }
            // Right = check cell above
            if(!canMove(robotRow - 1, robotCol)) {
                robotSensor.RW_detected = true;
            }
            // Left = check cell below
            if(!canMove(robotRow + 1, robotCol)) {
                robotSensor.LW_detected = true;
            }
            // Rear = check cell to the right
            if(!canMove(robotRow, robotCol + 1)) {
                robotSensor.RearW_detected = true;
            }
            break;
            
        case RIGHT:  // Robot facing RIGHT
            // Front = check cell to the right
            if(!canMove(robotRow, robotCol + 1)) {
                robotSensor.FW_detected = true;
            }
            // Right = check cell below
            if(!canMove(robotRow + 1, robotCol)) {
                robotSensor.RW_detected = true;
            }
            // Left = check cell above
            if(!canMove(robotRow - 1, robotCol)) {
                robotSensor.LW_detected = true;
            }
            // Rear = check cell to the left
            if(!canMove(robotRow, robotCol - 1)) {
                robotSensor.RearW_detected = true;
            }
            break;
    }
}

// Handle robot movement via keyboard
void robot::robot_movement() {
    
    if(IsKeyPressed(KEY_UP)) {
        robotDirection = UP;
        if(canMove(robotRow - 1, robotCol)) {
            robotRow--;
            visited[robotRow][robotCol] = true;
        }
    }
    if(IsKeyPressed(KEY_DOWN)) {
        robotDirection = DOWN;
        if(canMove(robotRow + 1, robotCol)) {
            robotRow++;
            visited[robotRow][robotCol] = true;
        }
    }
    if(IsKeyPressed(KEY_LEFT)) {
        robotDirection = LEFT;
        if(canMove(robotRow, robotCol - 1)) {
            robotCol--;
            visited[robotRow][robotCol] = true;
        }
    }
    if(IsKeyPressed(KEY_RIGHT)) {
        robotDirection = RIGHT;
        if(canMove(robotRow, robotCol + 1)) {
            robotCol++;
            visited[robotRow][robotCol] = true;
        }
    }
}
void robot::algo_hunter() {
     // calls.  known[r][c] is: -1 = not seen yet, 0 = open, 1 = wall.
    static bool initialized = false;
    static int known[row][col];
    static bool explored[row][col];
    static int stackRow[row * col], stackCol[row * col], stackSize = 0;
    static int raceRow[row * col], raceCol[row * col];
    static int raceLength = 0, raceIndex = 0;
    static int startRow = 0, startCol = 0;
    static int finishRow = -1, finishCol = -1;
    // 0 = explore, 1 = calculate route, 2 = wait for the start command,
    // 3 = race, 4 = finished (or the finish is unreachable).
    static int phase = 0;

    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};

    if (!initialized) {
        initialized = true;
        startRow = robotRow;
        startCol = robotCol;

        for (int r = 0; r < row; ++r) {
            for (int c = 0; c < col; ++c) {
                known[r][c] = -1;
                explored[r][c] = false;
            }
        }

        known[startRow][startCol] = 0;
        explored[startRow][startCol] = true;
        stackRow[stackSize] = startRow;
        stackCol[stackSize++] = startCol;
    }

    if (phase == 4) return;

    // Convert the four relative sensor values into absolute maze directions.
    // Direction values are ordered UP, RIGHT, DOWN, LEFT, matching dr/dc.
    bool blocked[4] = {
        robotSensor.FW_detected,
        robotSensor.RW_detected,
        robotSensor.RearW_detected,
        robotSensor.LW_detected
    };
    int sensedDirection[4] = {
        robotDirection,
        (robotDirection + 1) % 4,
        (robotDirection + 2) % 4,
        (robotDirection + 3) % 4
    };

    known[robotRow][robotCol] = 0;
    for (int i = 0; i < 4; ++i) {
        int direction = sensedDirection[i];
        int nr = robotRow + dr[direction];
        int nc = robotCol + dc[direction];
        if (nr >= 0 && nr < row && nc >= 0 && nc < col) {
            known[nr][nc] = blocked[i] ? 1 : 0;
        }
    }

    // In this simulator the finish-cell colour is available as cell metadata.
    // The route is still unknown until the robot physically visits this cell.
    if (maze[robotRow][robotCol].isFinishBox) {
        finishRow = robotRow;
        finishCol = robotCol;
    }

    if (phase == 0) {
        // Depth-first exploration visits every reachable open cell.  When a
        // branch ends, the DFS stack brings the robot back to continue.
        int nextDirection = -1;
        for (int direction = 0; direction < 4; ++direction) {
            int nr = robotRow + dr[direction];
            int nc = robotCol + dc[direction];
            if (nr >= 0 && nr < row && nc >= 0 && nc < col &&
                known[nr][nc] == 0 && !explored[nr][nc]) {
                nextDirection = direction;
                break;
            }
        }

        if (nextDirection != -1) {
            int nr = robotRow + dr[nextDirection];
            int nc = robotCol + dc[nextDirection];
            explored[nr][nc] = true;
            visited[nr][nc] = true;
            stackRow[stackSize] = nr;
            stackCol[stackSize++] = nc;
            robotDirection = (Direction)nextDirection;
            robotRow = nr;
            robotCol = nc;
            return;
        }

        // No unexplored neighbour: return one cell along the DFS tree.
        if (stackSize > 1) {
            --stackSize;
            int nr = stackRow[stackSize - 1];
            int nc = stackCol[stackSize - 1];
            for (int direction = 0; direction < 4; ++direction) {
                if (robotRow + dr[direction] == nr && robotCol + dc[direction] == nc) {
                    robotDirection = (Direction)direction;
                    break;
                }
            }
            robotRow = nr;
            robotCol = nc;
            return;
        }

        // DFS has returned to the blue start after mapping its whole component.
        phase = 1;
    }

    if (phase == 1) {
        if (finishRow == -1) {
            cout << "Finish cell is not reachable from the start." << endl;
            phase = 3;
            return;
        }

        // Dijkstra's algorithm (all moves cost one cell).  A BFS would produce
        // the same result here, but this makes the shortest-path phase explicit.
        int distance[row][col];
        bool settled[row][col];
        int previousRow[row][col], previousCol[row][col];
        const int infinity = 1000000;

        for (int r = 0; r < row; ++r) {
            for (int c = 0; c < col; ++c) {
                distance[r][c] = infinity;
                settled[r][c] = false;
                previousRow[r][c] = -1;
                previousCol[r][c] = -1;
            }
        }
        distance[startRow][startCol] = 0;

        for (int step = 0; step < row * col; ++step) {
            int cr = -1, cc = -1, bestDistance = infinity;
            for (int r = 0; r < row; ++r) {
                for (int c = 0; c < col; ++c) {
                    if (!settled[r][c] && known[r][c] == 0 && distance[r][c] < bestDistance) {
                        bestDistance = distance[r][c];
                        cr = r;
                        cc = c;
                    }
                }
            }
            if (cr == -1) break;
            settled[cr][cc] = true;
            if (cr == finishRow && cc == finishCol) break;

            for (int direction = 0; direction < 4; ++direction) {
                int nr = cr + dr[direction];
                int nc = cc + dc[direction];
                if (nr >= 0 && nr < row && nc >= 0 && nc < col &&
                    known[nr][nc] == 0 && !settled[nr][nc] &&
                    distance[cr][cc] + 1 < distance[nr][nc]) {
                    distance[nr][nc] = distance[cr][cc] + 1;
                    previousRow[nr][nc] = cr;
                    previousCol[nr][nc] = cc;
                }
            }
        }

        if (distance[finishRow][finishCol] == infinity) {
            cout << "No route to finish exists." << endl;
            phase = 3;
            return;
        }

        int reverseRow[row * col], reverseCol[row * col], count = 0;
        int cr = finishRow, cc = finishCol;
        while (cr != -1 && count < row * col) {
            reverseRow[count] = cr;
            reverseCol[count++] = cc;
            if (cr == startRow && cc == startCol) break;
            int pr = previousRow[cr][cc];
            int pc = previousCol[cr][cc];
            cr = pr;
            cc = pc;
        }
        raceLength = count;
        for (int i = 0; i < raceLength; ++i) {
            raceRow[i] = reverseRow[raceLength - 1 - i];
            raceCol[i] = reverseCol[raceLength - 1 - i];
            shortestPath[raceRow[i]][raceCol[i]] = true;
        }
        raceIndex = 0;
        cout << "dps done. dijkastra:"
             << raceLength - 1 << " moves. Press SPACE to race." << endl;
        phase = 2;
    }

    if (phase == 2) {
        // Stay at the blue start while the purple shortest path is visible.
        if ( IsKeyPressed(KEY_SPACE)) {
            cout << "Race started." << endl;
            phase = 3;
        }
        return;
    }

    if (phase == 3) {
        if (raceIndex + 1 >= raceLength) {
            cout << "Finish reached." << endl;
            phase = 4;
            return;
        }

        int nr = raceRow[raceIndex + 1];
        int nc = raceCol[raceIndex + 1];
        for (int direction = 0; direction < 4; ++direction) {
            if (robotRow + dr[direction] == nr && robotCol + dc[direction] == nc) {
                robotDirection = (Direction)direction;
                break;
            }
        }
        robotRow = nr;
        robotCol = nc;
        visited[robotRow][robotCol] = true;
        ++raceIndex;
    }
}

robot myRobot;

void drawcell(const Cell &cell , int row, int col) {
            int x =maze_x + col* cellsize; 
            int y =maze_y + row* cellsize; 
            
            if(cell.wall) {
                DrawRectangle(x, y, cellsize, cellsize, BLACK);
            }
            else if(cell.isFinishBox) {
                DrawRectangle(x, y, cellsize, cellsize, RED);
            }
            else if(cell.start){
                DrawRectangle(x,y,cellsize,cellsize,BLUE);
            }
            else if(shortestPath[row][col]) {
                DrawRectangle(x, y, cellsize,cellsize, PURPLE);
            }
            else if(visited[row][col]) {
                DrawRectangle(x, y, cellsize, cellsize, GREEN);
            }
            
            if(!cell.wall && !cell.isFinishBox && !cell.start) {
                if(cell.top){
                    DrawLine(x,y,x+cellsize,y,BLACK);
                }
                 if (cell.bottom){
                        DrawLine(x,y+cellsize,x+cellsize,y+cellsize,BLACK);
                }
                 if (cell.left){
                        DrawLine(x,y,x,y+cellsize,BLACK);
                }
                 if (cell.right){
                        DrawLine(x+cellsize,y,x+cellsize,y+cellsize,BLACK);
                }
            }
}

void drawmaze(){
    for(int r=0 ; r< row;r++)
    {
        for(int c=0 ; c<col;c++){
            drawcell(maze[r][c],r,c);
        }
    }
}

void drawRobot() {
    int x = maze_x + myRobot.robotCol * cellsize + cellsize/2; ///circle centre x,y
    int y = maze_y + myRobot.robotRow * cellsize + cellsize/2;
    
    // Draw robot as yellow circle
    DrawCircle(x, y, 12, YELLOW);
    
    // Draw direction indicator line
    int dirX = x, dirY = y;
    switch(myRobot.robotDirection) {
        case UP:    dirY -= 15; break;
        case DOWN:  dirY += 15; break;
        case LEFT:  dirX -= 15; break;
        case RIGHT: dirX += 15; break;
    }
    DrawLine(x, y, dirX, dirY, RED);
}

void maze_init(){
    for(int r =0; r< row ; r++){
        for (int c=0 ; c < col ; c++){
            maze[r][c]={true,true,true,true,false,false,false};
        }
    }
        // Top border walls
    for(int c = 0; c < 15; c++) {
        maze[0][c].wall = false;
    }
    
    // Left border wall
    for(int r = 0; r < 15; r++) {
        maze[r][0].wall = false;
    }
    
    // Complex maze walls
    for(int c=1;c<14;c++){
        maze[1][c].wall = true;
    }
    
    for(int r=2;r<15;r++){
        maze[r][1].wall=true;
        if(r==12){
            maze[r][1].wall=false;
        }
    } 
    for(int r=0 ; r<15; r++){
        if(r==0||r==2||r==8||r==14){
            continue;
        }
        maze[r][3].wall=true;
    }
    for(int c=0;c<15;c++){
        if(c==0||c==1||c==2||c==9||c==14){
            continue;
        }
        maze[3][c].wall=true;
    }
    for(int r=0;r<15;r++){
        if(r==0||r==2||r==3||r==10||r==14){
            continue;
        }
        maze[r][13].wall=true;
    }
    for(int c=0;c<15;c++){
        if(c==0||c==2||c==14){
            continue;
        }
        maze[13][c].wall=true;
    }
   for(int c=0;c<15;c++){
    if(c==0||c==1||c==2||c==3||c==4||c==14){
        continue;
    }
    maze[7][c].wall=true;
   }

   for(int c=0;c<15;c++){
    if(c==0||c==1||c==2||c==6||c==11||c==14){
        continue;
    }
    maze[5][c].wall=true;
   }

   for(int r=8;r<12;r++){
    maze[r][5].wall=true;
   }
   for(int r =8;r<12;r++) {
    maze[r][11].wall=true;
   }
    maze[9][7].wall=true;
    maze[9][8].wall=true;
    maze[9][9].wall=true;
    maze[10][9].wall=true;
    maze[11][9].wall=true;
    maze[12][9].wall=true;
    maze[11][10].wall=true;
    maze[11][6].wall=true;
    maze[11][7].wall=true;

    maze[14][0].start=true;
    maze[10][10].isFinishBox = true;
}

int main () {
     InitWindow(1000,850,"maze_solver"); 
     SetTargetFPS(10);

    maze_init();
    myRobot.robot_init();

     while(WindowShouldClose()== false) {

        // Update sensors continuously (like real robot)
        myRobot.sensor();
        
        // Handle movement input
        myRobot.robot_movement();
        
       
        // Run pathfinding algorithm
       
          myRobot.algo_hunter();
       
        
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            drawmaze();
            drawRobot();
            
            // Display sensor status
            DrawText("=== SENSOR STATUS ===", 770, 10, 18, BLACK);
            DrawText(TextFormat("Front Wall: %s", myRobot.robotSensor.FW_detected ? "YES" : "NO"), 850, 35, 15, BLACK);
            DrawText(TextFormat("Right Wall: %s", myRobot.robotSensor.RW_detected ? "YES" : "NO"), 850, 55, 15, BLACK);
            DrawText(TextFormat("Left Wall:  %s", myRobot.robotSensor.LW_detected ? "YES" : "NO"),850, 75, 15, BLACK);
            DrawText(TextFormat("Rear Wall:  %s", myRobot.robotSensor.RearW_detected ? "YES" : "NO"), 850, 95, 15, BLACK);
            
            DrawText("=== ROBOT STATUS ===", 800, 130, 18, BLACK);
            DrawText(TextFormat("Position:[%d],[%d]", myRobot.robotRow, myRobot.robotCol), 800, 155, 15, BLACK);
            const char* dirStr[] = {"UP", "RIGHT", "DOWN", "LEFT"};
            DrawText(TextFormat("Direction: %s", dirStr[myRobot.robotDirection]), 800, 175, 15, BLACK);
            
            DrawText("Press Space", 800, 210, 14, DARKGRAY);
            
            EndDrawing();
            
     }
     CloseWindow();
     return 0 ; 
}
