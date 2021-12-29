#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <networkInfo.cpp>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BME280.h>
#include <stdio.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme; 

int drawX = 0;

void outputInfo(const char* data, bool refresh = true){
  if(refresh == true){
    display.clearDisplay();
  }

  Serial.println(data);
  display.println(data);

  if(refresh == true){
    display.display();
  }
  //display.drawRect(10,10, 20,20, WHITE);
}

void outputStringInfo(String data){
  int n = data.length();
  char char_array[n + 1];
  strcpy(char_array, data.c_str());

  outputInfo(char_array);
}



void outputInfo(float data){
   char array[10];
  sprintf(array, "%f", data);

  outputInfo(array);
}

void output(const char* data){
    Serial.print(data);
    display.print(data);
}

void output(float data){

   char array[10];
  sprintf(array, "%f", data);

  output(array);
}

void outputString(String data){
  int n = data.length();
  char char_array[n + 1];
  strcpy(char_array, data.c_str());

  output(char_array);
}


void setup() {
  Serial.begin(115200);



 if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  unsigned tempSensorStatus = bme.begin(0x76);
  if (!tempSensorStatus) {
    Serial.println(F("Could not find a valid BMP280 sensor"));
  }

    /* Default settings from datasheet. */
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BME280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BME280::SAMPLING_X16); /* Standby time. */

  WiFi.mode(WIFI_STA);
  WiFi.begin(NETWORK_SSID, NETWORK_PASS);

  outputInfo("Connecting...", true);
  display.display();

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    outputInfo("Connection Failed! Rebooting...", true);
    delay(5000);
    ESP.restart();
  }

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  // No authentication by default
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    outputStringInfo("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    outputInfo("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int progressPercent = (int) (progress / (total / 100));
    display.clearDisplay();
    outputStringInfo("Progress: " + progressPercent);
  });
  ArduinoOTA.onError([](ota_error_t error) {

    outputInfo("Error: " + error);
    if (error == OTA_AUTH_ERROR) {
      outputInfo("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      outputInfo("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      outputInfo("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      outputInfo("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      outputInfo("End Failed");
    }
  });

  ArduinoOTA.begin();

  display.clearDisplay();

  outputInfo("WiFi connected", false);
  outputInfo("IP address: ", false);
  outputInfo(WiFi.localIP().toString().c_str(), false);

  display.display();

  delay(5000);

  //pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
   
    display.setCursor(0, 0);
    output("Temperature = ");
    output(bme.readTemperature());
    outputInfo(" *C", false);

    // display.setCursor(0, 24);
    // output("Pressure = ");
    // output(bme.readPressure());
    // outputInfo(" Pa");

    display.setCursor(0, 24);
    output("Humidity = ");
    output(bme.readHumidity()); /* Adjusted to local forecast! */
    outputInfo(" %", false);

    outputInfo("", false); 


    display.display();
             
    delay(1000);  

}