#pragma once

#include "api.h"
#include "pros/rotation.hpp"

// Your motors, sensors, etc. should go here.  Below are examples
//intake motor
inline pros::MotorGroup intake({-10, 3});



//color sort
//extern pros::Optical colorSort;

//clamp piston
inline pros::adi::Pneumatics clamp1('A', false);

//intake piston
inline pros::adi::Pneumatics intake_piston1('C', false);

//doinker
inline pros::adi::Pneumatics doinker1('D', false);

