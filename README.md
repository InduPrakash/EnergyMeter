# EnergyMeter
Raspberry Pi &amp; Arduino based home power usage capture.

## Arduino
Arduino (nano) captures the power measurement using 2 AC current sensor SCT-013-000 100A, one on each power line. The output from the current sensors is fed into a voltage divider using 22R burder resistor (https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/interface-with-arduino). The power values are calculated and spit on the I2C channel every 2.5 seconds. The hard work of power calculations is done using EmonLib library.

The system is hard coded for run for 120V (defined in EnergyMeter.ino).

## Raspberry Pi
My system has been running on the first generation RaspBerry Pi B+ for 4 years now. It hosts the Emoncms website itself along with some other low usage servers. I have not seen any issues and so have not felt compelled to migrate it elsewhere.

Anyway, the process at Pi end is simple - capture I2C data and file it into Emoncms.

* Enable the I2C interface and locate the I2C address (https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c).
* A 4 pin connector (on pins 3,4,5,6) supplies power to arduino as well as provides I2C connection.
* The I2C interface at pi end is done in Node, the data is read every 10 seconds and filed to the emomcms instance.
* The settings for this end are in config.json (copy/rename  config.json.template).
* The power data is filed into the inputs *power1* and *power2* under the input_node.
* Register the node script to run on startup. I am using [PM2](http://pm2.keymetrics.io/) to launch it.
   
   
### Settings

```
{
    "input_node": 1, //The input node id
    "apiKey": "",  //Write API key
    "fileInterval": 10000,  //Data logging interval
    "emonUrl": "http://raspberrypi/emoncms",    //Emoncms url
    "debugLogging": false,
    "i2cAddress": 4    //I2C address    
}
```

## ToDo
* Measure voltage as well.
