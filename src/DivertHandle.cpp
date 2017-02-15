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

#include "DivertHandle.hpp"

namespace Divert
{
	namespace Net
	{

		DivertHandle::DivertHandle()
		{

		}

		DivertHandle::~DivertHandle()
		{
			this->!DivertHandle();
		}

		DivertHandle::!DivertHandle()
		{
			Close();

			m_handle = INVALID_HANDLE_VALUE;
		}

		DivertHandle::DivertHandle(HANDLE handle, const bool fromWinDivert)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(handle != nullptr, u8"In DivertHandle::DivertHandle(HANDLE) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			m_handle = handle;
			m_fromWinDivert = fromWinDivert;
		}

		bool DivertHandle::Close()
		{
			if (Valid)
			{
				BOOL result = false;
				if (m_fromWinDivert)
				{
					result = WinDivertClose(m_handle) == 1;					
				}
				else
				{
					result = CloseHandle(m_handle) != 0;					
				}

				if (result)
				{
					m_handle = INVALID_HANDLE_VALUE;
				}

				return result;
			}
			else
			{
				return false;
			}
		}

		bool DivertHandle::Valid::get()
		{
			return m_handle != INVALID_HANDLE_VALUE;
		}

		HANDLE DivertHandle::UnmanagedHandle::get()
		{
			return m_handle;
		}

		void DivertHandle::UnmanagedHandle::set(HANDLE value)
		{
			m_handle = value;
		}

	} /* namespace Net */
} /* namespace Divert  */
