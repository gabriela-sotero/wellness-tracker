#pragma once

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace util {
    // Header-only helpers keep calendar range construction consistent across
    // the service, user interfaces, and demo data generator.
    // Formats a calendar date as an ISO string (YYYY-MM-DD).
    inline std::string formatDate(const std::tm& date) {
        char buf[11];                        // "YYYY-MM-DD" + '\0'
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &date);
        return std::string(buf);
    }

    // Today at noon, so adding days never lands on a daylight-saving shift.
    inline std::tm todayDate() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);   // local day of the user
        tm.tm_hour = 12;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        std::mktime(&tm);
        return tm;
    }

    // Returns the current local date as an ISO string (YYYY-MM-DD).
    inline std::string today() {
        return formatDate(todayDate());
    }

    // Every date from first to last, inclusive. Empty when first is malformed.
    inline std::vector<std::string> datesBetween(
        const std::string& first,
        const std::string& last
    ) {
        std::vector<std::string> dates;

        std::tm current{};
        std::istringstream input(first);
        input >> std::get_time(&current, "%Y-%m-%d");
        if (input.fail()) {
            return dates;
        }
        current.tm_hour = 12;
        std::mktime(&current);

        while (formatDate(current) <= last) {
            dates.push_back(formatDate(current));
            current.tm_mday += 1;
            std::mktime(&current);
        }

        return dates;
    }

    // The last N days, ending today.
    inline std::vector<std::string> lastDays(int days) {
        std::tm first = todayDate();
        first.tm_mday -= days - 1;
        std::mktime(&first);
        return datesBetween(formatDate(first), today());
    }

    // Every day of the current month up to today.
    inline std::vector<std::string> monthToDate() {
        std::tm first = todayDate();
        first.tm_mday = 1;
        std::mktime(&first);
        return datesBetween(formatDate(first), today());
    }

    // Every day of the current year up to today.
    inline std::vector<std::string> yearToDate() {
        std::tm first = todayDate();
        first.tm_mon = 0;
        first.tm_mday = 1;
        std::mktime(&first);
        return datesBetween(formatDate(first), today());
    }
}
