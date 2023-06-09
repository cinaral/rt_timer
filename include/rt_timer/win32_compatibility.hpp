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

#ifndef WIN32_COMPATIBILITY_HPP_CINARAL_230330_0047
#define WIN32_COMPATIBILITY_HPP_CINARAL_230330_0047

#if defined(__WIN32__) || defined(__WIN32) || defined(_WIN32) || defined(WIN32) || \
    defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define WIN32
#endif

#include <cstdio>

#ifdef WIN32
	#include <windows.h>
#endif

namespace rt_timer
{
inline void
set_process_priority()
{
#ifndef WIN32
		//? what do:   #include <sys/resource.h>... int which = PRIO_PROCESS;
		// setpriority(which,
		// pid, priority); etc
#else
	//? REALTIME_PRIORITY_CLASS gives HIGH_PRIORITY_CLASS priority
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
		printf("Failed to set priority: %lu\n", GetLastError());
		// printf("Current thread priority is %lu\n",
		// GetPriorityClass(GetCurrentProcess()));
	}
#endif
}
} // namespace rt_timer

#endif