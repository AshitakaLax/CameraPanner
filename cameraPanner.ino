
/**
 * Author Levi Balling
 * This code is free for anyone to use.
 * This project is using the MIT license
 */
#include <AccelStepper.h>
#include <Bounce2.h>
#include <LiquidCrystal.h>
#include <NikonRemote.h>


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
#define IR_LED_PIN 17

//LCD Pins
#define LC_RS_PIN  23
#define LC_EN_PIN  21
#define LC_D7_PIN  41
#define LC_D6_PIN  40
#define LC_D5_PIN  39
#define LC_D4_PIN  28

// Define Globals
AccelStepper XAxisStepper(1, STEPPER_STEP_PIN, STEPPER_DIRECTION_PIN);
Bounce debouncer = Bounce();
Bounce limitSwitchOne = Bounce();
Bounce limitSwitchTwo = Bounce();
LiquidCrystal lcd(LC_RS_PIN, LC_EN_PIN, LC_D7_PIN, LC_D6_PIN, LC_D5_PIN, LC_D4_PIN);

NikonRemote camera(IR_LED_PIN);

void setup() {
  // Setup the Stepper Motor
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

  lcd.begin(16, 2);
  lcd.print("Starting DSLR Panner");
}

void loop() {
  handleMotorUpdate();
  handleButtonUpdate();
  handleLimitUpdate();
  delay(5000);
  camera.Snap();
}

/**
 * This function is to handle the common updates that are required
 */
void handleMotorUpdate()
{
  bool result = XAxisStepper.runSpeed();
  
  // check where the current position of the stepper motor is where it should be.
  if(result)
  {
    lcd.print("MM:");
    lcd.setCursor(3, 0);
    lcd.print(XAxisStepper.currentPosition());
  }
}

/**
 * This function is to handle the common updates that are required
 */
void handleButtonUpdate()
{
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
}

void handleLimitUpdate()
{
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
}

