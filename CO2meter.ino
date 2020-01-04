/*
  Version: 1.01

  Arduino Nano
  CO2 - MH-Z19B
  Temperature, humidity, pressure - BME280
  OLED - SSD1306 128x32
  Code, fonts, layout etc - https://github.com/killadog/CO2meter

                     Mode 0                   Mode 1
            ┌───────────────────────┬───────────────────────┐
   Sensor 0 │  Current temperature  |  Min-Max temperature  │
            ├───────────────────────┼───────────────────────┤
   Sensor 1 │    Current humidity   |    Min-Max humidity   │
            ├───────────────────────┼───────────────────────┤
   Sensor 2 │    Current pressure   |    Min-Max pressure   │
            ├───────────────────────┼───────────────────────┤
   Sensor 3 │      Current CO2      |      Min-Max CO2      │
            ├───────────────────────┼───────────────────────┤
   Sensor 4 │      ALL sensors      |      Ver. & Year      │
            └───────────────────────┴───────────────────────┘
*/

#include <SparkFunBME280.h>
#include <U8g2lib.h>
#include <GyverButton.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>

#define RX_PIN                        4                  // RX pin
#define TX_PIN                        5                  // TX pin
#define RED_PIN                       6                  // RED pin
#define YELLOW_PIN                    7                  // YELLOW pin 
#define GREEN_PIN                     8                  // GREEN pin
#define BUTTON_PIN                    9                  // BUTTON pin
#define SDA_PIN                       A4                 // SDA pin
#define SCL_PIN                       A5                 // SCL pin

const uint8_t SENSORS               = 4;                 // number of sensors
float VALUE[SENSORS];                                    // current value of each sensor
float CALIBRATION[SENSORS]          = { -0.3, 0, 0, 0};  // calibration of each sensor (t,%,T,ppm)
uint16_t MIN[SENSORS];                                   // Min value of each sensor
uint16_t MAX[SENSORS];                                   // Max value of each sensor

boolean MODE                        = 0;                 // start mode (0 - normal, 1 - min/max)
uint8_t SENSOR_NUMBER               = 4;                 // start sensor (screen) for show
uint32_t SENSORS_TIMESTAMP          = millis();
#define CHECK_TIME                    20000              // sensors check time in millis

boolean DISPLAY_MODE                = 0;                 // normal (0) or rotate (1) screen

BME280 bme280;
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);
MHZ19 myMHZ19;

GButton button_select(BUTTON_PIN);

SoftwareSerial co2(RX_PIN, TX_PIN);

const uint8_t lcdnums_10x16[330] U8G2_FONT_SECTION("lcdnums_10x16") =
  "\26\0\4\3\5\4\3\4\6\26\17\0\373\12\0\0\0\0\0\0\0\1\61$ \365:\66\224\224\310\220"
  "H\210HL\224H\210H\34\210H\210HHD\234\224\310P\251\241R#%\33\212;l\214\305D\305"
  "\244\210\11\211\31\11\314d&$&*&*&l\2\0&\13\346\274)\34\224\10\351\4\0'\5\0"
  "\210)(\5\0\210))\5\0\210)*\14\345\274gL\304\350&b\2\0+\5\0\210),\5\0"
  "\210)-\5\0\210).\6b\70$\30/\5\0\210)\60\20\351\71k\134DTL\361\277\232\250\210"
  "\270\0\61\13\346=+\320\14\351rP\62\24\351\71+\34\204X\304\351\302B\302BNw\21&\7"
  "\1\63\24\351\71+\34\204X\304\351\244\42\342\62N\27\26\7\22\0\64\16\351\71+H\361\325DE\304"
  "e\234>\65\24\351\71k\34HX\310i\27ar\20\247\27\26\7\22\0\66\25\351\71k\34HX\310"
  "ix\21Q\61\305\253\211\212\210\13\0\67\16\351\71+\34\204X\304)\323\235~\4\70\27\351\71k\134"
  "DTLq\65Q\21q\21\67\305\325DE\304\5\0\71\25\351\71k\134DTLq\65Q\21q\31"
  "\247\13\213\3\11\0\0\0\0";

