// shows getting a specific record from file

const COMMAND_GET 17;
const ID_TO_GET 3;

read Light;
output File (Light, SequenceNumber), filename "LightData.csv";

// remote reading is implicit
read RemoteCommand;
read RemoteSequenceNumber;
when RemoteCommand = COMMAND_GET:
     output Serial, file "LightData.csv", where RemoteSequenceNumber = ID_TO_GET;
end
