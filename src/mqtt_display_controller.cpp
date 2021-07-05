#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ClickEncoder.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <string>

// OLED
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

// WiFi
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
SSD1306AsciiWire oled;

// Rotary Encoder
#define ENCODER_PINA     D5
#define ENCODER_PINB     D4
#define ENCODER_BTN      D3
#define ENCODER_STEPS_PER_NOTCH     2
ClickEncoder encoder = ClickEncoder(ENCODER_PINA,ENCODER_PINB,ENCODER_BTN,ENCODER_STEPS_PER_NOTCH);

//------------------------------------------------------------------------------

int8 position = 0;

String menu[7] = {
  "Menu item one",
  "Menu item 2",
  "Menu item 333",
  "Menu item 44444",
  "5",
  "6",
  "Brenden"
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

void setup() {
  Serial.begin(115200);
  
  Wire.begin();
  Wire.setClock(400000L);

  encoder.setButtonHeldEnabled(true);
  encoder.setDoubleClickEnabled(true);
  encoder.setButtonOnPinZeroEnabled(true);


  if(RST_PIN >= 0){
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  } else {
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  }

  oled.setFont(Adafruit5x7);

  // Pub/Sub MQTT

  if(connectWifi(WIFI_SSID, WIFI_PASSWORD)) {
    oled.clear();
    oled.set2X();
    oled.println("Ready!!"); // Change to update display with menu after wifi connect and mqtt subscription
    delay(2000);
    oled.set1X();
    oled.clear();
  }
}
//------------------------------------------------------------------------------
void loop() {
  static uint32_t lastService = 0;
  if (lastService + 1000 < micros()) {
    lastService = micros();                
    encoder.service();  
  }

  static int16_t last, value;
    value += encoder.getValue();
  
  #define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0]))) - 1

  if (value != last) {
    last = value;

    if(value > NUMITEMS(menu)){
      value = 0;
    } 
    if (value < 0) {
      value = NUMITEMS(menu);
      last = value;
    }

    updateDisplay(menu[value]);
  }

    ClickEncoder::Button b = encoder.getButton();
  if (b != ClickEncoder::Open) {
    Serial.print("Button: ");
    #define VERBOSECASE(label) case label: updateDisplay(#label); break;
    switch (b) {
      VERBOSECASE(ClickEncoder::Pressed);
      VERBOSECASE(ClickEncoder::Held)
      VERBOSECASE(ClickEncoder::Released)
      VERBOSECASE(ClickEncoder::Clicked)
      VERBOSECASE(ClickEncoder::DoubleClicked)
    }
  }    
}
