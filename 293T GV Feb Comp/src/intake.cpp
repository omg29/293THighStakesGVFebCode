#include "intake.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include "pros/optical.hpp"
#include "pros/screen.hpp"

//intake motors
pros::Motor bottomIntake(3);
pros::Motor upperIntake(-10);

pros::MotorGroup intake{(bottomIntake, upperIntake)};

//color sensor
pros::Optical colorSensor(21);

int intakeSpeed;

void setIntake(int speed){
    intakeSpeed = speed;
}

void asyncIntakeControl(void * param){
    while(true){
        intake.move(intakeSpeed);
        printf("Hue value: %lf \n", colorSensor.get_hue());
    }
}