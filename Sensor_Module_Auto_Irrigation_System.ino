//// ----------------------------------------------- ////
// Libraries
#include <Wire.h>
#include <RTClib.h>
DateTime now;
RTC_DS3231 rtc;

// Moisture Sensor Constants
int moistPin = A3;      
int moistVal = 0; 

// LED Constants
// int ledVal = 0;    
// int ledPin = LED_BUILTIN;   

// Global Constants & Variables
const int MOISTSENSORMAX = 660;

long double output = 0;
double waterStateDeterminer = 0;
int waterStateOutput = 0;

// Time Intervals
const int rtcCheckInterval = 200;
const int moistSensorInterval = 500;
const int oneSecond = 1000;
const int oneMinute = (60 * 1000);

unsigned long currentMillis = 0;
unsigned long RTCMillis = 0;
unsigned long tempMillis = 0;
unsigned long moistSensorMillis = 0;

// UDFs
void ticks();
void rtcCheck();

double calcWaterState(long double);
int read_moistureSensor();

void displayTime(); 
void displayDate();

//// ----------------------------------------------- ////
// Initialization
//// ----------------------------------------------- ////
void setup()
{
  Serial.begin(9600);

  rtcCheck();
}

//// ----------------------------------------------- ////
// Main program
//// ----------------------------------------------- ////
void loop()
{
  ticks();

  if (now.minute() == 0) {
    if ((currentMillis - tempMillis) >= (5 * oneSecond))
    {
      tempMillis += ((currentMillis - tempMillis) / (5 * oneSecond)) * (5 * oneSecond);
      
      moistVal = read_moistureSensor();

      waterStateDeterminer = calcWaterState(moistVal);

      if (waterStateDeterminer > 0.5) {
        waterStateOutput = 1;
      }

      else {
        waterStateOutput = 0;
      }

      Serial.println(moistVal);
      Serial.println(" ");

    }
  }
}

//// ----------------------------------------------- ////
// Arduino Tick Keeper & RTC
//// ----------------------------------------------- ////
void ticks()
{
  currentMillis = millis();

  if ((currentMillis - RTCMillis) >= (rtcCheckInterval))
  {
    RTCMillis += rtcCheckInterval;
    now = rtc.now();

  }
}

//// ----------------------------------------------- ////
// RTC Not Working Checker
//// ----------------------------------------------- ////
void rtcCheck()
{
  if (! rtc.begin()) 
  {
    {
      Serial.println("RTC Module not Present");
      while (1);

    }

    if (rtc.lostPower()) 
    {
      Serial.println("RTC power failure, reset the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    }
  }
}

//// ----------------------------------------------- ////
// Moisture Sensor Read Code
//// ----------------------------------------------- ////
int read_moistureSensor()
{
  if ((currentMillis - moistSensorMillis) >= (moistSensorInterval))
  {
    moistSensorMillis += ((currentMillis - moistSensorMillis) / moistSensorInterval) * moistSensorInterval;

    output = analogRead(moistPin);
    output = (output * 255) / MOISTSENSORMAX;

    return output;

  }
}

double calcWaterState(long double input1)
{
  output = (input1 / 255);

  return output;

}

//// ----------------------------------------------- ////
// Display Time in serial Print
//// ----------------------------------------------- ////
void displayTime()
{
  Serial.print("Time:");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.print(now.second());
  Serial.println("    ");

} 

//// ----------------------------------------------- ////
// Display Date in serial Print
//// ----------------------------------------------- ////
void displayDate()
{
  Serial.print("Date:");
  Serial.print(now.day());
  Serial.print('/');
  Serial.print(now.month());
  Serial.print('/');
  Serial.println(now.year());

}

//// ----------------------------------------------- ////