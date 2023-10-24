#define SERIAL_BAUD     (115200)

typedef struct
{
  float TemperatureDegC_f;
  float Humidity_PC_f;
  float Pressure_hPa_f;  
} WeatherData_t;

WeatherData_t WeatherData_s;

void setup()
{
  Serial.begin(SERIAL_BAUD);
  while(!Serial) {} // Wait
  Serial.println("Serial Initialised.");
  Init_BME280();
  LEDMAT_Init_LED_Matrix();
}

//////////////////////////////////////////////////////////////////
void loop()
{
  char WeatherDataString[100];
  char rhUnit[] = "%";

  printBME280Data(&Serial, &WeatherData_s);
  
  sprintf(WeatherDataString, "%2.2f C %04.2f hPa %3.2f %s   ", WeatherData_s.TemperatureDegC_f, WeatherData_s.Pressure_hPa_f, WeatherData_s.Humidity_PC_f, rhUnit);
  LEDMAT_ShowScrollText(WeatherDataString);
  delay(500);
}

