/*
 * rt_timer
 *
 * MIT License
 *
 * Copyright (c) 2023 Cinar, A. L.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TIMER_HPP_CINARAL_230328_1603
#define TIMER_HPP_CINARAL_230328_1603

#include "types.hpp"
#include <chrono>

namespace rt_timer
{
using std::chrono::duration;

template <typename Action_T> class Timer
{
  public:
	Timer(const Real_T timer_period, Action_T &action, const ActionFun_T<Action_T> fun)
	    : timer_period(ns(static_cast<size_t>(std::nano::den * timer_period))), action(action),
	      fun(fun){};

	/*`check()`:
	 * Check if the timer period has elapsed. If so, call the action function. Returns
	 * approximate time until the next call time.
	 */
	time
	check()
	{
		now_time = clock::now();

		if (never_checked) {
			never_checked = false;
			call_time = now_time;
			prev_sample_time = now_time;
		}

		if (now_time >= call_time) {
			(action.*fun)();
			const auto act_elapsed = clock::now() - now_time;
			const auto lag = now_time - call_time;

			act_elapsed_sum += act_elapsed;
			call_lag_sum += lag;

			if (act_elapsed > act_elapsed_max) {
				act_elapsed_max = act_elapsed;
			}

			if (lag > call_lag_max) {
				call_lag_max = lag;
			}

			if (lag > timer_period) {
				++overtime_count;
			}

			++call_count;
			call_time += timer_period;
		}
		return call_time;
	}

	/*`sample()`:
	 * Sample the timer's rate, average elapsed time, maximum elapsed time and the
	 * number of times the action function took longer than the timer period.
	 */
	void
	sample(Real_T &timer_time, Real_T &call_lag_max, Real_T &act_elapsed_max,
	       size_t &call_count, size_t &overtime_count, Real_T &rate_avg, Real_T &call_lag_avg,
	       Real_T &act_elapsed_avg)
	{
		/** these can be reported at any sample time: */
		timer_time = duration<Real_T>(this->call_count * timer_period).count();
		call_lag_max = duration<Real_T>(this->call_lag_max).count();
		act_elapsed_max = duration<Real_T>(this->act_elapsed_max).count();
		call_count = this->call_count;
		overtime_count = this->overtime_count;

		/* however, rate and avg_elapsed can only be reported after a couple of
		 * samples: */
		const auto since_sample = now_time - prev_sample_time;
		const auto call_ct_diff = this->call_count - call_count_prev;

		if (since_sample > ns(0) && call_ct_diff > 0) {
			rate_avg = static_cast<Real_T>(call_ct_diff) /
			    duration<Real_T>(since_sample).count();

			const auto call_lag_sum_diff = call_lag_sum - call_lag_prev_sum;
			call_lag_avg = duration<Real_T>(call_lag_sum_diff).count() / call_ct_diff;

			const auto act_elap_sum_diff = act_elapsed_sum - act_elap_prev_sum;
			act_elapsed_avg =
			    duration<Real_T>(act_elap_sum_diff).count() / call_ct_diff;

			prev_sample_time = now_time;
			call_count_prev = this->call_count;
			call_lag_prev_sum = call_lag_sum;
			act_elap_prev_sum = act_elapsed_sum;
		} else {
			rate_avg = this->rate_avg;
			call_lag_avg = this->call_lag_avg;
			act_elapsed_avg = this->act_elapsed_avg;
		}
	}

  private:
	const stopwatch timer_period;
	Action_T &action;
	const ActionFun_T<Action_T> fun;
	bool never_checked = true;
	time now_time;
	time call_time;
	time prev_sample_time;
	size_t call_count = 0;
	size_t overtime_count = 0;
	size_t call_count_prev = 0;
	Real_T rate_avg = 0;
	Real_T call_lag_avg = 0;
	Real_T act_elapsed_avg = 0;
	stopwatch call_lag_max = ns(0);
	stopwatch call_lag_sum = ns(0);
	stopwatch call_lag_prev_sum = ns(0);
	stopwatch act_elapsed_max = ns(0);
	stopwatch act_elapsed_sum = ns(0);
	stopwatch act_elap_prev_sum = ns(0);
};
} // namespace rt_timer
#endif