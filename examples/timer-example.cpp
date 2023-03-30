#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
// #include <cstdlib>
#include <thread>

using Real_T = rt_timer::Real_T;
using ns = rt_timer::ns;
using std::chrono::duration;
using std::chrono::steady_clock;
using time_sc = steady_clock::time_point;

constexpr Real_T action_rate = 1e3; //* [Hz]
constexpr Real_T sample_rate = 0.8;  //* [Hz]
static_assert(sample_rate <= action_rate,
              "The action rate must be less than or equal to the sample rate.");
constexpr Real_T timer_period = 1. / action_rate;  //* [s]
constexpr Real_T sample_period = 1. / sample_rate; //* [s]

#if defined(__WIN32__) || defined(__WIN32) || defined(_WIN32) || defined(WIN32) || \
    defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define WIN32
#endif

#ifdef WIN32
	#include <windows.h>
#endif

/* This is a class that is used to perform an action */
class Action
{
  public:
	void
	fun()
	{
		const auto now_time = rt_timer::clock::now();
		const Real_T task_duration = max_period * std::nano::den * rand() / RAND_MAX;

		while (rt_timer::clock::now() - now_time < ns(static_cast<size_t>(task_duration))) {
			/** do something */
			++counter;
		}
	}

  private:
	static constexpr Real_T max_period = 1. * timer_period;
	size_t counter = 0;
};

/* This is a class that is used to sample the timer and print the results */
class Sampler
{
  public:
	Sampler(rt_timer::Timer<Action> &timer) : timer(timer)
	{
		/** initialize the command line info priting: */
		for (size_t i = 0; i < clinfo_length; ++i) {
			printf("\n");
		}

#ifdef WIN32
		/** enable VT100 for win32*/
		DWORD l_mode;
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleMode(hStdout, &l_mode);
		SetConsoleMode(hStdout,
		               l_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING |
		                   DISABLE_NEWLINE_AUTO_RETURN);
#endif
	}

	/*`sample():`
	 * Sample the timer and print the results.
	 */
	void
	sample()
	{
		/** sample the timer */
		timer.sample(timer_time, rate, avg_elapsed, max_elapsed, call_count,
		             overtime_count);

		if (never_sampled) {
			never_sampled = false;
			start_time = steady_clock::now();
		}
		real_time = duration<Real_T>(steady_clock::now() - start_time).count();

		/** print the results */
		for (size_t i = 0; i < clinfo_length; ++i) {
			printf("\033[A\033[2K\r"); //* move the cursor up then clear the line
		}
		printf("| %-16s | %-16s | %-16s |\n", "Real Time:", "Rate:", "Call #:");
		printf("| %14.5g s | %13.5g Hz | %16zu |\n", real_time, rate, call_count);

		printf("| %-16s | %-16s | %-16s | \n", "Time:", "Avg. Elapsed:", "Overtime #:");
		printf("| %14.5g s | %13.5g ms | %16zu | \n", timer_time,
		       std::milli::den * avg_elapsed, overtime_count);

		printf("| %-16s | %-16s | %-16s |\n", "Lag:", "Max. Elapsed:", "Overtime Ratio:");
		printf("| %13.5g ms | %13.5g ms | %15.5g%% | \n",
		       std::milli::den * (real_time - timer_time), std::milli::den * max_elapsed,
		       static_cast<Real_T>(overtime_count) / call_count * 100);
	}

  private:
	static constexpr size_t clinfo_length = 6;
	bool never_sampled = true;
	time_sc start_time;
	Real_T real_time;
	Real_T timer_time;
	Real_T rate;
	Real_T avg_elapsed;
	Real_T max_elapsed;
	size_t call_count;
	size_t overtime_count;
	rt_timer::Timer<Action> &timer;
};

int
main()
{
	printf("Do the action every %.5g milliseconds and sample the action every every %.5g "
	       "seconds:\n",
	       std::milli::den * timer_period, sample_period);

	/** create a timer thread to call the action periodically */
	Action action;
	rt_timer::Timer<Action> action_timer(timer_period, action, &Action::fun);

	/** create a second timer thread to sample the previous timer periodically */
	Sampler sampler(action_timer);
	rt_timer::Timer<Sampler> sampler_timer(sample_period, sampler, &Sampler::sample);

	std::thread action_thread([&action_timer] {
		while (true) {
			action_timer.check();
		}
	});

	std::thread sampler_thread([&sampler_timer] {
		while (true) {
			sampler_timer.check();
		}
	});

	action_thread.join();
	sampler_thread.join();

	return 0;
}