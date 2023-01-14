using namespace vex;

extern brain Brain;

// VEXcode devices
extern drivetrain Drivetrain;
extern controller Controller1;
extern motor_group LauncherGroup;
extern motor PickerUper;
extern motor LauncherFeeder;


extern motor_group RightDriveSmart;
extern motor_group LeftDriveSmart;

extern unsigned int x, y;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );