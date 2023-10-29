/******************************************************************

  Includes

******************************************************************/
#include "build_sw.h"
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
  E_SM_CONNECTED
};

/******************************************************************

  Local Globals

******************************************************************/

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
#ifdef DEBUGGING          
  Serial.println("Serial Initialised.");
#endif
}

/******************************************************************

  Main Loop

******************************************************************/
void loop()
{
  RunMainStateMachine();
}

/******************************************************************

  Do State Machine

******************************************************************/
void RunMainStateMachine (void)
{
  static int16_t State_s16 = E_SM_INIT;
  static uint8_t NumRetries_u16 = 0;
  char GeneralStrBuf[50];

  switch(State_s16)
  {
    case E_SM_INIT:
      BME280_Init();
      LEDMAT_Init_LED_Matrix();
      AVG_Init();
      NumRetries_u16 = 0;
      State_s16 = E_SM_CONNECT;
      break;
    
    case E_SM_CONNECT:
      LEDMAT_ShowScrollTextLeft("   CONNECTING...   ");
      INET_SetupAndSyncTime();
      LEDMAT_ShowScrollTextLeft("   Inet Up   ");
      State_s16 = E_SM_CONNECTED;
      break;
    
    case E_SM_CONNECTED:
      
      if(false == isWifiConnected())
      {
        if(NumRetries_u16 < 0xFFFF)
        {
          NumRetries_u16 += 1;
          sprintf(GeneralStrBuf, "   Inet Down. Retry #%d   ", NumRetries_u16);
          LEDMAT_ShowScrollTextLeft(GeneralStrBuf);
          State_s16 = E_SM_CONNECT;
        }
        else
        {
          LEDMAT_ShowScrollTextLeft("   Inet Unavailable.   ");
          State_s16 = E_SM_INIT;
        }
      }
      else
      {
        Do_WeatherSensorHandling();
        
        if(true == RTC_IsNewSecond())
        {
#ifdef DEBUGGING          
          Serial.print("New Second");
#endif
          AVG_UpdateSecondAverages(WeatherData_s.TemperatureDegC_f, WeatherData_s.Humidity_PC_f, WeatherData_s.Pressure_hPa_f);
#ifdef DEBUGGING          
          {
            char buf[100];
            sprintf(buf, "Sec Averages. Temp [%3.2f] Humd [%3.2f] Pres [%04.2f]",  AVG_ReturnSecondsAverage[E_AVG_TEMP],
                                                                                    AVG_ReturnSecondsAverage[E_AVG_HUMD],
                                                                                    AVG_ReturnSecondsAverage[E_AVG_PRES]);
            Serial.println(buf);
          }
#endif
          if(true == RTC_IsNewMinute())
          {
#ifdef DEBUGGING          
          Serial.print("New Minute");
#endif
            AVG_UpdateMinuteAverages();
#ifdef DEBUGGING          
            {
              char buf[100];
              sprintf(buf, "Min Averages. Temp [%3.2f] Humd [%3.2f] Pres [%04.2f]",   AVG_ReturnMinutesAverage[E_AVG_TEMP],
                                                                                      AVG_ReturnMinutesAverage[E_AVG_HUMD],
                                                                                      AVG_ReturnMinutesAverage[E_AVG_PRES]);
              Serial.println(buf);
            }
#endif
          }

        }
        Do_TimeDateHandling();
        Do_InetConnHandling();
      }
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
  LEDMAT_ShowScrollTextLeft(WeatherDataString);
  delay(INTER_DISPLAY_MSG_TIME_MS);
}

/******************************************************************

  Handle Time Data

******************************************************************/
void Do_TimeDateHandling (void)
{
  char TimeDateString[100] = "   No Data!   ";
  RTC_UpdateTimeDateString(TimeDateString);
  LEDMAT_ShowScrollTextLeft(TimeDateString);
  delay(INTER_DISPLAY_MSG_TIME_MS);
}

/******************************************************************

  Handle Internet Connection Data

******************************************************************/
void Do_InetConnHandling (void)
{
  INET_UpdateWifiStatus();
  LEDMAT_ShowScrollTextLeft(InetInfoStrings_s.ConnInfoStr);
  delay(INTER_DISPLAY_MSG_TIME_MS);
}