const uint8_t lcdnums_12x32[407] U8G2_FONT_SECTION("lcdnums_12x32") =
  "\26\0\4\3\4\6\4\6\6\21 \0\354\14\0\0\0\0\0\0\0\1~$%\237(\23\303\310H\214"
  "HDH\204\304HDH\204\304HDHD\12\222\221\210\10\241\64Bi\204\322\10\245\11%%Y-"
  "\303\306TLPLPLPLPLPL\324L\134\276\231\212\11\212\11\212\11\212\11\212\11\212\211\232"
  "\0&\13\226\354\230\302A\211\220>\1'\6\0 \10\2(\6\0 \10\2)\6\0 \10\2*\14"
  "\226\354\230\6E\220>\212\240\0+\6\0 \10\2,\6\0 \10\2-\10\66\350\246\302\3\2.\10"
  "R\60\243\302\301\0/\6\0 \10\2\60\25\12*\343\306AD\204\305\30\377\377\377g\23\26\21\7\21"
  "\0\61\16\6\62\343\2\315\220\376\377\377O\16\12\62\30\12*\343\302\201\310E\240\376/,&,&,"
  "\4\365\30qr \63\34\12*\343\302\3\212\210\3A\375\207\7\23\7\23\7\21\201\372\277\70\70\230"
  "\70\230\0\64\23\12*\343\202\30\377\277\240\230\260\210\70\210\24\324\377\7\65\30\12*\343\306\301\304\205\240"
  "\376/,\42,\42,\4\365\377\342\301\4\0\66\32\12*\343\306\301\304\205\240\376\207\21'\27\22\25d"
  "\374?\233\260\210\70\210\0\67\26\12*\343\302\201\310E\240\356\4\345\4\345\4\345\4\365\377?\2\70\35"
  "\12*\343\306AD\204\305\30\377g\23\26\21\7\21\21\26c\374?\233\260\210\70\210\0\71\30\12*\343"
  "\306AD\204\305\30\377/*$N.\2\365\377\342\301\4\0\0\0\0";

void setup()
{
  Serial.begin(9600);

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button_select.setDebounce(50);         // настройка антидребезга (по умолчанию 80 мс)
  button_select.setTimeout(1500);        // настройка таймаута на удержание (по умолчанию 500 мс)
  button_select.setClickTimeout(300);    // настройка таймаута между кликами (по умолчанию 300 мс)

  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, LOW);

  u8g2.begin();
  u8g2.setDrawColor(1);

  bme280.settings.I2CAddress = 0x76;
  bme280.settings.commInterface = I2C_MODE;
  bme280.begin();
  if (!bme280.beginI2C())                            //Begin communication over I2C
  {
    Serial.println(F("BME280 did not respond. Please check wiring."));
    while (1);                                       //Freeze
  }

  co2.begin(9600);                                   // Begin Stream with MHZ19 baudrate
  myMHZ19.begin(co2);                                // *Important, Pass your Stream reference
  myMHZ19.autoCalibration(0);                        // Turn auto calibration OFF (disable with autoCalibration(false))

  for (uint8_t s = 0; s < SENSORS; s++)
  {
    MIN[s] = 3000;                                   //INCORRECT(!) values for minimums
  }

  START_SCREEN();
}

void START_SCREEN()                                  //Countdown. Center text
{
  u8g2.setFont(lcdnums_12x32);
  uint32_t START_TIME = millis();
  while (millis() < START_TIME + CHECK_TIME)
  {
    u8g2.firstPage();
    do {
      char charBuf[3];
      String COUNTDOWN = String((CHECK_TIME + 1000 - millis()) / 1000);
      COUNTDOWN.toCharArray(charBuf, 3);
      u8g2_uint_t TEXT_WIDTH = u8g2.getStrWidth(charBuf);
      u8g2.drawStr((128 - TEXT_WIDTH) / 2, 12, charBuf);
    } while (u8g2.nextPage());
  }
}

void loop()
{
  u8g2.firstPage();
  do {
    draw();
  } while (u8g2.nextPage());

  CHECK_BUTTON();

  if (CHECK_TIME < millis() - SENSORS_TIMESTAMP)
  {
    CHECK_SENSORS();
  }
}

void CHECK_BUTTON()
{
  button_select.tick();
  if (button_select.isSingle())
  {
    SENSOR_NUMBER++;
    if (SENSOR_NUMBER > SENSORS) SENSOR_NUMBER = 0;
    Serial.print(F("Single click. Sensor "));
    Serial.print(SENSOR_NUMBER);
    Serial.print(F(" Mode "));
    Serial.println(MODE);
  }
  if (button_select.isDouble())
  {
    MODE = !MODE;
    Serial.print(F("Double click. Sensor "));
    Serial.print(SENSOR_NUMBER);
    Serial.print(F(" Mode "));
    Serial.println(MODE);
  }
  if (button_select.isHolded())
  {
    DISPLAY_MODE = !DISPLAY_MODE;
    if (DISPLAY_MODE) u8g2.setDisplayRotation(U8G2_R0);
    else u8g2.setDisplayRotation(U8G2_R2);
    Serial.println(F("Holded. Rotate display."));
  }
}

void draw()
{
  u8g2.setFont(lcdnums_12x32);
  switch (SENSOR_NUMBER)
  {
    case 0: SENSOR_0(); break;
    case 1: SENSOR_1(); break;
    case 2: SENSOR_2(); break;
    case 3: SENSOR_3(); break;
    case 4: SENSOR_4(); break;
  }
}

void SENSOR_0()                                       //Temperature
{
  switch (MODE)
  {
    case 0:
      u8g2.setCursor(32, 12);
      u8g2.print(VALUE[0], 1);
      u8g2.print(F("*"));                             //degree symbol
      break;
    case 1:
      u8g2.setCursor(0, 12);
      u8g2.print(MIN[0]);
      u8g2.print(F("-"));
      u8g2.print(MAX[0]);
      u8g2.print(F("*"));                             //degree symbol
      break;
  }
}

