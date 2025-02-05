#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "lb.hpp"
#include "intake.hpp"
#include "subsystems.hpp"
#include "robodash/api.h"

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup leftMotors({-6, -5, -4},
                            pros::MotorGearset::blue); // left motor group - ports 6,5, and 4, all reversed
pros::MotorGroup rightMotors({3, 2, 12}, pros::MotorGearset::blue); // right motor group - ports 3, 2, and 1, all not reversed

// Inertial Sensor on port 7
pros::Imu imu(7);


// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor, port 18, not reversed
pros::Rotation horizontal_enc(-8);

// horizontal tracking wheel. 2.75" diameter, 5.75" offset, back of the robot (negative)
lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_enc, lemlib::Omniwheel::NEW_275, -2.25);

// drivetrain settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              12.5, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
lemlib::ControllerSettings linearController(10, // proportional gain (kP)
                                            0, // integral gain (kI)
                                            3, // derivative gain (kD)
                                            0, // anti windup
                                            0, // small error range, in inches
                                            0, // small error range timeout, in milliseconds
                                            0, // large error range, in inches
                                            0, // large error range timeout, in milliseconds
                                            0 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(3, // proportional gain (kP)
                                             0, // integral gain (kI)
                                             23, // derivative gain (kD)
                                             3, // anti windup
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
);

// sensors for odometry
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel set to nullptr as we don't have one
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            &horizontal_tracking_wheel, // horizontal tracking wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

rd::Console console;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    //pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors

    //lb
    pros::Task ladyBrownTask(lbAsyncControl);

    //intake control
    pros::Task intakeTask(asyncIntakeControl);

    //reset rotation sensor
    rotationSensor.reset_position();

    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        while (true) {
            // print robot location to the brain screen
            console.printf("X:", chassis.getPose().x); // x
            console.printf("Y:", chassis.getPose().y); // y
            console.printf("Theta:", chassis.getPose().theta); // heading
            // log position telemetry
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());

            //horizontal
            //pros::lcd::print(3, "Rotation Sensor: %i", horizontalEnc.get_position());

            // delay to save resources
            pros::delay(50);


        }
    });
}

/*
void blueGoal(){
    chassis.setPose(0, 0, 0);
    chassis.turnToHeading(90, 1000);
}*/

//rd::Selector selector({
    //{"Blue Goal", blueGoal},
//});

/**
 * Runs while the robot is disabled
 */
void disabled() {}

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {}

// get a path used for pure pursuit
// this needs to be put outside a function
ASSET(example_txt); // '.' replaced with "_" to make c++ happy



/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */
void autonomous() {
    //selector.run_auton();
    chassis.setPose(0, 0, 0);
    chassis.moveToPoint(0, 24, 5000);
}

std::string currentColor = "adi = goat";


/**
 * Runs in driver control
 */
void opcontrol() {
    // controller
    // loop to continuously update motors
    while (true) {
        // get joystick positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        // move the chassis with curvature drive
        chassis.tank(leftY, rightY);
        
        //intake
        if(controller.get_digital(DIGITAL_R1)) {
            setIntake(127);
        }
        else if(controller.get_digital(DIGITAL_UP)) {
            setIntake(-127);
        }
        else {
            setIntake(0);
        }

        //lb
        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)){
            nextState();
        }

        // clamp
        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)){
            clamp1.toggle();
        }

        // doinker
        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)){
            doinker1.toggle();
        }

        //color sort
        if(controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)){
            cycleAllianceColor();
        }

        //print alliance color on controller
        if(allianceColor == 0){
            currentColor = "RED";
        }
        else if (allianceColor == 1){
            currentColor = "BLUE";
        }
        else{
            currentColor = "NEUTRAL";
        }
        controller.print(0, 0, "Counter: %d", currentColor);
        }

        // delay to save resources
        pros::delay(25);

        controller.clear();
}