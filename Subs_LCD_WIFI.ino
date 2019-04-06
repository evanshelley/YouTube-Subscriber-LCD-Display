#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFiNINA.h>

// Header file containing various initialisations and declarations
#include "Subs_Declarations.h"

// Header file containing declarations of char ssid[] and char pass[] used for WiFi login
#include "WiFi_Login.h"

// Header file containing API key in form of String key = "GET /apps/thinghttp/send_request?api_key=XXXXXXXXXXXXXX"
#include "API_Key.h"


void setup() {
  // For debugging
  Serial.begin(9600);
  // Setup LCD
  lcd.begin(16,2);

  // Set piezo to output, output nothing for now
  pinMode(speaker,OUTPUT);
  digitalWrite(speaker,LOW);

  // Print "Connecting to WiFi SSID"
  lcd.setCursor(1,0);
  lcd.print("Connecting to");
  lcd.setCursor(4,1);
  lcd.print("WiFi");
  lcd.setCursor(9,1);
  lcd.print(ssid);

  // Connect to WiFi network
  status = WiFi.begin(ssid,pass);

  // If connection fails, print message and do nothing
  // otherwise, print message and proceed
  if (status != WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Failed to");
    lcd.setCursor(2,1);
    lcd.print("connect :(");
    while(1);
    
  } else {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Connected :)");
    delay(THREESECONDS);
  }

  // Set up display
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("XXXXXX Subs");
  lcd.setCursor(3,1);
  lcd.print("Loading...");

}

void loop() {

  // Declare int and string that will contain sub count in the form of int and String respectively
  int sub_number;
  String sub_string;

  // define a previous sub counter to track change
  prev_sub_number = sub_number;

  // Call function to return sub count
  getsubcount(&sub_number, &sub_string);

  // Calculate to see if sub count went up or down
  int difference = sub_number - prev_sub_number;
  
  // Call function only if sub count has changed or it is the first time running
  if ((difference != 0) || (startup == 1)) {
    printsubcount(difference, sub_string);
  }
  
  // Wait 10 seconds
  delay(TENSECONDS);

}

void getsubcount(int *sub_number, String *sub_string) {
  
  // Connect to ThingSpeak, make request for data
  if (client.connect(servername,80)) {
    client.println(key);
    client.println();
  }

  // Wait for client to become available
  while (!(client.available())) {
  }

  // Buffer for incoming data
  char data;
  
  // Receive and parse data
   if (client.available()) {
    // loop until desired data is received (has '>' before it)
    
    while ((client.read()) != '>') {
    }

    // string that will have sub count in format "XXXXXXXXX" for converting to int
    String subsnocomma;

    // turn data into strings until all desired data is received (has '<' after it)
    data = (client.read());
    while (data != '<') {
      *sub_string += data;
      if (data != ',') {
        subsnocomma += data;
      }
      data = client.read();
    }

    // create sub_number int
    *sub_number = subsnocomma.toInt();
  }

  // read through remaining data until all received
  while (client.available()) {
    client.read();
  }
  
}

void printsubcount(int difference, String sub_string) {

  // remove "Loading..." if first time running
  if (startup == 1) {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("XXXXXXX Subs");
    lcd.setCursor(3,1);
    lcd.print(sub_string);
    playsound(UP);
    startup = 0;
    return;
  }

  // if gained subscriber(s), print "+X" for X amount gained
  // if lost subscriber(s), print "-X" for X amount lost
  if (difference > 0) {
    lcd.setCursor(12,1);
    lcd.print("+");
    lcd.setCursor(13,1);
    lcd.print(difference);
    playsound(UP);
  } else {
    lcd.setCursor(12,1);
    lcd.print(difference);
    playsound(DOWN);
  }
  
  // update subcount on LCD
  lcd.setCursor(3,1);
  lcd.print(sub_string);
  
}

void playsound(int sound) {

  // if subs increase, play happy sound
  // if subs decrease, play sad sound :(
  if (sound == UP) {
    tone(speaker,B6,short_time);
    delay(short_time);
    tone(speaker,E7,long_time);
  } else {
    tone(speaker,E7,short_time);
    delay(short_time);
    tone(speaker,B6,long_time);
  }
  
}
