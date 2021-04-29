
#ifndef _TEXT_LOGGING_
#define _TEXT_LOGGING_


#include "imxrt.h"

#include "StringUtil.hxx"
#include "Time.hpp"
#include "Arduino.h"

namespace AmpedUp
{
    class TextLogging
    {
    public:

        template<typename... VarType>
        static void debug(const char* file, uint32_t line, const VarType&... values)
        {
            if (isDebugEnabled_)
            {
                logMessage(LogSeverity::DEBUG, file, line, StringUtil::toString(values...));
            }
        }

        template<typename... VarType>
        static void info(const char* file, uint32_t line, const VarType&... values)
        {
            if (isInfoEnabled_)
            {
                logMessage(LogSeverity::INFO, file, line, StringUtil::toString(values...));
            }
        }

        template<typename... VarType>
        static void warning(const char*file, uint32_t line, const VarType&... values)
        {
            if (isWarningEnabled_)
            {
                logMessage(LogSeverity::WARNING, file, line, StringUtil::toString(values...));
            }
        }

        template<typename... VarType>
        static void critical(const char* file, uint32_t line, const VarType&... values)
        {
            if (isCriticalEnabled_)
            {
                logMessage(LogSeverity::CRITICAL, file, line, StringUtil::toString(values...));
            }
        }

        template<typename... VarType>
        static void fatal(const char* file, uint32_t line, const VarType&... values)
        {
            if (isFatalEnabled_)
            {
                logMessage(LogSeverity::FATAL, file, line, StringUtil::toString(values...));
                panic();
            }
        }

        static void initialize();

        static void enableDebugVerbosity();

        static void enableInfoVerbosity();

        static void enableWarningVerbosity();

        static void enableCriticalVerbosity();

        static void enableFatalVerbosity();

        static void disableDebugVerbosity();

        static void disableInfoVerbosity();
        
        static void disableWarningVerbosity();

        static void disableCriticalVerbosity();
        
        static void disableFatalVerbosity();

    private:
        
        enum class LogSeverity : uint32_t
        {
            DEBUG,
            INFO,
            WARNING,
            CRITICAL,
            FATAL
        };

        static inline bool isDebugEnabled_{};
        static inline bool isInfoEnabled_{};
        static inline bool isWarningEnabled_{};
        static inline bool isCriticalEnabled_{};
        static inline bool isFatalEnabled_{};

        static void logMessage(LogSeverity severity, const char* file, uint32_t line, const std::string& message, bool allowRecursion = true);

        static void panic();

    };
}

#endif