/*
 * ==============================================================================
 * Project: Non-Invasive Glucose Monitoring System Using Phototransistor Sensor
 * Paper  : IEEE SPICSCON 2025 (DOI: 10.1109/SPICSCON69221.2025.11504207)
 * Authors: Dip Muhuri, Sifat Chowdhury, Dip Paul
 * ==============================================================================
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Bluetooth module on Software Serial (D2: RX, D3: TX)
SoftwareSerial BTSerial(2, 3);

const int sensorPin = A0;      // Phototransistor analog output
const int buttonPin = 4;       // Trigger push button
const int maxReadings = 50;    // Number of samples to average

int sensorValue = 0;
bool buttonState = false;
int count = 0;
long totalValue = 0;

void setup() {
    Serial.begin(9600);
    BTSerial.begin(9600);

    pinMode(buttonPin, INPUT_PULLUP);

    // Initialize OLED display at I2C address 0x3C
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED initialization failed!"));
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print("System Ready!");
    display.setCursor(10, 30);
    display.print("Place Finger...");
    display.display();
    delay(1500);
}

void loop() {
    if (count < maxReadings) {
        sensorValue = analogRead(sensorPin);
        buttonState = (digitalRead(buttonPin) == LOW);
        totalValue += sensorValue;

        // Output to Serial Monitor & Bluetooth
        Serial.print("Sample ");
        Serial.print(count + 1);
        Serial.print("/50 - ADC: ");
        Serial.println(sensorValue);

        BTSerial.print("Sample ");
        BTSerial.print(count + 1);
        BTSerial.print("/50 - ADC: ");
        BTSerial.println(sensorValue);

        // Update OLED status
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(10, 10);
        display.print("Acquiring Data...");
        display.setTextSize(2);
        display.setCursor(10, 30);
        display.print(count + 1);
        display.print("/50");
        display.display();

        count++;
        delay(100); // 100ms sampling interval
    } else {
        // Compute mean sensor value
        int averageValue = totalValue / maxReadings;

        Serial.print("Mean Sensor ADC: ");
        Serial.println(averageValue);

        BTSerial.print("Mean Sensor ADC: ");
        BTSerial.println(averageValue);

        // Apply piecewise linear regression calibration
        float predicted_glucose = 0.0;
        if (averageValue > 400) {
            // Non-diabetic cohort calibration line
            predicted_glucose = (0.0115 * averageValue) + 0.9864;
        } else {
            // Diabetic cohort calibration line
            predicted_glucose = (0.0564 * averageValue) - 11.3220;
        }

        if (predicted_glucose < 0) {
            predicted_glucose = 0.0;
        }

        Serial.print("Predicted Glucose: ");
        Serial.print(predicted_glucose, 2);
        Serial.println(" mmol/L");

        BTSerial.print("Predicted Glucose: ");
        BTSerial.print(predicted_glucose, 2);
        BTSerial.println(" mmol/L");

        // Display final diagnosis on OLED
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(10, 5);
        display.print("GLUCOSE LEVEL");
        display.setTextSize(2);
        display.setCursor(10, 25);
        display.print(predicted_glucose, 2);
        display.setTextSize(1);
        display.setCursor(85, 32);
        display.print("mmol/L");
        display.setCursor(10, 50);
        display.print("ADC: ");
        display.print(averageValue);
        display.display();

        while (true); // Stop loop
    }
}
