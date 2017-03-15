 /**
 * Author Levi Balling
 * This code is free for anyone to use.
 * This project is using the MIT license
 */
#include <MenuEntry.h>
#include <MenuIntHelper.h>
#include <MenuLCD.h>
#include <MenuManager.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include <NikonRemote.h>// http://www.cibomahto.com/2008/10/october-thing-a-day-day-7-nikon-camera-intervalometer-part-1/

const int led = LED_BUILTIN;
//#define ENABLE_STEPPER 1
#define STEPPER_ENABLE_PIN  0
#define STEPPER_M0_PIN  1
#define STEPPER_M1_PIN  2
#define STEPPER_M2_PIN  3
#define STEPPER_RESET_PIN  4
#define STEPPER_SLEEP_PIN  5
#define STEPPER_STEP_PIN  6
#define STEPPER_DIRECTION_PIN  7

//IR Remote Pins
//#define ENABLE_IR_LED 1
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
#ifdef ENABLE_STEPPER
AccelStepper XAxisStepper(1, STEPPER_STEP_PIN, STEPPER_DIRECTION_PIN);
#endif

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

#ifdef ENABLE_LCD
MenuLCD lcdController(LC_RS_PIN, LC_EN_PIN, LC_D4_PIN, LC_D5_PIN, LC_D6_PIN, LC_D7_PIN, 16, 2);
MenuManager menuController( &lcdController);  

//LiquidCrystal lcd(LC_RS_PIN, LC_EN_PIN, LC_D4_PIN, LC_D5_PIN, LC_D6_PIN, LC_D7_PIN);
#endif

#ifdef ENABLE_IR_LED
NikonRemote camera(IR_LED_PIN);
#endif

void setup() {
  pinMode(led, OUTPUT);
  // Setup the Stepper Motor

#ifdef ENABLE_STEPPER
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);
  digitalWrite(STEPPER_ENABLE_PIN, LOW);
  pinMode(STEPPER_M0_PIN, OUTPUT);
  digitalWrite(STEPPER_M0_PIN, LOW);
  pinMode(STEPPER_M1_PIN, OUTPUT);
  digitalWrite(STEPPER_M1_PIN, LOW);
  pinMode(STEPPER_M2_PIN, OUTPUT);
  digitalWrite(STEPPER_M2_PIN, LOW);
  pinMode(STEPPER_RESET_PIN, OUTPUT);
  digitalWrite(STEPPER_RESET_PIN, HIGH);
  pinMode(STEPPER_SLEEP_PIN, OUTPUT);
  digitalWrite(STEPPER_SLEEP_PIN, HIGH);
  XAxisStepper.setMaxSpeed(10);
