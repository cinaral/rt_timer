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

#ifndef TIMER_THREAD_HPP_CINARAL_230330_1303
#define TIMER_THREAD_HPP_CINARAL_230330_1303

#include "timer.hpp"
#include <atomic>
#include <thread>

namespace rt_timer
{
template <typename Action_T> class TimerThread
{
  public:
	TimerThread(Timer<Action_T> &timer) : timer(timer){};
	~TimerThread()
	{
		stop();
	}

	void
	start()
	{
		if (!running) {
			running = true;
			thread = std::thread([this] {
				while (running) {
					call_time = timer.check();
					//! This breaks action rates between 100-900 Hz, probably due to scheduling
					//std::this_thread::sleep_until(call_time);
				}
			});
		}
	}

	void
	run_for(stopwatch stopwatch_limit)
	{
		start();
		const auto start_time = clock::now();
		std::this_thread::sleep_until(start_time + stopwatch_limit);
		stop();
	}

	void
	stop()
	{
		if (running) {
			running = false;
			thread.join();
		}
	}

  private:
	Timer<Action_T> &timer;
	std::thread thread;
	time call_time;
	std::atomic<bool> running = false;
};
} // namespace rt_timer

#endif