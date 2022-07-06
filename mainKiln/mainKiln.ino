//PINS
//
//A0 - Wood Moisture Meter ORANGE
//A1 XXX
//A2 XXX
//A3 XXX
//A4 XXX
//A5 XXX
//
//0  - Dehumidifier Relay
//1  - Light Relay
//2  - Temperature & Humidity Sensor
//3  XXX
//4  XXX
//5  XXX
//6  XXX
//7  XXX
//8  XXX
//9  XXX
//10 XXX
//11 XXX
//12 XXX
//13 XXX


// TEMPERATURE AND HUMIDITY SENSOR
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <moisture_criteria.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

#define LIGHT 8
#define DEHUM 9

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// DISPLAY
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



float temperature = 0;
float humidity = 0;

float moistureDiff = 0;
unsigned long prevMillis = millis();

// WOOD MOISTURE METER
const int sensorPin = A0;  // Analog input pin that senses Vout
int sensorValue = 0;       // sensorPin default value
float Vin = 5;             // Input voltage
float Vout = 0;            // Vout default value
float Rref = 993000;          // Reference resistor's value in ohms (you can give this value in kiloohms or megaohms - the resistance of the tested resistor will be given in the same units)
float R = 0;               // Tested resistors default value



float megaOhms [] = {
  
  0.00001, 
  0.00002, 
  0.00003, 
  0.00004, 
  0.00006, 
  0.00009, 
  0.00014, 
  0.00021, 
  0.00032, 
  0.00047, 
  0.00071, 
  0.00106, 
  0.00159, 
  0.00238, 
  0.00356, 
  0.00533, 
  0.00798, 
  0.01196, 
  0.01791, 
  0.02683, 
  0.04018, 
  0.06018, 
  0.09014, 
  0.13501, 
  0.20222, 
  0.5, 
  0.63, 
  0.8, 
  0.95, 
  1.45, 
  2.09, 
  3.02, 
  4.6, 
  7.3, 
  11.3, 
  18.2, 
  32, 
  63, 
  125, 
  265, 
  630, 
  1590, 
  4790, 
  14400, 
  40000, 
  120000
};

int nLevels = 46;

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;


  // LIGHTS AND DEHUMIDIFIER

  digitalWrite(LIGHT, LOW);
  digitalWrite(DEHUM, LOW);
  pinMode(LIGHT, OUTPUT);
  pinMode(DEHUM, OUTPUT);

  delay(500);
  
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("SETUP");
}

