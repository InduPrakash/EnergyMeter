// 10/24/2015
// Reads data from Arduino over I2C device at address 0x04 and files to local emoncms

/*jshint bitwise: false*/
const fs = require('fs');
const i2c = require("i2c-bus");
const request = require('request');
const config = require("./config.json");

//The I2C address was obtained by running "i2cdetect -y 1"
//sudo apt-get install -y i2c-tools
const wire = i2c.openSync(1);
const buffer = Buffer.alloc(12); //10 bytes of data

const output = fs.createWriteStream('./stdout.log');
const errorOutput = fs.createWriteStream('./stderr.log');
const logger = new console.Console(output, errorOutput); //process.output
const EMONCMSURL = `${config.emonUrl}/input/post.json?apikey=${config.apiKey}&node=${config.input_node}&json=`;

//power1[4 bytes], power2[4 bytes], future[2 bytes]
function parseAndFileData(data) {
    for (var i = 0; i < data.length; i++) {	//Remove bad data
        if (isNaN(parseInt(data[i]))) {
            data[i] = 0;
        }
    }

    //The first 4 bytes are for power line #1, next 4 for line #2, next 2 are reserved for future
    var obj = {
        power1: bytesToLong(data, 0),
        power2: bytesToLong(data, 4)
    };

    var json = JSON.stringify(obj);
    log(json);

    //File data
    var url = EMONCMSURL + json;
    log(url);
    request(url, function (err) {
        if (err) {
            logErr(err);
        }
    });
}

function getI2CData() {
    //Just issue a read request
    wire.i2cRead(config.i2cAddress, 10, buffer, function (err, bytes, bufOut) { //10 bytes
        if (err) {
            logErr(err);
        }
        else {
            log("Read " + bufOut.toString('ascii'));
            parseAndFileData(bufOut);
        }
    });
}

function bytesToLong(arr, index) {
    var num = parseInt(arr[index++]);
    num |= parseInt(arr[index++]) << 8;
    num |= parseInt(arr[index++]) << 16;
    num |= parseInt(arr[index++]) << 24;
    return num;
}

/*function bytesToInt(arr, index) {
    var num = parseInt(arr[index++]);
    num |= parseInt(arr[index++]) << 8;
    return num;
}*/

function log(text) {
    if (config.debugLogging) {
        logger.log((new Date()).toLocaleString() + " " + text);
    }
}

function logErr(text) {
    logger.error((new Date()).toLocaleString() + " " + text);
}

setInterval(getI2CData, config.fileInterval);
