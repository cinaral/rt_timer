#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

using Real_T = rt_timer::Real_T;
using ns = rt_timer::ns;
using std::chrono::steady_clock;
using time_sc = steady_clock::time_point;

constexpr Real_T action_rate = 1e3;                   //* [Hz]
constexpr Real_T timer_period = 1. / action_rate;     //* [s]
constexpr Real_T action_duration = .1 * timer_period; //* [s]
constexpr size_t test_duration = 1;                   //* [s]

/* This is a class that is used to perform an action */
class Action
{
  public:
	void
	fun()
	{
		const auto now_time = rt_timer::clock::now();
		/** the action duration is half of the timer period */
		
		while (rt_timer::clock::now() - now_time < ns(static_cast<size_t>(std::nano::den * action_duration))) {
			/** do something */
			++counter;
		}
	}

  private:
	size_t counter = 0;
};

int
main()
{
	rt_timer::set_process_priority();

	/** create a timer thread to call the action periodically */
	Action action;
	rt_timer::Timer action_timer(timer_period, action, &Action::fun);
	rt_timer::TimerThread action_thread(action_timer);

	/** start the timer thread for timed duration */
	action_thread.run_for(std::chrono::seconds(test_duration));

	/** sample the timer */
	Real_T timer_time;
	Real_T call_lag_max;
	Real_T act_elapsed_max;
	Real_T rate_avg;
	Real_T call_lag_avg;
	Real_T act_elapsed_avg;
	size_t call_count;
	size_t rt_viol_count;
	action_timer.sample(timer_time, call_lag_max, act_elapsed_max, call_count, rt_viol_count,
	                    rate_avg, call_lag_avg, act_elapsed_avg);

	// clang-format off
	printf("| %-16s | %-16s | %-16s | %-16s |\n",  
	"Timer Time:", "RT Violations:", "Max. Call Lag:", "Avg. Call Lag:");
	printf("| %14.4g s | %16zu | %13.4g ms | %13.4g ms |\n", 
	timer_time, rt_viol_count, std::milli::den * call_lag_max, std::milli::den * call_lag_avg);
	printf("| %-16s | %-16s | %-16s | %-16s |\n", 
	"Avg. Rate", "Violation Ratio", "Max. Elapsed:", "Avg. Elapsed:");
	printf("| %13.4g Hz | %15.4g%% | %13.4g ms | %13.4g ms |\n", 
	rate_avg, static_cast<double>(rt_viol_count) / call_count * 100, std::milli::den * act_elapsed_max, std::milli::den * act_elapsed_avg);
	// clang-format on
	fflush(stdout);

	//** verify */
	if (rt_viol_count < 1 && call_lag_max < timer_period) {
		return 0;
	} else {
		return 1;
	}
}