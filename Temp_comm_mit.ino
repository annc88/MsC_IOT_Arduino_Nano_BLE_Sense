

//Reads Temp/Humidity from Nano33Sense BLE 1/5/21

#include <ArduinoBLE.h>
#include <Arduino_HS300x.h>
 
BLEService environmentService("00001101-0000-1000-8000-00805f9b34fb");
BLEUnsignedShortCharacteristic TempLevelChar ("00002101-0000-1000-8000-00805f9b34fb", BLERead | BLENotify);
BLEUnsignedShortCharacteristic HumLevelChar  ("00002a6f-0000-1000-8000-00805f9b34fb", BLERead | BLENotify);


int temperature;
short humidity;

const float CALIBRATION_FACTOR = -4; // Temperature calibration factor (celcius)

int ledState = LOW;

/*********************************************************************/
void setup() 
/*********************************************************************/
{
  Serial.begin(115200);
  // wait for the serial port display to be activated
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  if (!BLE.begin()) 
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
  
  if (!HS300x.begin()) 
  {
    Serial.println("Failed to initialize HUM/TEMP sensor!");
  }


  BLE.setLocalName("ArduinoTempSense");
  BLE.setAdvertisedService(environmentService);
  environmentService.addCharacteristic(TempLevelChar);
  environmentService.addCharacteristic(HumLevelChar);
  BLE.addService(environmentService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}
/*********************************************************************/

/*********************************************************************/
void loop() 
/*********************************************************************/
{
 
  BLEDevice central = BLE.central();

  if (central) 
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) 
    {
      
      // Read the HS300x temperature and humidity
      temperature = getTemperature(CALIBRATION_FACTOR);
      humidity    = getHumidity();
      
      Serial.print(" Temp: ");
      Serial.print(temperature);
      Serial.print(" Hum: ");
      Serial.println(humidity);

      TempLevelChar.writeValue(temperature);
      HumLevelChar.writeValue(humidity);
    
      Serial.println("Waiting...");

      BLE.poll();
      delay(100);
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}
/*********************************************************************/

/*********************************************************************/
unsigned short getTemperature(float calibration) 
/*********************************************************************/
{
  // Get calibrated temperature as signed 16-bit int for BLE characteristic
  // convert from C to F
  //return (int) (((HTS.readTemperature() + calibration) *9.0/5.0+32.0) * 100);
  return (unsigned short) (HS300x.readTemperature() * 10);
}
/*********************************************************************/

/*********************************************************************/
unsigned short getHumidity() 
/*********************************************************************/
{
  // Get humidity as unsigned 16-bit int for BLE characteristic
  return (unsigned short) (HS300x.readHumidity() * 10);
}
/*********************************************************************/
