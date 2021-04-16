
#ifndef _STRING_UTIL_
#define _STRING_UTIL_

#include <string>
#include <sstream>

namespace AmpedUp
{
    class StringUtil
    {
    public:

        template<typename... VarTypes>
        static std::string toString(const VarTypes&... values)
        {
            std::ostringstream oss;
            printAll(oss, values...);
            return oss.str();
        }

    private:

        template<typename T, typename... VarTypes>
        static void printAll(std::ostream& oss, const T& value, const VarTypes&... others)
        {
            printAll(oss << value, others...);
        }

        static void printAll(std::ostream&)
        {
            // Nothing to do here, nothing needs to be printed
        }
    };
}

#endif