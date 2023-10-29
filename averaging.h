#ifndef AVERAGING_H
#define AVERAGING_H

/******************************************************************

  Includes

******************************************************************/

/******************************************************************

  Types and Defines

******************************************************************/

enum
{
  E_AVG_TEMP = 0,
  E_AVG_HUMD,
  E_AVG_PRES,

  E_AVG_NUM_AVGS
};

#define AVG_SAMPLE_LENGTH     (60)

typedef struct
{
  float AvgArray[AVG_SAMPLE_LENGTH];
  float CurrentAvg_f;
  uint8_t NumCounts_u8;
  uint8_t SampleLength_u8;
  bool isAvgArrayFilled_bool;
} Average_t;

/******************************************************************

  Extern Data 

******************************************************************/

/******************************************************************

  Extern Functions 

******************************************************************/

extern void   AVG_Init(void);

extern void   AVG_UpdateSecondAverages (float Temp_f, float Humd_f, float Pres_f);
extern void   AVG_UpdateMinuteAverages (void);

extern float  AVG_ReturnSecondsAverage(uint8_t ArrayIdx_u8);
extern float  AVG_ReturnMinutesAverage(uint8_t ArrayIdx_u8);

extern bool   AVG_isSecondAvgArrayFull(uint8_t ArrayIdx_u8);
extern bool   AVG_isMinuteAvgArrayFull(uint8_t ArrayIdx_u8);

#endif // END AVERAGING_H
