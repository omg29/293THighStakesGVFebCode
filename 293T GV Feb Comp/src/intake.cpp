#include "intake.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/optical.hpp"
#include "pros/screen.hpp"

//intake motors
pros::Motor bottomIntake(-13);
pros::Motor upperIntake(19);

pros::MotorGroup intake{(bottomIntake, upperIntake)};

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

        //delay
        pros::delay(20);
    }
}