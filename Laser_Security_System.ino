#include <Keypad.h>     //Calling the keypad library 
#include <LiquidCrystal.h>
#include <EEPROM.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
const byte ROWS = 4;    //set key to four rows
const byte COLS = 3;    //set keypad to three columns
char keys[ROWS][COLS] =
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {4, 3, 2, 1};    //connect to the row pinouts of the   keypad
byte colPins[COLS] = {5, 6, 7};          //connect to the column pinouts of the  keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

enum DisplayMenu {
  MainMenu,
  ActivateMenu,
  DeactivateMenu,
  PinDeactivateMenu,
  SettingMenu,
  ChangePinMenu,
  SetupMenu,
  ResetMenu
};



//Pins//
String currentpin; //= "00000";
String enteredpin;
String newpin;
String confirmpin;
//


DisplayMenu Menu;
bool systemLocked = false;
bool systemActivated = false;
bool alarmTriggered = false;
int indicator = A0;
int LightSource = A1;
int sensor = A2;
int buzzer = 0;
void setup()
{

  pinMode(indicator, OUTPUT);
  pinMode(LightSource, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(sensor, INPUT);

  lcd.begin(16, 4);
  unLockSystem();
  if  (isSystemLocked() == true) {
    lcd.setCursor(0, 0);
    lcd.print("Visit manual to");
    lcd.setCursor(0, 1);
    lcd.print("reset system");
    return;
  }
  displayMainMenu();
  //SavePinToEEPROM("00000");
  loadPin();

}

//locking
void LockSystem() {
  systemLocked = true;
  EEPROM.write(0, 1);
}

void unLockSystem() {
  systemLocked = false;
  EEPROM.write(0, 0);
}
void displayResetMenu() {
  Menu = ResetMenu;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter reset code");
  lcd.setCursor(0, 1);
  lcd.print("____________________");
}
bool isSystemLocked() {
  byte b = EEPROM.read(0);
  if (b == 1 ) {
    systemLocked = true;
  } else {
    systemLocked = false;
  }
  return systemLocked;
}
//pin
void loadPin() {
  currentpin = ReadPinFromEEProm();
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print(SavedPin);
}

//Main Menu
void displayMainMenu()
{
  lcd.clear();
  Menu = MainMenu;
  lcd.setCursor(0, 0);
  lcd.write("Select option");
  lcd.setCursor(0, 1);
  lcd.print("1. Enable security");
  lcd.setCursor(4, 2);
  lcd.print("2. Settings");
}

//Activate Menu
void displayActivateMenu()
{
  lcd.clear();
  Menu = ActivateMenu;
  lcd.setCursor(0, 0);
  lcd.print("Press '#' key to");
  lcd.setCursor(0, 1);
  lcd.print("continue '*' to");
  lcd.setCursor(4, 2);
  lcd.print("cancel.");
}

//Deactivate Menu
int retries = 4;
void displayDeactivateMessage() {
  Menu = DeactivateMenu;
  retries = 4;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("**System activated**");
  lcd.setCursor(0, 1);
  lcd.print("Press '*' to deactiv");
  lcd.setCursor(4, 2);
  lcd.print("ate");
}

void displayPinDeactivateMenu() {
  Menu = PinDeactivateMenu;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter current pin");
  lcd.setCursor(0, 1);
  lcd.print("_____  retries(" + String(retries) + ")");
}
//

//Settings Menu//
short cp = 0;
void displaySettingMenu() {
  lcd.clear();
  Menu = SettingMenu;
  lcd.setCursor(0, 0);
  lcd.print("Select option");
  lcd.setCursor(0, 1);
  lcd.print("1. Change PIN");
  lcd.setCursor(4, 2);
  lcd.print("2. Setup");
  lcd.setCursor(4, 3);
  lcd.print("0. Back");
  cp = 0;
  enteredpin = "";
  newpin = "";
  confirmpin = "";
}

//Change pin
void displayChangePinMenu(short i)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  if (i == 0) {
    lcd.print("Enter current pin");
  } else if (i == 1) {
    lcd.print("Enter new pin");
  } else if (i == 2) {
    lcd.print("Confirm pin");
  }
  lcd.setCursor(0, 1);
  if ( i == 0) {
    lcd.print("_____  retries(" + String(retries) + ")");
  } else {
    lcd.print("_____");
  }
  lcd.setCursor(4, 2);
  lcd.print("Press * to exit");
  if (i == 3) {
    cp = 0;
    displaySettingMenu();
  } else {
    cp++;
  }
}