void loop() {
  // GETTING TEMPERATURE AND HUMIDITY
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
//    Serial.print(F("Temperature: "));
//    Serial.print((event.temperature * 1.8) + 32);
//    Serial.println(F("°F"));
    temperature = (event.temperature * 1.8) + 32;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
//    Serial.print(F("Humidity: "));
//    Serial.print(event.relative_humidity);
//    Serial.println(F("%"));
    humidity = event.relative_humidity;
  }

  // GETTING WOOD MOISTURE CONTENT
  sensorValue = analogRead(sensorPin);  // Read Vout on analog input pin A0 (Arduino can sense from 0-1023, 1023 is 5V)
  Vout = (Vin * sensorValue) / 1023;    // Convert Vout to volts
  R = Rref * (1 / ((Vin / Vout) - 1)) / 1000000;  // Formula to calculate tested resistor's value
  Serial.print("R: ");                  
  Serial.println(R);                    // Give calculated resistance in Serial Monitor
  Serial.println(Rref * (1 / ((Vin / Vout) - 1)));
  
  if(temperature > 80) {
    double temp = (temperature - 32) * (5.0 / 9.0); // Convert to celcius
    double tempDiff = temp - ((80 - 32) * (5.0 / 9.0)); // Take difference over 80 fahrenheit
    double tempDiffMult = tempDiff / 10; // Divide by 10
    R = (0.5 * R) / tempDiffMult; // Calculate accurate resistance
    Serial.print("tempDiffMult: ");
    Serial.println(tempDiffMult);
  }
  
  float closestReading = getMoisturePct(megaOhms, nLevels, R);
  float moisturePct = moistureLevel[search(megaOhms, nLevels, closestReading)];
  Serial.print("Closest Reading: ");
  Serial.println(closestReading);
  Serial.print("Approx. Moisture Content: ");
  Serial.println(moisturePct);
  

  if(moisturePct >= 40) { // Moisture content > 40%
    Serial.println(">40");
    if(temperature < 110 && digitalRead(LIGHT == LOW)) { // If too cold and lights off, turn them on
      digitalWrite(LIGHT, HIGH);
      Serial.println("Turning lights on");
    }
    if(temperature >= 110 && digitalRead(LIGHT == HIGH)) { // If too hot and lights on, turn them off
      digitalWrite(LIGHT, LOW);
      Serial.println("Turning lights off");
    }
    if(humidity > 90 && digitalRead(DEHUM == LOW)) { // If too humid and dehumidifier off, turn it on
      digitalWrite(DEHUM, HIGH);
      Serial.println("Turning dehumidifier on");
    }
    if(humidity <= 90 && digitalRead(DEHUM == HIGH)) { // If not humid enough and dehumidifier on, turn it off
      digitalWrite(DEHUM, LOW);
      Serial.println("Turning dehumidifier off");
    }
  } else if(moisturePct >= 30) { // Moisture content > 30%
    Serial.println(">30");
    if(temperature < 110 && digitalRead(LIGHT == LOW)) { // If too cold and lights off, turn them on
      digitalWrite(LIGHT, HIGH);
      Serial.println("Turning lights on");
    }
    if(temperature >= 110 && digitalRead(LIGHT == HIGH)) { // If too hot and lights on, turn them off
      digitalWrite(LIGHT, LOW);
      Serial.println("Turning lights off");
    }
    if(humidity > 75 && digitalRead(DEHUM == LOW)) { // If too humid and dehumidifier off, turn it on
      digitalWrite(DEHUM, HIGH);
      Serial.println("Turning dehumidifier on");
    }
    if(humidity <= 75 && digitalRead(DEHUM == HIGH)) { // If not humid enough and dehumidifier on, turn it off
      digitalWrite(DEHUM, LOW);
      Serial.println("Turning dehumidifier off");
    }
//    if(temperature < 40) {
//      digitalWrite(1, LOW);
//      Serial.println("Turning lights on");
//    } else if(temperature >= 40) {
//      digitalWrite(1, LOW);
//      Serial.println("Turning lights off");
//    }
//    if(humidity > 75.00) {
//      digitalWrite(0, LOW);
//      Serial.println("Turning dehumidifier on");
//    } else if(humidity <= 75.00) {
//      digitalWrite(0, LOW);
//      Serial.println("Turning dehumidifier off");
//    }
  } else if(moisturePct >= 25) { // Moisture content > 25%
    Serial.println(">25");
    if(temperature < 120 && digitalRead(LIGHT == LOW)) {
      digitalWrite(LIGHT, HIGH);
      Serial.println("Turning lights on");
    }
    if(temperature >= 120 && digitalRead(LIGHT == HIGH)) {
      digitalWrite(LIGHT, LOW);
      Serial.println("Turning lights off");
    }
    if(humidity > 40 && digitalRead(DEHUM == LOW)) {
      digitalWrite(DEHUM, HIGH);
      Serial.println("Turning dehumidifier on");
    }
    if(humidity <= 40 && digitalRead(DEHUM == HIGH)) {
      digitalWrite(DEHUM, LOW);
      Serial.println("Turning dehumidifier off");
    }
  } else if(moisturePct >= 20) { // Moisture content > 20%
    Serial.println(">20");
    if(temperature < 130 && digitalRead(LIGHT == LOW)) {
      digitalWrite(LIGHT, HIGH);
      Serial.println("Turning lights on");
    }
    if(temperature >= 130 && digitalRead(LIGHT == HIGH)) {
      digitalWrite(LIGHT, LOW);
      Serial.println("Turning lights off");
    }
    if(humidity > 25 && digitalRead(DEHUM == LOW)) {
      digitalWrite(DEHUM, HIGH);
      Serial.println("Turning dehumidifier on");
    }
    if(humidity <= 25 && digitalRead(DEHUM == HIGH)) {
      digitalWrite(DEHUM, LOW);
      Serial.println("Turning dehumidifier off");
    }
  } else if(moisturePct >= 15) { // Moisture content > 15%
    Serial.println(">15");
    if(temperature < 140 && digitalRead(LIGHT == LOW)) {
      digitalWrite(LIGHT, HIGH);
      Serial.println("Turning lights on");
    }
    if(temperature >= 140 && digitalRead(LIGHT == HIGH)) {
      digitalWrite(LIGHT, LOW);
      Serial.println("Turning lights off");
    }
    if(humidity > 15 && digitalRead(DEHUM == LOW)) {
      digitalWrite(DEHUM, HIGH);
      Serial.println("Turning dehumidifier on");
    }
    if(humidity <= 15 && digitalRead(DEHUM == HIGH)) {
      digitalWrite(DEHUM, LOW);
      Serial.println("Turning dehumidifier off");
    }
  } else { // Moisture content below 15%
    Serial.println("<15");
    if(temperature < 160 && digitalRead(LIGHT == LOW)) {
      digitalWrite(LIGHT, HIGH);
      Serial.println("Turning lights on");
    }
    if(temperature >= 160 && digitalRead(LIGHT == HIGH)) {
      digitalWrite(LIGHT, LOW);
      Serial.println("Turning lights off");
    }
    if(humidity > 20 && digitalRead(DEHUM == LOW)) {
      digitalWrite(DEHUM, HIGH);
      Serial.println("Turning dehumidifier on");
    }
    if(humidity <= 20 && digitalRead(DEHUM == HIGH)) {
      digitalWrite(DEHUM, LOW);
      Serial.println("Turning dehumidifier off");
    }
  }

   // PRINTING TO LCD DISPLAY

  // T:125.40*F
  // 1234567890123456
  // H:50.00% MC:10%
  // 1234567890123456

  lcd.display();
  lcd.clear();
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("*F");
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("%");
  lcd.print(" MC:");
  lcd.print(static_cast<int>(moisturePct));
  lcd.print("%");


  // INTERVAL
  delay(15000); // Delay for 15 seconds

  
}


