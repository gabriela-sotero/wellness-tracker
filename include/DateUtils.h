#pragma once

#include <ctime>
#include <string>

namespace util {
    // Returns the current local date as an ISO string (YYYY-MM-DD).
    inline std::string today() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);   // local day of the user

        char buf[11];                        // "YYYY-MM-DD" + '\0'
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);

        return std::string(buf);
    }
}
