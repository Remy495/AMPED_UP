
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



payload1String = str([i for i in range(0, 300)])
payload2String = str([i for i in range(300, 600)])

btsock = BluetoothSocket(RFCOMM)
btsock.connect(("E8:DB:84:03:F1:32", 1))

i = 0
while True:

    payload1 = payload1String.encode('utf-8')
    payload2 = payload2String.encode('utf-8')

    payload = payload1 + payload2
    print(len(payload))

    payloadSizeBytes = len(payload).to_bytes(2, 'little')
    packet1 = payloadSizeBytes + payload1
    packet2 = payload2

    btsock.send(packet1 + packet2)
    # time.sleep(1)
    # btsock.send(packet2)

    recievedSizeBytes = btsock.recv(2)
    recievedSize = int.from_bytes(recievedSizeBytes, 'little')

    recievedData = b''
    remainingSize = recievedSize
    while remainingSize > 0:
        newData = btsock.recv(remainingSize)
        remainingSize -= len(newData)
        recievedData += newData
        print("recieved")

    if (recievedData == payload):
        print("Recieved correct response")
    else:
        print("Error: Recieved incorrect response: " + str(recievedSize))
        print(len(recievedData))
        print(recievedData)

    time.sleep(1)

    i += 1

btsock.close()