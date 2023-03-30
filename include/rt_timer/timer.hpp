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

#ifndef RT_TIMER_HPP_CINARAL_230328_1603
#define RT_TIMER_HPP_CINARAL_230328_1603

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
	 * Check if the timer period has elapsed. If so, call the action function.
	 */
	void
	check()
	{
		now_time = clock::now();

		if (never_checked) {
			never_checked = false;
			start_time = now_time;
			call_time = start_time;
			prev_sample_time = start_time;
		}

		if (now_time >= call_time) {
			(action.*fun)();

			const auto elapsed = clock::now() - now_time;
			call_elap_sum += elapsed;

			if (elapsed > timer_period) {
				++overtime_counter;
			}

			if (elapsed > max_call_elapsed) {
				max_call_elapsed = elapsed;
			}

			++call_counter;
			call_time = start_time + call_counter * timer_period;
		}
	}

	/*`sample()`:
	 * Sample the timer's rate, average elapsed time, maximum elapsed time and the number of
	 * times the action function took longer than the timer period.
	 */
	void
	sample(Real_T &timer_time, Real_T &rate, Real_T &avg_elapsed, Real_T &max_elapsed,
	       size_t &call_count, size_t &overtime_count)
	{
		/** these can be reported at any sample time: */
		timer_time = duration<Real_T>((call_counter - 1) * timer_period).count();
		max_elapsed = duration<Real_T>(max_call_elapsed).count();
		call_count = call_counter;
		overtime_count = overtime_counter;

		/** however, rate and avg_elapsed can only be reported after a couple of samples: */
		const auto elapsed = now_time - prev_sample_time;
		const auto call_ct_diff = call_counter - prev_call_count;

		if (elapsed > ns(0) && call_ct_diff > 0) {
			rate =
			    static_cast<Real_T>(call_ct_diff) / duration<Real_T>(elapsed).count();

			const auto elap_sum_diff = call_elap_sum - prev_call_elap_sum;
			avg_elapsed = duration<Real_T>(elap_sum_diff).count() / call_ct_diff;

			prev_sample_time = now_time;
			prev_call_count = call_counter;
			prev_call_elap_sum = call_elap_sum;
		} else {
			rate = this->rate;
			avg_elapsed = this->avg_act_elapsed;
		}
	}

  private:
	const stopwatch timer_period;
	Action_T &action;
	const ActionFun_T<Action_T> fun;
	Real_T rate = 0;
	Real_T avg_act_elapsed = 0;
	size_t call_counter = 0;
	size_t prev_call_count = 0;
	size_t overtime_counter = 0;
	stopwatch max_call_elapsed = ns(0);
	stopwatch call_elap_sum = ns(0);
	stopwatch prev_call_elap_sum = ns(0);
	bool never_checked = true;
	time now_time;
	time call_time;
	time start_time;
	time prev_sample_time;
};
} // namespace rt_timer
#endif