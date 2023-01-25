#include "vex.h"
#include "userFunctions.h"

using namespace vex;
using std::cout;
using std::endl;


// Variables
float LauncherVel;
int RunLauncher = 0;
int runLauncherFeeder = 0;
int runMainFeeder = 0;

bool launchControllIsRunning = false;

// User Functions

// Rev up and down for the disk launcher
void StartLauncherControl() {
  //Brain.Screen.print("DEBUG: Starting Rev Code");

  launchControllIsRunning = true;
  
  double maxVel = 100;

  while (true) {
    wait(0.05, seconds);

    if (LauncherGroup.temperature(vex::temperatureUnits::celsius) > 45) {
      maxVel = 50;
    } else { maxVel = 100; };

    if (RunLauncher == 1) {
      LauncherVel = LauncherVel + 5.0;
    } else if (RunLauncher == 0 ) {
      LauncherVel = LauncherVel + -5.0;
    }
    if (LauncherVel > maxVel) {
      LauncherVel = maxVel;
    }
    if (LauncherVel < 0.0) {
      LauncherVel = 0.0;
    }
    
    if (!Controller1.ButtonDown.pressing()) {
      LauncherGroup.setVelocity(LauncherVel, percent);
    } else {
      LauncherGroup.setVelocity(-50, percent);
    }


    if (runLauncherFeeder == 0) {
      LauncherFeeder.setVelocity(0, percent);
    }
    if (runLauncherFeeder == 1) {
      LauncherFeeder.setVelocity(100, percent);
    }
    if (runLauncherFeeder == -1) {
      LauncherFeeder.setVelocity(-100, percent);
    }

    if (runMainFeeder == 0) { PickerUper.setVelocity(0, percent); }
    if (runMainFeeder == 1) { PickerUper.setVelocity(100, percent); }
    if (runMainFeeder == -1) { PickerUper.setVelocity(-100, percent); }

  }
}

// Controller Button Functions
void buttonAPressed() {
  // Toggle Pickerupper
  if (PickerUper.velocity(percent) > 50.0) {
    runMainFeeder = 0;
  } else {
    runMainFeeder = 1;
  }
}

void buttonYPressed() {
  //Drivetrain.driveFor(vex::directionType::fwd, 3, vex::distanceUnits::in);
}

void buttonBPressed() {
  // Launcher Belt Control
  
  if (runLauncherFeeder == 0) {
    runLauncherFeeder = 1;
  } else {
    runLauncherFeeder = 0;
  }
}

void buttonL2Pressed() {
  runLauncherFeeder = -1;
}

void buttonL2Released() {
  runLauncherFeeder = 0;
}

void buttonR2Pressed() {
  RunLauncher = 1;
}

void buttonR2Released() {
  RunLauncher = 0;
  runLauncherFeeder = 0;
}

void buttonR1Pressed() {
  runMainFeeder = 1;
}

void buttonL1Pressed() {
  runMainFeeder = -1;
}

void buttonR1Released() {
  runMainFeeder = 0;
}

void buttonL1Released() {
  runMainFeeder = 0;
}


void record(const char* pathFile) {
  // create a file output stream
  std::ofstream output_file(pathFile);

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 0);
  Controller1.Screen.print("Y - Stop Recording");

  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(2, 2);
  Brain.Screen.print("Recording for path: ");
  Brain.Screen.setCursor(3, 4);
  Brain.Screen.print(pathFile);


  bool running = true;

  double deltaTime = 0;

  while (running) {
    
    double startTime = Brain.timer(vex::timeUnits::msec);

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(2, 2);
    Brain.Screen.print("Left Pos: ");
    //Brain.Screen.print(LeftDriveSmart.position(vex::rotationUnits::rev));

    Brain.Screen.setCursor(4, 2);
    Brain.Screen.print("Right Pos: ");
    //Brain.Screen.print(RightDriveSmart.position(vex::rotationUnits::rev));
   
    Brain.Screen.setCursor(6, 2);
    Brain.Screen.print(deltaTime);
  
    
    // output the values to the file
    output_file << rightMotorA.velocity(percent) << "," << leftMotorA.velocity(percent) << "," << rightMotorB.velocity(percent) << "," << leftMotorB.velocity(percent) << "," << RunLauncher << "," << runLauncherFeeder << "," << runMainFeeder << "," <<  deltaTime << "," << std::endl;
    

    if (Controller1.ButtonY.pressing()) {
      Controller1.Screen.newLine();
      Controller1.Screen.print("Saved");
      output_file.close();
      running = false;
    }
    
    
    
    //if (deltaTime > 100) { Brain.Screen.clearScreen(); Brain.Screen.print("Reading is too slow"); Controller1.rumble(".."); Controller1.Screen.setCursor(2, 2); Controller1.Screen.print(deltaTime);}

    vex::task::sleep(10);

    
    double endTime = Brain.timer(vex::timeUnits::msec);
    deltaTime = endTime - startTime;
  }

}