void SENSOR_1()                                       //Humidity
{
  switch (MODE)
  {
    case 0:
      u8g2.setCursor(32, 12);
      u8g2.print(VALUE[1], 1);
      u8g2.print(F("%"));
      break;
    case 1:
      u8g2.setCursor(0, 12);
      u8g2.print(MIN[1]);
      u8g2.print(F("-"));
      u8g2.print(MAX[1]);
      u8g2.print(F("%"));
      break;
  }
}

void SENSOR_2()                                       //Pressure
{
  switch (MODE)
  {
    case 0:
      u8g2.setCursor(26, 12);
      u8g2.print(VALUE[2], 1);
      u8g2.print(F("&"));                             //Torr symbol
      break;
    case 1:
      u8g2.setCursor(0, 12);
      u8g2.print(MIN[2]);
      u8g2.print(F("-"));
      u8g2.print(MAX[2]);
      u8g2.print(F("&"));                             //Torr symbol
      break;
  }
}
void SENSOR_3()                                       //CO2
{
  switch (MODE)
  {
    case 0:
      u8g2.setCursor(32, 12);
      u8g2.print(VALUE[3], 0);
      u8g2.print(F("$"));                             //ppm symbol
      break;
    case 1:
      u8g2.setCursor(0, 12);
      u8g2.print(MIN[3]);
      u8g2.print(F("-"));
      u8g2.print(MAX[3]);
      u8g2.print(F("$"));                             //ppm symbol
      break;
  }
}

void SENSOR_4()
{
  switch (MODE)
  {
    case 0:                                           //show all sensors
      u8g2.setFont(lcdnums_10x16);
      u8g2.setCursor(0, 10);
      u8g2.print(VALUE[0], 1);
      u8g2.print(F("*"));                             //degree symbol

      u8g2.setCursor(0, 27);
      u8g2.print(VALUE[1], 1);
      u8g2.print(F("%"));

      u8g2.setCursor(60, 10);
      u8g2.print(VALUE[2], 1);
      u8g2.print(F("&"));                             //Torr symbol

      u8g2.setCursor(60, 27);
      u8g2.print(VALUE[3], 0);
      u8g2.print(F("$"));                             //ppm symbol
      break;
    case 1:
      u8g2.setFont(lcdnums_10x16);
      u8g2.setCursor(50, 10);
      u8g2.print("1.01");                             //version
      u8g2.setCursor(40, 27);
      u8g2.print("2019");                             //year
      break;
  }
}

void CHECK_SENSORS()
{
  for (uint8_t s = 0; s < SENSORS; s++)
  {
    switch (s)
    {
      case 0:
        VALUE[s] = bme280.readTempC() + CALIBRATION[s];
        if  (VALUE[s] < MIN[s]) MIN[s] = VALUE[s];
        if  (VALUE[s] > MAX[s]) MAX[s] = VALUE[s];
        Serial.print (F("T: "));
        Serial.print(VALUE[s], 1);
        Serial.print (F("°C ("));
        Serial.print(MIN[s], 1);
        Serial.print(F("/"));
        Serial.print(MAX[s], 1);
        Serial.print(F(")   "));
        break;
      case 1:
        VALUE[s] = bme280.readFloatHumidity() + CALIBRATION[s];
        if  (VALUE[s] < MIN[s]) MIN[s] = VALUE[s];
        if  (VALUE[s] > MAX[s]) MAX[s] = VALUE[s];
        Serial.print (F("H: "));
        Serial.print(VALUE[s], 1);
        Serial.print (F("% ("));
        Serial.print(MIN[s], 1);
        Serial.print(F("/"));
        Serial.print(MAX[s], 1);
        Serial.print(F(")   "));
        break;
      case 2:
        VALUE[s] = bme280.readFloatPressure() / 133.3224 + CALIBRATION[s];
        if  (VALUE[s] < MIN[s]) MIN[s] = VALUE[s];
        if  (VALUE[s] > MAX[s]) MAX[s] = VALUE[s];
        Serial.print (F("P: "));
        Serial.print(VALUE[s], 1);
        Serial.print (F("T ("));
        Serial.print(MIN[s], 1);
        Serial.print(F("/"));
        Serial.print(MAX[s], 1);
        Serial.print(F(")   "));
        break;
      case 3:
        VALUE[s] = myMHZ19.getCO2() + CALIBRATION[s];
        if  (VALUE[s] < MIN[s]) MIN[s] = VALUE[s];
        if  (VALUE[s] > MAX[s]) MAX[s] = VALUE[s];
        Serial.print (F("CO2:"));
        Serial.print(VALUE[s], 0);
        Serial.print (F(" ppm ("));
        Serial.print(MIN[s], 1);
        Serial.print(F("/"));
        Serial.print(MAX[s], 1);
        Serial.println(F(")"));
        break;
    }
  }
  CHECK_DANGER();
  SENSORS_TIMESTAMP = millis();
}

void CHECK_DANGER()
{
  uint16_t VALUE = myMHZ19.getCO2() + CALIBRATION[3] ;
  if (VALUE < 800)
  {
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
  }
  if ((VALUE >= 800) && (VALUE < 1000))
  {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
  }
  if (VALUE >= 1000)
  {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
  }
}
