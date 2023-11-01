#ifndef INET_NTP_H
#define INET_NTP_H

/******************************************************************

  Includes

******************************************************************/

/******************************************************************

  Types and Defines

******************************************************************/

typedef struct
{
  char ConnInfoStr[100];
  char ConnInfoAdvStr[100];
} InetInfoStrings_t;

/******************************************************************

  Extern Data 

******************************************************************/

extern int wifiStatus;
extern InetInfoStrings_t InetInfoStrings_s;
extern unsigned long SecondsSinceStartup_u32;

/******************************************************************

  Extern Functions 

******************************************************************/

#endif // END INET_NTP_H
