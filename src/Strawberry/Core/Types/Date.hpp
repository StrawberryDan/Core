#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Standard Library
#include <compare>
#include <cstdint>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
    struct DateInterval
    {
        int32_t years  = 0;
        int32_t months = 0;
        int32_t days   = 0;


        DateInterval(int32_t years, int32_t months, int32_t days);


        static DateInterval Days(int32_t days);
        static DateInterval Months(int32_t months);
        static DateInterval Years(int32_t years);


        bool operator==(const DateInterval& rhs) const = default;
        bool operator!=(const DateInterval& rhs) const = default;


        DateInterval  operator+(const DateInterval& rhs) const;
        DateInterval  operator-(const DateInterval& rhs) const;
        DateInterval& operator+=(const DateInterval& rhs);
        DateInterval& operator-=(const DateInterval& rhs);
    };


    class Date
    {
        public:
            Date(int32_t year, uint8_t month, uint8_t day);


            [[nodiscard]] int32_t GetYear() const;
            [[nodiscard]] int32_t GetMonth() const;
            [[nodiscard]] int32_t GetDay() const;


            void SetYear(int32_t year);
            void SetMonth(uint8_t month);
            void SetDay(uint8_t day);


            Date  operator+(const DateInterval& interval) const;
            Date  operator-(const DateInterval& interval) const;
            Date& operator+=(const DateInterval& interval);
            Date& operator-=(const DateInterval& interval);


            std::partial_ordering operator<=>(const Date& rhs) const = default;


            [[nodiscard]] bool IsLeapYear() const;

        protected:
            [[nodiscard]] bool IsValidDate() const;
            void               Normalise();


            [[nodiscard]] uint8_t GetMonthLength() const;

        private:
            int32_t mYear;
            int32_t mMonth;
            int32_t mDay;
    };
}
