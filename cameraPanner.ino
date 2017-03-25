
 /**
 * Author Levi Balling
 * This code is free for anyone to use.
 * This project is using the MIT license
 */
#include <MenuEntry.h>
#include <MenuIntHelper.h>
#include <MenuLCD.h>
#include <MenuManager.h>

#include <BasicStepperDriver.h>
#include <DRV8825.h>

#include <Bounce2.h>
#include <NikonRemote.h>// http://www.cibomahto.com/2008/10/october-thing-a-day-day-7-nikon-camera-intervalometer-part-1/

const int led = LED_BUILTIN;

//IR Remote Pins
#define ENABLE_IR_LED 1
#define IR_LED_PIN 27

//LCD Pins
#define ENABLE_LCD 1
#define LC_RS_PIN  23
#define LC_EN_PIN  21
#define LC_D7_PIN  41
#define LC_D6_PIN  40
#define LC_D5_PIN  39
#define LC_D4_PIN  38

// Define Globals
#define ENABLE_STEPPER 1

#define STEPPER_GEAR_RATIO 2 //It spins a gear that is half the size. meaning for every 2 revolutions the camera will spin 1.
#define STEPPER_RPM_SPEED 60 //So it can do 360 degree turn in 1 second

#define STEPPER_STEPS_PER_REV 200
#define STEPPER_ENABLE_PIN  0
#define STEPPER_M0_PIN  1
#define STEPPER_M1_PIN  2
#define STEPPER_M2_PIN  3
#define STEPPER_RESET_PIN  4
#define STEPPER_SLEEP_PIN  5
#define STEPPER_STEP_PIN  6
#define STEPPER_DIRECTION_PIN  7

DRV8825 XAxisStepper(STEPPER_STEPS_PER_REV, STEPPER_DIRECTION_PIN, STEPPER_STEP_PIN, STEPPER_M0_PIN, STEPPER_M1_PIN, STEPPER_M2_PIN);

// Example uses 4 buttons
// User Input Switches
#define BUTTON_UP_PIN  45
#define BUTTON_DOWN_PIN  44
#define BUTTON_BACK_PIN  43
#define BUTTON_SELECT_PIN  42
Bounce debounceUp = Bounce();
Bounce debounceDown = Bounce();
Bounce debounceBack = Bounce();
Bounce debounceSelect = Bounce();

String VERSION = "0.0.3";

// System Settings Variable

// Panoramic Settings
double g_TotalViewingAngle = 25.0;
int g_FocalLength = 55;//highest for my current lens
double g_Overlapping = 0.1;//overlap each lense by 10 percent of Current Field of View
double g_shutterDuration = 0.05;//this ranges from 0.00025 in increments 0.01

#define FOCAL_LENGTH_COUNT 37
int g_FocalLengthIndex = 16;
int g_FocalLengthArr[FOCAL_LENGTH_COUNT] ={
10,
11,
12,
14,
15,
17,
18,
19,
20,
24,
28,
30,
35,
45,
50,
55,
60,
70,
75,
80,
85,
90,
100,
105,
120,
125,
135,
150,
170,
180,
200,
210,
300,
400,
500,
600,
800
};

double g_FieldOfViewAngleArr[FOCAL_LENGTH_COUNT] = {
99.0,
93.5,
88.5,
79.8,
75.9,
69.1,
66.0,
63.2,
60.7,
52.0,
45.4,
42.6,
37.0,
29.1,
26.3,
24.0,
22.1,
19.0,
17.7,
16.6,
15.7,
14.8,
13.3,
12.7,
11.1,
10.7,
9.9,
8.9,
7.9,
7.4,
6.7,
6.4,
4.5,
3.4,
2.7,
2.2,
1.7
};


// Tracking Settings
double g_DegreesPerMin = 5.0;//

// Global Settings
bool g_RightToLeft = false;// the camera pans from Right to Left

#ifdef ENABLE_LCD
MenuLCD lcdController(LC_RS_PIN, LC_EN_PIN, LC_D4_PIN, LC_D5_PIN, LC_D6_PIN, LC_D7_PIN, 16, 2);
MenuManager menuController( &lcdController);  

