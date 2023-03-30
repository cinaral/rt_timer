#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

using Real_T = rt_timer::Real_T;
using ns = rt_timer::ns;
using std::chrono::steady_clock;
using time_sc = steady_clock::time_point;

constexpr Real_T action_rate = 1e4;                   //* [Hz]
constexpr Real_T timer_period = 1. / action_rate;     //* [s]
constexpr size_t test_duration = 1;                   //* [s]
constexpr Real_T error_thres = 1e-4;                  //* [s]
constexpr size_t overtime_thres = action_rate * .001; //* maximum .1% error rate

/* This is a class that is used to perform an action */
class Action
{
  public:
	void
	fun()
	{
		const auto now_time = rt_timer::clock::now();
		const Real_T task_duration = std::nano::den * max_period;

		while (rt_timer::clock::now() - now_time < ns(static_cast<size_t>(task_duration))) {
			/** do something */
			++counter;
		}
	}

  private:
	static constexpr Real_T max_period = .5 * timer_period;
	size_t counter = 0;
};

int
main()
{
	rt_timer::set_process_priority();

	/** create a timer thread to call the action periodically for the test duration */
	Action action;
	rt_timer::Timer<Action> action_timer(timer_period, action, &Action::fun);

	std::thread action_thread([&action_timer] {
		const auto start_time = steady_clock::now();

		while (steady_clock::now() - start_time < std::chrono::seconds(test_duration)) {
			action_timer.check();
		}
	});
	action_thread.join();

	/** sample the timer */
	Real_T timer_time;
	Real_T rate;
	Real_T avg_elapsed;
	Real_T max_elapsed;
	size_t call_count;
	size_t overtime_count;
	action_timer.sample(timer_time, rate, avg_elapsed, max_elapsed, call_count, overtime_count);

	/** print the results */
	printf("| %-16s | %-16s |\n", "Rate:", "Call count:");
	printf("| %13.5g Hz | %16zu |\n", rate, call_count);

	printf("| %-16s | %-16s |  \n", "Time:", "Overtime ct.:");
	printf("| %14.5g s | %16zu | \n", timer_time, overtime_count);

	printf("| %-16s | %-16s |\n", "Max. Elapsed:", "Avg. Elapsed:");
	printf("| %13.5g ms | %13.5g ms | \n", std::milli::den * max_elapsed,
	       std::milli::den * avg_elapsed);

	//** verify */
	if (call_count == test_duration * action_rate && overtime_count < overtime_thres &&
	    avg_elapsed < error_thres) {
		return 0;
	} else {
		return 1;
	}
}