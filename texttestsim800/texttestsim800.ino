#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #3 & #2
TinyGPSPlus gps;
SoftwareSerial ss(4, 3);
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
  ReceiveMode();
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
  mySerial.println("AT+CMGS=\"+1XXXXXXXXXX\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
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

//void toSerial(){
//  delay(200);
//  textMessage = mySerial.readString();
//  delay(100);
//  Serial.print(textMessage);
//  tf = textMessage.indexOf("hi");  
//  if (tf != -1){
//        SendSMS();
//        tf = -1; 
//  } 
//}

void readSMS() {
  mySerial.listen();
       //When SIM800L sends something to the Arduino... problably the SMS received... if something else it's not a problem
  Received_SMS=mySerial.read();  //"char Received_SMS" is now containing the full SMS received
  Serial.print(Received_SMS);   //Show it on the serial monitor (optional)     
  textMessage.concat(Received_SMS);    //concatenate "char received_SMS" to RSMS which is "empty"
  tf=textMessage.indexOf("DHT");   //And this is why we changed from char to String, it's to be able to use this function "indexOf    
      if (tf != -1){
        SendSMS();
        tf=-1;
      }
      //toSerial();
}

void ReceiveMode(){//Set the SIM800L Receive mode
    mySerial.listen();
    mySerial.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
    updateSerial();
    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
    textMessage = mySerial.readString();
    delay(100);
    Serial.print(textMessage);
    updateSerial();
    }