#endif

#ifdef ENABLE_IR_LED
NikonRemote camera(IR_LED_PIN);
#endif

void setup() {
  pinMode(led, OUTPUT);
  // Setup the Stepper Motor

  //setupStepperMotor();
  // Setup Buttons for the Menu System
  setupMenu();
  setupButtons();
}

/**
 * Setup the Stepper Motor
 */
void setupStepperMotor()
{
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);
  digitalWrite(STEPPER_ENABLE_PIN, LOW);
  pinMode(STEPPER_RESET_PIN, OUTPUT);
  digitalWrite(STEPPER_RESET_PIN, HIGH);
  pinMode(STEPPER_SLEEP_PIN, OUTPUT);
  digitalWrite(STEPPER_SLEEP_PIN, HIGH);
  XAxisStepper.setRPM(STEPPER_RPM_SPEED);
}

/**
 * Setup the menu for the System
 */
void setupMenu()
{
  lcdController.MenuLCDSetup();
  // Define the Menu objects 
  MenuEntry * rootMenuEntry = new MenuEntry("Camera Panner", NULL, NULL);

  MenuEntry * PanoramicMenuEntry = new MenuEntry("Panoramic", NULL, emptyCallback);
  MenuEntry * PanoramicStartMenuEntry = new MenuEntry("Start", NULL, StartPanoramicPhoto);
  MenuEntry * PanoramicSettingsMenuEntry = new MenuEntry("Settings", NULL, emptyCallback);
  MenuEntry * PanoramicTotalViewAngleMenuEntry = new MenuEntry("Total View Angle", NULL, TotalViewingAngle);
  MenuEntry * PanoramicFocalLengthMenuEntry = new MenuEntry("Focal Length", NULL, CameraFocalLength);
  MenuEntry * PanoramicOverlappingMenuEntry = new MenuEntry("Overlapping", NULL, PanOverLapDegrees);

  MenuEntry * TrackingMenuEntry = new MenuEntry("Tracking", NULL, emptyCallback);
  MenuEntry * TrackingStartMenuEntry = new MenuEntry("Start", NULL, emptyCallback);
  MenuEntry * TrackingSettingsMenuEntry = new MenuEntry("Settings", NULL, emptyCallback);
  MenuEntry * TrackingSpeedMenuEntry = new MenuEntry("Speed", NULL, emptyCallback);

  MenuEntry * OptionsMenuEntry = new MenuEntry("Options", NULL, emptyCallback);
  MenuEntry * OptionsRightToLeftMenuEntry = new MenuEntry("Right To Left", NULL, emptyCallback);
  MenuEntry * ExposureMenuEntry = new MenuEntry("Exposure Time", NULL, ShutterSpeedSetting);//we need to figure out the Camera.Snap code
  MenuEntry * OptionsVersionMenuEntry = new MenuEntry("Version", &VERSION, DisplayUserDataString);

  //Add the root node, then it's children
  menuController.addMenuRoot(rootMenuEntry);
  menuController.addChild(PanoramicMenuEntry);
  
  menuController.MenuSelect();//Select the Panoramic Menu
  menuController.addChild(PanoramicStartMenuEntry);
  menuController.addChild(PanoramicSettingsMenuEntry);
  
  menuController.MenuSelect();//Select the Start level
  menuController.MenuDown();//Move down to Settings
  menuController.addChild(PanoramicTotalViewAngleMenuEntry);
  menuController.addChild(PanoramicFocalLengthMenuEntry);    
  menuController.addChild(PanoramicOverlappingMenuEntry);
  
  menuController.MenuBack();// back from settings to Panoramic Menu
  menuController.addSibling(TrackingMenuEntry);
  
  menuController.MenuDown();//Move to the Tracking Node  
  menuController.addChild(TrackingStartMenuEntry);
  menuController.addChild(TrackingSettingsMenuEntry);
  menuController.MenuSelect();//Move to the Tracking start Settings Level
  menuController.MenuDown();//Move to the Settings Node
  menuController.addChild(TrackingSpeedMenuEntry);
  
  menuController.MenuBack();// back from settings to Tracking
  menuController.addSibling(OptionsMenuEntry);
  menuController.MenuDown();// Move to the Options Node
  menuController.addChild(OptionsRightToLeftMenuEntry);
  menuController.addChild(ExposureMenuEntry);
  menuController.addChild(OptionsVersionMenuEntry);
  menuController.SelectRoot();
  menuController.DrawMenu();   
}

