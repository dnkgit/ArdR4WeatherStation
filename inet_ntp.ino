/******************************************************************

  Includes

******************************************************************/
#include "inet_ntp.h"

// Include the RTC library
#include "RTC.h"

//Include the NTP library
#include <NTPClient.h>

#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

#include <WiFiUdp.h>
#include "secrets.h" 

/******************************************************************

  Defines and Enums

******************************************************************/

/******************************************************************

  Local Globals

******************************************************************/

InetInfoStrings_t InetInfoStrings_s = { "No Data", "No Data" };

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp;                // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);

const int led = LED_BUILTIN;

uint32_t SecondsSinceStartup_u32 = 0;

/******************************************************************

  Functions

******************************************************************/

/******************************************************************

  Convert RSSI to HMR

******************************************************************/
void UpdateRSSI_HMR (char *str, long rssi)
{
  if(rssi < -90)
  {
    sprintf(str, "Unusable");
  }
  else if(rssi < -80)
  {
    sprintf(str, "Poor");
  } 
  else if(rssi < -70)
  {
    sprintf(str, "Good");
  } 
  else if(rssi < -67)
  {
    sprintf(str, "Very Good");
  } 
  else if(rssi < -30)
  {
    sprintf(str, "Excellent");
  } 
  else if(rssi < 0)
  {
    sprintf(str, "Astonishing");
  }
  else
  {
    sprintf(str, "Unknown");
  }
}

/******************************************************************

  Print WiFi Connection Information

******************************************************************/

void INET_UpdateWifiStatus (void)
{
  IPAddress ip = WiFi.localIP();
  long rssi = WiFi.RSSI();
  char rssiString[15];

  UpdateRSSI_HMR(rssiString, rssi);

  sprintf(InetInfoStrings_s.ConnInfoAdvStr, "   SSID: %s | IP Address: %u.%u.%u.%u | RSSI: %d dBm (%s)   ",
                                                                                                        WiFi.SSID(),
                                                                                                        ip[0],ip[1],ip[2],ip[3],
                                                                                                        rssi,
                                                                                                        rssiString);
  sprintf(InetInfoStrings_s.ConnInfoStr, "   UP | IP: %u.%u.%u.%u | RSSI: %d dBm (%s)   ",
                                                                                      ip[0],ip[1],ip[2],ip[3],
                                                                                      rssi,
                                                                                      rssiString);
}

/******************************************************************

  Print WiFi Connection Information

******************************************************************/
void printWifiStatus (void)
{
  IPAddress ip = WiFi.localIP();
  long rssi = WiFi.RSSI();
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/******************************************************************

  Get WiFi State

******************************************************************/

bool isWifiConnected (void)
{
  return (WiFi.status() == WL_CONNECTED);
}

/******************************************************************

  Connect to WiFi

******************************************************************/
void connectToWiFi(void)
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
  while(wifiStatus != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);
    Serial.println("WiFi.begin");
    do
    {
      Serial.print("WiFi Status: ");
      Serial.println(WiFi.status());
      delay(2000);
    } while(WiFi.status() != WL_CONNECTED);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
}

/******************************************************************

  Connect to Wifi and set RTC from NTP

******************************************************************/
void INET_SetupAndSyncTime (void)
{
  connectToWiFi();
  RTC.begin();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  // Get the current date and time from an NTP server and convert
  // it to UTC +2 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
  auto timeZoneOffsetHours = 1;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  if (!RTC.setPeriodicCallback(periodicCallback_256thSec, Period::N256_TIMES_EVERY_SEC))
  {
    pinMode(led, OUTPUT);
    Serial.println("ERROR: periodic callback not set");
  }

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));
}

/******************************************************************

  RTC periodic callback "interrupt"
  
  Called every 1/256 = 0.00390625s
  100ms = ~25.6 counts
  500ms = 128 counts
  etc

******************************************************************/
void periodicCallback_256thSec()
{
  static uint8_t Counter_u8 = 0;

  if(Counter_u8 == 0)
  {
    SecondsSinceStartup_u32++;
  }
  else if(Counter_u8 < 26)
  {
    digitalWrite(led, 1);
  }
  else
  {
    digitalWrite(led, 0);
  }
  Counter_u8++;
}

/******************************************************************

 Is a new second?
  
******************************************************************/
bool RTC_IsNewSecond(void)
{
  RTCTime timeNow;
  bool isNewSecond_bool = false;
  static int16_t lastSecond_s16 = 61;
  int16_t thisSecond_s16;

  RTC.getTime(timeNow);
  thisSecond_s16 = timeNow.getSeconds();

  if(thisSecond_s16 != lastSecond_s16)
  {
    isNewSecond_bool = true;
  }
  lastSecond_s16 = thisSecond_s16;
}

/******************************************************************

 Is a new minute?
  
******************************************************************/
bool RTC_IsNewMinute(void)
{
  RTCTime timeNow;
  bool isNewMinute_bool = false;
  static int16_t lastMinute_s16 = 61;
  int16_t thisMinute_s16;

  RTC.getTime(timeNow);
  thisMinute_s16 = timeNow.getSeconds();

  if(thisMinute_s16 != lastMinute_s16)
  {
    isNewMinute_bool = true;
  }
  lastMinute_s16 = thisMinute_s16;
}

/******************************************************************

  RTC_UpdateTimeDateString
  
  Create a string of the current time

******************************************************************/
void RTC_UpdateTimeDateString(char *TimeDateString)
{
  RTCTime timeNow;
  RTC.getTime(timeNow);

  sprintf(TimeDateString, "%02d/%02d/%04d %02d:%02d:%02d",   timeNow.getDayOfMonth(),
                                                             Month2int(timeNow.getMonth()),
                                                             timeNow.getYear(),
                                                             timeNow.getHour(),
                                                             timeNow.getMinutes(),
                                                             timeNow.getSeconds());
}
