#include "vex.h"
#include "userFunctions.h"

using namespace vex;


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

  while (running) {
    
    // get the values of the controller's joystick axes
    double left_Axis = Controller1.Axis3.position();
    double right_Axis = Controller1.Axis2.position();

    // output the values to the file
    output_file << left_Axis << "," << right_Axis << "," << RunLauncher << ","<< runLauncherFeeder << ","<< runMainFeeder << ","<<std::endl;


    if (Controller1.ButtonY.pressing()) {
      Controller1.Screen.newLine();
      Controller1.Screen.print("Saved");
      output_file.close();
      running = false;
    }

    vex::task::sleep(10);
    
  }

}


void replay( const char* pathFile) {
  // open the file for reading
  std::ifstream input_file(pathFile);

  Brain.Screen.setPenColor(vex::color::white);

  bool debug = true;

  while (true) {
    // read a line from the file
    std::string line;
    std::getline(input_file, line);

    // check if the end of the file has been reached
    if (input_file.eof()) {
      // reset the file stream to the beginning of the file
      input_file.clear();
      input_file.seekg(0, std::ios::beg);
      x = 0;
      y = 0;
      break;
    }

  
    // parse the values from the line
    std::stringstream ss(line);

    unsigned int runlaunch, runlaunchfeed, runmainfeed = 0;

    std::string x_str;
    std::getline(ss, x_str, ',');
    std::istringstream xStream(x_str.c_str());
    xStream >> x;


    std::string y_str;
    std::getline(ss, y_str, ',');
    std::istringstream yStream(y_str.c_str());
    yStream >> y;


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


    RunLauncher = runlaunch;
    runLauncherFeeder = runlaunchfeed;
    runMainFeeder = runmainfeed;

    if (!launchControllIsRunning) {
      if (RunLauncher == 1) {
        LauncherVel = LauncherVel + 5.0;
      } else if (RunLauncher == 0 ) {
        LauncherVel = LauncherVel + -5.0;
      }
      if (LauncherVel > 100) {
        LauncherVel = 100;
      }
      if (LauncherVel < 0.0) {
        LauncherVel = 0.0;
      }

      LauncherGroup.setVelocity(LauncherVel, percent);
      LauncherFeeder.setVelocity(runLauncherFeeder * 100, percent);
      PickerUper.setVelocity(runMainFeeder, percent);
      
    }


    if (debug) {

      Brain.Screen.clearScreen();
      Brain.Screen.setCursor(1, 5);
      Brain.Screen.print("Left: ");
      Brain.Screen.print(x);
      Brain.Screen.setCursor(1, 17);
      Brain.Screen.print("Right: ");
      Brain.Screen.print(y);  
      Brain.Screen.setCursor(2, 5);
      Brain.Screen.print("RunLaunch: ");
      Brain.Screen.print(runlaunch); 
      Brain.Screen.setCursor(3, 5);
      Brain.Screen.print("RunLaunchFeed: ");
      Brain.Screen.print(runlaunchfeed); 
      Brain.Screen.setCursor(4, 5);
      Brain.Screen.print("RunMainFeed: ");
      Brain.Screen.print(runmainfeed); 
      Brain.Screen.setCursor(6, 5);
      Brain.Screen.print("Lancher Vel: ");
      Brain.Screen.print(LauncherVel);     

    }

    vex::task::sleep(10);
  }

}
