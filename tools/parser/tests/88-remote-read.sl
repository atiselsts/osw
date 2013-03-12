// read random values from OS, send to radio
read Random;
output Radio (Random);

// read random values from a neighbor, send to serial
read RemoteRandom;
output Serial (RemoteRandom);
