#include "intake.hpp"

int intakeSpeed;

void setIntake(int speed){
    intakeSpeed = speed;
}

void asyncIntakeControl(void * param){
    while(true){
        intake.move(intakeSpeed);
    }
}