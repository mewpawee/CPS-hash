// include outside libraries
#include "Camera_Exp.h"
#include <WiFi.h>
//------------------------//
char fromArd[18]; //Initialized variable to store recieved data from Arduino
HardwareSerial & commu = Serial; // declare commu as Serial
CAMERA cam;

char ssid[] = "CIE2";      //  your network SSID (name)
char pass[] = "1212312121";   // your network password
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
  /*Initialize data if getting trouble on connection*/
  float hum = 0.00;
  float temp = 0.00;
  int light_status = 0;
  int humidifier_status = 0;
  int tray_pos = 0;
  
  /* Reading from Arduino*/
  commu.readBytes(fromArd,18);    //reading data from Arduino
  if(*fromArd != '\0'){
    String message = String(fromArd);                      //Change what we get from Arduino to string
    hum = message.substring(0,5).toFloat();                //Cut 0-4 positions of String to be hum
    temp = message.substring(6,11).toFloat();              //Cut 6-10 positions of String to be temp
    light_status = message.substring(12,13).toInt();       //Cut 12 positions of String to be light_status
    humidifier_status = message.substring(14,15).toInt();  //Cut 14 positions of String to be humidifier_status
    tray_pos = message.substring(16,17).toInt();           //Cut 16 positions of String to be tray_pos
  }
  
  //set what to send back to arduino(get from website)
  float set_humid = 58.00;
  float set_temp = 37.00;
  int   set_light  = 1;
  int   set_humidifier = 0;
  int   set_tray =0;  
  
  /* sending data to Arduino*/   
  String msg = String(set_humid)+"/"+String(set_temp)+"/"+String(set_light)+"/"+String(set_humidifier)+"/"+String(set_tray)+"/";
  const char *toArd = msg.c_str();
  commu.write(toArd);

  /*create it own server*/
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

//----function to capture a picture-----//
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
