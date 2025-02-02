#pragma once

#include "api.h"
#include "pros/rotation.hpp"

// Your motors, sensors, etc. should go here.  Below are examples

//clamp piston
inline pros::adi::Pneumatics clamp1('A', false);

//intake piston
inline pros::adi::Pneumatics intake_piston1('C', false);

//doinker
inline pros::adi::Pneumatics doinker1('B', false);




