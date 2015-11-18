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

#include "Diversion.hpp"
#include <vcclr.h>

namespace Divert
{
	namespace Net
	{

		Diversion^ Diversion::Open(System::String^ filter, DivertLayer layer, uint16_t priority, FilterFlags^ flags)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			// Check if the string is null or empty. Empty filter strings are not supported, AFAIK.
			if (System::String::IsNullOrEmpty(filter) || System::String::IsNullOrWhiteSpace(filter))
			{			
				e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - Supplied filter string is null, empty or whitespace.");
				throw e;
			}

			Diversion^ diversion = gcnew Diversion();
			
			// Marshal the supplied string to a native C string for supplying to WinDivertOpen
			const char* charString = static_cast<const char*>((System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filter)).ToPointer());

			if (charString == nullptr)
			{
				e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - Failed to marshal filter string.");
				throw e;
			}
						
			uint64_t flagsInt = (uint64_t)System::Convert::ChangeType(flags, System::UInt64::typeid);
			
			HANDLE divertHandle = WinDivertOpen(charString, (WINDIVERT_LAYER)layer, priority, flagsInt);
			
			if (divertHandle == INVALID_HANDLE_VALUE)
			{
				int error = System::Runtime::InteropServices::Marshal::GetLastWin32Error();

				switch (error)
				{
					case ERROR_FILE_NOT_FOUND:
					{
						e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - The driver files WinDivert32.sys or WinDivert64.sys were not found.");
					}
					break;

					case ERROR_ACCESS_DENIED:
					{
						e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - The calling application does not have Administrator privileges.");
					}
					break;

					case ERROR_INVALID_PARAMETER:
					{
						e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - Filter string, layer, priority, or flags parameters contain invalid values.");
					}
					break;

					case ERROR_INVALID_IMAGE_HASH:
					{
						e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - The WinDivert32.sys or WinDivert64.sys driver does not have a valid digital signature.");
					}
					break;

					case ERROR_DRIVER_BLOCKED:
					{
						e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - The driver is blocked from operating. This can happen for various reasons, such as interference from security software, or usage inside a virtualization environment that does not support drivers.");
					}
					break;

					default:

						e = gcnew System::Exception(u8"In Diversion::Open(System::String^, DivertLayer, uint16_t, uint64_t) - Failed to open WinDivert HANDLE. Cause is unknown. Use Marshal::GetLastWin32Error() for more information.");
				}
				
				throw e;
			}

			// Initialize a managed handle, indicating that the handle was obtained through WinDivertOpen
			// by passing "true" as the second constructor argument. Must be done so that the managed object
			// can properly manage the unmanaged handle. Geeze, say managed again!
			DivertHandle^ handle = gcnew DivertHandle(divertHandle, true);

			// Assign the handle member to the created, valid handle.
			diversion->Handle = handle;

			// Free the marshalled filter string
			System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)charString));

			return diversion;
		}

		Diversion::Diversion()
		{

		}

		Diversion::Diversion(DivertHandle^ handle)
		{
			Handle = handle;
		}

		Diversion::~Diversion()
		{
			this->!Diversion();
		}

		Diversion::!Diversion()
		{
			delete m_handle;
		}

		DivertHandle^ Diversion::Handle::get()
		{
			return m_handle;
		}	

		void Diversion::Handle::set(DivertHandle^ value)
		{
			m_handle = value;
		}

		bool Diversion::Receive(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% receiveLength)
		{
			System::Exception^ e = nullptr;

			if (packetBuffer->Length == 0)
			{
				e = gcnew System::Exception(u8"In Diversion::Receive(array<System::Byte>^, Address^, out uint32_t) - Supplied buffer has a length of zero. Not possible to read in to.");
				throw e;
			}

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			unsigned char* pb = static_cast<unsigned char*>(byteArray);

			if (pb == nullptr)
			{
				// This shouldn't every happen, but TRUST NO 1
				e = gcnew System::Exception(u8"In Diversion::Receive(array<System::Byte>^, Address^, out uint32_t) - Failed to pin packet buffer.");
				throw e;
			}
			
			uint32_t readLen = 0;

			int result = WinDivertRecv(m_handle->GetUnmanagedHandle(), pb, packetBuffer->Length, address->GetUnmanagedAddress(), &readLen);

			receiveLength = readLen;

			return result == 1;
		}

		bool Diversion::ReceiveAsync(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% receiveLength, DivertAsyncResult^ asyncResult)
		{
			System::Exception^ e = nullptr;

			if (packetBuffer->Length == 0)
			{
				e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Supplied buffer has a length of zero. Not possible to read in to.");
				throw e;
			}

			// If the user did not supply a valid DivertAsyncResult object, then the user does not care to wait for a pending
			// result. In the Receive method, I can't imagine a scenario where this is useful. However, the parameter is
			// optional. If the user has supplied a valid DivertAsyncResult object, then we need to pin the pointer of the 
			// supplied array inside the DivertAsyncResult object, in order to preserve the buffer pointer under the
			// async IO completes.
			if (asyncResult == nullptr)
			{
				// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
				pin_ptr<System::Byte> byteArray = &packetBuffer[0];

				unsigned char* pb = static_cast<unsigned char*>(byteArray);

				if (pb == nullptr)
				{
					// This shouldn't every happen, but TRUST NO 1
					e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}

				// XXX TODO - Attempt a read, if it fails, because the user did not provide an async result object, we just
				// abandon the entire operation.
			}
			else
			{
				// In this instance, we can't rely on a pin_ptr. As such, we need to permanently pin the array
				// and give the handle to the DivertAsyncResult object to manage. Once the user is done with the
				// async result, it will release the handle, unpinning the address.
				//
				// We have to do this because we have no idea when the pointer provided to the WinDivert API will
				// actually be used, since we're leaving that up to the user.
				System::Runtime::InteropServices::GCHandle bufferHandle = System::Runtime::InteropServices::GCHandle::Alloc(packetBuffer, System::Runtime::InteropServices::GCHandleType::Pinned);

				if (!asyncResult->Reset())
				{
					e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to reset DivertAsyncResult.");
					throw e;
				}

				asyncResult->Buffer = bufferHandle;
				asyncResult->WinDivertHandle = m_handle;

				System::IntPtr bPtr = bufferHandle.AddrOfPinnedObject();

				if (bPtr == System::IntPtr::Zero)
				{
					// Pointer of pinned array is null
					e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}

				uint32_t recvLength = 0;

				if (!WinDivertRecvEx(m_handle->GetUnmanagedHandle(), static_cast<void*>(bPtr), packetBuffer->Length, 0, address->GetUnmanagedAddress(), &recvLength, asyncResult->GetUnmanagedOverlapped()))
				{
					int lastError = System::Runtime::InteropServices::Marshal::GetLastWin32Error();
					if (lastError != ERROR_IO_PENDING)
					{
						// Read failed entirely
						asyncResult->ErrorCode = lastError;
						asyncResult->NoError = false;
					}
					return false;
				}

				// Read succeeded immediately, no waiting necessary.
				asyncResult->Length = recvLength;
				return true;
			}

			return true;
		}

		bool Diversion::Close()
		{
			if (m_handle && m_handle->Valid)
			{
				return m_handle->Close();
			}

			return false;
		}

	} /* namespace Net */
} /* namespace Divert */
