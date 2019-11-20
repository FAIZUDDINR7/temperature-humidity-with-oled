 /****************************************************************************
*Code developed by IoT@FORGE DevOps on 08 October 2019 at 11.15PM************
*Components Used : ESP32 and DHT22*******************************************
*Connections Used : GND - GND || VCC - 3V3 || OUT - D15**********************
*****************************************************************************  
*****************************************************************************
*****************************************************************************
***__________***__________***__________****____________***___________********
**|          |*|          |*|          |**|            |*|           |*******
**|    ______|*|   ____   |*|    ___    |*|    ________|*|     ______|*******
**|    |*******|  |    |  |*|   |   |   |*|   |**********|    |**************
**|    |___****|  |    |  |*|   |___|   |*|   |***-----**|    |__************
**|       |****|  |    |  |*|    __   |***|   |**|_    |*|     __|***********
**|   ____|****|  |____|  |*|   |  |   |**|   |____|   |*|    |______********
**|   |********|          |*|   |   |   |*|            |*|           |*******
**|___|********|__________|*|___|***|___|*|____________|*|___________|*******
*****************************************************************************
*****************************************************************************
*****************************************************************************
*****************************************************************************
*ESP32 Dev Module, Disabled, Default 4MB with spiffs(1.2MB APP/1.5MB SPIFFS)*
*240MHz(WiFi/BT), QIO, 80MHz, 4MB, 115200 BaudRatio**************************
****************************************************************************/


//Initialize the libraries here
#include<WiFi.h>
#define CAYENNE_PRINT Serial
#include <CayenneMQTTWiFi.h>
#include<DHTesp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//int a=10;
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


//Define the DHT pin here
#define DHTPIN 15
DHTesp dht;


// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "c96eca10-f421-11e9-b49d-5f4b6757b1bf";
char password[] = "39dcabb74c6bdd53a871cd2b7441fc6a01f62e85";
char clientID[] = "03dba5f0-f423-11e9-ba7c-716e7f5ba423";


unsigned long lastMillis = 0;


// replace with your wifi ssid and wpa2 key
const char *ssid =  "RFN";     
const char *pass =  "miphone123";


//Set the channel number here
int channel_1 = 3;
int channel_2 = 4;
int channel_3 = 5;


//defining the wificlient function
WiFiClient client;


void setup() {

   Serial.begin(9600);


   //the ESP32 is now starting to connect to WiFi you mentioned using the below code
   WiFi.begin(ssid, pass);


   //Checking the WiFi status here
   while (WiFi.status() != WL_CONNECTED) 
     {
            delay(100);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");


    //Cayenne starts to connect here  
    Cayenne.begin(username, password, clientID);


    //setting the module DHT22 or DHT11
    dht.setup(DHTPIN, DHTesp::DHT22);

      if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
for(;;);
  }
}


void loop() {
  
  Cayenne.loop();

  
  //Publish data every 1 seconds (1000 milliseconds). Change this value to publish at a different interval.
  if(millis() - lastMillis > 1000) {
    lastMillis = millis();


    //Storing the temp and hum values to push
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();


    //Printing the recieved values on the plotter for testing
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity, 1);
    Serial.print("\t\t");
    Serial.print(temperature, 1);
    Serial.print("\t\t");

    
    //delay(2000);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.print("Temp:");
  display.println(temperature);
  display.print("Hmd:");
  display.println(humidity);
  display.display(); 

    //Write data to Cayenne here.
    Cayenne.virtualWrite(channel_1, lastMillis);
    Cayenne.celsiusWrite(channel_2, temperature);
    Cayenne.virtualWrite(channel_3, humidity, "rel_hum", "p");
   
  }
}


//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
