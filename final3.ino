#include <SoftwareSerial.h>  
#include <TinyGPS.h>
#include <Wire.h>
int trigPin = 12;    // Trigger
int echoPin = 13;    // Echo
int buzz=9;
long duration, cm, inches;
int ADXL345 = 0x53;
int X_out, Y_out, Z_out;
TinyGPS gps;
SoftwareSerial SIM800A(10,11);
 
void setup() {
  //Serial Port begin
  Serial.begin (9600);
  SIM800A.begin(9600);
   Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();

  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzz,OUTPUT);
  
  Wire.begin();

  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D); //talk to POWER_CTL Register - 0x2D
  
  Wire.write(8); // Bit D3 High for measuring enable (8dec -> 0000 1000 binary)
  Wire.endTransmission();
  delay(10);
}
 
void loop() {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);

  X_out = ( Wire.read() | Wire.read() << 8);
  Y_out = ( Wire.read() | Wire.read() << 8);
  Z_out = ( Wire.read() | Wire.read() << 8);

  Serial.print(X_out);
  Serial.print(" ");
  Serial.print(Y_out);
  Serial.print(" ");
  Serial.println(Z_out);
  delay(100);
   

  if (SIM800A.available()>0)  
   Serial.write(SIM800A.read()); 
   for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
   if(inches==1&&X_out>=150&&X_out<=180)
   {
   digitalWrite(buzz,HIGH);
   }
   else{
    digitalWrite(buzz,LOW);
   }
   
  if(inches==1&&X_out>=150&&X_out<=180)
  {
    
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
  Serial.println ("Sending Message");   
  SIM800A.println("AT+CMGF=1");  //Sets the GSM Module in Text Mode  
  delay(1000);  
  Serial.println ("Set SMS Number");  
  SIM800A.println("AT+CMGS=\"+918179108138\"\r"); //Type Your Mobile number to send message  
  delay(1000);  
  Serial.println ("Set SMS Content");  
  SIM800A.println("ACCIDENT ALERT");// Messsage content  
   SIM800A.print("http://maps.google.com/maps?q=loc:");
  delay(100); 
  SIM800A.print(flat==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(",");
    SIM800A.print(flon==TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    delay(200); 
  Serial.println ("Finish");  
  SIM800A.println((char)26);// ASCII code of CTRL+Z  
  delay(1000);  
  Serial.println ("Message has been sent ->SMS pavani"); 
     Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  
    
  delay(250);
}
gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}

