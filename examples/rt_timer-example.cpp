#include "cxxopts.hpp"
#include "rt_timer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>

using Size = rt_timer::Size;
using Real = rt_timer::Real;
using ns = rt_timer::ns;
using std::chrono::duration;
using std::chrono::steady_clock;
using time_sc = steady_clock::time_point;

const std::string action_rate_default = std::to_string(1e4);
const std::string sample_rate_default = std::to_string(.8);
const std::string action_factor_default = std::to_string(.1);

/* This is a class that is used to perform an action */
class Action
{
  public:
	Action(const Real action_duration) : action_duration(std::nano::den * action_duration){};

	void
	fun()
	{

		const auto now_time = rt_timer::clock::now();
		/** the action duration is half of the timer period */
		while (rt_timer::clock::now() - now_time < ns(static_cast<Size>(action_duration))) {
			/** do something */
			++counter;
		}
	}

  private:
	const Real action_duration;
	Size counter = 0;
};

/* This is a class that is used to sample the timer and print the results */
class Sampler
{
  public:
	Sampler(rt_timer::Timer<Action> &timer) : timer(timer)
	{
		/** initialize the command line info priting: */
		for (Size i = 0; i < clinfo_length; ++i) {
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
		timer.sample(timer_time, call_lag_max, act_elapsed_max, call_count, rt_viol_count,
		             rate_avg, call_lag_avg, act_elapsed_avg);

		if (never_sampled) {
			never_sampled = false;
			start_time = steady_clock::now();
		}
		Realime = duration<Real>(steady_clock::now() - start_time).count();

		/** print the results */
		for (Size i = 0; i < clinfo_length; ++i) {
			printf("\033[A\033[2K\r"); //* move the cursor up then clear the line
		}
		// clang-format off
		printf("| %-16s |\n", "Real Time:");
		printf("| %14.4g s |\n", Realime);
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
	}

  private:
	static constexpr Size clinfo_length = 6;
	bool never_sampled = true;
	time_sc start_time;
	Real Realime;
	Real timer_time;
	Real call_lag_max;
	Real act_elapsed_max;
	Size call_count;
	Size rt_viol_count;
	Real rate_avg;
	Real call_lag_avg;
	Real act_elapsed_avg;
	rt_timer::Timer<Action> &timer;
};

int
main(int argc, char const *argv[])
{
	cxxopts::Options options("rt_timer example.\n");
	options.show_positional_help();
	// clang-format off
	options.add_options()
		("h,help", "Print usage")
		("a,action-rate", "Action frequency [hz]", cxxopts::value<std::string>()->default_value(action_rate_default))
		("s,sampling-rate", "Sampling frequency [hz]", cxxopts::value<std::string>()->default_value(sample_rate_default))
		("d,duration-factor", "Action duration as a ratio of timer period", cxxopts::value<std::string>()->default_value(action_factor_default));
	// clang-format on
	auto result = options.parse(argc, argv);
	const Real action_rate = std::stod(result["action-rate"].as<std::string>());
	const Real sample_rate = std::stod(result["sampling-rate"].as<std::string>());
	const Real duration_factor = std::stod(result["duration-factor"].as<std::string>());

	if (result.count("help")) {
		printf("%s", options.help().c_str());
		return 0;
	}

	if (sample_rate > action_rate) {
		printf("The action rate must be greater than or equal to the sample rate.\n");
		return 1;
	}
	const Real timer_period = 1. / action_rate;  //* [s]
	const Real sample_period = 1. / sample_rate; //* [s]
	const Real action_duration = duration_factor * timer_period;

	rt_timer::set_process_priority();

	printf("Timer to call %.5g ms long function to execute every %.5g ms,\nand sample the "
	       "timer every %.5g "
	       "seconds. Press any key to stop...\n",
	       std::milli::den * action_duration, std::milli::den * timer_period, sample_period);

	/** create a timer thread to call the action periodically */
	Action action(action_duration);
	rt_timer::Timer action_timer(timer_period, action, &Action::fun);
	rt_timer::TimerThread action_thread(action_timer);

	/** create a second timer thread to sample the previous timer periodically */
	Sampler sampler(action_timer);
	rt_timer::Timer sampler_timer(sample_period, sampler, &Sampler::sample);
	rt_timer::TimerThread sampler_thread(sampler_timer);

	/** start the timer threads */
	action_thread.start();
	sampler_thread.start();

	/** wait for key press to stop the timer threads */
	std::getchar();
	action_thread.stop();
	sampler_thread.stop();

	return 0;
}