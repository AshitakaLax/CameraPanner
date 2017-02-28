
/**
 * Author Levi Balling
 * This code is free for anyone to use.
 * This project is using the MIT license
 */
#include <AccelStepper.h>
#include <Bounce2.h>
#include <LiquidCrystal.h>

#define STEPPER_DIRECTION_PIN  0
#define STEPPER_STEP_PIN  1

#define BUTTON_INPUT_PIN  25

#define SWITCH_LIMIT_ONE_PIN  18
#define SWITCH_LIMIT_TWO_PIN  19


#define LC_RS_PIN  17
#define LC_EN_PIN  16
#define LC_D7_PIN  15
#define LC_D6_PIN  14
#define LC_D5_PIN  13
#define LC_D4_PIN  12

// Define Globals
AccelStepper XAxisStepper(1, STEPPER_STEP_PIN, STEPPER_DIRECTION_PIN);
Bounce debouncer = Bounce();
Bounce limitSwitchOne = Bounce();
Bounce limitSwitchTwo = Bounce();
LiquidCrystal lcd(LC_RS_PIN, LC_EN_PIN, LC_D7_PIN, LC_D6_PIN, LC_D5_PIN, LC_D4_PIN);

void setup() {
  // Setup the Stepper Motor
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

