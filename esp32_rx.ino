#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>
#include <LiquidCrystal_I2C.h>
#define pin_D0 25
#define pin_D1 26
#define pin_D2 27
#define pin_D3 14
#define mot_1 13
const char* ssid = "ZTE-apXCDX";
const char* pass = "5gz7346j";
const char* server_name = "http://maker.ifttt.com/trigger/sensor_readings/with/key/cYbKXDtQ72rsAgGgf1_K0mXNemhaQh35fdodol07A0r";

// The TinyGPSPlus object
TinyGPSPlus gps;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(pin_D0, INPUT);
  pinMode(pin_D1, INPUT);
  pinMode(pin_D2, INPUT);
  pinMode(pin_D3, INPUT);
  pinMode(mot_1, OUTPUT);
  digitalWrite(mot_1, LOW);
  WiFi.begin(ssid, pass);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Smart Helmet");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connected!");
  delay(100);
  delay(3000);
}

void loop() {
  if ((digitalRead(pin_D3) == HIGH) && (digitalRead(pin_D2) == HIGH) && (digitalRead(pin_D1) == HIGH) && (digitalRead(pin_D0) == HIGH))//1111 RF Not connected
  {
    lcd.clear();
    lcd.print("Helmet Unlocked");
    lcd.setCursor(0, 1);
    lcd.print("Engine OFF!!");
    delay(1000);
    digitalWrite(mot_1, LOW);
    delay(1000);
  }
  else if ((digitalRead(pin_D3) == HIGH) && (digitalRead(pin_D2) == HIGH) && (digitalRead(pin_D1) == HIGH) && (digitalRead(pin_D0) != HIGH))//1110 //Helemt locked check(OK condition)
  {
    lcd.clear();
    lcd.print("Helmet Locked");
    lcd.setCursor(0, 1);
    lcd.print("Engine ON!!");
    delay(1000);
    digitalWrite(mot_1, HIGH);
    delay(1000);
  }
  else if ((digitalRead(pin_D3) == HIGH) && (digitalRead(pin_D2) == HIGH) && (digitalRead(pin_D1) != HIGH) && (digitalRead(pin_D0) != HIGH))//1100 //Helemt locked vibration detected
  {
    lcd.clear();
    lcd.print("Accident Alert");
        digitalWrite(mot_1, LOW);
    delay(3000);
    while (Serial2.available() > 0)
      if (gps.encode(Serial2.read()))
        displayInfo();
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      lcd.clear();
      lcd.print("No GPS Connected");
      Serial.println(F("No GPS detected: check wiring."));
      while (true)
        ;
    }
  }
  else if ((digitalRead(pin_D3) == HIGH) && (digitalRead(pin_D2) != HIGH) && (digitalRead(pin_D1) != HIGH) && (digitalRead(pin_D0) != HIGH))//1000
  {
    digitalWrite(mot_1, LOW);
    lcd.clear();
    lcd.print("Alcohol Detected");
    lcd.setCursor(0, 1);
    lcd.print("Engine OFF!!");
    delay(1000);
  }
  else if ((digitalRead(pin_D3) == HIGH) && (digitalRead(pin_D2) != HIGH) && (digitalRead(pin_D1) == HIGH) && (digitalRead(pin_D0) != HIGH))//1010
  {
    digitalWrite(mot_1, LOW);
    lcd.clear();
    lcd.print("Alcohol Detected");
    lcd.setCursor(0, 1);
     lcd.print("Engine OFF!!");
    delay(1000);
  }
}

void displayInfo() {
  Serial.print(F("Location: "));
  lcd.clear();
  lcd.setCursor(0, 0);
  //lcd.print("Location:")
  if (gps.location.isValid()) {
    Serial.print("Lat: ");
    lcd.print("La:");
    float l_lat = gps.location.lat();
    lcd.print(l_lat);
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print("Lng: ");
    float l_lon = gps.location.lng();
    //lcd.setCursor(0, 1);
    lcd.print("Lo:");
    lcd.print(l_lon);
    Serial.print(gps.location.lng(), 6);
    Serial.println();
    HTTPClient http;
    http.begin(server_name);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "Longitude:" + String(l_lon) + "&Latitude:" + String(l_lat);// + "&value3=" + String(random(50));
    int httpResponseCode = http.POST(httpRequestData);
    lcd.setCursor(0, 1);
    lcd.print("HTTP:");
    lcd.print(httpResponseCode);
    http.end();
  }

  else {
    lcd.clear();
    lcd.print("Location:Invalid");
    Serial.println(F("INVALID"));
    delay(1000);
  }
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    Serial2.write(Serial.read());  //Forward what Serial received to Software Serial Port
  }
  while (Serial2.available()) {
    Serial.write(Serial2.read());  //Forward what Software Serial received to Serial Port
  }
}
