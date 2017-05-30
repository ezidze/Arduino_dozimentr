

 //Arduino geiger counter

// Подключение библиотек
#include "LiquidCrystal.h"
#include <avr/delay.h>
#include <TimerOne.h>
#include <rgb_lcd.h>
#include <Wire.h>

#define WARNING_LEVEL 10000
#define GEIGER_TIME 10
#define PIN_PR 0
#define PERIOD 60000000
#define PERIOD2 500000

long int rad_now = 0;
long int rad_mass[GEIGER_TIME];
char str_buff[17];


rgb_lcd lcd;
const int colorR = 255;
const int colorG = 255;
const int colorB = 255;


// функция подающая импульсы
void impuls(void)
{
  digitalWrite(3, HIGH);
  _delay_ms(10);
  digitalWrite(3, LOW);
  _delay_ms(10);
  
}


// уровень тревоги (выводит на экран слово "WARNING")
void warning(void)
{
  if (rad_now > WARNING_LEVEL)
  {
    while (rad_now > WARNING_LEVEL)
    {
      // экран
      lcd.clear();
      lcd.print("*** WARNING ***");
      // Serial порт
      Serial.println("***WARNING***");
    }
  }
}

// функция подсчета импульсов
void schetchik(void)
{
  long int iz = 0;
  for (int jk = GEIGER_TIME - 2; jk > 0 ; jk--) rad_mass[jk] = rad_mass[jk + 1]; //перезапись массива
  rad_mass[0] = 0;
  rad_mass[0] = analogRead(2);  // присвивание нулевому элементу массива значения текущего значения с аналогового входа 2
  impuls();
}


void Timer1_action(void)
{
  rad_now = 0;
  for (int i = 0; i <= GEIGER_TIME - 1; i++) rad_now += rad_mass[i];
  rad_now = rad_now/GEIGER_TIME;
  

}

// функция первичной накачки
void The_First_Step(void)
{
  for (int i = 0; i <= GEIGER_TIME; i++)
  {
    impuls();
    for (int jk = GEIGER_TIME - 2; jk > 0 ; jk--) rad_mass[jk] = rad_mass[jk + 1];
    rad_mass[0] = analogRead(2);
  }
  for (int i = 0; i <= GEIGER_TIME; i++) rad_now += rad_mass[i];
  rad_now = rad_now/3;

}

// инизилизация функций
void impuls(void);
void warning(void);
void schetchik(void);
void Timer1_action(void);
void The_First_Step(void);

/////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  Timer1.initialize(PERIOD);

  for (int i = 0; i <= GEIGER_TIME; i += 1) {
    rad_mass[i] = 0;
  }

  The_First_Step();
  Serial.begin(9600);
  Timer1.attachInterrupt(schetchik, PERIOD2);
  Timer1.attachInterrupt(Timer1_action, PERIOD);


  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Geiger Counter");
  lcd.setCursor(0, 1);
  lcd.print("Wait a moment");
  delay(1500);


  // the first impuls
  impuls();

}

//////////////////////////////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:

  lcd.clear();
  warning();
  lcd.setCursor(0, 0);
  sprintf(str_buff, "%6lu uR/h", map(rad_now, 0, 2000, 0, 1100)/3);
  // для экрана
  lcd.print("Background");
  lcd.setCursor(5, 1);
  lcd.print(str_buff);
  delay(10);
  // для  Serial порта
  Serial.println(str_buff);
  Serial.println("\n");
  

}
