#include "main.h"
#include "intake.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/optical.hpp"
#include "pros/screen.hpp"

//intake motors
pros::Motor bottomIntake(-14);
pros::Motor upperIntake(19);

pros::MotorGroup intake{-14, 19};

//color sensor
pros::Optical colorSensor(10);

//color sort
const int RED = 0;
const int BLUE = 1;
const int NEUTRAL = 2;

int allianceColor = 0;

const int redLowThreshold = 340;
const int redHighThreshhold = 20;

const int blueLowThreshold = 20;
const int blueHighThreshold = 60;

int intakeSpeed;

std::string seenColor = "NO RING DETECTED";
int seenHue;

std::string colorSortStates[3] = {"RED", "BLUE", "NEUTRAL"};

void setIntake(int speed){
    intakeSpeed = speed;
}

void setAllianceColor(int color){
    allianceColor = color;
}

void cycleAllianceColor(){
    allianceColor += 1;
    if (allianceColor == 3){
        allianceColor = 0;
    }
    setAllianceColor(allianceColor);
}

void asyncIntakeControl(void * param){
    bool wrongRing = false;
    int elapsedTime = 0;
    int ejectTreshold = 4;

    while(true){
        //run intake
        intake.move(intakeSpeed);        

        //color sort code
        seenHue = colorSensor.get_hue();
        if (seenHue > redLowThreshold || seenHue < redHighThreshhold){
            seenColor = "RED";
        }
        else if(seenHue > blueLowThreshold && seenHue < blueHighThreshold){
            seenColor = "BLUE";
        }
        else{
            seenColor = "NO RING DETECTED";
        }

        //print on brain
        printf("Hue value: %lf \n", colorSensor.get_hue());
        printf("Seen COlor: %lf \n", allianceColor);

        if(!wrongRing){
            if(allianceColor == 0 && seenColor == "BLUE"){
                wrongRing = true;
                elapsedTime = 0;
            }
            else if(allianceColor == 1 && seenColor == "RED"){
                wrongRing = true;
                elapsedTime = 0;
            }
            else if(allianceColor == 2){
                wrongRing = false;
            }
        }

        if(elapsedTime > ejectTreshold && wrongRing){
            upperIntake.move(0);
            pros::delay(300);
            wrongRing = false;
        }

        //delay
        pros::delay(20);
        elapsedTime += 1; //idk if it should be +20 or +1

    }
}