void displayPinError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pin retries exceeded!");
  lcd.setCursor(0, 1);
  lcd.print("Read manual to reset");
  lcd.setCursor(4, 2);
  lcd.print("device.");
}

void displayPinConfirmationError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error! Confirmation");
  lcd.setCursor(0, 1);
  lcd.print("pin not equal to");
  lcd.setCursor(4, 2);
  lcd.print("new pin.");
  lcd.setCursor(4, 3);
  lcd.print("Press * to exit");
}

void displayPin(char ch)
{

  if (Menu == PinDeactivateMenu) { //Disable security using pin
    enteredpin += ch;
    lcd.setCursor(enteredpin.length() - 1, 1);
    if (enteredpin.length() == 5 && enteredpin == currentpin) {
      enteredpin = "";
      //deactivate system
      systemActivated = false;
      alarmTriggered = false;
      retries = 4;
      displayMainMenu();
      return;
    } else if (enteredpin.length() == 5) {
      //wrong pin
      enteredpin = "";
      retries--;
      if (retries == 0) {
        //intruder alert
        alarmTriggered = true;
        retries = 4;
        displayDeactivateMessage();
      } else {
        displayPinDeactivateMenu();
      }
      return;
    }
  } else {  //Change pin
    if (cp - 1 == 0) {
      enteredpin += ch;
      lcd.setCursor(enteredpin.length() - 1, 1);
      if (enteredpin.length() == 5 && enteredpin == currentpin) {
        enteredpin = "";
        displayChangePinMenu(cp);
        retries = 4;
      } else if (enteredpin.length() == 5) {
        //error message here
        enteredpin = "";
        retries--;
        if (retries == 0) {
          //lock system
          displayPinError();
          LockSystem();
        } else {
          displayChangePinMenu(cp = 0);
        }
        return;
      }
    } else if (cp - 1 == 1) {
      newpin += ch;
      lcd.setCursor(newpin.length() - 1, 1);
      if (newpin.length() == 5) {
        displayChangePinMenu(cp);
      }
    } else if (cp - 1 == 2) {
      confirmpin += ch;
      lcd.setCursor(confirmpin.length() - 1, 1);
      if (confirmpin.length() == 5 && confirmpin == newpin) {
        //set new pin
        currentpin = newpin;
        SavePinToEEPROM(newpin);
        cp = 0;
        displaySettingMenu();;
        return;
      } else if (confirmpin.length() == 5) {
        //error message here
        displayPinConfirmationError();
        return;
      }

    }
  }
  lcd.print('*'); //mask pin
}
//
//Setting - Setup menu
bool laserDetected = false;
void displaySetupMenu() {
  Menu = SetupMenu;
  laserDetected = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please align laser");
  lcd.setCursor(0, 1);
  lcd.print("to the sensor");
  lcd.setCursor(4, 2);
  lcd.print("press * to exit");
}

void displaySetupSuccessfulMenu() {
  Menu = SetupMenu;
  laserDetected = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup succesful:");
  lcd.setCursor(0, 1);
  lcd.print("Laser Detected");
  lcd.setCursor(4, 2);
  lcd.print("press * to exit");
}

String resetCode = "12345678901234567890";
String enteredResetCode = "";
void displayResetCode(char ch) {
    enteredResetCode += ch;
    lcd.setCursor(enteredResetCode.length() - 1, 1);
    if (enteredResetCode.length() == 20 && enteredResetCode == resetCode) {
      enteredResetCode = "";
      //unlock system
       unLockSystem();  
       displayMainMenu();
       return;
    } else if (enteredResetCode.length() == 20) {
      //wrong pin
      enteredResetCode = "";
      displayResetMenu();
      return;
    }
    lcd.print('*');
}
//Secure
bool detectLightsource() {
  int sensorState = 0;
  if (modulateLight() == true) {
    digitalWrite(LightSource, HIGH);
    //delay(5);
    sensorState = digitalRead(sensor);
    if (sensorState == HIGH) { //if sensor value is high
      return true;
    } else {
      //no light source
      int c1 = 0;
      for (int c = 0; c < 30; c++) {
        digitalWrite(LightSource, HIGH);
        delay(1);
        sensorState = digitalRead(sensor);
        if (sensorState == HIGH) {
          c1++;
        } else {
          c1--;
        }
      }
      if (c1 < 20) {
        //lcd.clear();
        //lcd.setCursor(0, 0);
        // lcd.print("false 1 " + String(c1));
        return false;
      }
      return true;
    }
  } else {
    digitalWrite(LightSource, LOW);
    sensorState = digitalRead(sensor);
    if (sensorState == LOW) { //if sensor value is low
      return true;
    } else {
      //light sorce detected
      int c1 = 0;
      for (int c = 0; c < 30; c++) {
        digitalWrite(LightSource, LOW);
        delay(1);
        sensorState = digitalRead(sensor);
        if (sensorState == LOW) {
          c1++;
        } else {
          c1--;
        }
      }
      if (c1 < 20) {
        //lcd.clear();
        //lcd.setCursor(0, 0);
        // lcd.print("false 2 " + String(c1));
        return false;
      }
      return true;
    }
  }
}

