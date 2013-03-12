
when Light > 100lx: // luxes
  use redled, on;
end

when Temperature > 30C: // celsium degrees
  use redled, on;
end

when Humidity > 60%: // per cent (relative humidity)
  use redled, on;
end

// complex time period
use greenled, period 1h3min5s;
