// Read LED led status and tur on gren led if the red one is on

use redled, initialTimeout 4s, on;

read RedLed;
when RedLed:
  use greenLed, on;
end