float getMoisturePct(float arr[], int n, float target) {
  if (target <= arr[0])
    return arr[0];
  if (target >= arr[n - 1])
    return arr[n - 1];

  // Doing binary search
  int i = 0, j = n, mid = 0;
  while (i < j) {
    mid = (i + j) / 2;

    if (arr[mid] == target)
      return arr[mid];

    /* If target is less than array element,
          then search in left */
    if (target < arr[mid]) {

      // If target is greater than previous
      // to mid, return closest of two
      if (mid > 0 && target > arr[mid - 1])
        return getClosest(arr[mid - 1],
                          arr[mid], target);

      /* Repeat for left half */
      j = mid;
    }

    // If target is greater than mid
    else {
      if (mid < n - 1 && target < arr[mid + 1])
        return getClosest(arr[mid],
                          arr[mid + 1], target);
        // update i
        i = mid + 1; 
    }
  }

  // Only single element left after search
  return arr[mid];
}

float getClosest(float val1, float val2, float target) {
  if (target - val1 >= val2 - target)
    return val2;
  else
    return val1;
}

int search(float A[], int nLevels, float value) {
  Serial.print("Arr[0]: ");
  Serial.println(A[0]);
  for(int i = 0; i < nLevels; i++){
    if(A[i] == value){
      return i;
    }
  }
  return 100;
}
