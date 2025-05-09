//// ----------------------------------------------- ////
// Libraries
#include <Wire.h>
#include <RTClib.h>
DateTime now;
RTC_DS3231 rtc;

// Global Constants
const int SolinoidValve_1 = 8;
const int SolinoidValve_2 = 9;
const int SolinoidValve_3 = 10;

const int switch_1 = 2;
const int switch_2 = 3;

// Time Intervals
const int oneSecond = 1000;
const int rtcCheckInterval = 200;
const int switchInterval = 200;
const int potenInterval = 500;

unsigned long currentMillis = 0;
unsigned long RTCMillis = 0;
unsigned long switchMillis_1 = 0;
unsigned long switchMillis_2 = 0;
unsigned long potenMillis = 0;
unsigned long waterStateMillis = 0;

unsigned long waterCycleMillis = 0;

unsigned long motorMillis_1 = 0;
unsigned long motorMillis_2 = 0;
unsigned long motorMillis_3 = 0;

// Switch & State Vars
byte switchState_1 = 0;
byte switchState_2 = 0;

byte motorState_1 = 0;
byte motorState_2 = 0;
byte motorState_3 = 0;

long potVal = 0;
int WATERMAX = 100;
const int POTENMAX = 675;

// Random Constants
byte rain_state = 0;
byte hot_temprature_state = 0;

int time_hours = 8;
int time_minutes = 29;
int time_seconds = 55;

// Watering Cycle Vars
int waterState = 2;			// 0 - Start Cycle, 1 - End Cycle
							          // 2 - Idle,		    3 - Has Rained

int waterTime = 0;

// UDFs
void ticks();
void RTC();

void read_Switch_1();
void read_Switch_2();
int read_Poten();

void watering_cycle(int, int);

void Open_Valve_1(int);
void Open_Valve_2(int);
void Open_Valve_3(int);

void Close_Valve_1();
void Close_Valve_2();
void Close_Valve_3();


//// ----------------------------------------------- ////
// Initialization
//// ----------------------------------------------- ////
void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  
  pinMode(switch_1, INPUT);
  pinMode(switch_2, INPUT);
  
  pinMode(SolinoidValve_1, OUTPUT);
  pinMode(SolinoidValve_2, OUTPUT);
  pinMode(SolinoidValve_2, OUTPUT);
}


