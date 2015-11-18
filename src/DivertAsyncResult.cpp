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

#include "DivertAsyncResult.hpp"

namespace Divert
{
	namespace Net
	{

		DivertAsyncResult::DivertAsyncResult()
		{
			
		}

		DivertAsyncResult::~DivertAsyncResult()
		{
			this->!DivertAsyncResult();
		}

		DivertAsyncResult::!DivertAsyncResult()
		{
			if (m_buffer.IsAllocated)
			{
				m_buffer.Free();
			}		

			if (m_overlapped != nullptr)
			{
				free(m_overlapped);
			}
		}

		bool DivertAsyncResult::NoError::get()
		{
			return m_noError;
		}

		void DivertAsyncResult::NoError::set(bool value)
		{
			m_noError = value;
		}

		int DivertAsyncResult::ErrorCode::get()
		{
			return m_errorCode;
		}

		void DivertAsyncResult::ErrorCode::set(int value)
		{
			m_errorCode = value;
		}	

		uint32_t DivertAsyncResult::Length::get()
		{
			return m_ioLength;
		}

		void DivertAsyncResult::Length::set(uint32_t value)
		{
			m_ioLength = value;
		}

		bool DivertAsyncResult::Get(uint32_t timeoutInMilliseconds)
		{
			// We don't want to destroy the app if somehow a single invalid handle
			// got created. So, we return false, set noError to false to let the user
			// know the operation failed. That's all that can be done.
			if (m_overlappedEventHandle == nullptr ||
				m_overlappedEventHandle->GetUnmanagedHandle() == nullptr || 
				m_overlappedEventHandle->GetUnmanagedHandle() == INVALID_HANDLE_VALUE
				)
			{
				m_noError = false;
				return false;
			}

			// We don't want to destroy the app if somehow a single invalid handle
			// got created. So, we return false, set noError to false to let the user
			// know the operation failed. That's all that can be done.
			if (m_winDivertHandle == nullptr ||
				m_winDivertHandle->GetUnmanagedHandle() == nullptr ||
				m_winDivertHandle->GetUnmanagedHandle() == INVALID_HANDLE_VALUE
				)
			{
				m_noError = false;
				return false;
			}

			switch (WaitForSingleObject(m_overlappedEventHandle->GetUnmanagedHandle(), timeoutInMilliseconds))
			{
				case WAIT_OBJECT_0:
					m_noError = true;
					break;
				case WAIT_TIMEOUT:
					m_noError = false;
					m_errorCode = System::Runtime::InteropServices::Marshal::GetLastWin32Error();
				default:
					m_noError = false;
			}

			if (m_noError == true)
			{
				DWORD ioLength;
				if (!GetOverlappedResult(m_winDivertHandle->GetUnmanagedHandle(), m_overlapped, &ioLength, TRUE))
				{
					m_errorCode = System::Runtime::InteropServices::Marshal::GetLastWin32Error();
					m_noError = false;

					// Close the overlapped event handle, not needed anymore.
					m_overlappedEventHandle->Close();

					// Don't need to keep the buffer pinned anymore.
					if (m_buffer.IsAllocated)
					{
						m_buffer.Free();
					}

					return false;
				}

				m_ioLength = static_cast<uint32_t>(ioLength);

				// Close the overlapped event handle, not needed anymore.
				m_overlappedEventHandle->Close();

				// Don't need to keep the buffer pinned anymore.
				if (m_buffer.IsAllocated)
				{
					m_buffer.Free();
				}

				return true;
			}
		}

		System::Runtime::InteropServices::GCHandle DivertAsyncResult::Buffer::get()
		{
			return m_buffer;
		}

		void DivertAsyncResult::Buffer::set(System::Runtime::InteropServices::GCHandle value)
		{
			m_buffer = value;
		}		

		DivertHandle^ DivertAsyncResult::WinDivertHandle::get()
		{
			return m_winDivertHandle;
		}

		void DivertAsyncResult::WinDivertHandle::set(DivertHandle^ value)
		{
			m_winDivertHandle = value;
		}

		DivertHandle^ DivertAsyncResult::OverlappedEventHandle::get()
		{
			return m_overlappedEventHandle;
		}

		void DivertAsyncResult::OverlappedEventHandle::set(DivertHandle^ value)
		{
			m_overlappedEventHandle = value;
		}

		OVERLAPPED* DivertAsyncResult::GetUnmanagedOverlapped()
		{
			return m_overlapped;
		}

		void DivertAsyncResult::Init()
		{
			m_overlapped = static_cast<OVERLAPPED*>(malloc(sizeof(OVERLAPPED)));
		}

		bool DivertAsyncResult::Reset()
		{
			// Reset the overlapped object
			memset(m_overlapped, 0, sizeof(*m_overlapped));

			m_noError = false;
			m_errorCode = 0;
			m_ioLength = 0;

			// Create the overlapped event handle
			HANDLE overlappedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			// If this failed, all is lost. Set m_errorCode to GetLastError
			if (overlappedEvent == nullptr)
			{
				m_errorCode = System::Runtime::InteropServices::Marshal::GetLastWin32Error();
				return false;
			}			

			// Set the event property on overlapped
			m_overlapped->hEvent = overlappedEvent;

			// Create a new handle to wrap the newly created overlapped event. The DivertHandle wrapper
			// will automatically close handles when destroyed.
			m_overlappedEventHandle = gcnew DivertHandle(overlappedEvent, false);

			return true;
		}

	} /* namespace Net */
} /* namespace Divert  */