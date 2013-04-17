// read Light;
// when Light < 100:
//    read Humidity, period 2000;
//    when 1 > 2:
//        read Temperature;
//    end
// end

// use RedLed, once;
// use GreenLed, times 2;
// use Print, once;

// use redled, initialTimeout 4s, on;
// read RedLed;
// when RedLed:
//   use greenLed, on;
// end

// Use RedLed, InitialTimeout 2000ms;

// use redled, blink, InitialTimeout 2000ms;
// use greenled, blink true, period 500ms, InitialTimeout 2000ms;
// use blueled, blink 100ms, InitialTimeout 2000ms;


// read Light; read Temperature; read Humidity;

// when Light < 20lx: // luxes
//    use redled, on;
// end

// when Temperature > 28C: // celsium degrees
//   use redled, on;
// end

// when Humidity > 60%: // per cent (relative humidity)
//    use redled, on;
// end

// complex time period
//use greenled, period 1h3min5s;

// simple time periods
// use greenled, period 1;
//use greenled, period 1ms;
//use greenled, period 1;



// slow global period
// do, initialTimeout 1000ms, period 2000ms, times 3:
// //do, period 2000ms, times 2:
//     // fast local period (overrides the global)
//     // but not "times" or "initialTimeout" or "duration"?
//     use redled, period 100ms;
// end

// do, period 3000ms:
//     // use (inherit) the global period
//     use greenled;
// end


// do, period 500ms, once:
//     use redled, on;
// then, period 500ms, once:
//     use greenled, on;
// then, period 500ms, once:
//     use blueled, on;
// then:
//     use redled, off;
//     use greenled, off;
//     use blueled, off;
// end

// read remotelight;
// read remoterandom;
// define foo remotelight;
// read foo;
// output serial;




// ---- watering (not compiling)

// const MIN_HUMIDITY_THRESHOLD 20%;
// // take soil humidity out of network
// // read RemoteSoilHumidity;
// // define the minimal humidity as minimum of values
// // in last 60 seconds, max 20 value array
// define MinHumidity min(takeRecent(RemoteSoilHumidity, 60s, 20));
// // read MinHumidity;
// when MinHumidity < MIN_HUMIDITY_THRESHOLD:
//    // use watering system in this case
//    use Watering;
// end

// TODO: do not require to read remote-based sensors!




// --- calibrating (ok)

// set CalibrationOffset 0;

// // calibrate temperature sensor specifically for each node
// when Variables.localAddress == 0x1:
//     set CalibrationOffset 3;
// elsewhen Variables.localAddress == 0x2:
//     set CalibrationOffset -2;
// elsewhen Variables.localAddress == 0x3:
//     set CalibrationOffset 1;
// end

// define CalibratedLight add(Light, CalibrationOffset);
// // read & send out the calibrated sensor
// read CalibratedLight;
// output Network, protocol CSMA;


// --- luster (fail: remote address not recognized)

// // On data storage node
// const DATA_QUERY_COMMAND 1;

// // remote light packet
// read RemoteLight; read RemoteAddress; read RemoteTimestamp;
// Output File (RemoteLight, RemoteAddress, RemoteTimestamp), filename "LightData.bin";

// // storage query packet
// read RemoteCommand;
// when RemoteCommand = DATA_QUERY_COMMAND:
//     Output Network, protocol LiteTDMA, file "LightData.bin",
//         where 
//             Address = RemoteAddress 
//             and Timestamp >= RemoteTimestamp;
// end


// --- volcano (ok)

// const COMMAND_EVENT_DETECTED 1;
// const COMMAND_REQUEST_DATA 2;
// const DATA_COLLECTION_INTERVAL 60s;
// const EWMA_COEFF_1 0.15;
// const EWMA_COEFF_2 0.2;
// const EVENT_DETECTION_THRESHOLD 0x1234;

// read AcousticSensor, period 10ms;
// read SeismicSensor, period 10ms;
// Output File (AcousticSensor, SeismicSensor, Timestamp),
//     name "SensorData.bin";

// define CombinedSensor sum(AcousticSensor, SeismicSensor);
// define EventDetectionFunction difference(
//     EWMA(CombinedSensor, EWMA_COEFF_1),
//     EWMA(CombinedSensor, EWMA_COEFF_2));
// read EventDetectionFunction;
// when EventDetectionFunction > EVENT_DETECTION_THRESHOLD:
//     output Network (Command), command COMMAND_EVENT_DETECTED;
// end;

// read RemoteCommand;
// read RemoteTimestamp;
// when RemoteCommand == COMMAND_REQUEST_DATA:
//     Output Network, file "SensorData.bin",
//         where RemoteTimestamp >= Timestamp
//         and RemoteTimestamp <= add(Timestamp, DATA_COLLECTION_INTERVAL);
// end


// --- lynxnet (TODO: watchdog should be read automatically)

// const RESET_PORT 1; const RESET_PIN  1;
// const POWER_PORT 1; const POWER_PIN  2;
// const BATTERY_LOW_PORT 1; const BATTERY_LOW_PIN 3;
// const WATCHDOG_PORT 1; const WATCHDOG_PIN 4;

// define ResetOut DigitalOut, port RESET_PORT, pin RESET_PIN;
// define PowerOut DigitalOut, port POWER_PORT, pin POWER_PIN;
// define BatteryLow DigitalIn, port BATTERY_LOW_PORT, pin BATTERY_LOW_PIN;
// define Watchdog DigitalIn, port WATCHDOG_PORT, pin WATCHDOG_PIN, interrupt, risingEdge;

