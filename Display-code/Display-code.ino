/*
Bench power supply display control, showing voltages and currents.
Using the Xiao SAMD21 microcontroller programmed with Arduino and showing:
- 5V voltage and current
- 3V3 voltage and current
- 12V voltage

Featuring an i2c 16x2 LCD display and 4.5-11.5V variable power supply
*/

#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_PCF8574.h>

// Pin definitions
#define Temperature A0
#define Voltage5V   A1
#define Voltage3V3  A2
#define Voltage12V  A3
#define Current5V   A7
#define Current3V3  A8
#define led         13

// LCD parameters
#define col   16 
#define lin   2 
#define addr  0x3F
//LiquidCrystal_I2C lcd(addr, col, lin);
LiquidCrystal_PCF8574 lcd(0x3F);

// Timers and globals
unsigned long ledtime = 0;
unsigned long ntctime = 0;
unsigned long lcdtime = 0;
unsigned long analogReadingsTime = 0;
double tempCelsius = 0.0;
float voltagesAndCurrents[5] = {0};        // Index: 0=5V, 1=3V3, 2=12V, 3=5V current, 4=3V3 current
static int counter = 0;
static float averagedValues[5] = {0};

// Thermistor parameters
const double beta = 3950.0;
const double r0 = 10000.0;
const double t0 = 273.0 + 25.0;
const double rx = r0 * exp(-beta / t0);

// Voltage divider and ADC setup
const double vcc = 4.92;
const double R = 9810.0;
const int nAmostras = 5;

int error;

void setup() {
  Wire.begin();
  Wire.beginTransmission(0x3F);
  analogReadResolution(12); // 12-bit ADC for SAMD21 (0-4095)
  pinMode(led, OUTPUT);

  error = Wire.endTransmission();
  

  if (error == 0) {
    
    int show = 0;
    lcd.begin(16, 2);  // initialize the lcd

    //lcd.createChar(1, dotOff);
    //lcd.createChar(2, dotOn);

  } else {
    
  }  // if

  //lcd.init(); 
  //lcd.backlight(); 
  lcd.setBacklight(127);
  //lcd.clear(); 
}

void loop() {
  blinkLED();
  tempCelsius = readTemperature();
  readVoltagesAndCurrents(voltagesAndCurrents);
  updateDisplay(tempCelsius, voltagesAndCurrents);
}

void blinkLED() {
  if (millis() - ledtime > 300) {
    ledtime = millis();
    digitalWrite(led, !digitalRead(led));
  }
}

float readTemperature() {
  if (millis() - ntctime > 1000) {
    ntctime = millis();
    int soma = 0;
    for (int i = 0; i < nAmostras; i++) {
      soma += analogRead(Temperature);
      delay(10);
    }

    double v = (vcc * soma) / (nAmostras * 4096.0);
    double rt = (vcc * R) / v - R;
    double t = beta / log(rt / rx);
    tempCelsius = t - 273.0;
  }
  return tempCelsius;
}

void readVoltagesAndCurrents(float voltagesAndCurrents[5]) {
  if (millis() - analogReadingsTime > 50) {
    analogReadingsTime = millis();
    counter++;

    if (counter < 5) {
      voltagesAndCurrents[0] += analogRead(Voltage5V) * (3.3 / 4096.0) * 2; // voltage divider x2
      voltagesAndCurrents[1] += analogRead(Voltage3V3) * (3.3 / 4096.0) * 2;
      voltagesAndCurrents[2] += analogRead(Voltage12V) * ((3.3 / 4096.0) / 0.15); // assume scaling factor
      voltagesAndCurrents[3] += ((analogRead(Current5V) * (5.0 / 4096.0)) - 2.5) * 5.4;
      voltagesAndCurrents[4] += ((analogRead(Current3V3) * (5.0 / 4096.0)) - 2.5) * 5.4;
    } else {
      counter = 0;
      for (int i = 0; i < 5; i++) {
        averagedValues[i] = voltagesAndCurrents[i] / 4.0;
        voltagesAndCurrents[i] = 0; // reset accumulation
      }
    }
  }
}

void updateDisplay(double tempCelsius, float voltagesAndCurrents[5]) {
  if (millis() - lcdtime > 1000) {
    lcdtime = millis();
    //lcd.clear();
    /*lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(tempCelsius, 1);
    lcd.print((char)223); // degree symbol
    lcd.print("C ");

    lcd.print("5V:");
    lcd.print(averagedValues[0], 1);

    lcd.setCursor(0, 1);
    lcd.print("3V3:");
    lcd.print(averagedValues[1], 1);

    lcd.print(" 12V:");
    lcd.print(averagedValues[2], 1);*/
    lcd.setCursor(0, 0);
    lcd.print("*** first line.");
    lcd.setCursor(0, 1);
    lcd.print("*** second line.");
  }
}