void replay( const char* pathFile) {
  // open the file for reading
  std::ifstream input_file(pathFile);

  Brain.Screen.setPenColor(vex::color::white);
  
  int launchVel = 0;

  double readDeltaTime = 0;
  double deltaTime;

  bool debug = true;

  double avgDelta = 0;
  double avgReadDelta = 0;
  double avgWaitTime = 0;
  double highestDelta = 0;
  double highestReadDelta = 0;
  int totalLines = 0;

  replaying = true;

  while (true) {

    double startTime = Brain.timer(vex::timeUnits::msec);


    //if ( LeftDriveSmart.position(vex::rotationUnits::rev) != lastLeftPos ) { LeftDriveSmart.rotateTo(lastLeftPos, vex::rotationUnits::rev); }
    //if ( RightDriveSmart.position(vex::rotationUnits::rev) != lastRightPos ) { RightDriveSmart.rotateTo(lastRightPos, vex::rotationUnits::rev); }

    // read a line from the file
    std::string line;
    std::getline(input_file, line);

    // check if the end of the file has been reached
    if (input_file.eof()) {
      // reset the file stream to the beginning of the file
      input_file.clear();
      input_file.seekg(0, std::ios::beg);

      motorFL = 0;
      motorFR = 0;
      motorBL = 0;
      motorBR = 0;

      replaying = false;


      int i;
      for ( i = 0; i < 20; i ++ ) { cout << "" << endl; }

      cout << "Replaying Done: " << endl;
      cout << "" << endl;
      cout << "Average Writing Delta: " << ( avgReadDelta / totalLines ) << endl;
      cout << "Average Reading Delta: " << ( avgDelta / totalLines ) << endl;
      cout << "Average Waiting Delta: " << ( avgWaitTime / totalLines ) << endl;
      cout << "" << endl;
      cout << "Highest Writing Delta: " << highestReadDelta << endl;
      cout << "Highest Reading Delta: " << highestDelta << endl;
      cout << "" << endl;

      //wait(10, seconds);
      break;
    }

  
    // parse the values from the line
    std::stringstream ss(line);

    unsigned int runlaunch, runlaunchfeed, runmainfeed, fr, br, fl, bl = 0;

    std::string fr_str;
    std::getline(ss, fr_str, ',');
    std::istringstream frStream(fr_str.c_str());
    frStream >> fr;
    motorFR = fr;

    std::string fl_str;
    std::getline(ss, fl_str, ',');
    std::istringstream flStream(fl_str.c_str());
    flStream >> fl;
    motorFL = fl;

    std::string br_str;
    std::getline(ss, br_str, ',');
    std::istringstream brStream(br_str.c_str());
    brStream >> br;
    motorBR = br;

    std::string bl_str;
    std::getline(ss, bl_str, ',');
    std::istringstream blStream(bl_str.c_str());
    blStream >> bl;
    motorBL = bl;

    std::string runlaunch_str;
    std::getline(ss, runlaunch_str, ',');
    std::istringstream runlaunchStream(runlaunch_str.c_str());
    runlaunchStream >> runlaunch;


    std::string runlaunchfeed_str;
    std::getline(ss, runlaunchfeed_str, ',');
    std::istringstream runlaunchfeedStream(runlaunchfeed_str.c_str());
    runlaunchfeedStream >> runlaunchfeed;


    std::string runmainfeed_str;
    std::getline(ss, runmainfeed_str, ',');
    std::istringstream runmainfeedStream(runmainfeed_str.c_str());
    runmainfeedStream >> runmainfeed;
    

    std::string readDeltaTime_str;
    std::getline(ss, readDeltaTime_str, ',');
    std::istringstream readDeltaTimeStream(readDeltaTime_str.c_str());
    readDeltaTimeStream >> readDeltaTime;




    if (runlaunch == 1) {
      launchVel = launchVel + 5.0;
    } else if (runlaunch == 0 ) {
      launchVel = launchVel + -5.0;
    }
    if (launchVel > 100) {
      launchVel = 100;
    }
    if (launchVel < 0.0) {
      launchVel = 0.0;
    }

    LauncherGroup.setVelocity(launchVel, percent);
    LauncherFeeder.setVelocity(runlaunchfeed * 100, percent);
    PickerUper.setVelocity(runmainfeed, percent);

    if (debug) {

      Brain.Screen.clearScreen();
      Brain.Screen.setCursor(1, 5);
      Brain.Screen.print("FL: ");
      Brain.Screen.print(motorFL);
      Brain.Screen.print("  FR: ");
      Brain.Screen.print(motorFR);
      Brain.Screen.print("  BL: ");
      Brain.Screen.print(motorBL);
      Brain.Screen.print("  BR: ");
      Brain.Screen.print(motorBR);  
      Brain.Screen.setCursor(2, 5);
      Brain.Screen.print("RunLaunch: ");
      Brain.Screen.print(runlaunch); 
      Brain.Screen.setCursor(3, 5);
      Brain.Screen.print("RunLaunchFeed: ");
      Brain.Screen.print(runlaunchfeed); 
      Brain.Screen.setCursor(4, 5);
      Brain.Screen.print("RunMainFeed: ");
      Brain.Screen.print(runmainfeed);

      Brain.Screen.setCursor(5, 5);
      Brain.Screen.print("Delata: ");
      Brain.Screen.print(readDeltaTime);
      Brain.Screen.print("   ");
      Brain.Screen.print(deltaTime);
      Brain.Screen.print("   ");
      Brain.Screen.print(readDeltaTime - deltaTime);

      Brain.Screen.setCursor(6, 5);
      Brain.Screen.print("Lancher Vel: ");
      //Brain.Screen.print(LauncherVel);    
 
    }

    double endTime = Brain.timer(vex::timeUnits::msec);
    deltaTime = endTime - startTime;

    Brain.Screen.setCursor(7, 5);
    Brain.Screen.print(readDeltaTime - deltaTime);
    
    avgDelta = avgDelta + deltaTime;
    avgReadDelta = avgReadDelta + readDeltaTime;
    avgWaitTime = avgWaitTime + ( readDeltaTime - deltaTime ) ;

    if ( deltaTime > highestDelta ) { highestDelta = deltaTime; }
    if ( readDeltaTime > highestReadDelta ) { highestReadDelta = readDeltaTime; }

    vex::task::sleep(fabs(readDeltaTime - deltaTime));
  }

};
