// slow global period
do, period 2000ms:
    // fast local period (overrides the global)
    use redled, period 100ms;
end