/**
 * Sets up 4 debounced buttons to handle the Menu Options
 */
void setupButtons()
{
  // Setup the Inputs and debouncers
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  debounceUp.attach(BUTTON_UP_PIN);
  debounceUp.interval(5);
  
  // Setup Down Button
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  debounceDown.attach(BUTTON_DOWN_PIN);
  debounceDown.interval(5);

  // Setup Back Button
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  debounceBack.attach(BUTTON_BACK_PIN);
  debounceBack.interval(5);

  // Setup Select Button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  debounceSelect.attach(BUTTON_SELECT_PIN);
  debounceSelect.interval(5);
}

void loop() 
{
  updateButtons();
 // handleMotorUpdate();
}

/**
 * Simple function to listen to the button presses to trigger the menu actions
 * TODO change this from a Menu Action to return Bool[4] for better code reuse while executing sub menu routines
 */
void updateButtons()
{
  debounceUp.update();
  debounceDown.update();
  debounceBack.update();
  debounceSelect.update();
  
  if(debounceUp.fell())
  {  
    menuController.DoMenuAction(MENU_ACTION_UP);
  }
  else if(debounceDown.fell())
  {  
    menuController.DoMenuAction(MENU_ACTION_DOWN);
  }
  else if(debounceBack.fell())
  {  
    menuController.DoMenuAction(MENU_ACTION_BACK);
  }
  else if(debounceSelect.fell())
  {  
    menuController.DoMenuAction(MENU_ACTION_SELECT);
  }
}

/**
 * Populates a boolean array that was passed in with the values
 */
void getButtonsPressed(bool* buttonArr)
{
  debounceUp.update();
  debounceDown.update();
  debounceBack.update();
  debounceSelect.update();
  
  buttonArr[0] = debounceUp.fell();
  buttonArr[1] = debounceDown.fell();
  buttonArr[2] = debounceBack.fell();
  buttonArr[3] = debounceSelect.fell();
  
}


/**
 * This function is to handle the common updates that are required
 */
void handleMotorUpdate()
{
  XAxisStepper.setMicrostep(1);
  XAxisStepper.rotate(360);
}

/**
* Gets the amount of Field of view angle that will be overlapping between each photo
*/
double GetOverlappingAngle()
{
	// multiple the number of 
	return g_Overlapping * g_FieldOfViewAngleArr[g_FocalLengthIndex];
}

/**
* Based on the FOV, and the amount of overlapping this will determine the number of pictures that will need to be taken.
*/
int GetNumberOfPanoramicPhotos()
{
	// Get the Total Viewing Angle
	g_TotalViewingAngle;
	// Get the single picture Field of View Angle
	double fov = g_FieldOfViewAngleArr[g_FocalLengthIndex];
	// Subtract the overlapping amount from the Field of View Angle
	fov = fov - GetOverlappingAngle();
	// divide the total viewing angle by the result
	int numberOfPics = (int)(g_TotalViewingAngle / fov);
	//This is to always take the extra picture at the end to make sure that we get all the image we need.
	numberOfPics++;
	return numberOfPics;
}
double GetRotatingAngle()
{
  g_TotalViewingAngle;
  // Get the single picture Field of View Angle
  double fov = g_FieldOfViewAngleArr[g_FocalLengthIndex];
  // Subtract the overlapping amount from the Field of View Angle
  fov = fov - GetOverlappingAngle();
  return fov;
}

/**
 * The Callback for the Sub Node M1-S2
 */
