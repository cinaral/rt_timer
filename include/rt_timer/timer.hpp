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
	Timer(const Real_T timer_period)
	    : timer_period(ns(static_cast<size_t>(std::nano::den * timer_period)))
	{
		start_time = clock::now();
		prev_sample_time = clock::now();
	};

	void
	check()
	{
		now_time = clock::now();

		if (now_time >= act_time) {
			(action.*fun)();
			const auto elapsed = clock::now() - now_time;

			if (elapsed > timer_period) {
				++overtime_counter;
			}
			if (elapsed > max_act_elapsed) {
				max_act_elapsed = elapsed;
			}
			act_elap_sum += elapsed;
			++act_counter;
			act_time = start_time + act_counter * timer_period;
		}
	}

	void
	sample(Real_T &rate, Real_T &avg_elapsed, Real_T &max_elapsed, size_t &over_time_ct)
	{
		max_elapsed = duration<Real_T>(max_act_elapsed).count();
		over_time_ct = overtime_counter;

		const auto act_ct_diff = act_counter - prev_act_count;
		const auto elapsed = now_time - prev_sample_time;

		if (elapsed > ns(0) && act_ct_diff > 0) {
			rate = static_cast<Real_T>(act_ct_diff) / duration<Real_T>(elapsed).count();

			const auto elap_sum_diff = act_elap_sum - prev_act_elap_sum;
			avg_elapsed = duration<Real_T>(elap_sum_diff).count() / act_ct_diff;

			prev_act_count = act_counter;
			prev_act_elap_sum = act_elap_sum;
			prev_sample_time = now_time;
		} else {
			rate = this->rate;
			avg_elapsed = this->avg_elapsed;
		}
	}

  private:
	const Action_T action;
	const ActionFun_T<Action_T> fun;
	const stopwatch timer_period;
	Real_T rate = 0;
	Real_T avg_act_elapsed = 0;
	size_t act_counter = 0;
	size_t prev_act_count = 0;
	size_t overtime_counter = 0;
	time now_time;
	time act_time;
	time start_time;
	time prev_sample_time;
	time prev_avg_sample_time;
	stopwatch prev_act_elap_sum;
	stopwatch act_elap_sum;
	stopwatch max_act_elapsed;
};
} // namespace rt_timer
#endif