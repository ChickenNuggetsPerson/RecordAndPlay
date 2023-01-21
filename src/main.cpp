/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    19, 20, 11, 12  
// Controller1          controller                    
// LauncherGroup        motor_group   14, 15          
// PickerUper           motor         16              
// LauncherFeeder       motor         13              
// DiskLimitSwitch      limit         B               
// fnewmatics           digital_out   A               
// LineLeft             line          G               
// LineMid              line          F               
// LineRight            line          H               
// ---- END VEXCODE CONFIGURED DEVICES ----


#include "vex.h"

#include "userFunctions.h"

using namespace vex;


const char* pathNames[5] = {
  "Paths/Skills/main.txt",
  "Paths/Left/Launch.txt",
  "Paths/Right/GoToRoller.txt",
  "Paths/Right/RollerToMid.txt",
  "test.txt"
};


int readFile(const char* fileName) {
  unsigned int ival = 0;
  unsigned char readBuff[4];
  Brain.SDcard.loadfile(fileName, readBuff, 4);
	ival = *(unsigned int*)(readBuff);
  return ival;
};


// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................

  Brain.Screen.newLine();
  Brain.Screen.print("Starting Autonomous Stuff");

  replay(pathNames[0]);
 
  wait(0.5, seconds);

  
    
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {

  motorFL = 0;
  motorFR = 0;
  motorBL = 0;
  motorBR = 0;

  replaying = false;

  // Start Launcher rev code
  StartLauncherControl();

}





// Once the match is started
int whenStarted() {

  // Start the motors with 0% velocity
  LauncherGroup.spin(forward);
  LauncherGroup.setVelocity(0.0, percent);
  LauncherFeeder.spin(forward);
  LauncherFeeder.setVelocity(0.0, percent);
  PickerUper.spin(forward);
  PickerUper.setVelocity(0.0, percent);



  andrewDriving = readFile("andrewDriving");



  int hover = 0;
  int selected = 0;


  while (true) {
    Brain.Screen.clearScreen();
    Brain.Screen.setPenColor(vex::color::white);
    Brain.Screen.setCursor(2, 3);
    Brain.Screen.print("Select A Path:");
     Brain.Screen.print("FL: ");
      Brain.Screen.print(motorFL);
      Brain.Screen.print("  FR: ");
      Brain.Screen.print(motorFR);
      Brain.Screen.print("  BL: ");
      Brain.Screen.print(motorBL);
      Brain.Screen.print("  BR: ");
      Brain.Screen.print(motorBR);  

    int i;
    for (i=0; i < 5; i++) {
      if ( i == hover ) {
        Brain.Screen.setPenColor(vex::color::yellow);
        Brain.Screen.drawRectangle(35, 50 + ( i * 40 ), 350, 40);
      }
      if ( i == selected ) {
        Brain.Screen.setPenColor(vex::color::green);
        Brain.Screen.setCursor(4 + ( i * 2 ), 5);
        Brain.Screen.print(pathNames[i]);
      } else {
        Brain.Screen.setPenColor(vex::color::white);
        Brain.Screen.setCursor(4 + ( i * 2 ), 5);
        Brain.Screen.print(pathNames[i]);
      }

    }

    Controller1.Screen.clearScreen();

    Controller1.Screen.setCursor(1, 0);
    Controller1.Screen.print("Left - Record");
    Controller1.Screen.setCursor(2, 0);
    Controller1.Screen.print("Right - Play");



    if (Controller1.ButtonDown.pressing()) { hover++; }
    if (Controller1.ButtonUp.pressing()) { hover--; }

    if (hover > 4) { hover = 0; }
    if (hover < 0) { hover = 4; }

    if (Controller1.ButtonA.pressing()) { selected = hover; Controller1.Screen.print(selected);}

    if (Controller1.ButtonLeft.pressing()) {
      motorFL = 0;
      motorFR = 0;
      motorBL = 0;
      motorBR = 0;

      record(pathNames[selected]);
    }

    if (Controller1.ButtonRight.pressing()) {
      
      motorFL = 0;
      motorFR = 0;
      motorBL = 0;
      motorBR = 0;


      replay(pathNames[selected]);
    }

    wait(0.2, seconds);
  }
 
  return 0;
}


//
// Main will set up the competition functions and callbacks.
//
int main() {

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  
  // Run the pre-autonomous function.
  pre_auton();

  // Set up User Callbacks

  Controller1.ButtonA.pressed(buttonAPressed);
  Controller1.ButtonY.pressed(buttonYPressed);
  Controller1.ButtonB.pressed(buttonBPressed);

  Controller1.ButtonL2.pressed(buttonL2Pressed);
  Controller1.ButtonL2.released(buttonL2Released);

  Controller1.ButtonR2.pressed(buttonR2Pressed);
  Controller1.ButtonR2.released(buttonR2Released);


  Controller1.ButtonR1.pressed(buttonR1Pressed);
  Controller1.ButtonL1.pressed(buttonL1Pressed);

  Controller1.ButtonR1.released(buttonR1Released);
  Controller1.ButtonL1.released(buttonL1Released);

  // Start the 
  whenStarted();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}




