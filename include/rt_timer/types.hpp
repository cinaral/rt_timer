/*
 * rt_timer
 *
 * MIT License
 *
 * Copyright (c) 2022 Cinar, A. L.
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

#ifndef TYPES_HPP_CINARAL_220926_2329
#define TYPES_HPP_CINARAL_220926_2329

#include <chrono>
#include <cstddef>

namespace rt_timer
{
using size_t = std::size_t;
#ifdef USE_SINGLE_PRECISION
using Real_T = float;
#else
using Real_T = double;
#endif

using clock = std::chrono::high_resolution_clock;
using steady_clock = std::chrono::steady_clock;
using ns = std::chrono::nanoseconds;
using time = clock::time_point;
using stopwatch = clock::duration;

using std::chrono::duration;
typedef duration<size_t, std::nano> Duration_T;

template <typename Action_T> using ActionFun_T = void (Action_T::*)();
} // namespace rt_timer
#endif
