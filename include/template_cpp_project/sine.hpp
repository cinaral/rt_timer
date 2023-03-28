/*
 * template_cpp_project
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

#ifndef SINE_HPP_CINARAL_220923_1711
#define SINE_HPP_CINARAL_220923_1711

#include "types.hpp"
#include <cmath>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

namespace template_cpp_project
{
/* Generates a sine wave.
 *
 * `sine(f, t_arr, OUT:x_arr)`
 *
 * 1. `f`: frequency
 * 2. `t_arr`: [T_DIM] input time array
 *
 * OUT:
 * 3. `x_arr`: [T_DIM] sine time array
 */
template <size_t T_DIM>
void
sine(const Real_T f, const Real_T (&t_arr)[T_DIM], Real_T (&x_arr)[T_DIM])
{
	for (size_t i = 0; i < T_DIM; ++i) {
#ifdef USE_SINGLE_PRECISION
		x_arr[i] = sinf(t_arr[i] * 2 * M_PI * f);
#else
		x_arr[i] = sin(t_arr[i] * 2 * M_PI * f);
#endif
	}
}

} // namespace template_cpp_project

#endif
