#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// pins:
const int thermistorPin = A0; 
const int fanPin = 9;
const int heatingPin = 10;
// thermistor const
const double R0 = 10000; //25C
const double T0 = 25 + 273.15; // Kelvin
const double beta = 3950; // thermistor beta
// desired (can be implemented by poti)
const double desiredTemp = 37.0;
//display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() {
  Serial.begin(9600);
  pinMode(fanPin, OUTPUT);
  pinMode(heatingPin, OUTPUT);
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}
void loop() {
  int sensorValue = analogRead(thermistorPin);
  //thermistor
  double R = 1023.0 / sensorValue - 1.0; //R=inverse of V/Vref-1
  R = R0 * R;
  //Steinhart-Hart
  double T = 1.0 / (1.0/T0 + log(R/R0)/beta); // Calculate in Kelvin
  T = T - 273.15; // Convert to Celsius
  //heating element
  if (T < desiredTemp) {
    digitalWrite(heatingPin, HIGH);
  } else {
    digitalWrite(heatingPin, LOW);
  }
  // fan
  int fanSpeed = map(abs(desiredTemp - T), 0, 10, 255, 0);
  fanSpeed = constrain(fanSpeed, 0, 255); //PWM
  analogWrite(fanPin, fanSpeed);
  //heat-resistors
  String heatingStatus = digitalRead(heatingPin) == HIGH ? "ON" : "OFF";
  int estimatedRPM = (int)(fanSpeed / 255.0 * 3000); //max prm based on fan - change for 100% or as desired (max)
  //OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Temp: ");
  display.print(T);
  display.println(" C");
  display.setCursor(0, 15);
  display.print("Heat: ");
  display.println(heatingStatus);
  display.setCursor(0, 30);
  display.print("Fan: ");
  display.print(estimatedRPM);
  display.println(" RPM");
  display.display();
  delay(1000); // can be longer
}