void DisplayUserDataString( char* menuText, void *userData)
{
  String * versionPtr = (String*)userData;
//  char versionStr[16];
//  versionPtr->toCharArray(versionStr, versionPtr->length()+1);
//  char *menuLines[2] = {versionStr, "" };
//  lcdController.PrintMenu(menuLines, 2, 3);// "Hello" is the string to print, 0 is the Row
  String strTwo = "";
  lcdPrintString(versionPtr, &strTwo);
  delay(5000);
}


/**
 * The callback for the Total Viewing Angle
 */
void StartPanoramicPhoto( char* menuText, void *userData)
{
  String lineOne = "# ";
  String lineTwo = "";
  int numOfPictures = GetNumberOfPanoramicPhotos();
  int currentPic = 0;
  int delayBetween = (int)(g_shutterDuration * 1000);//convert to an int
  lineOne.concat(currentPic);
  
  lineOne.concat(" of ");
  lineOne.concat(numOfPictures);
  lcdPrintString(&lineOne, &lineTwo);

  // TODO
  // Calibrate the start position
  
  //rotating angle 
  //1 rev/Second
  // rotateAngle / 360 = percentage of 1 revolution
  double rotateAngle = GetRotatingAngle();
  int milliSecondRotateDelay = 1000;// enough time for 1 whole rotation

  while(currentPic < numOfPictures)
  {
    lineOne = "# ";
    lineOne.concat(currentPic);
    
    lineOne.concat(" of ");
    lineOne.concat(numOfPictures);
    lcdPrintString(&lineOne, &lineTwo);
	camera.Snap();
    // Determine if the setting is bulb
    // Wait till the photo is done taking a photo
    delay(delayBetween);
    // if bulb send camera snap again to close the picture
    
    // rotate motor
	XAxisStepper.rotate(rotateAngle);
    //wait for rotation to be complete
	delay(milliSecondRotateDelay);
    currentPic++;//move to take the next photo
  }
}



void emptyCallback( char* menuText, void *userData)
{
  
  char *menuLines[2] = {"Temp Callback", "Temp Callback"};
  lcdController.PrintMenu(menuLines, 2, 3);// "Hello" is the string to print, 0 is the Row
}

/**
 * The callback for the Total Viewing Angle
 */
void TotalViewingAngle( char* menuText, void *userData)
{
  String lineOne = "Tot View Angle";
  String lineTwo = "";
  lineTwo.concat(g_TotalViewingAngle);
  lcdPrintString(&lineOne, &lineTwo);

  while(true)
  {
    bool buttonState[4];
    getButtonsPressed(buttonState);
    if(buttonState[0])
    {
      //up pressed
      g_TotalViewingAngle += 5.0;
      if(g_TotalViewingAngle > 360)
      {
        g_TotalViewingAngle = 360.0;
      }
    }
    else if(buttonState[1])
    {
      //Down pressed
      g_TotalViewingAngle -= 5.0;
      if(g_TotalViewingAngle < 0)
      {
        g_TotalViewingAngle = 0;
      }
    }
    else if(buttonState[2] || buttonState[3])
    {
      //Back or Select Pressed
      return;
    }
    else
    {
      continue;
    }
    lineTwo = "";
    lineTwo.concat(g_TotalViewingAngle);
    lcdPrintString(&lineOne, &lineTwo);
  }
}

/**
 * The callback for Setting the Camera Focal Length
 */
