#pragma once



#include <chrono>



namespace Strawberry::Standard
{
	class Clock
	{
	public:
	    explicit Clock(bool run = true);

	    void Start();

	    double Stop();

	    [[nodiscard]] double      Read() const;
	    double operator*() const { return Read(); }
	    operator  double() const { return Read(); }

	    void Restart();
	private:
	    using Duration = std::chrono::duration<double, std::ratio<1>>;
	    using Instant = std::chrono::time_point<std::chrono::system_clock, Duration>;

	    bool mIsRunning;
	    Duration mBuffer;
	    Instant  mStart, mEnd;
	};
}
