
import time
from bluetooth import *
import flatbuffers


from AmpedUpMessaging import Message, MessagePayload, LogMessage, LogSeverity

# builder = flatbuffers.Builder(1024)
# filenameOffset = builder.CreateString("test.cxx")
# messageOffset = builder.CreateString("This is an example log message. It's probably pretty similar in size to a real log message.")
# LogMessage.LogMessageStart(builder)
# LogMessage.LogMessageAddSeverity(builder, LogSeverity.LogSeverity().CRITICAL)
# LogMessage.LogMessageAddFileName(builder, filenameOffset)
# LogMessage.LogMessageAddLineNumber(builder, 12345)
# LogMessage.LogMessageAddMessage(builder, messageOffset)
# logMessageOffset = LogMessage.LogMessageEnd(builder)

# Message.MessageStart(builder)
# Message.MessageAddPayloadType(builder, MessagePayload.MessagePayload().LogMessage)
# Message.MessageAddPayload(builder, logMessageOffset)
# messageOffset = Message.MessageEnd(builder)

# builder.Finish(messageOffset)
# buf = builder.Output()
# print(len(buf))


btsock = BluetoothSocket(RFCOMM)
btsock.connect(("E8:DB:84:03:F1:32", 1))

i = 0
while True:

    recievedSizeBytes = btsock.recv(2)
    recievedSize = int.from_bytes(recievedSizeBytes, 'little')

    recievedData = b''
    remainingSize = recievedSize
    while remainingSize > 0:
        newData = btsock.recv(remainingSize)
        remainingSize -= len(newData)
        recievedData += newData

    print(recievedData)

    message = Message.Message.GetRootAsMessage(recievedData, 0)
    if (message.PayloadType() == MessagePayload.MessagePayload().LogMessage):
        logMessage = LogMessage.LogMessage()
        logMessage.Init(message.Payload().Bytes, message.Payload().Pos)

        if (logMessage.Severity() == LogSeverity.LogSeverity().WARNING):
            print("WARN: ")

        print(logMessage.FileName(), ":", logMessage.LineNumber())
        print(logMessage.Message())
    else:
        print("Wrong message type:", message.PayloadType())
        
btsock.close()