void CameraFocalLength( char* menuText, void *userData)
{
  String lineOne = "F Length:";
  String lineTwo = "FOV Angle:";
  lineOne.concat(g_FocalLengthArr[g_FocalLengthIndex]);
  lineTwo.concat(g_FieldOfViewAngleArr[g_FocalLengthIndex]);
  lcdPrintString(&lineOne, &lineTwo);
  
  while(true)
  {
    bool buttonState[4];
    getButtonsPressed(buttonState);
    if(buttonState[0])
    {
      //up pressed
      g_FocalLengthIndex++;
      if(g_FocalLengthIndex > FOCAL_LENGTH_COUNT)
      {
        g_FocalLengthIndex = FOCAL_LENGTH_COUNT;
      }
    }
    else if(buttonState[1])
    {
      //Down pressed
      g_FocalLengthIndex--;
      if(g_FocalLengthIndex < 0)
      {
        g_FocalLengthIndex = 0;
      }
    }
    else if(buttonState[2] || buttonState[3])
    {
      //Back or Select Pressed
      return;
    }
    else
    {
      continue;
    }
	
    lineOne = "F Length:";
    lineTwo = "FOV Angle:";
    lineOne.concat(g_FocalLengthArr[g_FocalLengthIndex]);
    lineTwo.concat(g_FieldOfViewAngleArr[g_FocalLengthIndex]);
    lcdPrintString(&lineOne, &lineTwo);
  }
}

/**
 * The callback for Setting the Panning overlap by percentage
 */
void PanOverLapDegrees( char* menuText, void *userData)
{
  String lineOne = "FOV %:";
  String lineTwo = "FOV Overlap:";
  lineOne.concat(g_Overlapping);
  lineTwo.concat(GetOverlappingAngle());
  lcdPrintString(&lineOne, &lineTwo);
  
  while(true)
  {
    bool buttonState[4];
    getButtonsPressed(buttonState);
	
    if(buttonState[0])
    {
      //up pressed
      g_Overlapping += 0.05;
      if(g_Overlapping >0.95)
      {
        g_Overlapping = 0.95;
      }
    }
    else if(buttonState[1])
    {
      //Down pressed
      g_Overlapping -= 0.05;
      if(g_Overlapping < 0.0)
      {
        g_Overlapping = 0.0;
      }
    }
    else if(buttonState[2] || buttonState[3])
    {
      //Back or Select Pressed
      return;
    }
    else
    {
      continue;
    }
	

    lineOne = "FOV %:";
    lineTwo = "FOV Overlap:";
    lineOne.concat(g_Overlapping);
    lineTwo.concat(GetOverlappingAngle());
    lcdPrintString(&lineOne, &lineTwo);
  }
}

/**
 * The callback for Setting the Shutter Speed
 */
void ShutterSpeedSetting( char* menuText, void *userData)
{
  String lineOne = "Shutter Speed";
  String lineTwo = "";
  lineTwo.concat(g_shutterDuration);
  lcdPrintString(&lineOne, &lineTwo);
  
  while(true)
  {
    bool buttonState[4];
    double incrementAmount = 0.001;
    getButtonsPressed(buttonState);
    
    //determine the increment amount
	
    if(g_shutterDuration >= 0.01 && g_shutterDuration <= 0.15)
    {
      incrementAmount = 0.01;
    }
    if(g_shutterDuration >= 0.1 && g_shutterDuration <= 1.5)
    {
      incrementAmount = 0.1;
    }
    if(g_shutterDuration > 1.0)
    {
      incrementAmount = 1;
    }

    if(buttonState[0])
    {
      //up pressed
      g_shutterDuration += incrementAmount;
      if(g_shutterDuration >30.0)
      {
        g_shutterDuration = 30.0;
      }
    }
    else if(buttonState[1])
    {
      //Down pressed
      g_shutterDuration -= incrementAmount;
      if(g_shutterDuration < 0.00025)
      {
        g_Overlapping = 0.00025;
      }
    }
    else if(buttonState[2] || buttonState[3])
    {
      //Back or Select Pressed
      return;
    }
    else
    {
      continue;
    }

    lineOne = "Shutter Speed";
    lineTwo = "";
    lineTwo.concat(g_shutterDuration);
    lcdPrintString(&lineOne, &lineTwo);
  }
}


void lcdPrintString(String* lineOne, String* lineTwo)
{
  char strOne[16] = "";
  char strTwo[16] = "";
  lineOne->toCharArray(strOne, lineOne->length()+1);
  lineTwo->toCharArray(strTwo, lineTwo->length()+1);
  
  char *menuLines[2] = {strOne, strTwo};
  lcdController.PrintMenu(menuLines, 2, 3);// "Hello" is the string to print, 0 is the Row
}

