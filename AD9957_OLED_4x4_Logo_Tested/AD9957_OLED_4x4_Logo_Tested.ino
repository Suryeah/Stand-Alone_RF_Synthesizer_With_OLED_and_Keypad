#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define   pinUpdate(pin) { digitalWrite(IO_UPDATEPIN,LOW); delayMicroseconds(1); digitalWrite(IO_UPDATEPIN,HIGH); delayMicroseconds(1); digitalWrite(IO_UPDATEPIN,LOW); }
#define   pinReset(pin)  { digitalWrite(MASTER_R,HIGH); delayMicroseconds(1); digitalWrite(MASTER_R,LOW); }

#define   IO_UPDATEPIN   PA2
#define   MASTER_R       PA3
#define   pinSPI_SS      PA4
#define   pinSPI_CLK     PA5
#define   pinSPI_MISO    PA6
#define   pinSPI_MOSI    PA7

#define   Fsck           1440000000

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

byte h = 0, v = 0, EXIT = 0;         // variables used in for loops & EXIT to STOP Operation
const int period = 50;     // A little delay to avoid errors.
int kdelay = 0;            // Non Blocking Delay
const byte rows = 4;       // Rows in Keypad
const byte columns = 4;    // Columns in Keypad
const byte Output[rows] = {PA15, PA10, PA9, PA8}; //Row connceted to Arduino Pins
const byte Input[columns] = {PB15, PB14, PB13, PB12}; //Columns connected to Arduino Pins
String inputString;
long inputInt, MULT;
char val;
String range = "Hz";
double    FQ, SFQ, EFQ, STEP = 0;
int       REP, DELAY;
uint32_t  FTW_D ;//FREQUNCY TUNE WORD IN DECIMAL
float     FTW_F;

volatile float     ASF = 0.99994;       //  AMPLITUDE CONTROL FOR DDS1(0.1-1)
float     ASF_R = ASF * 16384; // ASF DECIMAL IS WRITE IN THE REGISER
uint16_t  ASF_R_D = ASF_R;

