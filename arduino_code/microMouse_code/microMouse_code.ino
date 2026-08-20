const int row =8;
const int col=8;
const int maze_x=50;
const int maze_y=50;
const int cellsize=50;


//
// Ultrasonic sensor pins
#define F_TRIG 2
#define F_ECHO 3

#define R_TRIG 4
#define R_ECHO 5

#define L_TRIG 6
#define L_ECHO 7

#define REAR_TRIG 8
#define REAR_ECHO 9

#define LIR 1
#define MIR 2
#define RIR 3

    int sensorValues[3];
    uint16_t sensorMin[3]=[0,0,0];
    uint16_t sensorMax[3]=[1023,1023,1023];
    uint16_t threshold[3]]=[512,512,512];
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
bool visited[row][col]; 
bool shortestPath[row][col];  

void readIR(uint8_t irsensor){
    uint8_t sensorValues[0]= analogRead(LIR); 
    uint8_t sensorValues[1]= analogRead(MIR);
    uint8_t sensorValues[2]= analogRead(RIR); 
}
bool previousValue[3]={false ,false false};
bool currentValue[3]={false,false,false}; 
bool  stopdetector(uint8_t IRval){
    readIR(); 
    bool blackdetected = false;
    for(i=0; i<3; i++){
    if(IRval[i]<threshold[i] && IRval[i]>irmin[i]) {
        return currentValue[i]=false; 
    }
    if(IRval[i]>threshold[i] && IRval[i]<irmax[i]){
        return cuurrentValue[i]= true; 
    }
    ////here stil some work remains . need to set current value to previous value and comparing current value with previous value need to set previous value and then it will detect blackbox 
 }
 
}
void sensorInit(){
    FS();
    RS();
    LS();
    rearS();
    readIR();
}
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

void robot::robot_init() {
    robotRow = 0;
    robotCol = 0;
    robotDirection = UP;
    sensorInit();
    for(int r = 0; r < row; r++) {
        for(int c = 0; c < col; c++) {
            visited[r][c] = false;
        }
    }
    
    visited[robotRow][robotCol] = true;
    
    cout << "Robot initialized at (" << robotRow << ", " << robotCol << ")" << endl;
}
float getDistance(int trigPin, int echoPin){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);

    if (duration == 0)
        return 999.0;

    float distance = duration * 0.0343 / 2.0;

    return distance;
}

bool FS(){
   //if front sensor  distance reading is greater that cell size . then it will detect gap .
    float distance = getDistance(F_TRIG, F_ECHO);
    if (distance > CELL_SIZE)
        return true;

    return false;
 
}
bool RS(){
   //if right sensor  distance reading is greater that cell size . then it will detect gap .
   float distance = getDistance(R_TRIG,R_ECHO);
   if (distance<CELL_SIZE) return true; 

   return false ; 
}

bool LS(){
   //if left sensor  distance reading is greater that cell size . then it will detect gap .
   float distance = getDistance(L_TRIG,L_ECHO);
   if (distance<CELL_SIZE) return true; 
   return false ; 
}

bool rearS(){
   //if rear sensor  distance reading is greater that cell size . then it will detect gap .
   float distance = getDistance(REAR_TRIG,REAR_ECHO);
   if (distance<CELL_SIZE) return true; 

   return false ; 
}

void robot::sensor() {
    robotSensor.FW_detected = false;
    robotSensor.RW_detected = false;
    robotSensor.LW_detected = false;
    robotSensor.RearW_detected = false;
    
    switch(robotDirection) {
        
        case UP:  
            if(!FS) {robotSensor.FW_detected = true; }
            if(!RS) {robotSensor.RW_detected = true; }
            if(!LS) {robotSensor.LW_detected = true;}
            if(!rearS) {robotSensor.RearW_detected = true;}
            break;
            
        case DOWN:  
            if(!rearS) {robotSensor.FW_detected = true;}
            if(!LS) {robotSensor.RW_detected = true;}
            if(!RS) {robotSensor.LW_detected = true;}
            if(!FS) {robotSensor.RearW_detected = true;}
            break;
            
        case LEFT:  
            if(!LS) {robotSensor.FW_detected = true;}
            if(!FS) {robotSensor.RW_detected = true;}
            if(!rearS) {robotSensor.LW_detected = true;}
            if(!RS) {robotSensor.RearW_detected = true;}
            break;
            
        case RIGHT: 
            if(!RS) {robotSensor.FW_detected = true;}
            if(!rearS) {robotSensor.RW_detected = true;}
            if(!FS) {robotSensor.LW_detected = true;}
            if(!LS) {robotSensor.RearW_detected = true;}
            break;
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

        // Dijkstra's algorithm (all moves cost one cell).
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
        cout << "dps done. dijkstra:"
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
void setup() {
    myRobot.robot_init();

    Serial.begin(9600);

    pinMode(F_TRIG, OUTPUT);
    pinMode(F_ECHO, INPUT);

    pinMode(R_TRIG, OUTPUT);
    pinMode(R_ECHO, INPUT);

    pinMode(L_TRIG, OUTPUT);
    pinMode(L_ECHO, INPUT);

    pinMode(REAR_TRIG, OUTPUT);
    pinMode(REAR_ECHO, INPUT);

    pinMode(LIR,INPUT): 
    pinMode(MIR,INPUT);
    pinMode(RIR,INPUT); 


}

void loop() {
        myRobot.sensor();
        myRobot.robot_movement();       
        myRobot.algo_hunter();
}
