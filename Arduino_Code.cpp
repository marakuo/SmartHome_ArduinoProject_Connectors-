#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <Servo.h>

// DHT Configuration
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Ultrasonic Configuration
int trigPin = 9;
int echoPin = 10;

// Servo Configuration
Servo myservo;
// flame detection 
int flamePin = 3;
int ledPin = 13;
int buzzer = 8;
int flameState = HIGH;
//bluetooth 
char Incoming_value = 0;
int control_led =7;

void setup() {
  Serial.begin(9600);

  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Reading Sensor...");
  display.display();
  delay(2000);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myservo.attach(6);
  myservo.write(0); // closed door as a default 

  // flame 
  pinMode(flamePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // bluetooth
  pinMode(control_led, OUTPUT);   
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
// display the values on the OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Temperature: ");
  display.print(temp);
  display.println(" C");

  display.setCursor(0, 30);
  display.print("Humidity: ");
  display.print(hum);
  display.println(" %");
  display.display();

  // distance measurement 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration / 58;

  Serial.print("Distance: ");
  Serial.println(distance);
// conditions for door opening 
  if (distance < 15) {
    myservo.write(180);  // open the door if a person is detected 
    delay(2000);        
  } else {
    myservo.write(0);   // close the door after
  }

  delay(1000);
  // flame conditions
  flameState = digitalRead(flamePin);

    if (flameState == LOW) {
        Serial.println("Flame Detected!");
        digitalWrite(ledPin, HIGH);
        digitalWrite(buzzer, HIGH);
        myservo.write(180); // open the door for easy escape 
        delay(10);
        digitalWrite(buzzer, LOW);
        delay(10);

    } else {
        Serial.println("No Flame");
        digitalWrite(ledPin, LOW);
        digitalWrite(buzzer, LOW);
    }
    delay(200);

    // bluetooth
    if(Serial.available() > 0)  
  {
    Incoming_value = Serial.read();      
    Serial.print(Incoming_value);        
    Serial.print("\n");        
    if(Incoming_value == '1')             
      digitalWrite(control_led, HIGH);  
    else if(Incoming_value == '0')       
      digitalWrite(control_led, LOW);   
  }              



}

