#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//NRF
RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "00001";

//WaterLevel1Sensor
#define pinWaterlevel2 A0

int lowTide = 50;  //sets threshold for water level sensor
int MedTide = 60;
int HighTide = 120;

int waterlevelReadings = 0;  //Store sensor data in variable value.


void setup() {
  //code for NRF
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  //WaterLevel1Sensor
  Serial.begin(9600);           // opens serial port, sets data rate to 9600 bps
  pinMode(pinWaterlevel2, INPUT);  //sets the Analog pin 0 as INPUT
}
//global variable for waterlevel1
  int sensorRead() {
  waterlevelReadings = analogRead(pinWaterlevel2);
  return waterlevelReadings;
}

void loop() {
  
  //CODE FOR WATER LEVEL
  int elevation = sensorRead();
  Serial.print("Water Level on Sensor 2:");
  Serial.println(elevation);
  delay(3000);

 
  //Code for Sensor 1
  if (elevation >= HighTide) {
    //for water level serial monitor only
    Serial.println("Water Level Sensor 2: Normal");

    //transmitting waterlevel: Level to Accelerometer Receiver
     const char alertNormal[] = "Water: Normal";
    radio.write(&alertNormal, sizeof(alertNormal));
  }

  else if (elevation >= MedTide && elevation <= HighTide) {
    //for water level serial monitor only
    Serial.println("Water Level Sensor 2: Alert Level: Moderate");

    //transmitting waterlevel: Level to Accelerometer Receiver
    const char alertModerate[] = "Water: Moderate";
    radio.write(&alertModerate, sizeof(alertModerate));

  }

  else if (elevation <= lowTide) {
    Serial.println("Water Level Sensor 2:ON HIGH ALERT : DANGER");

    //transmitting waterlevel: Level to Accelerometer Receiver
    const char alertHighAlert[] = "Water: DANGER!";
    radio.write(&alertHighAlert, sizeof(alertHighAlert));
  }
  delay(1000);
}

//SENDER