// On sensor node:
// const READ_PERIOD 10s;
// read Light, period READ_PERIOD;
// output Network (Light, Address, Timestamp), protocol LiteTDMA;


// On data storage node
const DATA_QUERY_COMMAND 1;

// remote light packet
read RemoteLight; read RemoteAddress, read RemoteTimestamp;
Output File (RemoteLight, RemoteAddress, RemoteTimestamp), filename "LightData.bin";

// storage query packet
read RemoteCommand;
when RemoteCommand = DATA_QUERY_COMMAND:
    Output Network, protocol LiteTDMA, file "LightData.bin",
        where 
            Address = RemoteAddress 
            and Timestamp >= RemoteTimestamp;
end