// // The logic is simple:
// // when battery of the main controller is low, turn it off,
// // but turn it back on after a while (however, limit the number of tries)

// read BatteryLow;
// when BatteryLow:
//     use PowerOut, off;
// else:
//     do, initialTimeout 10min, once:  // add 10 minute hysteresis
//         use PowerOut, on;
//     end
// end

// // check for watchdog signal, and if none received, reset the main MCU
// const WATCHDOG_MAX_TIME 20s; // seconds
// const MAX_RETRY 5;
// when not Watchdog:
//     // at the end of 10 seconds: turn power off
//     do, period WATCHDOG_MAX_TIME, times MAX_RETRY:
//         use ResetOut;
//     then:
//         // give up
//     end
// end



// --- pwm (ok)

// // define input signal channel
// define InputPin AnalogIn, channel 6;
// // define output signal channel
// define OutputPin AnalogOut, pin 3, port 3;
// // define input->output mapping: input range is 120..255, output: 0..100.
// define MySignal map(InputPin, 120, 255, 0, 100), out OutputPin;
// // define the action
// use MySignal;


// -- tunnels (ok)

// const SAMPLING_INTERVAL 5s;
// const INTERIOR_REPORT_INTERVAL 5min;
// const ENTRANCE_REPORT_INTERVAL 30s;
// const SATURATED_VALUE 0xffff;

// // Load C-based extensions for this scenario
// load "TunnelDataCollector.py"; // component description
// load "TunnelDataCollector.c";  // component implementatio

// // choose the sampling interval depending on system variables
// // (assume that 'moteIsInInterior' is a variable
// //  that can be changed via management protocol)
// when Variables.moteIsInInterior:
//     set reportInterval INTERIOR_REPORT_INTERVAL; // TODO: convert to ms!
// else:
//     set reportInterval ENTRANCE_REPORT_INTERVAL; // TODO: convert to ms!
// end

// // there are 4 light sensors; ignore the ones that are saturated
// define Light1 filterLess(LightWithId, SATURATED_VALUE), id 1;
// define Light2 filterLess(LightWithId, SATURATED_VALUE), id 2;
// define Light3 filterLess(LightWithId, SATURATED_VALUE), id 3;
// define Light4 filterLess(LightWithId, SATURATED_VALUE), id 4;
// // one measurement is an average of all valid light sensors
// define LightAvg average(tuple(Light1, Light2, Light3, Light4));
// // read the average value with 5 second period
// read LightAvg, period SAMPLING_INTERVAL; // TODO: convert to ms!
// // use a custom data collector component
// output TunnelDataCollector, interval reportInterval;


// -- potholes (ok)


// const ACCEL_Z 2; // channel number

// const THRESHOLD 100;

// define AccelZ AnalogIn, channel ACCEL_Z;
// define Deviation stdev(take(AccelZ, 10));

// read Deviation;
// when Deviation > THRESHOLD:
//    use RedLed, on;
//    use Beeper, on, duration 200, frequency 1000;
// else:
//    use RedLed, off;
// end





// // Z measurements:
// // 1680 - norm       (1g)
// // 2080 - weightless (0g)
// // 1280 (assumed) -  (2g)
// const THRESHOLD_LOW 1360;
// const THRESHOLD_HIGH 2000;

// // channel number
// const ACCEL_Z 2;

// define AccelZ AnalogIn, channel ACCEL_Z;

// read AccelZ;
// when AccelZ < THRESHOLD_LOW or AccelZ > THRESHOLD_HIGH:
//    use RedLed, on;
//    use Beeper, on, duration 200, frequency 1000;
// else:
//    use RedLed, off;
// end





// // 0g is ~2080 accel value.
// const THRESHOLD_LOW 1930;
// const THRESHOLD_HIGH 2230;

// // channels
// define AccelX AnalogIn, channel 0;
// define AccelY AnalogIn, channel 1;
// define AccelZ AnalogIn, channel 2;

// read AccelX; read AccelY; read AccelZ;
// when AccelX > THRESHOLD_LOW and AccelX < THRESHOLD_HIGH
//         and AccelY > THRESHOLD_LOW and AccelY < THRESHOLD_HIGH
//         and AccelZ > THRESHOLD_LOW and AccelZ < THRESHOLD_HIGH:
//    use RedLed, on;
//    use Beeper, on, duration 200, frequency 1000;
// else:
//    use RedLed, off;
// end






// const ACCEL_Z 2; // channel number

// const THRESHOLD 100;

// define AccelZ AnalogIn, channel ACCEL_Z;
// define Difference multiply(stdev(take(AccelZ, 2)), 2);

// read Difference;
// when Difference > THRESHOLD:
//    use redLed, on;
//    use Beeper, on, duration 200, frequency 1000;
// else:
//    use redLed, off;
// end



// --- lowpass filtering


// // constants
// const NUM_SAMPLES 5;
// const ADJUSTMENT_WEIGHT 5;
// const ACCEL_CHANNEL 2;
// // define out accelerometer sensor (make sure cache is disabled)
// define Accelerometer AnalogIn, channel ACCEL_CHANNEL, cache False;
// // define accelerometer sensor with low pass filter applied
// define SmoothedAccelerometer smoothen(Accelerometer, NUM_SAMPLES, ADJUSTMENT_WEIGHT);
// // read the noise-removed sensor
// read SmoothedAccelerometer, period 10ms;
// // output to serial port
// output Serial, aggregate;
