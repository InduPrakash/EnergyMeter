# EnergyMeter
Raspberry Pi &amp; Arduino based home power usage capture.

## Arduino
Arduino (nano) captures the power measurement using 2 AC current sensor SCT-013-000 100A, one on each power line. The output from SCT is fed into a voltage divider using 22R burder resistor (https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/interface-with-arduino). The power values are calculated and spit on the I2C channel every 2.5 seconds. The hard work of power calculations is done using EmonLib library.

## Raspberry Pi
My system has been running on the first generation RaspBerry Pi B+ for 4 years now. It hosts the Emoncms website itself along with some other low usage servers. I have not seen any issues and so have not felt compelled to migrate it elsewhere.

Anyway, the process at Pi end is simple - capture I2C data and file it into Emoncms.

* Enable the I2C interface and locate the I2C address (https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c).
* A 4 pin connector (on pins 3,4,5,6) supplies power to arduino as well as provides I2C connection.
* The I2C interface at pi end is done in Node, the data is read every 10 seconds and filed to the emomcms instance.
* The settings for this end are in config.json (copy/rename  config.json.template).
* The power data is filed into the inputs *power1* and *power2* under the input_node.
* The node script itself is registered as a service:
    * Edit *energymeter* and update dir to point at the folder where energymeter.js is present.
    ```
    dir="/home/U_S_E_R/EnergyMeter/pi"
    ```
    * Make the file executable
    ```
    chmod +x energymeter
    ```
    * Copy it into /etc/init.d and change its owner to root
    ```
    sudo cp energymeter /etc/init.d
    sudo chown root.root /etc/init.d/energymeter
    ```
    * Now you should be able to start the service which would start filing data.
    ```
    sudo /etc/init.d/energymeter start
    ```
    The log and error files are in /var/log.

### Settings

```
{
    "input_node": 1, //The input node id
    "apiKey": "",  //Write API key
    "fileInterval": 10000,  //Data logging interval
    "emonUrl": "http://raspberrypi/emoncms",    //Emoncms url
    "debugLogging": false,
    "i2cAddress": 4,    //I2C address
    "i2cDevice": "/dev/i2c-1"   //I2C device (in case this was not a Pi)
}
```