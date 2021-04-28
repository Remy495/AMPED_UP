
from bluetooth import *
import time
from queue import SimpleQueue
from select import select
import threading

from text_logging import TextLogging

from AmpedUpMessaging import Message, MessagePayload, LogMessage, LogSeverity

tlog = TextLogging()

class HardwareComms:


    def __init__(self, mac, channel):
        self.mac = mac
        self.channel = channel
        self.isConnected = False
        self.isRunning = False
        self.socket = BluetoothSocket(RFCOMM)

        self.backgroundThread = None

        self.incomingQueue = SimpleQueue()
        self.outgoingQueue = SimpleQueue()

    def run(self):
        self.isRunning = True

        while self.isRunning:
            
            # Connect to the hardware bluetooth device if we are not already connected
            while not self.isConnected:
                try:
                    self.socket.connect((self.mac, self.channel))
                except Exception as e:
                    # Failed to connect. Wait a couple seconds and try again
                    time.sleep(2)
                else:
                    self.isConnected = True
                    tlog.info("Connected to AmpedUp hardware device.")

            # Send the message at the front of the outgoing queue if there is one
            if not self.outgoingQueue.empty():
                try:
                    self.sendMessage(self.outgoingQueue.get())
                except Exception as e:
                    self.socket.close()
                    self.isConnected = False
                    tlog.critical("Disconnected from hardware due to an error while trying to send a message: " + str(e))
                    continue

            # Try to recieve a message from the 
            if self.readyToRecieve():
                try:
                    recievedData = self.recieveMessage()
                except Exception as e:
                    self.socket.close()
                    self.isConnected = False
                    tlog.critical("Disconnected from hardware due to an error while trying to recieve a message: " + str(e))
                    continue
                else:
                    self.handleRecievedData(recievedData)
            


    def runAsync(self):
        self.backgroundThread = threading.Thread(target=self.run, daemon=True)
        self.backgroundThread.start()

    def readyToRecieve(self):
        return select([self.socket], [], [], 0)[0]

    def recieveMessage(self):
        recievedSizeBytes = self.socket.recv(2)
        recievedSize = int.from_bytes(recievedSizeBytes, 'little')

        recievedData = b''
        remainingSize = recievedSize
        while remainingSize > 0:
            newData = self.socket.recv(remainingSize)
            remainingSize -= len(newData)
            recievedData += newData

        return recievedData

    def sendMessage(self, message):
        tlog.info("Sending message...")
        outgiongSizeBytes = len(message).to_bytes(2, 'little')
        self.socket.send(outgiongSizeBytes)
        self.socket.send(message)

    def handleRecievedData(self, recievedData):
        message = Message.Message.GetRootAsMessage(recievedData, 0)
        if (message.PayloadType() == MessagePayload.MessagePayload().LogMessage):
            logMessage = LogMessage.LogMessage()
            logMessage.Init(message.Payload().Bytes, message.Payload().Pos)
            tlog.remoteLog(logMessage)




    