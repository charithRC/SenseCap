//#define temp01_h

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include "addons/RTDBHelper.h"

//gps
#include <TinyGPS++.h> // library for GPS module
#include <SoftwareSerial.h>

TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(4, 5); // The serial connection to the GPS device
//gps

#define WIFI_SSID "virus"
#define WIFI_PASSWORD "adamapple"

#define DATABASE_URL "esp8266-c1e5d-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "1btcmF5aSgjSIvRpI4kdB3DV4ojYKHJ7KnSy1Bod"

int pingPin = D5; // Trigger Pin of Ultrasonic Sensor
int echoPin = D6; // Echo Pin of Ultrasonic Sensor
int pingPin2 = D7; // Trigger Pin of Ultrasonic Sensor
int echoPin2 = D8; // Echo Pin of Ultrasonic Sensor
int buzzer01 = D3;
int buzzer02 = D0;
int mapval;
int count = 0,newc=0;
int pushbutton=D4;

//String bvalue;
String batteryTemp="88",mobile;

//gps
float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;
WiFiServer server(80);
//gps

FirebaseData fbdo; /*Define the Firebase Data object */
FirebaseAuth auth; /*Define the FirebaseAuth data for authentication data */
FirebaseConfig config; /*Define the FirebaseConfig data for config data */

void setup()
{
  //gps
    Serial.begin(19200);
    ss.begin(9600);
    pinMode(buzzer01,OUTPUT);
    pinMode(buzzer02,OUTPUT);
    
    pinMode(pushbutton,INPUT_PULLUP);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    
    Serial.println("WiFi connected");
    server.begin();
    Serial.println("Server started");
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth); /* Initialize the library with the Firebase authen and config */

}

void loop()
{
    vibratingCalc();
    vibratingCalc2();
    
    if(newc==200){
    latlong();
    digitalWrite(buzzer01,LOW);
    digitalWrite(buzzer02,LOW);

    String batteryTemp = readBattery();
    Serial.println("remaining battery percentage = "+batteryTemp);
    firebaseUpload("/battrey",batteryTemp);
    String Button=buttonvalue();
    firebaseUpload("/"+mobile+"/emergency",Button);
    Serial.printf("Get int... %s\n", Firebase.getInt(fbdo, "/value") ? mobile=String(fbdo.intData()).c_str() : fbdo.errorReason().c_str());
    Serial.println("mobile is"+mobile);
    newc=0;  
    firebaseUpload("/"+mobile+"/latitude",lat_str);
    firebaseUpload("/"+mobile+"/longitude",lng_str);
    firebaseUpload("/date",date_str);
    firebaseUpload("/time",time_str);
 }
       
}

void vibratingCalc()
{
    digitalWrite(buzzer01,LOW);
    int temp = calculateDist();
    
    if(temp < 10)
    {
        digitalWrite(buzzer01,HIGH);
        delay(100);
    }
    
    else if(temp >=11 && temp <20)
    {
        digitalWrite(buzzer01,LOW);
        delay(200);
        digitalWrite(buzzer01,HIGH);
        delay(200);
    }
    
    else if(temp >=21 && temp <30)
    {
        digitalWrite(buzzer01,LOW);
        delay(300);
        digitalWrite(buzzer01,HIGH);
        delay(300);
     }
     
     else if(temp >30)
     {
        digitalWrite(buzzer01,LOW);
     }
    Serial.println("sonar 1");
    newc++;
}
void vibratingCalc2()
{
    digitalWrite(buzzer02,LOW);
    int temp = calculateDist2();
    
    if(temp < 10)
    {
        digitalWrite(buzzer02,HIGH);
        delay(100);
    }
    
    else if(temp >=11 && temp <20)
    {
        digitalWrite(buzzer02,LOW);
        delay(200);
        digitalWrite(buzzer02,HIGH);
        delay(200);
    }
    
    else if(temp >=21 && temp <30)
    {
        digitalWrite(buzzer02,LOW);
        delay(300);
        digitalWrite(buzzer02,HIGH);
        delay(300);
     }
     
     else if(temp >30)
     {
        digitalWrite(buzzer02,LOW);
     }
    Serial.println("sonar 2");
    newc++;
}
void firebaseUpload(String path,String values){
    if(values!=""){
    Serial.printf("Set string... %s\n", Firebase.setString(fbdo, path,values) ? "ok" : fbdo.errorReason().c_str());
    }
    
 }
String readBattery()
{
    uint8_t percentage = 100;
    float voltage = analogRead(A0); // 4096.0 * 7.23;//4.24;    // Wemos / Lolin D1 Mini 100K series resistor added
    mapval = map(voltage,0,1024 ,0,100);
    return String(mapval)+"%";
}

int calculateDist()
{
   long duration, inches, cm;
  
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
   delay(100);
   return cm;
}

int calculateDist2()
{
   long duration, inches, cm;
  
   pinMode(pingPin2, OUTPUT);
   digitalWrite(pingPin2, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin2, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin2, LOW);
   pinMode(echoPin2, INPUT);
   
   duration = pulseIn(echoPin2, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
   delay(100);
   return cm;
}

long microsecondsToInches(long microseconds) 
{
    return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) 
{
   return microseconds / 29 / 2;
}

//gps
void latlong()
{

    while (ss.available() > 0) //while data is available
    if (gps.encode(ss.read())) //read gps data
    {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude , 6); //longitude location is stored in a string
        Serial.println("lat="+lat_str);
        Serial.println("long="+lng_str);
      }
      if (gps.date.isValid()) //check whether gps date is valid
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date);// values of date,month and year are stored in a string
        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month); // values of date,month and year are stored in a string
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year); // values of date,month and year are stored in a string
        Serial.println("date="+date_str);
      }
      
      if (gps.time.isValid())  //check whether gps time is valid
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30); // converting to IST
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;   // converting to IST
        if (hour >= 12)  // checking whether AM or PM
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second); //values of hour,minute and time are stored in a string
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }
    delay(100);
}
}
String buttonvalue()
{
    int bvalue=digitalRead(pushbutton);
    Serial.println("button value is"+bvalue);
    if(bvalue==0){
          Serial.println("EMERGENCY REQUIRED !..........");
          //firebaseUpload("/emergency",bvalue);
          return String(bvalue);
    }
    else{
        Serial.println("EMERGENCY NOT REQUIRED !........ ");
        return String(bvalue);
    }
}
