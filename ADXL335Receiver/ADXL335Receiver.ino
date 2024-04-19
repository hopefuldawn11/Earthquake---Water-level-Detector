#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RH_ASK.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // pins for LCD Connection
RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "00001";

#define xPin A3
#define yPin A1
#define zPin A0
const int buzzerPin = 9;

const int samples = 10;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.print("Earthquake &");
  lcd.setCursor(0, 1);
  lcd.print("Tsunami Alert");
  delay(2000);
  lcd.clear();
  lcd.print("Loading...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");

  while (!Serial);
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  pinMode(buzzerPin, OUTPUT);
  delay(3000);
  lcd.clear();
  lcd.print("Device Ready");
}

void loop() {
  int xRaw = 0, yRaw = 0, zRaw = 0;
  for (int i = 0; i < samples; i++) {
    xRaw += analogRead(xPin);
    yRaw += analogRead(yPin);
    zRaw += analogRead(zPin);
  }
  xRaw /= samples;
  yRaw /= samples;
  zRaw /= samples;

  if (radio.available()) {
   
    if (xRaw > 590 && xRaw < 650 || yRaw > 598 && yRaw < 650 || zRaw > 597 && zRaw < 650) {
       
      char alertNormal[32];
      radio.read(alertNormal, sizeof(alertNormal));

      displayAlert("EQStatus: Normal", alertNormal);
      noTone(buzzerPin);


    } else if ((xRaw > 570 && xRaw < 589) || (zRaw > 580 && zRaw < 595)) {
      
      char alertModerate[32];
      radio.read(alertModerate, sizeof(alertModerate));

      displayAlert("Status: Moderate", alertModerate);
      toneBuzzer(1000, 100, 5);
    
    } else {
      char alertHigh[32];
      radio.read(alertHigh, sizeof(alertHigh));
      displayAlert("EQStatus: DANGER!", alertHigh);
      toneBuzzer(5000, 300, 10);
    }
  }

  Serial.print(xRaw);
  Serial.print("\t");
  Serial.print(yRaw);
  Serial.print("\t");
  Serial.print(zRaw);
  Serial.println();
  delay(200);
}




void displayAlert(const char* status, const char* message) {
  lcd.clear();
  lcd.print(status);
  lcd.setCursor(0, 1);
  lcd.print(message);
}

void toneBuzzer(int frequency, int duration, int repeats) {
  for (int i = 0; i < repeats; i++) {
    tone(buzzerPin, frequency);
    delay(duration);
    noTone(buzzerPin);
    delay(duration);
  }
}
