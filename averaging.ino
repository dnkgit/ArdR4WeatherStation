/******************************************************************

  Includes

******************************************************************/
#include "averaging.h"

/******************************************************************

  Defines and Enums

******************************************************************/

/******************************************************************

  Local Globals

******************************************************************/

Average_t SecAverage_s[E_AVG_NUM_AVGS];   // up to 60s then average over last minute
Average_t MinAverage_s[E_AVG_NUM_AVGS];   // up to 60m then average over last hour

/******************************************************************

  Local Function Prototypes

******************************************************************/
void  insertNewValue (Average_t *pAvgArray, float NewValue_f);
float calculateAverage(Average_t *pAvgArray);

/******************************************************************

  Functions

******************************************************************/

/******************************************************************

  Init

******************************************************************/
void AVG_Init(void)
{
  uint8_t Idx_u8 = E_AVG_TEMP;
  while(Idx_u8 < E_AVG_NUM_AVGS)
  {
    SecAverage_s[Idx_u8].NumCounts_u8 = 0;
    SecAverage_s[Idx_u8].CurrentAvg_f = 0;
    SecAverage_s[Idx_u8].SampleLength_u8 = AVG_SAMPLE_LENGTH;
    SecAverage_s[Idx_u8].isAvgArrayFilled_bool = false;

    MinAverage_s[Idx_u8].NumCounts_u8 = 0;
    MinAverage_s[Idx_u8].CurrentAvg_f = 0;
    MinAverage_s[Idx_u8].SampleLength_u8 = AVG_SAMPLE_LENGTH;
    MinAverage_s[Idx_u8].isAvgArrayFilled_bool = false;

    Idx_u8++;
  }
}

/******************************************************************

  UpdateSecondAverages

******************************************************************/
void AVG_UpdateSecondAverages (float Temp_f, float Humd_f, float Pres_f)
{
  insertNewValue(&SecAverage_s[E_AVG_TEMP], Temp_f);
  insertNewValue(&SecAverage_s[E_AVG_HUMD], Humd_f);
  insertNewValue(&SecAverage_s[E_AVG_PRES], Pres_f);
}

/******************************************************************

  AVG_UpdateMinuteAverages

******************************************************************/
void AVG_UpdateMinuteAverages (void)
{
  insertNewValue(&MinAverage_s[E_AVG_TEMP], SecAverage_s[E_AVG_TEMP].CurrentAvg_f);
  insertNewValue(&MinAverage_s[E_AVG_HUMD], SecAverage_s[E_AVG_HUMD].CurrentAvg_f);
  insertNewValue(&MinAverage_s[E_AVG_PRES], SecAverage_s[E_AVG_PRES].CurrentAvg_f);
}

/******************************************************************

  AVG_ReturnSecondsAverage
  (Last 60s once buffer filled. Up to 60s otherwise)

******************************************************************/
float AVG_ReturnSecondsAverage(uint8_t ArrayIdx_u8)
{
  if(ArrayIdx_u8 < E_AVG_NUM_AVGS)
  {
    SecAverage_s[ArrayIdx_u8].CurrentAvg_f = calculateAverage(&SecAverage_s[ArrayIdx_u8]);
  }
  return SecAverage_s[ArrayIdx_u8].CurrentAvg_f;
}

/******************************************************************

  AVG_ReturnMinutesAverage
  (Last 60m once buffer filled. Up to 60m otherwise)

******************************************************************/
float AVG_ReturnMinutesAverage(uint8_t ArrayIdx_u8)
{
  if(ArrayIdx_u8 < E_AVG_NUM_AVGS)
  {
    MinAverage_s[ArrayIdx_u8].CurrentAvg_f = calculateAverage(&MinAverage_s[ArrayIdx_u8]);
  }
  return MinAverage_s[ArrayIdx_u8].CurrentAvg_f;
}

/******************************************************************

  AVG_isSecondAvgArrayFull
  
  Is there 60s worth of data in the array?

******************************************************************/
bool   AVG_isSecondAvgArrayFull(uint8_t ArrayIdx_u8)
{
  bool isArrayFilled_bool = false;

  if(ArrayIdx_u8 < E_AVG_NUM_AVGS)
  {
    isArrayFilled_bool = SecAverage_s[ArrayIdx_u8].isAvgArrayFilled_bool;
  }
  return isArrayFilled_bool;
}

/******************************************************************

  AVG_isMinuteAvgArrayFull
  
  Is there 60m worth of data in the array?

******************************************************************/
bool   AVG_isMinuteAvgArrayFull(uint8_t ArrayIdx_u8)
{
  bool isArrayFilled_bool = false;

  if(ArrayIdx_u8 < E_AVG_NUM_AVGS)
  {
    isArrayFilled_bool = MinAverage_s[ArrayIdx_u8].isAvgArrayFilled_bool;
  }
  return isArrayFilled_bool;
}

/******************************************************************

  insertNewValue

******************************************************************/
void insertNewValue (Average_t *pAvgArray, float NewValue_f)
{
    if(pAvgArray->NumCounts_u8 < pAvgArray->SampleLength_u8)
    {
      pAvgArray->AvgArray[pAvgArray->NumCounts_u8] = NewValue_f;
      pAvgArray->isAvgArrayFilled_bool = false;
      pAvgArray->NumCounts_u8++;
    }
    else
    {
      pAvgArray->isAvgArrayFilled_bool = true;
      memcpy(&pAvgArray->AvgArray[0], &pAvgArray->AvgArray[1], sizeof(float) * (pAvgArray->SampleLength_u8 - 1));
      pAvgArray->AvgArray[pAvgArray->NumCounts_u8-1] = NewValue_f;
    }
}

/******************************************************************

  calculateAverage

******************************************************************/
float calculateAverage(Average_t *pAvgArray)
{
  uint8_t Idx_u8;
  float Total_f = 0;
  float Result_f;

  for(Idx_u8 = 0; Idx_u8 < pAvgArray->NumCounts_u8; Idx_u8++)
  {
    Total_f += pAvgArray->AvgArray[Idx_u8];
  }

  Result_f = Total_f / pAvgArray->NumCounts_u8;
}
