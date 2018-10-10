/*-----( Import needed libraries )-----*/
#include "DHT.h" // Humidity and Temperature
#include <Wire.h>  // Comes with Arduino IDE
// Get the LCD I2C Library here: 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>
/*--------------------------------------*/

/*------------Define Things-------------*/
#define DHTPIN 2        // Humidity and Temperature on pin 2
#define WATER_SENSOR 7  // Water sensor  on pin 7
#define Solenoid 10     // Solenoid on pin 10
#define fan 4           // fan on pin 4
#define light 13       //  light on pin 13
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE); //Set up DHT
/*--------------------------------------*/

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
/*--------------------------------*/

/*-----( Declare Variables )-----*/
int maxHum = 100;
int minHum = 10;
int maxTemp = 40;
int minTemp = 10;
/*--------------------------------*/


void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);             // Used to type in characters
  pinMode(light, OUTPUT);        // set ledPin as OUTPUT
  pinMode(WATER_SENSOR, INPUT);   // set WATER_SENSOR as INPUT
  pinMode(Solenoid,OUTPUT);       // set Solenoid as OUTPUT
  
  dht.begin();       // Strat dht
  lcd.begin(20,4);   // initialize the lcd for 16 chars 2 lines, turn on backlight

//-------- Greeting Text -----------------
// NOTE: Cursor Position: (CHAR, LINE) start at 0  
  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("CPS-HASH!");
  lcd.setCursor(0,1);
  lcd.print("EGGY-EGG");
  delay(1000);
  lcd.clear();
}/*------------------(end setup )------------------*/


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  float humidity = dht.readHumidity();            // get humidity from dht
  float temp = dht.readTemperature();             // get temp from dht
  float water = digitalRead(WATER_SENSOR);        // get water from WATER_SENSOR


  /*---------Logic Part-----------*/
  if(temp >  maxTemp){ /*temp greater than maximum*/ /*set delay*/
    digitalWrite(fan,HIGH);
  }else{
    digitalWrite(fan,LOW); 
  }
  if(temp < minTemp){ /*temp less than minimum temp*/ /*set delay*/
    digitalWrite(light,HIGH);      
  }else{
    digitalWrite(light,LOW);
  }   
  /*--------------------------------*/



  /*Display Part*/
  //Show humidity in line 1
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  //-----------------------
  //Show Temperature in line 2
  lcd.setCursor(0,1);
  lcd.print("Temperature: ");
  lcd.print(temp);
  //-----------------------
  //Show Water in line 3
  lcd.setCursor(0,2);
  lcd.print("Water: ");
  lcd.print(water);
  delay(1000); //each loop delay for 1 second
}/* --(end main loop )-- */


/* ( THE END ) */

