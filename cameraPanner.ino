    
/**
 * Author Levi Balling
 * This code is free for anyone to use.
 * This project is using the MIT license
 */
#include <AccelStepper.h>
#include <Bounce2.h>
#include <LiquidCrystal.h>
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

//User Input Switches
#define BUTTON_INPUT_PIN  25

// MOTOR Limit Switches
#define SWITCH_LIMIT_ONE_PIN  18
#define SWITCH_LIMIT_TWO_PIN  19

//IR Remote Pins
//#define ENABLE_IR_LED 1
#define IR_LED_PIN 17

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
Bounce debouncer = Bounce();
Bounce limitSwitchOne = Bounce();
Bounce limitSwitchTwo = Bounce();

#ifdef ENABLE_LCD
LiquidCrystal lcd(LC_RS_PIN, LC_EN_PIN, LC_D4_PIN, LC_D5_PIN, LC_D6_PIN, LC_D7_PIN);
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
  XAxisStepper.setSpeed(1);
#endif
  // Setup the Inputs and debouncers
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
  debouncer.attach(BUTTON_INPUT_PIN);
  debouncer.interval(5);

  pinMode(SWITCH_LIMIT_ONE_PIN, INPUT_PULLUP);
  limitSwitchOne.attach(SWITCH_LIMIT_ONE_PIN);
  limitSwitchOne.interval(5);

  pinMode(SWITCH_LIMIT_TWO_PIN, INPUT_PULLUP);
  limitSwitchTwo.attach(SWITCH_LIMIT_TWO_PIN);
  limitSwitchTwo.interval(5);
#ifdef ENABLE_LCD
  lcd.begin(16, 2);
  lcd.print("Starting DSLR");
#endif
}

void loop() 
{
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Starting DSLR");
  lcd.setCursor(0, 1);
  lcd.print("Panner");
  //handleMotorUpdate();
  //handleButtonUpdate();
  //handleLimitUpdate();
  //delay(5000);
#ifdef ENABLE_IR_LED
  camera.Snap();
#endif
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

/**
 * This function is to handle the common updates that are required
 */
void handleButtonUpdate()
{
#ifdef ENABLE_LCD
  bool result = debouncer.update();
  if(result)
  {
    //pin state changed
    
    lcd.setCursor(0, 1);
    lcd.print("B0:");
    lcd.setCursor(3, 1);
    if(debouncer.rose())
    {
      // the button was just pressed
      lcd.print("1");
    }
    else
    {
      // the button was just released
      lcd.print("0");
    }
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

