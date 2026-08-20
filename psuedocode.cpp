struct wall_detected  {
bool FS=false ; 
bool RS=false ; 
bool LS=false ;
bool rearS=false; 
}

void robot::sensor() {
    robotSensor.FW_detected = false;
    robotSensor.RW_detected = false;
    robotSensor.LW_detected = false;
    robotSensor.RearW_detected = false;
    
    switch(robotDirection) {
        
        case UP:  
            if(!FS) {
                robotSensor.FW_detected = true;
            }
            if(!RS) {
                robotSensor.RW_detected = true;
            }
            if(!LS) {
                robotSensor.LW_detected = true;
            }
            if(!rearS) {
                robotSensor.RearW_detected = true;
            }
            break;
            
        case DOWN:  
            if(!rearS) {
                robotSensor.FW_detected = true;
            }
            if(!LS) {
                robotSensor.RW_detected = true;
            }
            if(!RS) {
                robotSensor.LW_detected = true;
            }
            if(!FS) {
                robotSensor.RearW_detected = true;
            }
            break;
            
        case LEFT:  
            if(!LS) {
                robotSensor.FW_detected = true;
            }
            if(!FS) {
                robotSensor.RW_detected = true;
            }
            if(!rearS) {
                robotSensor.LW_detected = true;
            }
            if(!RS) {
                robotSensor.RearW_detected = true;
            }
            break;
            
        case RIGHT: 
            if(!RS) {
                robotSensor.FW_detected = true;
            }
            if(!canMove(robotRow + 1, robotCol)) {
                robotSensor.RW_detected = true;
            }
            if(!canMove(robotRow - 1, robotCol)) {
                robotSensor.LW_detected = true;
            }
            if(!canMove(robotRow, robotCol - 1)) {
                robotSensor.RearW_detected = true;
            }
            break;
    }