// include outside libraries
#include "Camera_Exp.h"
#include <WiFi.h>

//------------------------//
char fromArd[18]; //Initialized variable to store recieved data from Arduino
HardwareSerial & commu = Serial; // declare commu as Serial
CAMERA cam;

char ssid[] = "CIE2";      //  your network SSID (name)
char pass[] = "1212312121";   // your network password

WiFiClient client;
//--------------------------------

void setup() {
  commu.begin(115200); // Begin the commu Serial at 9600 Baud
  commu.setTimeout(1000);
  WiFi.begin(ssid,pass);

  //--------Block loop for connecting wifi-----//
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("try to connect wifi........");
  }
  //-------------------------------------------//

  Serial.print(WiFi.localIP());
}

void loop() {
  /*Initialize data if getting trouble on connection*/
  float hum = 0.00;
  float temp = 0.00;
  int light_status = 1;
  int humidifier_status = 1;
  int tray_pos = 1;
  
  /* Reading from Arduino*/
//  commu.readBytes(fromArd,18);    //reading data from Arduino
//  if(*fromArd != '\0'){
  while(commu.available()){
    commu.readBytes(fromArd,18);
    String message = String(fromArd);                      //Change what we get from Arduino to string
    hum = message.substring(0,5).toFloat();                //Cut 0-4 positions of String to be hum
    temp = message.substring(6,11).toFloat();              //Cut 6-10 positions of String to be temp
    light_status = message.substring(12,13).toInt();       //Cut 12 positions of String to be light_status
    humidifier_status = message.substring(14,15).toInt();  //Cut 14 positions of String to be humidifier_status
    tray_pos = message.substring(16,17).toInt();           //Cut 16 positions of String to be tray_pos
  }
  
  //upload data to server
  const uint16_t port = 80;
  const char * host = "www.arbsuwan.com"; // ip or dns
  
  //Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  // This will send the request to the server
//  if (!client.connect(host, port)) {
//        Serial.print("nah");
//        delay(5000);
//        return;
//  }
  client.connect(host, port);
  String url_1 = "/kinaree/upload.php?humidity=";
  String url_2 = "&temperature=";
  String url_3 = "&light=";
  String url_4 = "&humidifier=";
  String url_5 = "&position=";
  String hum_url = String(hum);
  String temp_url = String(temp);
  String light_url = String(light_status);
  String humidifier_url = String(humidifier_status);
  String position_url = String(tray_pos);
  client.print(String("GET ") + url_1 + hum_url + url_2 + temp_url + url_3 + light_url + url_4 + humidifier_url + url_5 + position_url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  client.stop();
  
  //set what to send back to arduino(get from website)
  float set_humid = 68.50;
  float set_temp = 36.90;
  int   set_light  = 1;
  int   set_humidifier = 0;
  int   set_tray =0;  
  
  /* sending data to Arduino*/   
  String msg = String(set_humid)+"/"+String(set_temp)+"/"+String(set_light)+"/"+String(set_humidifier)+"/"+String(set_tray)+"/";
  const char *toArd = msg.c_str();
  commu.write(toArd);
  delay(5000);
}  
//----------------------------//
