#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>

using Real_T = rt_timer::Real_T;
using ns = rt_timer::ns;
using std::chrono::duration;
using std::chrono::steady_clock;
using time_sc = steady_clock::time_point;

constexpr Real_T action_rate = 1e4; //* [Hz]
constexpr Real_T sample_rate = 0.8; //* [Hz]
static_assert(sample_rate <= action_rate,
              "The action rate must be greater than or equal to the sample rate.");
constexpr Real_T timer_period = 1. / action_rate;  //* [s]
constexpr Real_T sample_period = 1. / sample_rate; //* [s]

/* This is a class that is used to perform an action */
class Action
{
  public:
	void
	fun()
	{
		const auto now_time = rt_timer::clock::now();
		/** the action duration is half of the timer period */
		static const Real_T fun_duration = std::nano::den * timer_period / 2; 

		while (rt_timer::clock::now() - now_time < ns(static_cast<size_t>(fun_duration))) {
			/** do something */
			++counter;
		}
	}

  private:
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
		timer.sample(timer_time, call_lag_max, act_elapsed_max, call_count, overtime_count,
		             rate_avg, call_lag_avg, act_elapsed_avg);

		if (never_sampled) {
			never_sampled = false;
			start_time = steady_clock::now();
		}
		real_time = duration<Real_T>(steady_clock::now() - start_time).count();

		/** print the results */
		for (size_t i = 0; i < clinfo_length; ++i) {
			printf("\033[A\033[2K\r"); //* move the cursor up then clear the line
		}
		// clang-format off
		printf("| %-16s | %-16s | %-16s |\n", "Real Time:", "Timer Time:", "Avg. Rate");
		printf("| %14.4g s | %14.4g s | %16.4g |\n",
			real_time, timer_time, rate_avg);
		printf("| %-16s | %-16s | %-16s |\n", "Overtimes:", "Max. Call Lag:", "Avg. Call Lag:");
		printf("| %16zu | %13.4g ms | %13.4g ms |\n",
			 overtime_count, std::milli::den * call_lag_max, std::milli::den * call_lag_avg);
		printf("| %-16s | %-16s | %-16s |\n", "Overtime %", "Max. Elapsed:", "Avg. Elapsed:");
		printf("| %15.4g%% | %13.4g ms | %13.4g ms |\n",
			static_cast<Real_T>(overtime_count) / call_count * 100, std::milli::den * act_elapsed_max, std::milli::den * act_elapsed_avg);
		// clang-format on
		fflush(stdout);
	}

  private:
	static constexpr size_t clinfo_length = 6;
	bool never_sampled = true;
	time_sc start_time;
	Real_T real_time;
	Real_T timer_time;
	Real_T call_lag_max;
	Real_T act_elapsed_max;
	size_t call_count;
	size_t overtime_count;
	Real_T rate_avg;
	Real_T call_lag_avg;
	Real_T act_elapsed_avg;
	rt_timer::Timer<Action> &timer;
};

int
main()
{
	rt_timer::set_process_priority();

	printf("Set a timer to call a %.5g ms long function to execute every %.5g ms,\nand sample the timer every %.5g "
	       "seconds. Press any key to stop...\n",
	       std::milli::den * timer_period /2, std::milli::den * timer_period, sample_period);

	/** create a timer thread to call the action periodically */
	Action action;
	rt_timer::Timer<Action> action_timer(timer_period, action, &Action::fun);
	rt_timer::TimerThread<Action> action_thread(action_timer);

	/** create a second timer thread to sample the previous timer periodically */
	Sampler sampler(action_timer);
	rt_timer::Timer<Sampler> sampler_timer(sample_period, sampler, &Sampler::sample);
	rt_timer::TimerThread<Sampler> sampler_thread(sampler_timer);

	/** start the timer threads */
	action_thread.start();
	sampler_thread.start();

	/** wait for key press to stop the timer threads */
	std::getchar();
	action_thread.stop();
	sampler_thread.stop();

	return 0;
}