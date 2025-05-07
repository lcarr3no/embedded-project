#include <dht_nonblocking.h>
#include <Wire.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal.h>

#define DHT_SENSOR_TYPE DHT_TYPE_11
#define DHT_SENSOR_PIN 2
#define PHOTO_PIN A8
#define WATER_PIN A12
#define THRESHOLD 400


Adafruit_BMP280 bme; //BMP280 sensor
LiquidCrystal lcd(12, 11, 3, 4 , 5 ,6 , 7, 8 ,9 , 10); //LCD Display
int readingDelay = 3000; 
bool metric = true; //used to determine unit system to use
int metric_button = 31;

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit BMP280 Test"); 
  pinMode(31, INPUT_PULLUP);

  //Checks to see that the BMP280 is working
  if(!bme.begin(0x76)){
    Serial.println("Could not find valid BMP280 sensor.");
    while(1);
  }

}

DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE); //DHT11 sensor

void loop() {

  //Switches modes if there is a button press by the user
  lcd.print("Switch Mode?");
  delay(2000);
  if(!digitalRead(metric_button)){
    lcd.clear();
    handleModeSwitch(); //Calls helper function to handle the unit system switch
    delay(500); 
  }
  lcd.clear();

  /// Variables that will be needed to print out weather report to LCD
  float temperature = 0.0; //Used by dht11
  float humidity = 0.0; //Used by dht11
  int sensor = analogRead(PHOTO_PIN); //Get value from photoresitor
  int value = analogRead(WATER_PIN); //Get value from water level dectection sensor
  

  //Only ouputs to LCD if we have a new reading alonside the delay of 3000 
  if (reading_available(&temperature, &humidity)) {
    printWeatherReport(temperature, humidity, value, sensor, abs( bme.readAltitude(1011.75)) , bme.readPressure() / 100.00 );
  }

  lcd.clear(); //Clear the screen after the weather report has finished displaying
}

/*
* Helper fucntion that switches the mode from metric to imperial & vice versa.
* Sets the lcd to alert the user of the unit system switch
*/
void handleModeSwitch() {
  metric = !metric;
  lcd.setCursor(0, 0);
  lcd.print("Mode set:");
  lcd.setCursor(0, 1);
  lcd.print(metric ? "Metric" : "Imperial");
  delay(readingDelay);
  lcd.clear();
}


/*
* Returns true if DHT has a new reading to display to LCD
*/
bool reading_available(float* tempature, float* humidity){
  if (dht_sensor.measure(tempature, humidity)){
    return true;
  }
  return false;
}

//Used to determine if it is day or night outside
bool is_day(int value){
  //True-Day time less than Threshold
  //False- Night time greater than or equal to Threshold
   if(value < THRESHOLD){
    return true;
   }
   return false;
}

/*
* Used to format the data to be outputted to lcd screen
*/
void printWeatherReport(double temp, double humidity, double waterLevel, double brightness, double altitude, double pressure) {

  if (metric == false) {
      temp = (temp * 1.8) + 32;
      altitude = altitude * 3.281;
      pressure = pressure / 68.95;
    }

    //Sets up the lcd for the weather report format
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    String weatherString = "Weather Report:";
    lcd.print(weatherString);

    lcd.setCursor(0, 1);
    weatherString = "Mode: ";
    weatherString += metric ? "Metric" : "Imperial";
    lcd.print(weatherString);
  
    delay(readingDelay);

    // Temperature
    lcd.clear();
    weatherString = "Temperature:";
    lcd.print(weatherString);
    
    lcd.setCursor(0, 1);
    weatherString = temp;
    weatherString += metric ? "C" : "F";
    lcd.print(weatherString);
    delay(readingDelay);

    //Humidity
    lcd.clear();
    weatherString = "Humidity:";
    lcd.print(weatherString);
    lcd.setCursor(0, 1);
    weatherString = humidity;
    weatherString += "%";
    lcd.print(weatherString);
    delay(readingDelay);

    //Altitude
    lcd.clear();
    weatherString = "Altitude:";
    lcd.print(weatherString);
    lcd.setCursor(0, 1);
    weatherString = altitude;
    weatherString += metric ? "m" : "ft";
    lcd.print(weatherString);
    delay(readingDelay);

    //Pressure
    lcd.clear();
    weatherString = "Atm Pressure:";
    lcd.print(weatherString);
    lcd.setCursor(0, 1);
    weatherString = pressure;
    weatherString += metric ? "hPa" : "psi";
    lcd.print(weatherString);
    delay(readingDelay);

    //Light reading
    lcd.clear();
    weatherString = "Daylight:";
    lcd.print(weatherString);
    lcd.setCursor(0, 1);
    weatherString = is_day(brightness) ? "False" : "True";
    lcd.print(weatherString);
    delay(readingDelay);

    //Water level
    lcd.clear();
    weatherString = "Water Reading:";
    lcd.print(weatherString);
    lcd.setCursor(0, 1);
    int waterReading = readWaterLevel(waterLevel);
    if (waterReading == 0) {
      weatherString = "Low";
    }
    else if (waterReading == 1) {
      weatherString = "Moderate";
    }
    else {
      weatherString = "High";
    }
    lcd.print(weatherString);
    delay(readingDelay);
    lcd.clear();

}

/*
* Returns an int from 0-2 based on the water level reading
* from the Water Level Dectection module sensor
*/

int readWaterLevel(double waterLevel) {
  if (waterLevel < 10.0) {
    return 0;
  }
  if (waterLevel < 150.0){
    return 1;
  }
  return 2;
}


