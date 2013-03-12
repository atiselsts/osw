read Light;
when Light < 50:
  do, period 2s, once:
     use RedLed; // blink it
  then, period 2s, once, initialTimeout 1s:
     use GreenLed; // blink it
  then:
     // turn all off
     use RedLed, off;
     use GreenLed, off;
  end
end