static const unsigned char PROGMEM logo16_glcd_bmp[] = {
  // 'LOGO, 128x64px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x3c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x83, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x7c, 0x00, 0x7e, 0x00, 0xfd, 0xff, 0x78, 0xf7, 0xf1, 0xfe, 0x0f, 0xc7, 0xc7, 0x7c, 0x3e, 0x3f,
  0x7f, 0xff, 0xbe, 0x01, 0xcc, 0xf7, 0x3c, 0x67, 0xbc, 0xef, 0x1c, 0xe3, 0xc6, 0x38, 0xf7, 0x73,
  0x3b, 0xff, 0xdc, 0x01, 0xc4, 0xe3, 0x3c, 0x67, 0x9c, 0xe7, 0x18, 0x73, 0xe6, 0x38, 0xe2, 0x71,
  0x3b, 0xef, 0xdc, 0x01, 0xc0, 0xe0, 0x3e, 0x67, 0x9c, 0xe7, 0xb8, 0x73, 0xe6, 0x39, 0xc0, 0x70,
  0xf3, 0xff, 0xdf, 0x01, 0xf0, 0xe4, 0x3e, 0x67, 0x9c, 0xe7, 0x38, 0x73, 0xf6, 0x39, 0xc0, 0x7c,
  0xf3, 0xff, 0xdf, 0x00, 0xf8, 0xfc, 0x37, 0x67, 0xb8, 0xef, 0x38, 0x73, 0x76, 0x39, 0xc0, 0x3e,
  0xf3, 0xff, 0xcf, 0x00, 0x7c, 0xfc, 0x37, 0x67, 0xf0, 0xfe, 0x38, 0x73, 0x3e, 0x39, 0xc0, 0x1f,
  0xf3, 0xff, 0xcf, 0x00, 0x1c, 0xe4, 0x33, 0xe7, 0x80, 0xee, 0x38, 0x73, 0x3e, 0x39, 0xc0, 0x0f,
  0x3b, 0xff, 0xdc, 0x00, 0x1c, 0xe1, 0x33, 0xe7, 0x80, 0xe7, 0x38, 0x73, 0x1e, 0x39, 0xc0, 0x07,
  0x3b, 0xff, 0xdc, 0x01, 0x1c, 0xe3, 0x31, 0xe7, 0x80, 0xe7, 0x9c, 0x73, 0x1e, 0x38, 0xe3, 0xc7,
  0x7f, 0xff, 0xbe, 0x01, 0xfc, 0xff, 0x31, 0xe7, 0x81, 0xe3, 0x9f, 0xe3, 0x8e, 0x38, 0xff, 0xff,
  0x7e, 0x00, 0x7c, 0x01, 0xf0, 0xff, 0x70, 0xe7, 0x81, 0xf3, 0xc7, 0x83, 0x86, 0x7c, 0x3e, 0x7c,
  0x3f, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0xc3, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x3c, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.begin(9600);         // Initializing Serail communication.
  inputString.reserve(10);

  for (byte i = 0; i < rows; i++) //Loop to declare output pins.
  {
    pinMode(Output[i], OUTPUT);
  }
  for (byte s = 0; s < columns; s++) // Loop to decalre Input pins, Initial Pulledup.
  {
    pinMode(Input[s], INPUT_PULLUP);
  }

  /////////////////////////////////SPI Settings \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

  SPI.begin();
  SPI.beginTransaction(SPISettings(60000000L, MSBFIRST, SPI_MODE0));
  pinMode(pinSPI_SS, OUTPUT);
  pinMode(IO_UPDATEPIN, OUTPUT);
  pinMode(MASTER_R, OUTPUT);
  pinReset(MASTER_R);

  //REGISTER 1  CFR1
  digitalWrite(pinSPI_SS, LOW); //
  SPI.transfer(0x00);
  SPI.transfer(0x01);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);

  //REGISTER 2 CFR2
  digitalWrite(pinSPI_SS, LOW);
  SPI.transfer(0x01);
  SPI.transfer(0x01);
  SPI.transfer(0x40);
  SPI.transfer(0x00);
  SPI.transfer(0x20);
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);

  // //REGISTER 3 CFR3
  digitalWrite(pinSPI_SS, LOW);
  SPI.transfer(0x02);
  SPI.transfer(0x1D);
  SPI.transfer(0x3F);
  SPI.transfer(0x41);
  SPI.transfer(0x48);
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);

  //REGISTER 4  DAC
  digitalWrite(pinSPI_SS, LOW);
  SPI.transfer(0x03);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x7F);
  SPI.transfer(0xFF);//127 FF=255;
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);

  display.clearDisplay();
  display.drawBitmap(0, 0,  logo16_glcd_bmp, 128, 64, WHITE);
  display.display();
  delay(2000);

}
void loop()
{
  startScreen();
}
void startScreen()
{
  display.stopscroll();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("MODE SELCT");
  display.println("A: F Fixed");
  display.println("B: F Sweep");
  display.println("C: A Sweep");
  display.display();
  delay(10);

  val = '&';
  while (val == '&')
    getkey();

  if (val >= 'A' && val <= 'D')
  {
    if (val == 'A') {
      display.clearDisplay();
      display.setTextSize(5);
      display.setTextColor(WHITE);
      display.setCursor(50, 15);
      display.println('A');
      display.display();
      delay(1000);
      FixedFreqMenu();
    }
    if (val == 'B') {
      display.clearDisplay();
      display.setTextSize(5);
      display.setTextColor(WHITE);
      display.setCursor(50, 15);
      display.println('B');
      display.display();
      delay(1000);
      SweepFreqMenu();
    }
  }
}
void FixedFreqMenu()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("       MONO TONE     ");

  display.setTextColor(WHITE);
  display.setCursor(0, 15);
  display.print("F1:");

  display.setCursor(0, 25);
  display.print("PO:");

  display.display();
  delay(100);

  ////  FQ

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
          inputString = "";               // clear input
          Serial.println(inputInt);
          display.setCursor(17, 15);
          display.print(inputInt);
          display.setCursor(45, 15);
          display.print(range);
          display.display();
          FQ = inputInt * MULT;
          Serial.println(FQ);
          MULT = 1;
          range = "Hz";
          delay(1000);
        }
      }
      else if (val >= 'A' && val <= 'C') {
        switch (val)
        {
          case 'A': {
              MULT = 1;
              range = "Hz";
            } break;
          case 'B': {
              MULT = 1000;
              range = "KHz";
            } break;
          case 'C': {
              MULT = 1000000;
              range = "MHz";
            } break;
        }
      }
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                   // Quit to Main Screen
      }
    }
  }

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          ASF = inputString.toFloat();
          inputString = "";               // clear input
          ASF /= 100;
          Serial.println(ASF);
          display.setCursor(17, 25);
          display.print(ASF);
          display.setCursor(45, 25);
          display.print("SF");
          display.setTextColor(BLACK, WHITE);
          display.setCursor(0, 50);
          display.print("      GENERATING     ");
          display.display();
          Fixed_Freq_Generation();
        }
      }
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                    // Quit to Main Screen
      }
    }
  }
}
void SweepFreqMenu()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("   FREQUENCY  SWEEP  ");

  display.setTextColor(WHITE);
  display.setCursor(0, 15);
  display.print("F1:");

  display.setCursor(60, 15);
  display.print("F2:");

  display.setCursor(0, 25);
  display.print("SF:");

  display.setCursor(60, 25);
  display.print("SD:");

  display.setCursor(0, 35);
  display.print("PO:");

  display.setCursor(60, 35);
  display.print("IT:");

  display.display();
  delay(100);

  ////  SFQ

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
          inputString = "";               // clear input
          Serial.println(inputInt);
          display.setCursor(17, 15);
          display.print(inputInt);
          display.setCursor(38, 15);
          display.print(range);
          display.display();
          SFQ = inputInt * MULT;
          Serial.println(SFQ);
          MULT = 1;
          range = "Hz";
          delay(1000);
        }
      }
      else if (val >= 'A' && val <= 'C') {
        switch (val)
        {
          case 'A': {
              MULT = 1;
              range = "Hz";
            } break;
          case 'B': {
              MULT = 1000;
              range = "KHz";
            } break;
          case 'C': {
              MULT = 1000000;
              range = "MHz";
            } break;
        }
      }
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                   // Quit to Main Screen
      }
    }
  }

  //// EFQ

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
          inputString = "";               // clear input
          Serial.println(inputInt);
          display.setCursor(77, 15);
          display.print(inputInt);
          display.setCursor(98, 15);
          display.print(range);
          display.display();
          EFQ = inputInt * MULT;
          Serial.println(EFQ);
          MULT = 1;
          range = "Hz";
          delay(1000);
        }
      }
      else if (val >= 'A' && val <= 'C') {
        switch (val)
        {
          case 'A': {
              MULT = 1;
              range = "Hz";
            } break;
          case 'B': {
              MULT = 1000;
              range = "KHz";
            } break;
          case 'C': {
              MULT = 1000000;
              range = "MHz";
            } break;
        }
      }
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                   // Quit to Main Screen
      }
    }
  }

  //// SF  - STEP FREQUENCY

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
          inputString = "";               // clear input
          Serial.println(inputInt);
          display.setCursor(17, 25);
          display.print(inputInt);
          display.setCursor(38, 25);
          display.print(range);
          display.display();
          STEP = inputInt * MULT;
          Serial.println(STEP);
          MULT = 1;
          range = "Hz";
          delay(1000);
        }
      }
      else if (val >= 'A' && val <= 'C') {
        switch (val)
        {
          case 'A': {
              MULT = 1;
              range = "Hz";
            } break;
          case 'B': {
              MULT = 1000;
              range = "KHz";
            } break;
          case 'C': {
              MULT = 1000000;
              range = "MHz";
            } break;
        }
      }
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                   // Quit to Main Screen
      }
    }
  }

  //// SD - DELAY

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
          inputString = "";               // clear input
          Serial.println(inputInt);
          display.setCursor(77, 25);
          display.print(inputInt);
          display.setCursor(98, 25);
          display.print("mS");
          display.display();
          DELAY = inputInt;
          Serial.println(DELAY);
          delay(1000);
        }
      }
      else if (val >= 'A' && val <= 'C') {}  // No Denomination
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                   // Quit to Main Screen
      }
    }
  }

  ///// Amplitude // PO

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          ASF = inputString.toFloat();
          inputString = "";               // clear input
          ASF /= 100;
          Serial.println(ASF);
          display.setCursor(17, 35);
          display.print(ASF);
          display.setCursor(45, 35);
          display.print("SF");
          display.display();
        }
      }
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                    // Quit to Main Screen
      }
    }
  }

  //// IT - Iterations

  val = '&';
  while (val != '+')
  {
    getkey();

    if ((val >= '0' && val <= '9') || (val >= 'A' && val <= 'D') || (val == '*') || (val == '#'))
    {
      if (val >= '0' && val <= '9') {     // only act on numeric keys
        inputString += val;               // append new character to input string
        val = '&';
      }
      else if (val == '#') {
        val = '+';
        if (inputString.length() > 0) {
          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
          inputString = "";               // clear input
          Serial.println(inputInt);
          display.setCursor(77, 35);
          display.print(inputInt);
          display.setCursor(110, 35);
          display.print('#');
          display.display();
          REP = inputInt;
          Serial.println(REP);
          display.setTextColor(BLACK, WHITE);
          display.setCursor(0, 50);
          display.print("      GENERATING     ");
          display.display();
          Sweep_Freq_Generation();
        }
      }
      else if (val >= 'A' && val <= 'C') {}  // No Denomination
      else if (val == '*') {
        inputString = "";                 // clear input
      }
      else if (val == 'D') {
        return;                   // Quit to Main Screen
      }
    }
  }
}
void Fixed_Freq_Generation()
{
  FQ =
    Serial.println("Fixed_Freq_Generation");
  ASF_R = ASF * 16384;                 // 2^128 Scaling Factor
  ASF_R_D = ASF_R;

  FTW_F = (FQ / Fsck) * 4294967296;
  FTW_D = FTW_F;
  digitalWrite(pinSPI_SS, LOW);
  SPI.transfer(0x0E);
  SPI.transfer((ASF_R_D >> 8) & 0xFF);
  SPI.transfer((ASF_R_D >> 0) & 0xFF);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer((FTW_D >> 24) & 0xFF);
  SPI.transfer((FTW_D >> 16) & 0xFF);
  SPI.transfer((FTW_D >> 8) & 0xFF);
  SPI.transfer((FTW_D >> 0) & 0xFF);
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);
  delayMicroseconds(100);

  while (val != 'D') {   // EXIT Statement to stop signal generation
    getkey();
  }
  FQ = 0;
  FTW_F = (FQ / Fsck) * 4294967296;
  FTW_D = FTW_F;
  digitalWrite(pinSPI_SS, LOW);
  SPI.transfer(0x0E);
  SPI.transfer((ASF_R_D >> 8) & 0xFF);
  SPI.transfer((ASF_R_D >> 0) & 0xFF);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer((FTW_D >> 24) & 0xFF);
  SPI.transfer((FTW_D >> 16) & 0xFF);
  SPI.transfer((FTW_D >> 8) & 0xFF);
  SPI.transfer((FTW_D >> 0) & 0xFF);
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);
  delayMicroseconds(10);
  Serial.println("Operation STOPPED ");
}
void Sweep_Freq_Generation()
{
  Serial.println("Sweep_Freq_Generation");
  ASF_R = ASF * 16384;              // 2^128 Scaling Factor
  ASF_R_D = ASF_R;

  for (int i = 1; i <= REP; i++)
  {
    for (FQ = SFQ ; FQ <= EFQ ; FQ = (FQ + STEP))
    {
      FTW_F = (FQ / Fsck) * 4294967296;
      FTW_D = FTW_F;
      digitalWrite(pinSPI_SS, LOW);
      SPI.transfer(0x0E);
      SPI.transfer((ASF_R_D >> 8) & 0xFF);
      SPI.transfer((ASF_R_D >> 0) & 0xFF);
      SPI.transfer(0x00);
      SPI.transfer(0x00);
      SPI.transfer((FTW_D >> 24) & 0xFF);
      SPI.transfer((FTW_D >> 16) & 0xFF);
      SPI.transfer((FTW_D >> 8) & 0xFF);
      SPI.transfer((FTW_D >> 0) & 0xFF);
      SPI.endTransaction();
      pinUpdate(IO_UPDATEPIN);
      digitalWrite(pinSPI_SS, HIGH);
      delayMicroseconds(DELAY);
      getkey();
      if (val == 'D')
      {
        EXIT = 1;
        break;  // EXIT Statement to stop signal generation
      }
    }
    if (EXIT) {
      EXIT = 0;
      break;
    }
    Serial.println(i);
  }
  FQ = 0;
  FTW_F = (FQ / Fsck) * 4294967296;
  FTW_D = FTW_F;
  digitalWrite(pinSPI_SS, LOW);
  SPI.transfer(0x0E);
  SPI.transfer((ASF_R_D >> 8) & 0xFF);
  SPI.transfer((ASF_R_D >> 0) & 0xFF);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer((FTW_D >> 24) & 0xFF);
  SPI.transfer((FTW_D >> 16) & 0xFF);
  SPI.transfer((FTW_D >> 8) & 0xFF);
  SPI.transfer((FTW_D >> 0) & 0xFF);
  SPI.endTransaction();
  pinUpdate(IO_UPDATEPIN);
  digitalWrite(pinSPI_SS, HIGH);
  delayMicroseconds(10);
  Serial.println("Operation STOPPED ");
}
void getkey()
{
  if (millis() - kdelay > period) //used to make non-blocking delay
  {
    kdelay = millis();    //capture time from millis function
    switch (keypad())     //Switch to get which button is pressed.
    {
      case 0: {
          Serial.print('1');
          val = '1';
        } break;
      case 1: {
          Serial.print('2');
          val = '2';
        } break;
      case 2: {
          Serial.print('3');
          val = '3';
        } break;
      case 3: {
          Serial.print('A');
          val = 'A';
        } break;
      case 4: {
          Serial.print('4');
          val = '4';
        } break;
      case 5: {
          Serial.print('5');
          val = '5';
        } break;
      case 6: {
          Serial.print('6');
          val = '6';
        } break;
      case 7: {
          Serial.print('B');
          val = 'B';
        } break;
      case 8: {
          Serial.print('7');
          val = '7';
        } break;
      case 9: {
          Serial.print('8');
          val = '8';
        } break;
      case 10: {
          Serial.print('9');
          val = '9';
        } break;
      case 11: {
          Serial.print('C');
          val = 'C';
        } break;
      case 12: {
          Serial.print('*');
          val = '*';
        } break;
      case 13: {
          Serial.print('0');
          val = '0';
        } break;
      case 14: {
          Serial.print('#');
          val = '#';
        } break;
      case 15: {
          Serial.print('D');
          val = 'D';
        } break;
      default:
        ;
    }
  }
}
byte keypad()
{
  static bool no_press_flag = 0;
  for (byte x = 0; x < columns; x++)
  {
    if (digitalRead(Input[x]) == HIGH);
    else
      break;
    if (x == (columns - 1))
    {
      no_press_flag = 1;
      h = 0;
      v = 0;
    }
  }
  if (no_press_flag == 1)
  {
    for (byte r = 0; r < rows; r++)
      digitalWrite(Output[r], LOW);
    for (h = 0; h < columns; h++)
    {
      if (digitalRead(Input[h]) == HIGH)
        continue;
      else
      {
        for (v = 0; v < rows; v++)
        {
          digitalWrite(Output[v], HIGH);
          if (digitalRead(Input[h]) == HIGH)
          {
            no_press_flag = 0;
            for (byte w = 0; w < rows; w++)
              digitalWrite(Output[w], LOW);
            return v * 4 + h;
          }
        }
      }
    }
  }
  return 50;
}
