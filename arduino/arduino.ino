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
 * fan1         pin 5
 * servo        pin 6
 * water sensor pin 7
 * humidifier   pin 8 (to relay)
 * ground alike pin 13
*/

//------ define some variables(pin)---//
#define DHTTYPE DHT22
#define DHTPIN 2
#define light 3
#define fan 4
#define fan1 5
#define water 7
#define humidifier 8
#define ground 13
//---------------------------//

//----------declare some variables------//
Servo myservo;                   // declare myservo as Servo                                                                                                                                         
DHT dht(DHTPIN, DHTTYPE);        // declare dht as DHT at pin 2 type DHT22
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
HardwareSerial & commu = Serial; // declare commu as Serial
//--------------------------------------//

//----------declare some global variables------//
  float hum_kg = 0.00;
  float hum_err = 0.00;
  float hum_est = 0.00;
  float temp_kg = 0.00;
  float temp_err = 0.00;
  float temp_est = 0.00;
  float bhum = 0.00;
  float btemp = 0.00;
//--------------------------------------//

char fromEsp[18]; //Initialized variable to store recieved data from ESPino32

/*Initiate part*/
void setup(){
  commu.begin(9600);          // Begin the commu Serial at 9600 Baud
  dht.begin();                // Begin the dht sensor
  lcd.begin(20,4);            // Initiate the lcd for 16 chars 2 lines, turn on backlight
  pinMode(water, INPUT);      // set water as input
  pinMode(light,OUTPUT);      // set light as output
  pinMode(humidifier,OUTPUT); // set humidifier as output
  pinMode(fan,OUTPUT);        // set fan as output
  pinMode(fan1,OUTPUT);       // set fan1 as output
  pinMode(ground,OUTPUT);     // set ground as output
  myservo.attach(6);          // attach servo to pin 6 
  
  /*greeting with "KINNAREE" */
  lcd.setCursor(0,0);
  lcd.print("KINNAREE");
  delay(1000);
  lcd.clear();

  /*turn pin 13 to ground*/
  digitalWrite(ground,LOW);
  
  /*start fan*/
  digitalWrite(fan, HIGH);
  digitalWrite(fan1, LOW);
}  


/*main loop*/
void loop(){
  
  /*get data from sensors*/
  float temp = dht.readTemperature(); // read temperature for dht
  float hum = dht.readHumidity();   // read humidity from dht
  float water_status = digitalRead(water);  // read water from water sensor
  
  /*Initialize setting for incubator, in case lost connect to ESPINO32*/
  float maxHum = 58.00;           //max Humadity
  float maxTemp = 37.00;          //max Temperature
  int   tray_pos = 0;             //Tray position (0 off, 1 on)
  int   light_status = 1;         //light status (0 off, 1 on)
  int   humidifier_status = 1;    //humidifier status
  
  /* Reading from ESP*/
  while(commu.available()){           
     commu.readBytes(fromEsp,18);
      String message = String(fromEsp);                           //Change what we get from ESP to string
      float set_hum = message.substring(0,5).toFloat();           //Cut 0-4 positions of String to be set_hum
      float set_temp = message.substring(6,11).toFloat();         //Cut 6-10 positions of String to be set_temp
      int set_light = message.substring(12,13).toInt();           //Cut 12 position of String to be set_light
      int set_humidifier = message.substring(14,15).toInt();      //Cut 14 position of String to be set_humidifier
      int set_tray = message.substring(16,17).toInt();            //Cut 16 position of String to be set_tray

      /*set variables according to Espino32*/
      light_status = set_light;                
      humidifier_status = set_humidifier;      
      tray_pos = set_tray;                     
      maxHum = set_hum;                         
      maxTemp = set_temp;                       
  }
  
  /*Check if is NAN then use the backup value*/
  if(isnan(temp)||isnan(hum)){
    temp = btemp;
    hum = bhum;
  }else{
    btemp = temp;
    bhum = hum;
  }

  /*use kalman filter*/
  kalman_filter(temp, temp_kg, 5.00, temp_err, temp_est);   //use tool error as 5 percents
  kalman_filter(hum, hum_kg, 5.00, hum_err, hum_est);       //use tool error as 5 percents
  
  /*Control System*/
  check_temp(temp,maxTemp);
  /*Start to Control Humidity when temperature is stable*/
  if(abs(temp-maxTemp)<0.50){
     check_humid(hum,maxHum);
  }else{
     digitalWrite(humidifier,LOW);
    }
  egg_position(tray_pos); //turn egg tray
  light_status = digitalRead(light);                  //get light_status
  humidifier_status = digitalRead(humidifier);        //get humidifier_status
  
  /* sending data to ESPINO32*/   
  String msg = String(hum) + "/" + String(temp) + "/" + String(light_status) + "/" + String(humidifier_status) + "/" + String(tray_pos)+"/";
  const char *toEsp = msg.c_str();
  commu.write(toEsp);
  
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
  lcd.setCursor(0,2);
  lcd.print("L: ");
  lcd.print(light_status);
  lcd.print(" HF: ");
  lcd.print(humidifier_status);
  delay(1000);
}


//---------------some functions-----------------//
/*Kalman Filter, eliminate noise and make it more accurate*/
void kalman_filter(float mea, float KG, float err_mea, float err_est, float EST){
      KG = err_est/(err_est+err_mea);
      EST = EST + KG*(mea-EST);
      err_est = (1-KG)*err_est;
  }
  
/*Turn motor according to egg position*/     
void egg_position(int pos){
  //pos == 0 for front position and pos ==1 for back
  if(pos == 0){
    myservo.write(60);
  }else if(pos == 1){
    myservo.write(180);
    }
}     

/*Check if we should turn the humidifier on or not*/
void check_humid(float humid, float maxHum){
    if (humid<= maxHum){
      digitalWrite(humidifier,HIGH);
    }else{
      digitalWrite(humidifier,LOW);
    }
}

/*Check if we should turn ther temperature on or not*/
void check_temp(float temp, float maxTemp){
  if(temp <= maxTemp){
    digitalWrite(light, HIGH); 
  }else{
    digitalWrite(light, LOW);
  }
}
 
