
import inspect
from colorama import Fore, Style
import colorama
import os.path
import threading
from datetime import datetime

from AmpedUpMessaging.LogSeverity import LogSeverity
from AmpedUpMessaging.LogMessage import LogMessage

def logLocation():
    locationInfo = inspect.getframeinfo(inspect.currentframe().f_back)
    print(locationInfo.filename, locationInfo.lineno, locationInfo.function)

class TextLogging:

    isDebugEnabled = False
    isInfoEnabled = True
    isWarningEnabled = True
    isCriticalEnabled = True
    isFatalEnabled = True

    areLogsPrintedToConsole = True
    areLogsInColor = True

    logFile = None

    __printlock = threading.Lock()

    def debug(self, messageString):
        if self.isDebugEnabled:
            locationInfo = inspect.getframeinfo(inspect.currentframe().f_back)
            self.log(LogSeverity.DEBUG, locationInfo.filename, locationInfo.function, locationInfo.lineno, messageString, False)

    def info(self, messageString):
        if self.isInfoEnabled:
            locationInfo = inspect.getframeinfo(inspect.currentframe().f_back)
            self.log(LogSeverity.INFO, locationInfo.filename, locationInfo.function, locationInfo.lineno, messageString, False)

    def warning(self, messageString):
        if self.isWarningEnabled:
            locationInfo = inspect.getframeinfo(inspect.currentframe().f_back)
            self.log(LogSeverity.WARNING, locationInfo.filename, locationInfo.function, locationInfo.lineno, messageString, False)

    def critical(self, messageString):
        if self.isCriticalEnabled:
            locationInfo = inspect.getframeinfo(inspect.currentframe().f_back)
            self.log(LogSeverity.CRITICAL, locationInfo.filename, locationInfo.function, locationInfo.lineno, messageString, False)

    def fatal(self, messageString):
        if self.isFatalEnabled:
            locationInfo = inspect.getframeinfo(inspect.currentframe().f_back)
            self.log(LogSeverity.FATAL, locationInfo.filename, locationInfo.function, locationInfo.lineno, messageString, False)

    def log(self, severity, file, function, line, messageString, fromHardware):

        timestampString = str(datetime.now())

        colorString = ""
        severityString = ""

        locationString = str(file)
        if function is not None:
            locationString += ":" + str(function)
        locationString += ":" + str(line)

        if severity == LogSeverity.DEBUG:
            severityString = "DEBUG"
            colorString = Fore.GREEN
        elif severity == LogSeverity.INFO:
            severityString = "INFO"
            colorString = Style.BRIGHT
        elif severity == LogSeverity.WARNING:
            severityString = "WARN"
            colorString = Style.BRIGHT + Fore.YELLOW
        elif severity == LogSeverity.CRITICAL:
            severityString = "CRIT"
            colorString = Style.BRIGHT + Fore.MAGENTA
        elif severity == LogSeverity.FATAL:
            severityString = "FATAL"
            colorString = Style.BRIGHT + Fore.RED

        originString = "LOCAL: "
        if (fromHardware):
            originString = "HRDWR: "

        logString = f'{timestampString} {locationString} {severityString} - {messageString}'

        if self.areLogsInColor:
            logString = colorString + logString + Style.RESET_ALL
            originString = Style.DIM + originString + Style.RESET_ALL

        logString = originString + logString

        with self.__printlock:
            if self.logFile is not None:
                self.logFile.write(logString)
                self.logFile.write('\n')
                self.logFile.flush()

            if self.areLogsPrintedToConsole:
                print(logString)

    def remoteLog(self, logMessage):
        self.log(logMessage.Severity(), logMessage.FileName().decode('utf-8'), None, logMessage.LineNumber(), logMessage.Message().decode('utf-8'), True)


    @classmethod
    def initialize(cls, logFileDirectory):
        colorama.init()

        if not os.path.exists(logFileDirectory):
            os.makedirs(logFileDirectory)

        logFileTitle = datetime.now().strftime("%Y-%m-%d-%H-%M-%S.txt")
        logFilename = os.path.join(logFileDirectory, logFileTitle)
        cls.logFile = open(logFilename, 'w')
