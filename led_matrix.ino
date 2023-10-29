#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

void LEDMAT_Init_LED_Matrix (void)
{
  matrix.begin();
  matrix.beginDraw();
  matrix.stroke(255, 0, 0);
  // add some static text
  // will only show "UNO" (not enough space on the display)
  const char text[] = "1.1";
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();
  matrix.endDraw();
  delay(2000);
}

void LEDMAT_ShowScrollTextLeft (const char string[])
{
  // Make it scroll!
  matrix.beginDraw();

  matrix.stroke(255, 0, 0);
  matrix.textScrollSpeed(50);

  // add the text
  // const char text[] = "    Scrolling text!    ";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 127, 0, 0);
  matrix.println(string);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void LEDMAT_ShowScrollTextRight (const char string[])
{
  // Make it scroll!
  matrix.beginDraw();

  matrix.stroke(255, 0, 0);
  matrix.textScrollSpeed(50);

  // add the text
  // const char text[] = "    Scrolling text!    ";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 127, 0, 0);
  matrix.println(string);
  matrix.endText(SCROLL_RIGHT);
  matrix.endDraw();
}