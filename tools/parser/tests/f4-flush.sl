// shows flushing a file

const COMMAND_FLUSH 17;

read Light;
output File (Light, SequenceNumber), filename "LightData.csv";

read RemoteCommand; // optional?
when RemoteCommand == COMMAND_FLUSH:
    output Radio, file "LightData.csv";
end
