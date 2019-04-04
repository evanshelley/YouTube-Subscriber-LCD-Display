#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFiNINA.h>

#define UP 1
#define DOWN 0

// Initialise WiFi connection
char ssid[] = "SSID";
char pass[] = "PASSWORD";
int status = WL_IDLE_STATUS;

// IP address for api.thingspeak.com
IPAddress server(18,214,44,70);

// Initialise library
WiFiClient client;

// Initialise LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Piezo on pin 8
const int speaker = 8;

// Function getsubcount handles sending HTML request, receiving and parsing desired data
void getsubcount(int *sub_number, String *sub_string);

// Function printsubcount handles updating the LCD display
void printsubcount(int difference, String sub_string);

// Function playsound plays sound indicating if subcount went up or down, indicated by sound = UP or DOWN respectively
void playsound(int sound);

// variable to tell if running functions for the first time
int startup = 1;

// variable to track previous subscriber count to see if there is increase / decrease
int prev_sub_number;

/*
  Pre much everything above this could go ina  header file, showing what the constants are and what functions are defined.
  Another header could be used for your secret info like WIFI credentials and an error on not finding this credentials file
*/

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
    delay(2500);
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

  // previous sub counter to track change
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
  delay(10000);

}

void getsubcount(int *sub_number, String *sub_string) {
  
  // Connect to ThingSpeak, make request for data
  if (client.connect(server,80)) {
    client.println("GET /apps/thinghttp/send_request?api_key=XXXXXXXXXXXXXXXX");
    client.println();
  }
  
  // Wait 10 seconds for data to be received
  /*
    This is a terrible idea, if your connection halts or is slow then this code dies; you should check for a HTTP response from the api.
    If this is not possible then make the delay a constant in your header file.
  */
  delay(10000);

  // Buffer for incoming data
  char data;
  
  // Receive and parse data
  /*
    If this fails, should add an else conditional to log some info on what happened, always utilise your else conditionals
  */
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
    /*
      If the below function fails it returns zero, be weary of using library functions, if you want to defend agains this case do it after the function call
    */
    *sub_number = subsnocomma.toInt();
  }

  // read through remaining data until complete
  /*
    Do we need to read through rest of the data? Seems like a waste, could you not jsut dump the rest?
  */
  while (client.available()) {
    data = client.read();
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
  /*
    Use constant varaibles for delay times and probably also for tone notes
  */
  if (sound == UP) {
    tone(8,1975,200);
    delay(200);
    tone(8,2637,400);
  } else {
    tone(8,2637,200);
    delay(200);
    tone(8,1975,400);
  }
  
}
