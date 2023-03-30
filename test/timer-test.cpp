#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>

using Real_T = rt_timer::Real_T;
using ns = rt_timer::ns;
using std::chrono::duration;
using std::chrono::steady_clock;
using time_sc = steady_clock::time_point;

constexpr Real_T action_rate = 1e2; //* [Hz]
constexpr Real_T sample_rate = 2.5;   //* [Hz]
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
		timer.sample(timer_time, rate, avg_elapsed, max_elapsed, act_ct, over_time_ct);

		if (never_sampled) {
			never_sampled = false;
			start_time = steady_clock::now();
		}
		real_time = duration<Real_T>(steady_clock::now() - start_time).count();

		/** print the results */
		for (size_t i = 0; i < clinfo_length; ++i) {
			printf("\033[A\033[2K\r"); //* move the cursor up then clear the line
		}
		printf("| %-16s | \n", "Real Time:");
		printf("| %14.5g s |\n", real_time);
		printf("| %-16s | %-16s | %-16s | %-16s |\n", "Time:", "Rate:", "Lag:", "Calls:");
		printf("| %14.5g s | %13.5g Hz | %14.5g s | %10zu times |\n", timer_time, rate,
		       real_time - timer_time, act_ct);
		printf("| %-16s | %-16s | %-16s | %-16s |\n",
		       "Avg. Elapsed:", "Max. Elapsed:", "Overtimes:", "Overtime Ratio:");
		printf("| %13.5g ms | %13.5g ms | %10zu times | %14.5g %% | \n",
		       std::milli::den * avg_elapsed, std::milli::den * max_elapsed, over_time_ct,
		       static_cast<Real_T>(over_time_ct) / act_ct * 100);
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
	size_t act_ct;
	size_t over_time_ct;
	rt_timer::Timer<Action> &timer;
};

int
main()
{
	printf("Do the action every %.5g seconds, and sample the action every every %.5g "
	       "seconds:\n",
	       std::micro::den * timer_period, sample_period);

	//* Create a timer for the action
	Action action;
	rt_timer::Timer<Action> timer(timer_period, action, &Action::fun);

	//* Create another timer for sampling the action
	Sampler sampler(timer);
	rt_timer::Timer<Sampler> sampler_timer(sample_period, sampler, &Sampler::sample);

	while (true) {
		timer.check();
		sampler_timer.check();
	}

	return 0;
}