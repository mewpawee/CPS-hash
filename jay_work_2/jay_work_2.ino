/*-----( Import needed libraries )-----*/
#include "DHT.h" // Humidity and Temperature
#include <Wire.h>  // Comes with Arduino IDE
// Get the LCD I2C Library here: 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
/*--------------------------------------*/

/*------------Define Things-------------*/
#define DHTPIN 2            // Humidity and Temperature on pin 2
#define WATER_SENSOR 7      // Water sensor  on pin 7
#define rotate_switch 6      // Rotate switch on pin 6
#define Solenoid 10         // Solenoid on pin 10
#define fan 4               // fan on pin 4
#define fan1 5              // second fan on pin 5
#define light 3             //  light on pin 3
#define DHTTYPE DHT22       // DHT 22  (AM2302), AM2321  
DHT dht(DHTPIN, DHTTYPE);
/*--------------------------------------*/

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
/*--------------------------------*/

/*-----( Declare Variables )-----*/
float maxHum = 58.00;
float maxTemp = 37.00;
bool egg_state = true;
/*---------------------------------*/ /*stepper*/
int in1Pin = 10;
int in2Pin = 11;
int in3Pin = 12;
int in4Pin = 13;
Stepper motor(512, in1Pin, in3Pin, in2Pin, in4Pin);
/*-------------------------*/


void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);             // Used to type in characters
  pinMode(rotate_switch, INPUT);
  pinMode(light, OUTPUT);        // set ledPin as OUTPUT
  pinMode(WATER_SENSOR, INPUT);   // set WATER_SENSOR as INPUT
  pinMode(Solenoid,OUTPUT);       // set Solenoid as OUTPUT
  pinMode(fan, OUTPUT);
  pinMode(fan1, OUTPUT);
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
//---------Stepper speed---------------------
  motor.setSpeed(25);
}/*------------------(end setup )------------------*/

bool rotate_egg(int state)
{   
           
    int steps = 120;
    if(state==false){
       steps = -120;
    }
    motor.step(steps);
    delay(500);
    return !state;
  }
  
void check_humid(float humid,float maxHum){
  if(humid >= maxHum){
    digitalWrite(fan,HIGH);
    digitalWrite(fan1,HIGH);
  }else{
    digitalWrite(fan,LOW);
    digitalWrite(fan1,LOW);
    }
  }  
  
void check_temp(float temp,float maxTemp)
{
  if(temp <= maxTemp ){ /*temp less than minimum temp*/ /*set delay*/
    digitalWrite(light,HIGH);      
  }else{
    digitalWrite(light,LOW);
  }   
   } 
    
void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  float humid = dht.readHumidity();            // get humidity from dht
  float temp = dht.readTemperature();             // get temp from dht
  float water = digitalRead(WATER_SENSOR);        // get water from WATER_SENSOR

 
  /*---------Logic Part-----------*/
  check_temp(temp,maxTemp);
  check_humid(humid,maxHum);
  bool is_switch = digitalRead(rotate_switch);
  if (is_switch==HIGH){
    egg_state = rotate_egg(egg_state);
  }
  /*--------------------------------*/

  /*-----------------------------*/ /*stepper part*/

  /*Display Part*/
  //Show humidity in line 1
  lcd.setCursor(0,0);
  lcd.print("H: ");
  lcd.print(humid);
  //-----------------------
  //Show Temperature in line 2;
  lcd.print(" T: ");
  lcd.print(temp);
  //-----------------------
  //Show Water in line 3
  lcd.setCursor(0,1);
  lcd.print("Water: ");
  lcd.print(water);
  //
  lcd.setCursor(0,2);
  lcd.print("Turn: ");
  lcd.print(egg_state);
  delay(1000); //each loop delay for 1 second
}/* --(end main loop )-- */


/* ( THE END ) */
