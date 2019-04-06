#define UP 1
#define DOWN 0
#define B6 1975
#define E7 2637
#define short_time 200
#define long_time 400
#define TENSECONDS 10000
#define THREESECONDS 3000

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

// WiFi connection status
int status = WL_IDLE_STATUS;

// Domain name for ThingSpeak
char servername[] = "api.thingspeak.com";

// Initialise library
WiFiClient client;

// variable to tell if running functions for the first time
int startup = 1;

// variable to track previous subscriber count to see if there is increase / decrease
int prev_sub_number;
