- [1. About rt_timer: Real(ish)-Time Timer](#1-about-rttimer-realish-time-timer)
- [2. Usage](#2-usage)
	- [2.1. Install](#21-install)
	- [2.2. Using a timer thread](#22-using-a-timer-thread)
	- [2.3. Using the timer](#23-using-the-timer)
	- [2.4. Verify the timer](#24-verify-the-timer)
	- [2.5. Stop](#25-stop)
	- [2.6. Example](#26-example)
- [3. To do](#3-to-do)

# 1. About rt_timer: Real(ish)-Time Timer 
1. An accurate timer to execute code as real-time as *the operating system allows*. 
2. Your experience may vary, but it will try its best. Test it out!
3. When running it on a regular OS, it will benefit from giving the process and the thread the highest possible priority.
4. Can be used to simulate hard real-time execution verifiably, so you can terminate the computation at the first detected instance of a missed deadline. 
5. Whether this library can realize hard real-time depends on the hardware, the OS, and the task.
6. If you must have reliable results, use a real-time microprocessor or a Linux system with PREEMPT_RT kernel. 
7. If the timing requirements are less than hard real-time, you can easily quantify the timer's performance for design.

# 2. Usage
## 2.1. Install
This is a header-only library. You can include or copy the ```include/``` folder to your project. You should use CMake and ```FetchContent```:
```CMake
	Include(FetchContent)
	FetchContent_Declare(
		rt_timer 
		URL https://github.com/cinaral/rt_timer/releases/download/v${rt_timer_VERSION}/src.zip
	)
	FetchContent_MakeAvailable(rt_timer)
```

## 2.2. Using a timer thread
When you want to call a function periodically as real-time as possible, create a timer and a timer thread, and start it:
```cpp
	/** create a timer thread to call the action periodically */
	rt_timer::Timer<Action_T> action_timer(timer_period, action, &Action_T::fun);
	rt_timer::TimerThread<Action_T> action_thread(action_timer);

	/** start the timer thread */
	action_thread.start();
```
## 2.3. Using the timer
If you do not want to use a timer thread, you can instead create a timer object and call it periodically:
```cpp
	/** create a timer thread to call the action periodically */
	rt_timer::Timer<Action_T> action_timer(timer_period, action, &Action_T::fun);

	while(running) {
		action_timer();
	}
```

## 2.4. Verify the timer
Sample the timer's performance periodically using another time thread:
```cpp
	/** create a second timer thread to sample the previous timer periodically */
	Sampler sampler(action_timer);
	rt_timer::Timer<Sampler> sampler_timer(sample_period, sampler, &Sampler::sample);
	rt_timer::TimerThread<Sampler> sampler_thread(sampler_timer);
	sampler_thread.start();
```

## 2.5. Stop
```cpp
	action_thread.stop();
	sampler_thread.stop();
```
## 2.6. Example
You can find the complete example in [rt_timer-example.cpp](./examples/rt_timer.cpp).

# 3. To do
1. Add/improve ```set_process_priority()``` for more platforms
2. A benchmark to identify the highest possible call frequency for a given task, hardware, and platform.
3. Test it out on a real-time microprocessor.
