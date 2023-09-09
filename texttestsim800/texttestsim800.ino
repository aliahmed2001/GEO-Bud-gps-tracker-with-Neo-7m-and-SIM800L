#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #3 & #2
TinyGPSPlus gps;
SoftwareSerial ss(4, 3);
int i;
char latitude[12];
char longitude[12];
String textMessage;
char Received_SMS; 
short tf=-1;

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  ss.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
}

void loop()
{

latlong();
}

void latlong(){
  ss.listen();
  while(ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      dtostrf(gps.location.lat(), 1, 6, latitude);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
      dtostrf(gps.location.lng(), 1, 6, longitude);
      delay(500);
      SendSMS();
      while (1) {
      }
    }
  }
}


void SendSMS()
{
  mySerial.listen();
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  mySerial.print("Latitude= ");
  updateSerial();
  mySerial.println(latitude); //text content
  updateSerial();
  mySerial.print("Longitude= ");
  updateSerial();
  mySerial.println(longitude); //text content
  updateSerial();
  mySerial.write(26);
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(mySerial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}



void ReceiveMode(){//Set the SIM800L Receive mode
    mySerial.listen();
    mySerial.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
    updateSerial();
    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
       while(textMessage == NULL){
    textMessage = mySerial.readString();
    delay(100);
    Serial.print(textMessage);
  tf=textMessage.indexOf("DHT");   //And this is why we changed from char to String, it's to be able to use this function "indexOf    
      if (tf != -1){
        SendSMS();
        tf=-1;
      }
} 
}

void checkSMS (){
   mySerial.listen();
   while(textMessage == NULL){
    textMessage = mySerial.readString();
    delay(100);
    Serial.print(textMessage);
  tf=textMessage.indexOf("DHT");   //And this is why we changed from char to String, it's to be able to use this function "indexOf    
      if (tf != -1){
        SendSMS();
        tf=-1;
      }
}
}
