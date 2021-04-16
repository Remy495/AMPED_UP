#include "Time.hpp"
#include "imxrt.h"
#include "TextLogging.hxx"

// #ifdef ARDUINO
    #include "Arduino.h"
    #define SERIAL_MONITOR_LOG
// #endif


void AmpedUp::TextLogging::initialize()
{
    #ifdef SERIAL_MONITOR_LOG
        Serial.begin(115200);
    #endif   
}

void AmpedUp::TextLogging::enableDebugVerbosity()
{
    isDebugEnabled_ = true;
}

void AmpedUp::TextLogging::enableInfoVerbosity()
{
    isInfoEnabled_ = true;
}

void AmpedUp::TextLogging::enableWarningVerbosity()
{
    isWarningEnabled_ = true;
}

void AmpedUp::TextLogging::enableCriticalVerbosity()
{
    isCriticalEnabled_ = true;
}

void AmpedUp::TextLogging::enableFatalVerbosity()
{
    isFatalEnabled_ = true;
}

void AmpedUp::TextLogging::disableDebugVerbosity()
{
    isDebugEnabled_ = false;
}

void AmpedUp::TextLogging::disableInfoVerbosity()
{
    isInfoEnabled_ = false;
}

void AmpedUp::TextLogging::disableWarningVerbosity()
{
    isWarningEnabled_ = false;
}

void AmpedUp::TextLogging::disableCriticalVerbosity()
{
    isCriticalEnabled_ = false;
}

void AmpedUp::TextLogging::disableFatalVerbosity()
{
    isFatalEnabled_ = false;
}

void AmpedUp::TextLogging::logMessage(LogSeverity severity, const std::string& file, uint32_t line, const std::string& message, bool allowRecursion)
{
    #ifdef SERIAL_MONITOR_LOG
    // Print a textual representation of the log entry to the serial monitor

    Serial.print(Time::now().getSeconds());
    Serial.print(" ");

    switch (severity)
    {
    case LogSeverity::DEBUG:
        Serial.print("DEBUG");
        break;
    case LogSeverity::INFO:
        Serial.print("INFO");
        break;
    case LogSeverity::WARNING:
        Serial.print("WARN");
        break;
    case LogSeverity::CRITICAL:
        Serial.print("CRIT");
        break;
    case LogSeverity::FATAL:
        Serial.print("FATAL");
        break;
    default:
        if (allowRecursion)
        {
            logMessage(LogSeverity::WARNING, __FILE__, __LINE__, "Encountered unknown log severity", false);
        }
        Serial.print("LOG");
    }

    Serial.print(" ");
    Serial.print(file.c_str());
    Serial.print(" ");
    Serial.print(line);
    Serial.print(" - ");
    Serial.println(message.c_str());

    #endif

}

void AmpedUp::TextLogging::panic()
{
    // Disable all interrupts and enter an infinite loop. 
    __disable_irq();
    while(true); 
}