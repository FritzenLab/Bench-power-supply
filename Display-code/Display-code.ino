/*
Bench power supply display control, showing voltages and currents.
Using the Xiao SAMD21 microcontroller programmed with Arduino and showing:
- 5V voltage and current
- 3V3 voltage and current
- 12V voltage

Featuring an i2c 16x2 LCD display and 4.5-11.5V variable power supply
*/
#include <Wire.h>
#define Temperature A0
#define Voltage5V A1
#define Voltage3V3 A2
#define Voltage12V A3
#define Current5V A7
#define Current3V3 A8
#define led 13

#define col 16 
#define lin  2 
#define addr  0x27 
LiquidCrystal_I2C lcd(addr,col,lin);

int ledtime= 0;
int ntctime= 0;
int lcdtime= 0;
int analogReadingsTime= 0;
double tempCelsius= 0.0;
float vc[5]= 0;

// Parâmetros do termistor
const double beta = 3950.0;
const double r0 = 10000.0;
const double t0 = 273.0 + 25.0;
const double rx = r0 * exp(-beta/t0);
 
// Parâmetros do circuito
const double vcc = 4.92;
const double R = 9810.0;
 
// Numero de amostras na leitura
const int nAmostras = 5;

void setup() {
  Wire.begin();
  analogReadResolution(12); // 4095 steps
  pinMode(13, OUTPUT);

  // reference for i2c LCD: https://www.blogdarobotica.com/2022/05/02/como-utilizar-o-display-lcd-16x02-com-modulo-i2c-no-arduino/
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear(); 

}

void loop() {
  // put your main code here, to run repeatedly:
  blinkLED();
  tempCelsius= readTemperature();
  voltagesAndCurrents= readVoltagesAndCurrents(vc);
  updateDisplay(tempCelsius, voltagesAndCurrents);
}

void blinkLED(){
  if(millis() - ledtime > 300){
    ledtime= millis();
    digitalWrite(led, !digitalRead(led));
  }
}
float readTemperature(){
  if(millis() - ntctime > 1000){
    ntctime= millis();
    // Le o sensor algumas vezes
    int soma = 0;
    for (int i = 0; i < nAmostras; i++) {
      soma += analogRead(Temperature);
      delay (10);
    }
  
    // Determina a resistência do termistor
    double v = (vcc*soma)/(nAmostras*4096.0);
    double rt = (vcc*R)/v - R;
  
    // Calcula a temperatura
    double t = beta / log(rt/rx);
    tempCelsius= t-273.0;
  }
  return tempCelsius;
}

void updateDisplay(double tempCelsius, float voltagesAndCurrents){
  if(millis() - lcdtime > 500){
    lcdtime= millis();
    lcd.clear();
    lcd.setCursor(1,0); // column 1, line 0
    lcd.print("FritzenLab");
    lcd.setCursor(1,1); // column 1, line 0
    lcd.print("tempCelsius");
  }
  
}
void readVoltagesAndCurrents(float voltagesAndCurrents[5]){
  if(millis() - analogReadingsTime > 50){
    analogReadingsTime= millis();
    counter++;
    if(counter < 4){
      voltagesAndCurrents[0]= analogRead(voltage5V) * (3.3 / 4096) * 2 + voltagesAndCurrents[0];
      voltagesAndCurrents[1]= analogRead(voltage3V3) * (3.3 / 4096) * 2 + voltagesAndCurrents[1];
      voltagesAndCurrents[2]= analogRead(voltage12V) * ((3.3 / 4096) / 0.15) + voltagesAndCurrents[2];
      voltagesAndCurrents[3]= ((analogRead(current5V) * (5 / 4096)) - 2.5) * 5.4 + voltagesAndCurrents[3];
      voltagesAndCurrents[4]= ((analogRead(current3V3) * (5 / 4096)) - 2.5) * 5.4 + voltagesAndCurrents[4];
    }else{
      counter= 0;
    }
  }
  return voltagesAndCurrents;
}