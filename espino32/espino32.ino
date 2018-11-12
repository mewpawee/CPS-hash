#include "Camera_Exp.h"
#include <WiFi.h>

char fromArd[22]; //Initialized variable to store recieved data from Arduino
HardwareSerial & commu = Serial;

//------------new----------------
CAMERA cam;
char ssid[] = "mine";      //  your network SSID (name)
char pass[] = "12345678";   // your network password
WiFiServer server(80);

String http_header = "HTTP/1.1 200 OK\r\n";
String http_stream = "Content-type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n\r\n";
String http_jpg = "Content-type: image/jpg\r\n\r\n";
String http_boundary = "--123456789000000000000987654321\r\n";
WiFiClient client;
//---------------------------------

void setup() {
  commu.begin(9600); // Begin the commu Serial at 9600 Baud
  WiFi.begin(ssid,pass);

  //--------Block loop for connecting wifi-----//
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("try to connect wifi........");
  }
  //-------------------------------------------//
  server.begin();
  cam.setFrameSize(CAMERA_FS_VGA);
  esp_err_t err = cam.init();
  Serial.print(WiFi.localIP());
}

void loop() {
  //Rx and change it to variable
  commu.readBytes(fromArd,22);
  String message = String(fromArd);
  float hum = message.substring(0,5).toFloat();
  float temp = message.substring(6,11).toFloat();
  int fan_di = message.substring(12,13).toInt();
  int fan_status = message.substring(14,15).toInt(); 
  int light_status = message.substring(16,17).toInt();
  int humidifier_status = message.substring(18,19).toInt();
  int tray_pos = message.substring(20,21).toInt();

  float set_humid = 57.50;
  float set_temp = 37.50;
  int   set_fan_di = 6;
  int   set_fan = 7;
  int   set_light  = 8;
  int   set_humidifier = 9;
  int   set_tray =0;  
  
  String msg = String(set_humid)+"/"+String(set_temp)+"/"+String(set_fan_di)+"/"+String(set_fan)+"/"+String(set_light)+"/"+String(set_humidifier)+"/"+String(set_tray)+"/";
  const char *toArd = msg.c_str();
  commu.write(toArd);
// Print variables  
//  commu.println(hum);
//  commu.println(temp);
//  commu.println(fan_di);
//  commu.println(fan_status);
//  commu.println(light_status);
//  commu.println(humidifier_status);
//  commu.println(tray_pos);

//----------new-----------//
  String httpreq;
  client = server.available();
  if (client)
  {
    Serial.println("New Client.");
    String httpreq = "";
    while (client.connected())
    {
      if (client.available())
      {
        String httpreq_line = client.readStringUntil('\n');
        httpreq += httpreq_line;
        if (httpreq_line == "\r")
        {
          if (httpreq.indexOf("GET /stream") != -1)
          {
                Serial.println("Stream");
                stream();
          }
          if (httpreq.indexOf("GET /capture") != -1)
          {
                Serial.println("Capture");
                capture();
          }
          
          httpreq = "";
          client.stop();
        }

      }
    }

  }
  delay(1000);
}  
//----------------------------//

//----function to capture-----//
void capture()
{
   esp_err_t err;
    err = cam.capture();
    if (err != ESP_OK)
    {
      Serial.println("Camera capture failed with error =" + String(err));
      return;
    }
   client.print(http_header);
   client.print(http_jpg);
   client.write(cam.getfb(),cam.getSize());
   
}
//----------------------------//
