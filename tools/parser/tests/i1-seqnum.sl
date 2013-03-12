// react on a command ("InputCommand" syntax)

const COMMAND_GET 17;

read RemoteCommand;
read RemoteSequenceNumber;
when RemoteCommand = COMMAND_GET:
   use print, format "got seqnum %d\n", arg1 RemoteSequenceNumber, once;
end
