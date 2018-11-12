#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#define DHTTYPE DHT22
#define DHTPIN 2

#define WATER_SENSOR 7
#define tray 6
#define Solenoid 10
#define fan 4
#define fan1 5
#define light 3
#define DHTTYPE DHT22
                                                                                                                                              
DHT dht(DHTPIN, DHTTYPE);
HardwareSerial & commu = Serial;
char fromEsp[22]; //Initialized variable to store recieved data from ESPino32

void setup(){
  commu.begin(9600); // Begin the commu Serial at 9600 Baud
  dht.begin();
}


void loop(){
  /* sending part*/
  float hum = dht.readTemperature();
  float temp = dht.readHumidity();
  int   fan_di = 1;             //0 in,1 out
  int   fan_status = 2;         //0 off, 1 on
  int   light_status = 3;       //0 off, 1 on
  int   humidifier_status = 4;  //0 off, 1 on
  int   tray_pos = 5;           //0 front 1 back
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
  bool egg_state = true;
  int set_fan_di = message.substring(12,13).toInt();
  int set_fan = message.substring(14,15).toInt(); 
  int set_light = message.substring(16,17).toInt();
  int set_humidifier = message.substring(18,19).toInt();
  int set_tray = message.substring(20,21).toInt();
  check_humid(hum,maxHum);
  check_temp(temp,maxTemp);
  commu.println(set_hum);
  commu.println(set_temp);
  commu.println(set_fan_di);
  commu.println(set_fan);
  commu.println(set_light);
  commu.println(set_humidifier);
  commu.println(set_tray);
  delay(1000);
}
  
void check_humid(float humid, float maxHum){
  if(humid >= maxHum){
    digitalWrite(fan, HIGH);
    digitalWrite(fan1, HIGH);
  }else{
    digitalWrite(fan, LOW);
    digitalWrite(fan1, LOW);
  }
}

void check_temp(float temp, float maxTemp){
  if(temp <= maxTemp){
    digitalWrite(light, HIGH);
  }else{
    digitalWrite(light, LOW);
  }
}
 