//// ----------------------------------------------- ////
// Main Code
//// ----------------------------------------------- ////
void loop()
{  
  ticks();

  read_Switch_1();
  
  // ------------- //
  // Manual 
  // ------------- //
  if (switchState_1 == 0) 
  {
    Open_Valve_3(2);
    
    Close_Valve_1();
    Close_Valve_2();
  }
  
  // ------------- //
  // Automatic 
  // ------------- //
  else if (switchState_1 == 1) 
  {
    Close_Valve_3();
    
    // 8:30 AM Watering Cycle
    if (waterState == 2 && time_hours == 8 && (time_minutes >= 30 && time_minutes < 59))
    {
      waterTime = read_Poten();
      
      waterState = 0;
      
      watering_cycle_start();
    }
    
    // Missed Watering Cycle Code
    else if (waterState == 0)
    {
      watering_cycle_start();
    } 
    
    // 7:30 PM - Restarts Water Cycle if Temprature is too Hot
    
  	// Sets WaterState to Idle at 2:30 AM, 5:30 AM, 9:30 PM, and 11:30 PM
    if (waterState == 1 && (time_hours == 2 || time_hours == 5 || time_hours == 21 || time_hours == 23) && time_minutes == 30)
    {
      if ((currentMillis - waterStateMillis) >= (30 * oneSecond))
      {
        waterStateMillis += ((currentMillis - waterStateMillis) / (30 * oneSecond)) * (30 * oneSecond);
        
        waterState = 2;
      }
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
// Switch 1 Read Code
//// ----------------------------------------------- ////
void read_Switch_1()
{
  if ((currentMillis - switchMillis_1) >= (switchInterval))
  {
    switchMillis_1 += switchInterval;
  	switchState_1 = digitalRead(switch_1);
  }
}

//// ----------------------------------------------- ////
// Switch 2 Read Code
//// ----------------------------------------------- ////
void read_Switch_2()
{
  if ((currentMillis - switchMillis_2) >= (switchInterval))
  {
    switchMillis_2 += ((currentMillis - switchMillis_2) / switchInterval) * switchInterval;
  	switchState_2 = digitalRead(switch_2);
  }
}

//// ----------------------------------------------- ////
// Potentiameter Read Code
//// ----------------------------------------------- ////
int read_Poten()
{
  if ((currentMillis - potenMillis) >= (potenInterval))
  {
    potenMillis += ((currentMillis - potenMillis) / potenInterval) * potenInterval;

    potVal = analogRead(A0);
    Serial.println(potVal);
    waterTime = (((float) (potVal * WATERMAX)) / POTENMAX);
    
    Serial.print("Time: ");
    Serial.println(waterTime);
  }
  
  return waterTime;
}
  
//// ----------------------------------------------- ////
// Main Watering Cycle Code
//// ----------------------------------------------- ////
void watering_cycle_start(void)
{
  if (waterTime > 0 || waterState == 0)
  {
    if ((currentMillis - waterCycleMillis) >= oneSecond)
    {      
      waterCycleMillis += ((currentMillis - waterCycleMillis) / oneSecond) * oneSecond;

      Open_Valve_1(0);
      --waterTime;
      Serial.print("UDF: ");
      Serial.print(waterState);
      Serial.print(" ");
      Serial.println(waterTime);

      if (waterTime == 0)
      {
        waterState = 1;
      }
    }
  }
}

//// ----------------------------------------------- ////
// Set Value 1 to Open
//// ----------------------------------------------- ////
void Open_Valve_1(int secs)
{
  // Duration to Run Before Pausing
  if (motorState_1 == 0) 
  {
    motorState_1 = 1;
    digitalWrite(SolinoidValve_1, HIGH);
    
  }
}

//// ----------------------------------------------- ////
// Set Value 2 to Open
//// ----------------------------------------------- ////
void Open_Valve_2(int secs)
{
  // Duration to Run Before Pausing
  if (motorState_2 == 0) 
  {
    motorState_2 = 1;
    digitalWrite(SolinoidValve_2, HIGH);
    
  }
}

//// ----------------------------------------------- ////
// Set Value 3 to Open
//// ----------------------------------------------- ////
void Open_Valve_3(int secs)
{
  // Checks if Automated System is OFF
  if (motorState_1 == 0 && motorState_2 == 0) {
    if (motorState_3 == 0) 
    {
      motorState_3 = 1;
      digitalWrite(SolinoidValve_3, HIGH);
    }
  }
  
  else
  {
    Close_Valve_3();
  }
}

//// ----------------------------------------------- ////
// Set Value 1 to Close
//// ----------------------------------------------- ////
void Close_Valve_1()
{
  if (motorState_1 == 1) {
    motorState_1 = 0;
    digitalWrite(SolinoidValve_1, LOW);
  }
}

//// ----------------------------------------------- ////
// Set Value 2 to Close
//// ----------------------------------------------- ////
void Close_Valve_2()
{
  if (motorState_2 == 1) {
    motorState_2 = 0;
    digitalWrite(SolinoidValve_2, LOW);
  }
}

//// ----------------------------------------------- ////
// Set Value 3 to Close
//// ----------------------------------------------- ////
void Close_Valve_3()
{
  if (motorState_3 == 1) {
    motorState_3 = 0;
    digitalWrite(SolinoidValve_3, LOW);
  }
}


/*
void Open_Valve_3(int secs)
{
  // Checks if Automated System is OFF
  if (motorState_1 == 0 && motorState_2 == 0) {
    // Duration to Run Before Pausing
    if (motorState_3 == 0) {
      if ((currentMillis - motorMillis_3) >= (secs * oneSecond))
      { 
        motorMillis_3 += ((currentMillis - motorMillis_3) / oneSecond) * secs * oneSecond;

        motorState_3 = 1;
        digitalWrite(SolinoidValve_3, HIGH);
      }
    }

    // Duration to Stop
    else {
      if ((currentMillis - motorMillis_3) >= (oneSecond))
      {
        motorMillis_3 += oneSecond;

        motorState_3 = 0;
        digitalWrite(SolinoidValve_3, LOW);
      }
    }
  }
  
  else
  {
    Close_Valve_3();
  }
}
*/