int loopValue = 0;
int loopCount = 0;
bool modulateLight() {
  if (loopCount == loopValue) {
    loopCount = 0;
    loopValue = random(1, 100); //generateRandomNumber(1, 500);
    return true;
  }
  loopCount ++;
  return false;
}

//Save pin to EEPROM//
void SavePinToEEPROM(String PinCode) {
  int address = 1;
  for (char ch : PinCode) {
    EEPROM.update(address, ch);
    //EEPROM.write(address,255);
    address++;
  }
}

String ReadPinFromEEProm() {
  String PinCode;
  for (int i = 0; i <= 4; i++) {
    byte b = EEPROM.read(i);
    if (b == 0 || b == 255) {
      return "00000";
    }
    PinCode += char(EEPROM.read(i));
  }
  return PinCode;
}

//bool ledState = true;
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 100;  //blinking freq
void loop()
{
  
  char key = keypad.getKey();
  //unlock device
  if (systemLocked == true && Menu != ResetMenu) {
    if (key == '*') {
      displayResetMenu();
    }
    return;
  }
  if (Menu == ResetMenu) {
    if (key != '*' && key != '#' && key != NO_KEY) {
      displayResetCode(key);
      return;
    }
  }
  //

  //control menu
  if (key != NO_KEY)
  {
    if (Menu == MainMenu) {
      switch (key)
      {
        case '1':
          displayActivateMenu();
          break;
        case '2':
          displaySettingMenu();
          break;
      }
    } else if (Menu == ActivateMenu) {
      switch (key)
      {
        case '#':
          //Activate system
          displayDeactivateMessage();
          systemActivated = true;
          randomSeed(currentpin.toInt() + millis());
          break;
        case '*':
          displaySettingMenu();
          break;
      }
    } else if (Menu == DeactivateMenu) {
      switch (key)
      {
        case '*':
          displayPinDeactivateMenu();
          break;
      }
    } else if (Menu == PinDeactivateMenu) {
      if ((key == '*' || key == '#')  == false) {
        displayPin(key);
        //return;
      }
    } else if (Menu == SettingMenu) {
      if (cp != 0 && (key != '*' && key != '#') ) {
        displayPin(key);
        return;
      }
      switch (key) {
        case '1':
          if (cp == 0) {
            displayChangePinMenu(cp);
          }
          break;
        case '2':
          displaySetupMenu();
          break;
        case '0':
          displayMainMenu();
          break;
        case '*':
          if (cp != 0) {
            displaySettingMenu();
          }
          break;
      }

    }
  }
if (Menu == SetupMenu) {
      //digitalWrite(LightSource, HIGH);
      //lcd.clear();
      if (key == '*') {
        digitalWrite(LightSource, HIGH);
        displaySettingMenu();
        return;
      } else {
        if (laserDetected == false) {
          digitalWrite(LightSource, HIGH);
          int sensorVal = digitalRead(sensor);
          if (sensorVal == HIGH) {
            //display setup sucessful message
            digitalWrite(LightSource, LOW);
            displaySetupSuccessfulMenu();
            return;
          }
        }
      }
    }
  if (systemActivated == true) {
    if (detectLightsource() == false) {
      //trigger Alarm
      alarmTriggered = true;
    }
  } else {
    digitalWrite(LightSource, LOW);
  }
  if (alarmTriggered == true) {
    //intruder alert
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      //indicator blinks when intruder is detected
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(indicator, ledState); //trigger Alarm
    }
    //digitalWrite(buzzer, HIGH);
    tone(buzzer, 1000);
  } else if (systemActivated == true && alarmTriggered == false)  {
    //indicator light on when system is activated
    digitalWrite(indicator, HIGH);
  } else {
    digitalWrite(indicator, LOW);
   noTone(buzzer);
    
  }

}
