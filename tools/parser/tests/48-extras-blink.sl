// "blink TIMEVALUE" parameter (synonym for "times 2, period TIMEVALUE"

use redled, blink, InitialTimeout 2000ms; // 1000 ms by default
use greenled, blink true, period 500ms, InitialTimeout 2000ms;
use blueled, blink 100ms, InitialTimeout 2000ms;
