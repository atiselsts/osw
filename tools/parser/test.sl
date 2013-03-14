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


do, period 500ms, once:
    use redled, on;
then, period 500ms, once:
    use greenled, on;
then, period 500ms, once:
    use blueled, on;
then:
    use redled, off;
    use greenled, off;
    use blueled, off;
end
