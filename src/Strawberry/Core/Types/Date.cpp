//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Date.hpp"
#include "Strawberry/Core/Assert.hpp"


//======================================================================================================================
//  Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	DateInterval::DateInterval(int32_t years, int32_t months, int32_t days)
			: years(years)
			, months(months)
			, days(days)
	{}


	DateInterval DateInterval::Days(int32_t days)
	{
		return {0, 0, days};
	}


	DateInterval DateInterval::Months(int32_t months)
	{
		return {0, months, 0};
	}


	DateInterval DateInterval::Years(int32_t years)
	{
		return {years, 0, 0};
	}


	DateInterval DateInterval::operator+(const DateInterval& rhs) const
	{
		return {years + rhs.years, months + rhs.months, days + rhs.days};
	}


	DateInterval DateInterval::operator-(const DateInterval& rhs) const
	{
		return {years - rhs.years, months - rhs.months, days - rhs.days};
	}


	DateInterval& DateInterval::operator+=(const DateInterval& rhs)
	{
		return *this = *this + rhs;
	}


	DateInterval& DateInterval::operator-=(const DateInterval& rhs)
	{
		return *this = *this - rhs;
	}


	Date::Date(int32_t year, uint8_t month, uint8_t day)
		: mYear(year)
		, mMonth(month)
		, mDay(day)
	{
		Core::Assert(IsValidDate());
	}


	int32_t Date::GetYear() const
	{
		return mYear;
	}


	int32_t Date::GetMonth() const
	{
		return mMonth;
	}


	int32_t Date::GetDay() const
	{
		return mDay;
	}


	Date Date::operator+(const DateInterval& interval) const
	{
		Date date(*this);
		date.mYear  += interval.years;
		date.mMonth += interval.months;
		date.mDay   += interval.days;
		date.Normalise();
		return date;
	}


	Date Date::operator-(const DateInterval& interval) const
	{
		Date date(*this);
		date.mYear  -= interval.years;
		date.mMonth -= interval.months;
		date.mDay   -= interval.days;
		date.Normalise();
		return date;
	}


	Date& Date::operator+=(const DateInterval& interval)
	{
		return *this = *this + interval;
	}


	Date& Date::operator-=(const DateInterval& interval)
	{
		return *this = *this + interval;
	}


	bool Date::IsLeapYear() const
	{
		return mYear % 4 == 0 && (mYear % 100 != 0 || mYear % 400 == 0);
	}


	bool Date::IsValidDate() const
	{
		return mMonth < 12 && mDay < GetMonthLength();
	}


	void Date::Normalise()
	{
		while (!IsValidDate())
		{
			if (mMonth < 12 && mDay >= GetMonthLength())
			{
				mDay -= GetMonthLength();
				mMonth += 1;
			}

			if (mMonth < 12 && mDay < 0)
			{
				mDay += GetMonthLength();
				mMonth -= 1;
			}

			if (mMonth >= 12)
			{
				mMonth -= 12;
				mYear  += 1;
			}

			if (mMonth < 0)
			{
				mMonth += 12;
				mYear  -= 1;
			}
		}
	}


	uint8_t Date::GetMonthLength() const
	{
		switch (mMonth)
		{
			case 0: return 31;                        // January
			case 1: return IsLeapYear() ? 29 : 28;    // February
			case 2: return 31;                        // March
			case 3: return 30;                        // April
			case 4: return 31;                        // May
			case 5: return 30;                        // June
			case 6: return 31;                        // July NOLINT(*-branch-clone)
			case 7: return 31;                        // August
			case 8: return 30;                        // September
			case 9: return 31;                        // October
			case 10: return 30;                       // November
			case 11: return 31;                       // December
			default: Core::Unreachable();
		}
	}
}