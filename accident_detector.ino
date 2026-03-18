#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <MPU6050.h>

LiquidCrystal_I2C lcd(0x27,16,2);
TinyGPSPlus gps;
MPU6050 mpu;

SoftwareSerial gpsSerial(4,3);
SoftwareSerial gsmSerial(7,8);

int buzzer = 9;
int toggleSwitch = 10;

int16_t ax, ay, az;

float threshold = 2.5;

String phoneNumber = "+91XXXXXXXXXX";

void setup()
{

Serial.begin(9600);
gpsSerial.begin(9600);
gsmSerial.begin(9600);

Wire.begin();
mpu.initialize();

lcd.init();
lcd.backlight();

pinMode(buzzer,OUTPUT);
pinMode(toggleSwitch,INPUT_PULLUP);

lcd.setCursor(0,0);
lcd.print("AUTOMATIC ACCIDENT");
lcd.setCursor(0,1);
lcd.print("DETECTION");
delay(3000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("By PRIYANKA");
lcd.setCursor(0,1);
lcd.print("SHAGUN DEEPIKA");
delay(3000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("ANKUR");
delay(2000);

lcd.clear();

Serial.println("System Ready");

}

void loop()
{

while(gpsSerial.available())
{
gps.encode(gpsSerial.read());
}

mpu.getAcceleration(&ax,&ay,&az);

float fax = ax / 16384.0;
float fay = ay / 16384.0;
float faz = az / 16384.0;

float accel = sqrt(fax*fax + fay*fay + faz*faz);

Serial.print("Acceleration: ");
Serial.println(accel);

float latitude = gps.location.lat();
float longitude = gps.location.lng();
float speed = gps.speed.kmph();

Serial.print("Latitude: ");
Serial.println(latitude,6);

Serial.print("Longitude: ");
Serial.println(longitude,6);

Serial.print("Speed: ");
Serial.println(speed);

lcd.setCursor(0,0);
lcd.print("Speed:");
lcd.print(speed);
lcd.print("km/h   ");

delay(1000);

if(accel > threshold)
{
accidentDetected();
}

}

void accidentDetected()
{

digitalWrite(buzzer,HIGH);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("ACCIDENT!");
lcd.setCursor(0,1);
lcd.print("Toggle to stop");

Serial.println("Accident detected!");

int timer = 0;

while(timer < 10)
{

if(digitalRead(toggleSwitch) == LOW)
{

Serial.println("Accident Cancelled");

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Accident");
lcd.setCursor(0,1);
lcd.print("Cancelled");

digitalWrite(buzzer,LOW);

delay(3000);

return;

}

delay(1000);
timer++;

}

float latitude = gps.location.lat();
float longitude = gps.location.lng();
float speed = gps.speed.kmph();

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Lat:");
lcd.print(latitude,4);

lcd.setCursor(0,1);
lcd.print("Lng:");
lcd.print(longitude,4);

delay(3000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Speed:");
lcd.print(speed);
lcd.print("km/h");

Serial.println("Sending SMS...");

sendSMS(latitude,longitude);

digitalWrite(buzzer,LOW);

}

void sendSMS(float lat,float lon)
{

String message = "Accident Detected!\n";
message += "Location:\n";
message += "https://maps.google.com/?q=";
message += String(lat,6);
message += ",";
message += String(lon,6);

gsmSerial.println("AT");
delay(1000);

gsmSerial.println("AT+CMGF=1");
delay(1000);

gsmSerial.print("AT+CMGS=\"");
gsmSerial.print(phoneNumber);
gsmSerial.println("\"");
delay(1000);

gsmSerial.print(message);
delay(1000);

gsmSerial.write(26);

Serial.println("SMS Sent");

}
