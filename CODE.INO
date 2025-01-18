# stopwatchtimer
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Initialize LCD (0x27 is the default I2C address, adjust if needed)
LiquidCrystal_I2C lcd(0x27, 20, 4);
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pin definitions
const int startButtonPin = 2;
const int stopButtonPin = 3;

// Variables for time tracking
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long previousMillis = 0;
bool isRunning = false;
float flowrate = 0.0;
const float VOLUME_LITERS = 0.25; // Volume in liters

// Button states
int startButtonState;
int lastStartButtonState = HIGH;
int stopButtonState;
int lastStopButtonState = HIGH;

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  // Set up buttons with internal pullup resistors
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);
  
  // Initial display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GREENTECHINSTRUMENTS");
  updateDisplay(0);
}

void loop() {
  // Read and display temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  lcd.setCursor(0, 3);
  lcd.print("AT:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C");
  
  lcd.setCursor(10, 3);
  lcd.print("Hum:");
  lcd.print(humidity, 1);
  lcd.print("%");
  
  // Read button states
  startButtonState = digitalRead(startButtonPin);
  stopButtonState = digitalRead(stopButtonPin);
  
  // Check start button press
  if (startButtonState == LOW && lastStartButtonState == HIGH) {
    elapsedTime = 0;
    startTime = millis();
    isRunning = true;
  }
  
  // Check stop button press
  if (stopButtonState == LOW && lastStopButtonState == HIGH) {
    if (isRunning) {
      isRunning = false;
      elapsedTime = millis() - startTime;
    }
  }
  
  // Update time if running
  if (isRunning) {
    elapsedTime = millis() - startTime;
    updateDisplay(elapsedTime);
  }
  
  // Update button states
  lastStartButtonState = startButtonState;
  lastStopButtonState = stopButtonState;
}

void updateDisplay(unsigned long time) {
  // Calculate minutes, seconds, and milliseconds
  unsigned long minutes = (time / 60000) % 100;
  unsigned long seconds = (time / 1000) % 60;
  unsigned long milliseconds = time % 1000;
  
  // Update display on line 2
  lcd.setCursor(0, 1);
  
  // Minutes
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  
  // Seconds
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(":");
  
  // Milliseconds
  if (milliseconds < 100) lcd.print("0");
  if (milliseconds < 10) lcd.print("0");
  lcd.print(milliseconds);
  
  // Calculate and display flow rate
  lcd.setCursor(0, 2);
  lcd.print("FlowRate:");
  
  if (time > 0) {
    // Convert milliseconds to minutes and calculate flow rate
    float timeInMinutes = time / 60000.0;
    flowrate = VOLUME_LITERS / timeInMinutes;
    
    if (flowrate > 999.99) {
      lcd.print("---");  // Show dashes if flow rate is too high
    } else {
      lcd.print(flowrate, 2);  // Display with 2 decimal places
    }
  } else {
    lcd.print("0.00");  // Show zero when not running
  }
  
  lcd.print("LPM ");  // Add space to clear any previous digits
}
