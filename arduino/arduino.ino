// include outside libraries
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
//------------------------//
/*
 * mapping
 * DHT          pin 2
 * light        pin 3 (to relay)
 * fan          pin 4
 * fan1         pin5
 * servo        pin 6
 * water sensor pin 7
 * humidifier   pin 8
*/
//------ define some variables(pin)---//
#define DHTTYPE DHT22
#define DHTPIN 2
#define light 3
#define fan 4
#define fan1 5
#define water 7
#define humidifier 8
//---------------------------//

//----------declare some variables------//
Servo myservo;                   // declare myservo as Servo                                                                                                                                         
DHT dht(DHTPIN, DHTTYPE);        // declare dht as DHT at pin 2 type DHT22
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
HardwareSerial & commu = Serial; // declare commu as Serial
//--------------------------------------//

char fromEsp[22]; //Initialized variable to store recieved data from ESPino32

/*Initiate part*/
void setup(){
  commu.begin(9600);          // Begin the commu Serial at 9600 Baud
  dht.begin();                // Begin the dht sensor
  lcd.begin(20,4);            // Initiate the lcd for 16 chars 2 lines, turn on backlight
  pinMode(water, INPUT);      // set water as input
  pinMode(light,OUTPUT);      // set light as output
  pinMode(humidifier,OUTPUT); // set humidifier as output
  pinMode(fan,OUTPUT);        // set fan as input
  pinMode(fan1,OUTPUT);       // set fan1 as input
  myservo.attach(6);          // attach servo to pin 6 
  
  /*greeting*/
  lcd.setCursor(0,0);
  lcd.print("KINNAREE");
  delay(1000);
  lcd.clear();

  /*start fan*/
  digitalWrite(fan, HIGH);
  digitalWrite(fan1, LOW);
}

/*main loop*/
void loop(){
  /* sending part*/
  
  float temp = dht.readTemperature(); // read temperature for dht
  float hum = dht.readHumidity();   // read humidity from dht
  float water_status = digitalRead(water);  // read water from water sensor

  /*mock data*/
  int   fan_di = 1;             //0 in,1 out
  int   fan_status = 2;         //0 off, 1 on
  int   light_status = 3;       //0 off, 1 on
  int   humidifier_status = 4;  //0 off, 1 on
  int   tray_pos = 0;           //0 front 1 back
  String msg = String(hum) + "/" + String(temp) + "/" + String(fan_di) + "/" + String(fan_status) + "/" + String(light_status) + "/" + String(humidifier_status) + "/" + String(tray_pos)+"/";
  const char *toEsp = msg.c_str();
  commu.write(toEsp);
  /* reading part*/
  
  commu.readBytes(fromEsp,22);
  String message = String(fromEsp);
  float set_hum = message.substring(0,5).toFloat();
  float set_temp = message.substring(6,11).toFloat();
  float maxHum = 58.00;
  float maxTemp = 37.00;
  int set_fan_di = message.substring(12,13).toInt();
  int set_fan = message.substring(14,15).toInt(); 
  int set_light = message.substring(16,17).toInt();
  int set_humidifier = message.substring(18,19).toInt();
  int set_tray = message.substring(20,21).toInt();
  
  check_temp(temp,maxTemp);
  /*Start to Control Humidity when temperature is stable*/
  if(abs(temp-maxTemp)<3.00){
     check_humid(hum,maxHum);
    }
    
  egg_position(tray_pos);

  /*show what we get from espino32*/
  commu.println(set_hum);
  commu.println(set_temp);
  commu.println(set_fan_di);
  commu.println(set_fan);
  commu.println(set_light);
  commu.println(set_humidifier);
  commu.println(set_tray);

  /*show on lcd*/
  lcd.setCursor(0,0);
  lcd.print("H: ");
  lcd.print(hum);
  lcd.print(" T: ");
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print("W: ");
  lcd.print(water_status);
  lcd.print(" P: ");
  lcd.print(tray_pos);
  delay(1000);
}


//---------------some functions-----------------//
void egg_position(int pos){
  //pos == 0 for front position and pos ==1 for back
  if(pos == 0){
    myservo.write(60);
  }else if(pos == 1){
    myservo.write(180);
    }
}     
void check_humid(float humid, float maxHum){
    if (humid<= maxHum){
      digitalWrite(humidifier,HIGH);
    }else{
      digitalWrite(humidifier,LOW);
    }
}

void check_temp(float temp, float maxTemp){
  if(temp <= maxTemp){
    digitalWrite(light, HIGH); 
  }else{
    digitalWrite(light, LOW);
  }
}
 
