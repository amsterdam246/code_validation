#include <Arduino.h> // u need this in ur pio projects 
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // creating an lcd object.

// password logic
#define password_Length 6 // user input password length.
char master[password_Length] = "12345";
char userInput[password_Length]; // this variable holds the user input.

// lcd menu logic.
const int noOfChars = 16;
int i = 0; // the buttons are toggling i.
char charArray[noOfChars] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'A', 'B', 'C', 'D', 'E', 'F'};
char currentChar = charArray[i]; // variable points at the the first index in the char array.
int pressCount = 0;              // keeps track of the press on the select button.

// button logic.
const int noOfInputs = 5; // the no of buttons we are going to have.
const int inputPins[noOfInputs] = {10, 9, 8, 7, 6};
bool lastInputState[noOfInputs] = {HIGH, HIGH, HIGH, HIGH, HIGH}; // previous button state.However, it's worth noting that initializing the lastInputState array to LOW is a more common practice in software development,// as it's often easier to reason about the default state of a system or a program if you assume that inputs are initially low. This is particularly true for digital inputs, where low typically means "off" or "not activated".
bool inputState[noOfInputs];
bool inputFlags[noOfInputs] = {HIGH, HIGH, HIGH, HIGH, HIGH}; 
int lastDebounceTime[noOfInputs] = {0, 0, 0, 0, 0};
int debounceDelay = 10;

//foward declarations for pio vs (u dont need this from ur arduino ide)
void printScreen();
void stringCompare();
void clearData();
void inputAction(int input);
void resolveInputFlags();
void setInputFlags();

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("Entre Password: ");

  // setup the pins as inputs
  for (int j = 0; j < noOfInputs; j++)
  {
    pinMode(inputPins[j], INPUT_PULLUP);
  }
}

// main function.
void loop()
{
  lcd.setCursor(pressCount, 1);
  lcd.print(currentChar);
  setInputFlags();
  resolveInputFlags();
}


void setInputFlags() // debounce the buttons.
{
  for (int j = 0; j < noOfInputs; j++)
  {
    int reading = digitalRead(inputPins[j]); // captures the button has become high and hands over the result to the varable reading.
    if (reading != lastInputState[j])        // that is if a change is detected ie state change detection.
    {
      lastDebounceTime[j] = millis();
    }

    if ((millis() - lastDebounceTime[j]) > debounceDelay)
    { // that is if enough time has passed to ignore the bounces of the buttons
      if (reading != inputState[j])
      {
        inputState[j] = reading;
        if (inputState[j] == LOW)
        {
          inputFlags[j] = LOW;
        }
      }
    }
    lastInputState[j] = reading;
  }
}

void resolveInputFlags()
{
  for (int j = 0; j < noOfInputs; j++)
  {
    if (inputFlags[j] == LOW)
    {
      inputAction(j);
      inputFlags[j] = HIGH;
      printScreen();
    }
  }
}

void inputAction(int input)
{
  if (input == 0) // if left button is pressed R for right
  {
    if (i == 0)
    {
      i = noOfChars - 1;
    }
    else
    {
      i--;
    }
  }
  else if (input == 1) // if right button is pressed L for left
  {
    if (i == noOfChars - 1)
    {
      i = 0;
    }
    else
    {
      i++;
    }
  }
  else if (input == 2) // this is select  button S for stop
  {
    userInput[pressCount] = currentChar;
    pressCount++;
    lcd.setCursor(pressCount, 1);
    i = 0;
  }
  else if (input == 3) // this is the clear button C for clear
  {
    i = 0;
    clearData();
  }
  else if (input == 4) // this is entre button E for enter
  {

    userInput[pressCount] = currentChar;
    Serial.print(F("value of stringinput: "));
    Serial.println(userInput);
    stringCompare();
    i = 0;
  }
  else // this is for handling long press of buttons.
  {
    int longPressDelay = 1000;               // set the long press delay to 1 second
    unsigned long startPressTime = millis(); // get the current time

    // loop until the button is released or until the long press delay is reached.
    while (digitalRead(inputPins[input]) == LOW && (millis() - startPressTime) < longPressDelay)
    {
      // do nothing while waiting for the long press delay.
    }

    if ((millis() - startPressTime) >= longPressDelay)
    {
      // handle the long press event
      if (input == 0) // if left button is pressed R. 
      {
        if (i == 0)
        {
          i = noOfChars - 1;
        }
        else
        {
          i--;
        }
      }
      else if (input == 1) // if right button is pressed L
      {
        if (i == noOfChars - 1)
        {
          i = 0;
        }
        else
        {
          i++;
        }
      }
      printScreen(); // update the LCD display after the long press event.
    }
  }
}

void clearData()
{
  while (pressCount != 0)
  {
    userInput[pressCount--] = 0;
  }

  setup();
}

void stringCompare()
{
  lcd.setCursor(0, 0); // you have to print things again coz its a new function.
  lcd.print("Entre password");
  lcd.setCursor(0, 1);
  lcd.print(userInput);
  // delay(10000);

  if (strcmp(userInput, master) == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ACCESS GRANTED");
    lcd.setCursor(0, 1);
    lcd.print("WELCOME");
    delay(5000);
    clearData();
  }
  else if (strcmp(userInput, master) != 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ACCESS DENIED");
    delay(5000);
    clearData();
  }
}

void printScreen()
{
  Serial.print("value of stringinput: ");
  Serial.println(userInput);
  Serial.print("pressCount: ");
  Serial.println(pressCount);
  currentChar = charArray[i];
  Serial.println(currentChar);
  lcd.setCursor(pressCount, 1);
  lcd.print(currentChar);
}
