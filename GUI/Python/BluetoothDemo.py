
import time
from bluetooth import *


payload1String = str([i for i in range(0, 300)])
payload2String = str([i for i in range(300, 600)])

btsock = BluetoothSocket(RFCOMM)
btsock.connect(("E8:DB:84:02:58:4A", 1))

i = 0
try:
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

        if (recievedData == payload):
            print("Recieved correct response")
        else:
            print("Error: Recieved incorrect response: " + str(recievedSize))
            print(len(recievedData))
            print(recievedData)

        time.sleep(1)

        i += 1
except KeyboardInterrupt:
    pass

btsock.close()