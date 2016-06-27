/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Jesse Nicholson
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
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

#pragma once

#pragma managed(push, off)
#include <cstdlib>
#include <type_traits>
inline uint32_t ByteSwapUInt32(uint32_t val)
{
	return (val & 0x000000FFU) << 24 | (val & 0x0000FF00U) << 8 |
		(val & 0x00FF0000U) >> 8 | (val & 0xFF000000U) >> 24;
}


inline uint16_t ByteSwapUInt16(uint16_t val)
{
	return _byteswap_ushort(val);
}


template<typename T>
static T ByteSwap(T val)
{
	// Ensure supported types at compile time.
	static_assert((std::is_same<T, uint32_t>::value || std::is_same<T, uint16_t>::value), "Unsupported type!");

	if (std::is_same<T, uint32_t>::value)
	{
		return ByteSwapUInt32(val);
	}

	// std::is_same<T, uint16_t>::value must be true
	return ByteSwapUInt16(val);
}


#pragma managed(pop)