#endif
  XAxisStepper.setSpeed(1);

  lcdController.MenuLCDSetup();
  
  // Define the Menu objects 
  MenuEntry * rootMenuEntry = new MenuEntry("Camera Panner", NULL, NULL);

  MenuEntry * PanoramicMenuEntry = new MenuEntry("Panoramic", NULL, M2Callback);
  MenuEntry * PanoramicStartMenuEntry = new MenuEntry("Start", NULL, M2Callback);
  MenuEntry * PanoramicSettingsMenuEntry = new MenuEntry("Settings", NULL, M2Callback);
  MenuEntry * PanoramicTotalViewAngleMenuEntry = new MenuEntry("Total View Angle", NULL, M2Callback);
  MenuEntry * PanoramicFocalLengthMenuEntry = new MenuEntry("Focal Length", NULL, M2Callback);
  MenuEntry * PanoramicOverlappingMenuEntry = new MenuEntry("Overlapping", NULL, M2Callback);

  MenuEntry * TrackingMenuEntry = new MenuEntry("Tracking", NULL, M2Callback);
  MenuEntry * TrackingStartMenuEntry = new MenuEntry("Start", NULL, M2Callback);
  MenuEntry * TrackingSettingsMenuEntry = new MenuEntry("Settings", NULL, M2Callback);
  MenuEntry * TrackingSpeedMenuEntry = new MenuEntry("Speed", NULL, M2Callback);

  MenuEntry * OptionsMenuEntry = new MenuEntry("Options", NULL, M2Callback);
  MenuEntry * OptionsRightToLeftMenuEntry = new MenuEntry("Right To Left", NULL, M2Callback);
  MenuEntry * OptionsVersionMenuEntry = new MenuEntry("Version", NULL, M2Callback);

  //Add the root node, then it's children
  menuController.addMenuRoot(rootMenuEntry);
  menuController.addChild(PanoramicMenuEntry);
  
  menuController.MenuSelect();
  menuController.addChild(PanoramicStartMenuEntry);
  menuController.addChild(PanoramicSettingsMenuEntry);
  
  menuController.MenuSelect();
  menuController.addChild(PanoramicTotalViewAngleMenuEntry);
  menuController.addChild(PanoramicFocalLengthMenuEntry);    
  menuController.addChild(PanoramicOverlappingMenuEntry);
  
  menuController.MenuBack();// back from settings to Panoramic Menu
  menuController.addSibling(TrackingMenuEntry);
  
  menuController.MenuDown();//Move to the Tracking Node  
  menuController.addChild(TrackingStartMenuEntry);
  menuController.addChild(TrackingSettingsMenuEntry);
  menuController.MenuSelect();//Move to the Tracking Settings Node
  menuController.addChild(TrackingSpeedMenuEntry);
  
  menuController.MenuBack();// back from settings to Tracking
  menuController.addSibling(OptionsMenuEntry);
  menuController.MenuDown();// Move to the Options Node
  menuController.addChild(OptionsRightToLeftMenuEntry);
  menuController.addChild(OptionsVersionMenuEntry);
  menuController.SelectRoot();
  menuController.DrawMenu();   

  // Setup Buttons for the Menu System
  setupButtons();
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
//  digitalWrite(led, HIGH);
//  delay(100);
//  digitalWrite(led, LOW);
//  lcd.setCursor(0, 0);
//  lcd.print("Starting DSLR");
//  lcd.setCursor(0, 1);
//  lcd.print("Panner");
  updateButtons();
  //handleMotorUpdate();
  //handleButtonUpdate();
  //handleLimitUpdate();
  //delay(5000);
#ifdef ENABLE_IR_LED
  camera.Snap();
#endif
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
 * This function is to handle the common updates that are required
 */
void handleMotorUpdate()
{
#ifdef ENABLE_STEPPER
  bool result = XAxisStepper.runSpeed();
  
  // check where the current position of the stepper motor is where it should be.
  if(result)
  {
    lcd.print("MM:");
    lcd.setCursor(3, 0);
    lcd.print(XAxisStepper.currentPosition());
  }
#endif
}

void handleLimitUpdate()
{
#ifdef ENABLE_STEPPER
  bool result = limitSwitchOne.update();
  if(result)
  {
    //pin state changed
    
    lcd.setCursor(0, 1);
    lcd.print("B0:");
    lcd.setCursor(3, 1);
    if(limitSwitchOne.rose())
    {
      // the Limit was just Hit
      lcd.print("1");
    }
    else
    {
      // the Limit was just released
      lcd.print("0");
    }
  }
  result = limitSwitchTwo.update();
  if(result)
  {
    //pin state changed
    
    lcd.setCursor(0, 1);
    lcd.print("B0:");
    lcd.setCursor(3, 1);
    if(limitSwitchTwo.rose())
    {
      // the Limit was just Hit
      lcd.print("1");
    }
    else
    {
      // the Limit was just released
      lcd.print("0");
    }
  }
#endif
}


/**
 * The Callback for the Sub Node M1-S1
 */
void M1S1Callback( char* menuText, void *userData)
{
  char *menuLines[2] = {"M1-S1 Callback", "" };
  lcdController.PrintMenu(menuLines, 2, 3);// PrintMenu( char ** MenuString, int number of Lines, SelectedLine)
  delay(5000);
}

/**
 * The Callback for the Sub Node M1-S2
 */
void M1S2Callback( char* menuText, void *userData)
{
  char *menuLines[2] = {"M1-S2 Callback", "" };
  lcdController.PrintMenu(menuLines, 2, 3);// "Hello" is the string to print, 0 is the Row
  delay(5000);
}

/**
 * The callback for the sibiling node M2
 */
void M2Callback( char* menuText, void *userData)
{
  char *menuLines[2] = {"M2 Callback", "" };
  lcdController.PrintMenu(menuLines, 2, 3);
  delay(5000);
}

