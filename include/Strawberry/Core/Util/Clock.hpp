#pragma once



#include <chrono>



#include "Option.hpp"



namespace Strawberry::Core
{
	using Seconds = double;



	class Clock
	{
	public:
	    explicit Clock(bool start = true);

	    void Start();

	    double Stop();

	    Seconds              Read() const;
	    Seconds         operator*() const { return Read(); }
	    explicit operator Seconds() const { return Read(); }

	    void Restart();
	private:
	    using Duration = std::chrono::duration<double, std::ratio<1>>;
	    using Instant  = std::chrono::time_point<std::chrono::system_clock, Duration>;

	    Duration        mBuffer;
	    Option<Instant> mStartTime;
	};
}
