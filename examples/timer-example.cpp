#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>

using Real_T = rt_timer::Real_T;
using ms = std::chrono::microseconds;

constexpr Real_T action_rate = 1e3;                //* [Hz]
constexpr Real_T sample_rate = .75;                //* [Hz]
constexpr Real_T timer_period = 1. / action_rate;  //* [s]
constexpr Real_T sample_period = 1. / sample_rate; //* [s]
constexpr size_t clinfo_length = 2;
constexpr size_t max_delay = 2 * std::micro::den * timer_period;

#if defined(__WIN32__) || defined(__WIN32) || defined(_WIN32) || defined(WIN32) || \
    defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define WIN32
#endif

#ifdef WIN32
	#include <windows.h>
#endif

//* This is a class that is used to perform an action
struct Action {
	void
	fun()
	{
		auto now_time = rt_timer::clock::now();
		auto prev_time = rt_timer::clock::now();

		if (now_time - prev_time > ms(max_delay)) {
			prev_time = now_time;
			//* do something...
			++counter;
		}
	}
	rt_timer::time prev_time = rt_timer::clock::now();
	size_t counter = 0;
};

//* This is a class that is used to sample the timer
struct Sampler {
	Sampler(rt_timer::Timer<Action> &timer) : timer(timer)
	{
		for (size_t i = 0; i < clinfo_length; ++i) {
			printf("\n");
		}

#ifdef WIN32 //* Enabling VT100 for win32
		DWORD l_mode;
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleMode(hStdout, &l_mode);
		SetConsoleMode(hStdout,
		               l_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING |
		                   DISABLE_NEWLINE_AUTO_RETURN);
#endif
	}
	void
	print()
	{
		Real_T rate;
		Real_T avg_elapsed;
		Real_T max_elapsed;
		size_t over_time_ct;
		timer.sample(rate, avg_elapsed, max_elapsed, over_time_ct);

		for (size_t i = 0; i < clinfo_length; ++i) {
			printf("\033[A\033[2K\r"); //* move the cursor up then clear the line
		}
		printf("| %-16s | %-16s | %-16s | %-16s |\n",
		       "Rate [Hz]:", "Avg. Elap. [us]:", "Max. Elap. [us]:", "Overtime Ct.:");
		printf("| %16.3g | %16.3g | %16.3g | %16zu |\n", rate,
		       std::micro::den * avg_elapsed, std::micro::den * max_elapsed, over_time_ct);
	}
	rt_timer::Timer<Action> &timer;
};

int
main()
{
	printf("Do the action every %.3g seconds, and sample the action every every %.3g "
	       "seconds:\n",
	       std::micro::den * timer_period, sample_period);

	//* Create a timer for the action
	Action action;
	rt_timer::Timer<Action> timer(timer_period, action, &Action::fun);

	//* Create another timer for sampling the action
	Sampler sampler(timer);
	rt_timer::Timer<Sampler> sampler_timer(sample_period, sampler, &Sampler::print);

	while (true) {
		timer.check();
		sampler_timer.check();
	}

	return 0;
}