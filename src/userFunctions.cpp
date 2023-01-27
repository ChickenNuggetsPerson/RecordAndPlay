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

    if ( !replaying ) {

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

   
    Brain.Screen.drawRectangle(200, 80, 80, 80);
    Brain.Screen.drawLine(240, 120, 240 + strafeLRL, 120);
    Brain.Screen.drawLine(180, 120, 180, 120 - strafeFBL);
    Brain.Screen.drawLine(300, 120, 300, 120 - strafeFBR);


    //strafeFBL = Controller1.Axis3.position();
    //strafeFBR = Controller1.Axis2.position();

    //strafeLRL = Controller1.Axis4.position();
    //strafeLRR = Controller1.Axis1.position();
  
    
    // output the values to the file
    output_file << strafeFBL << "," << strafeFBR << "," << strafeLRL << "," << strafeLRR << "," << RunLauncher << "," << runLauncherFeeder << "," << PickerUper.current(percent) << "," <<  deltaTime << "," << std::endl;
    

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

      strafeFBL = 0;
      strafeFBR = 0;
      strafeLRL = 0;
      strafeLRR = 0;

      replaying = false;


      PickerUper.setVelocity(0, percent);
      LauncherFeeder.setVelocity(0, percent);
      LauncherGroup.setVelocity(0, percent);

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

    totalLines ++;

    // parse the values from the line
    std::stringstream ss(line);

    unsigned int runlaunch, runlaunchfeed, runmainfeed, fbl, fbr, lrl, lrr = 0;

    std::string fbl_str;
    std::getline(ss, fbl_str, ',');
    std::istringstream fblStream(fbl_str.c_str());
    fblStream >> fbl;
    strafeFBL = fbl;

    std::string fbr_str;
    std::getline(ss, fbr_str, ',');
    std::istringstream fbrStream(fbr_str.c_str());
    fbrStream >> fbr;
    strafeFBR = fbr;

    std::string lrl_str;
    std::getline(ss, lrl_str, ',');
    std::istringstream lrlStream(lrl_str.c_str());
    lrlStream >> lrl;
    strafeLRL = lrl;

    std::string lrr_str;
    std::getline(ss, lrr_str, ',');
    std::istringstream lrrStream(lrr_str.c_str());
    lrrStream >> lrr;
    strafeLRR = lrr;

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
    if ( runmainfeed > 0 ) { PickerUper.setVelocity(100, percent); }
    if ( runmainfeed < 0 ) { PickerUper.setVelocity(-100, percent); }
    if ( runmainfeed == 0 ) { PickerUper.setVelocity(0, percent); } 
    

    if (debug) {

      Brain.Screen.clearScreen();
      Brain.Screen.setCursor(1, 5);
      Brain.Screen.print("FBL: ");
      Brain.Screen.print(fbl);
      Brain.Screen.print("  FBR: ");
      Brain.Screen.print(fbr);
      Brain.Screen.print("  LRL: ");
      Brain.Screen.print(lrl);
      //Brain.Screen.print("  BR: ");
      //Brain.Screen.print(motorBR);  
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
