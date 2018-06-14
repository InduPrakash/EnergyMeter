//Indu Prakash
//
//Outputs power1[4 bytes], power2[4 bytes], future[2 bytes] over I2C
//I2C address is 0x04
//http://blog.retep.org/2014/02/15/connecting-an-arduino-to-a-raspberry-pi-using-i2c/

/*Pins
  GPIO2 (SDA) <-> Pin4 (SDA)
  GPIO3 (SCL) <-> Pin5 (SCL)

  A0,A1 - power monitoring
  2 - LED
*/

#include <Wire.h>
#include <EmonLib.h>

//http://openenergymonitor.org/emon/buildingblocks/calibration
//CT Ratio / Burden resistance = (100A / 0.05A) / 18 Ohms = 111.1 (for the emonTx V2)
//CT Ratio / Burden resistance = (100A / 0.05A) / 22 Ohms = 90.9 (for the emonTx V3 CT1-3) <---
//CT Ratio / Burden resistance = (100A / 0.05A) / 120 Ohms = 16.67 (for the emonTx V3 CT4)
//CT Ratio / Burden resistance = (100A / 0.05A) / 33 Ohms = 60.6 (for the emonTx Shield)

static const float VOLTAGE = 120;
static const float VOLTAGE_CALIB = 130.0;
static const float VOLTAGE_CALIB_PHASE_SHIFT = 1.7;
static const float CURRENT_CALIBRATION = 90.9;    // (2000 turns / 22 Ohm burden) = 90.9
static const int NUMBER_OF_SAMPLES = 1662;   //From emonTxV3.4
static const int NUMBER_OF_HALF_WAVELENGTHS =  30;
static const int EMONLIB_TIMEOUT =  2000;

static const int CURRENT_PIN1 = A0;
static const int CURRENT_PIN2 = A1;
static const int THERMISTOR_PIN = A2; //Future use
static const int VOLTAGE_PIN = A3; //Future use
static const int LED_PIN = 2;

static const int SLAVE_ADDRESS = 0x04;  //I2C slave address
static const int POWER_CAPTURE_INTERVAL = 2500; //ms between captures

EnergyMonitor emon1, emon2;

bool ACDetection = false;

long power1 = 0;
long power2 = 0;
byte bytes[10]; //Bytes array for sending out through I2C
int bytePos;  //Index position in bytes
unsigned long nextTime = 0;

void setup()
{
  delay(2000);  //Delay for allowing serial monitor to be launched

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.println("Energy Monitor");

  //Calibration
  double vrms = calc_rms(0, 1780) * (VOLTAGE_CALIB * (3.3 / 1024));
  ACDetection = (vrms > 90);
  emon1.current(CURRENT_PIN1, CURRENT_CALIBRATION);
  emon2.current(CURRENT_PIN2, CURRENT_CALIBRATION);
  if (ACDetection) {
    emon1.voltage(0, VOLTAGE_CALIB, VOLTAGE_CALIB_PHASE_SHIFT);
    emon2.voltage(0, VOLTAGE_CALIB, VOLTAGE_CALIB_PHASE_SHIFT);
  }

  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendData);
  Wire.onReceive(receiveData);
  Serial.println("ready");
}

void loop() {
  unsigned long startTime = millis();
  
  if (startTime > nextTime) {
    if (ACDetection) {
      emon1.calcVI(NUMBER_OF_HALF_WAVELENGTHS, EMONLIB_TIMEOUT);
      power1 = emon1.realPower;
      emon2.calcVI(NUMBER_OF_HALF_WAVELENGTHS, EMONLIB_TIMEOUT);
      power2 = emon2.realPower;
    } else {
      double Irms1 = emon1.calcIrms(NUMBER_OF_SAMPLES);
      double Irms2 = emon2.calcIrms(NUMBER_OF_SAMPLES);
      power1 = (long) (Irms1 * VOLTAGE);
      power2 = (long) (Irms2 * VOLTAGE);
    }

    Serial.print(power1);
    Serial.print(',');
    Serial.println(power2);

    blink();
    unsigned long runTime = millis() - startTime;
    nextTime = startTime + POWER_CAPTURE_INTERVAL - runTime;
  }
}

//Read all the input
void receiveData(int byteCount) {
  while (Wire.available()) {
    Wire.read();
  }
}

void sendData() {
  //Unable to gather data and return data in time within the I2C callback, so gather it separately
  //and send it when possible.

  bytePos = 0;
  putLongData(power1);	//4 bytes
  putLongData(power2);	//4 bytes
  putIntData(0);	//2 bytes
  Wire.write(bytes, 10);
}

