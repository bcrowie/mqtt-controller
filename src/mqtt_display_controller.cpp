#include <Wire.h>
#include <ESP8266WiFi.h>
#include <RotaryEncoder.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <string>

// OLED Display Definitions
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

// WiFi Definitions
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
SSD1306AsciiWire oled;

// Rotary Encoder Definitions
#define PIN_A D5
#define PIN_B D4
#define BUTTON D3
int16_t position = 0;
RotaryEncoder encoder(PIN_A, PIN_B, BUTTON);


//------------------------------------------------------------------------------

String menu[] = {
  "Menu item one",
  "Menu item 2",
  "Menu item 333",
  "Menu item 44444"
};

void updateDisplay(String data)
{
  oled.clear();
  oled.println(data);
}

bool connectWifi(String SSID, String PASSWORD) 
{
  WiFi.begin(SSID, PASSWORD);

  oled.clear();
  oled.println("Connecting to");
  oled.print(WIFI_SSID);
  oled.println();
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    oled.clear();
    oled.println("Connected!");
    oled.println(WiFi.localIP());
    delay(2000);
  }

  return true;
}

void ICACHE_RAM_ATTR encoderISR()
{
  encoder.readAB();
}

void ICACHE_RAM_ATTR encoderButtonISR()
{
  encoder.readPushButton();
}

void updateMenu() {
  // Update menu code
}

void setup() {
  // Start serial for monitoring
  Serial.begin(115200);
  Serial.println();
  
  Wire.begin();
  Wire.setClock(400000L);

  encoder.begin();                                                           //set encoders pins as input & enable built-in pullup resistors

  attachInterrupt(digitalPinToInterrupt(PIN_A),  encoderISR,       CHANGE);  //call encoderISR()       every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(BUTTON), encoderButtonISR, FALLING);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else 
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif

  oled.setFont(Adafruit5x7);

  if(connectWifi(WIFI_SSID, WIFI_PASSWORD))
  {
    oled.clear();
    oled.set2X();

    oled.println("Ready!!"); // Change to update display with menu
    delay(2000);
    oled.set1X();
    oled.clear();
  }
}
//------------------------------------------------------------------------------
void loop() {
  // Check for rotarty rotation, button presses then update display
  if (position != encoder.getPosition())
  {
    position = encoder.getPosition();

    if(position > sizeof(menu) - 1 ) 
    {
      position = 0;
    } 
    else if (position <= -1) 
    {
      position = sizeof(menu) - 1;
    };
    
    updateDisplay(menu[position]);
  }

  if (encoder.getPushButton() == true)
  {
    updateDisplay("PRESSED");
    //oled.println("PRESSED");
    //delay(2000);
    //oled.clear();
  }
}
