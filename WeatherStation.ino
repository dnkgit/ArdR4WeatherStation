/******************************************************************

  Includes

******************************************************************/
#include "build_sw.h"
#include <TimerEvent.h>
#include "inet_ntp.h"
#include "averaging.h"

/******************************************************************

  Defines and Enums

******************************************************************/

#define SERIAL_BAUD               (115200)
#define INTER_DISPLAY_MSG_TIME_MS (0)

typedef struct
{
  float TemperatureDegC_f;
  float Humidity_PC_f;
  float Pressure_hPa_f;  
} WeatherData_t;

enum
{
  E_SM_INIT = 0,
  E_SM_CONNECT,
  E_SM_CONNECTED,

  E_SM_NUM_STATES
};

/******************************************************************

  Local Globals

******************************************************************/

char stringTable[4][36] = 
{
  "   123456789012345678901234567890",
  "   ABCDEFGHIJABCDEFGHIJABCDEFGHIJ",
  "   abcdefghijabcdefghijabcdefghij",
  "   ----------"
};

bool NewDelta_bool = false;
uint32_t Delta_ms_u32 = 0;


const unsigned int timerOnePeriod = 500;
const unsigned int timerTwoPeriod = 20;
TimerEvent timerOne;
TimerEvent timerTwo;

WeatherData_t WeatherData_s;

/******************************************************************

  Functions

******************************************************************/

/******************************************************************

  Setup

******************************************************************/
void setup (void)
{
  Serial.begin(SERIAL_BAUD);
  while(!Serial) {} // Wait

  timerOne.set(timerOnePeriod, timerOneFunc);
  timerTwo.set(timerTwoPeriod, timerTwoFunc);

#ifdef WEATHER_DEBUG_PRINTS          
  Serial.println("Serial Initialised.");
#endif
}

/******************************************************************

  Main Loop

******************************************************************/
void loop()
{
  timerOne.update();
  timerTwo.update();
  RunMainStateMachine();
}

/******************************************************************

  Do State Machine

******************************************************************/
void RunMainStateMachine (void)
{
  static int16_t State_s16 = E_SM_INIT;
  static int16_t LastState = E_SM_NUM_STATES;
  static uint8_t NumRetries_u16 = 0;
  char GeneralStrBuf[50];

#ifdef WEATHER_DEBUG_PRINTS
  if(State_s16 != LastState)
  {
    Serial.print("SM=");
    Serial.print(State_s16);
    Serial.print(",");
    Serial.println(LastState);
    LastState = State_s16;
  }
#endif

  switch(State_s16)
  {
    case E_SM_INIT:
      BME280_Init();
      LEDMATRIX_Init(&stringTable[0][0]);
      //AVG_Init();
      NumRetries_u16 = 0;
      State_s16 = E_SM_CONNECT;
      break;
    
    case E_SM_CONNECT:
      //INET_SetupAndSyncTime();
      State_s16 = E_SM_CONNECTED;
      break;
    
    case E_SM_CONNECTED:
      
      if(NewDelta_bool)
      {
        Serial.println(Delta_ms_u32);
        NewDelta_bool = false;
      }
#if 0
      if(false == isWifiConnected())
      {
        if(NumRetries_u16 < 0xFFFF)
        {
          NumRetries_u16 += 1;
          State_s16 = E_SM_CONNECT;
        }
        else
        {
          State_s16 = E_SM_INIT;
        }
      }
      else
      {
        if(true == RTC_IsNewSecond())
        {
          RTC_getimeDateString(GeneralStrBuf);
          if(true == RTC_IsNewMinute())
          {
            RTC_getimeDateString(GeneralStrBuf);
          }
        }
      }
#endif
      break;
    
    default:
      State_s16 = E_SM_INIT;
      break;
  }
}

/******************************************************************

  Handle Weather Data

******************************************************************/
void Do_WeatherSensorHandling (void)
{
  char WeatherDataString[100];
  char rhUnit[] = "%";

  BME280_UpdateData(&WeatherData_s);
  BME280_PrintToSerial(&Serial, &WeatherData_s);
  BME280_UpdateData(&WeatherData_s);
  sprintf(WeatherDataString, "   %2.1f C | %04.2f hPa | %3.2f %s   ", WeatherData_s.TemperatureDegC_f,
                                                                      WeatherData_s.Pressure_hPa_f,
                                                                      WeatherData_s.Humidity_PC_f,
                                                                      rhUnit);
  //LEDMAT_ShowScrollTextLeft(WeatherDataString);
  delay(INTER_DISPLAY_MSG_TIME_MS);
}

/******************************************************************

  Handle Time Data

******************************************************************/
void Do_TimeDateHandling (void)
{
  char TimeDateString[100] = "   No Data!   ";
  RTC_getimeDateString(TimeDateString);
  delay(INTER_DISPLAY_MSG_TIME_MS);
}

/******************************************************************

  Handle Internet Connection Data

******************************************************************/
void Do_InetConnHandling (void)
{
  INET_UpdateWifiStatus();
  delay(INTER_DISPLAY_MSG_TIME_MS);
}

/******************************************************************

  timerOneFunc

******************************************************************/

void timerOneFunc()
{
  static int Index = 1;
  if(true == LEDMATRIX_isRunCompleteFlag())
  {
    //LEDMATRIX_SetString(&stringTable[Index][0]);
    LEDMATRIX_resetRunCompleteFlag();
    Index++;
    if(Index == 3)
    {
      Index = 0;
    }
  }
}

/******************************************************************

  timerTwoFunc

******************************************************************/
// This function will be called every timerTwoPeriod
void timerTwoFunc()
{
  static uint32_t LastMillis = 0;
  uint32_t ThisMillis = millis();
  uint32_t Delta_u32 = ThisMillis - LastMillis;
  if(LastMillis != 0)
  {
    Delta_ms_u32 = Delta_u32;
    NewDelta_bool = true;
  }
  LastMillis = ThisMillis;

  LEDMATRIX_Run();
}
