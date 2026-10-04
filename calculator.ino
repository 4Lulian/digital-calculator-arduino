#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

long firstNumber = 0;
long secondNumber = 0;
char operation = '\0';

String firstInput = "";
String secondInput = "";

bool enteringFirstNumber = true;
bool enteringSecondNumber = false;
bool resultDisplayed = false;

bool readyShown = false;
bool showingReady = false;
unsigned long readyStartTime = 0;

const long MAX_LONG = 2147483647;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {

  if (showingReady) {
    if (millis() - readyStartTime >= 3000) {
      showingReady = false;
      lcd.clear();
    }
    return;
  }

  char key = keypad.getKey();

  if (!key) {
    return;
  }

  if (key == '*') {

    firstNumber = 0;
    secondNumber = 0;
    operation = '\0';

    firstInput = "";
    secondInput = "";

    enteringFirstNumber = true;
    enteringSecondNumber = false;
    resultDisplayed = false;

    if (!readyShown) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calculator");
      lcd.setCursor(0, 1);
      lcd.print("Ready!");

      readyShown = true;
      showingReady = true;
      readyStartTime = millis();

    } else {

      lcd.clear();
    }

    return;
  }

  if (resultDisplayed) {
    return;
  }

  if (enteringFirstNumber) {

    if (key >= '0' && key <= '9') {

      int digit = key - '0';

      if (firstNumber > (MAX_LONG - digit) / 10) {
        lcd.clear();
        lcd.print("Error: Too large");
        resultDisplayed = true;
        return;
      }

      firstNumber = firstNumber * 10 + digit;
      firstInput += key;
      lcd.print(key);
    }

    else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {

      if (firstInput.length() > 0) {

        operation = key;
        enteringFirstNumber = false;
        enteringSecondNumber = true;

        if (key == 'A') lcd.print("/");
        else if (key == 'B') lcd.print("x");
        else if (key == 'C') lcd.print("-");
        else if (key == 'D') lcd.print("+");
      }
    }
  }

  else if (enteringSecondNumber) {

    if (key >= '0' && key <= '9') {

      int digit = key - '0';

      if (secondNumber > (MAX_LONG - digit) / 10) {
        lcd.clear();
        lcd.print("Error: Too large");
        resultDisplayed = true;
        return;
      }

      secondNumber = secondNumber * 10 + digit;
      secondInput += key;
      lcd.print(key);
    }

    else if (key == '#') {

      if (secondInput.length() > 0) {

        lcd.print("=");
        delay(500);

        if (operation == 'A') {

          if (secondNumber == 0) {
            lcd.clear();
            lcd.print("Error: Div by 0");
          }
          else {
            float result = (float)firstNumber / secondNumber;
            lcd.setCursor(0, 1);
            lcd.print(result);
          }
        }

        else if (operation == 'B') {

          if (firstNumber != 0 &&
              secondNumber > MAX_LONG / firstNumber) {
            lcd.clear();
            lcd.print("Error: Too large");
          }
          else {
            long result = firstNumber * secondNumber;
            lcd.setCursor(0, 1);
            lcd.print(result);
          }
        }

        else if (operation == 'C') {

          long result = firstNumber - secondNumber;
          lcd.setCursor(0, 1);
          lcd.print(result);
        }

        else if (operation == 'D') {

          if (secondNumber > MAX_LONG - firstNumber) {
            lcd.clear();
            lcd.print("Error: Too large");
          }
          else {
            long result = firstNumber + secondNumber;
            lcd.setCursor(0, 1);
            lcd.print(result);
          }
        }

        resultDisplayed = true;
      }
    }
  }
}
