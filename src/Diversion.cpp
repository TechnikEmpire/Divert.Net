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
				// This shouldn't ever happen, but TRUST NO 1
				e = gcnew System::Exception(u8"In Diversion::Receive(array<System::Byte>^, Address^, out uint32_t) - Failed to pin packet buffer.");
				throw e;
			}
			
			uint32_t readLen = 0;

			int result = WinDivertRecv(m_handle->UnmanagedHandle, pb, packetBuffer->Length, address->UnmanagedAddress, &readLen);

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

			uint32_t recvLength = 0;

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
					// This shouldn't ever happen, but TRUST NO 1
					e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}

				//Attempt a read, if it fails, because the user did not provide an async result object, we just
				// abandon the entire operation.
				if (!WinDivertRecvEx(m_handle->UnmanagedHandle, pb, packetBuffer->Length, 0, address->UnmanagedAddress, &recvLength, nullptr))
				{
					return false;
				}

				// Read succeeded immediately, no waiting necessary.
				receiveLength = recvLength;
				return true;
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

				if (!WinDivertRecvEx(m_handle->UnmanagedHandle, static_cast<void*>(bPtr), packetBuffer->Length, 0, address->UnmanagedAddress, &recvLength, asyncResult->UnmanagedOverlapped))
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

			return false;
		}

		bool Diversion::Send(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% sendLength)
		{
			System::Exception^ e = nullptr;

			if (packetBuffer->Length == 0)
			{
				e = gcnew System::Exception(u8"In Diversion::Send(array<System::Byte>^, Address^, out uint32_t) - Supplied buffer has a length of zero. Cannot inject nothing.");
				throw e;
			}

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			unsigned char* pb = static_cast<unsigned char*>(byteArray);

			if (pb == nullptr)
			{
				// This shouldn't ever happen, but TRUST NO 1
				e = gcnew System::Exception(u8"In Diversion::Send(array<System::Byte>^, Address^, out uint32_t) - Failed to pin packet buffer.");
				throw e;
			}

			uint32_t sendLen = 0;

			int result = WinDivertSend(m_handle->UnmanagedHandle, pb, packetBuffer->Length, address->UnmanagedAddress, &sendLen);

			sendLength = sendLen;

			return result == 1;
		}

		bool Diversion::SendAsync(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% sendLength, [System::Runtime::InteropServices::Optional]DivertAsyncResult^ asyncResult)
		{
			System::Exception^ e = nullptr;

			if (packetBuffer->Length == 0)
			{
				e = gcnew System::Exception(u8"In Diversion::SendAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Supplied buffer has a length of zero. Cannot inject nothing.");
				throw e;
			}

			uint32_t sendLen = 0;

			// If the user did not supply a valid DivertAsyncResult object, then the user does not care to wait for a pending
			// result. This is desirable for more use cases, enabling a sort of "fire and forget" mechanism. If the user has 
			// supplied a valid DivertAsyncResult object, then we need to pin the pointer of the supplied array inside the 
			// DivertAsyncResult object, in order to preserve the buffer pointer under the async IO completes.
			if (asyncResult == nullptr)
			{
				// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
				pin_ptr<System::Byte> byteArray = &packetBuffer[0];

				unsigned char* pb = static_cast<unsigned char*>(byteArray);

				if (pb == nullptr)
				{
					// This shouldn't ever happen, but TRUST NO 1
					e = gcnew System::Exception(u8"In Diversion::SendAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}

				// Attempt a Send, if it fails, because the user did not provide an async result object, we just
				// abandon the entire operation.
				if (!WinDivertSendEx(m_handle->UnmanagedHandle, pb, packetBuffer->Length, 0, address->UnmanagedAddress, &sendLen, nullptr))
				{
					return false;
				}

				// Send succeeded immediately, no waiting necessary.
				sendLength = sendLen;
				return true;
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
					e = gcnew System::Exception(u8"In Diversion::SendAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to reset DivertAsyncResult.");
					throw e;
				}

				asyncResult->Buffer = bufferHandle;
				asyncResult->WinDivertHandle = m_handle;

				System::IntPtr bPtr = bufferHandle.AddrOfPinnedObject();

				if (bPtr == System::IntPtr::Zero)
				{
					// Pointer of pinned array is null
					e = gcnew System::Exception(u8"In Diversion::SendAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}

				if (!WinDivertSendEx(m_handle->UnmanagedHandle, static_cast<void*>(bPtr), packetBuffer->Length, 0, address->UnmanagedAddress, &sendLen, asyncResult->UnmanagedOverlapped))
				{
					int lastError = System::Runtime::InteropServices::Marshal::GetLastWin32Error();
					if (lastError != ERROR_IO_PENDING)
					{
						// Send failed entirely
						asyncResult->ErrorCode = lastError;
						asyncResult->NoError = false;
					}
					return false;
				}

				// Send succeeded immediately, no waiting necessary.
				asyncResult->Length = sendLen;
				return true;
			}

			return false;
		}

		bool Diversion::Close()
		{
			if (m_handle && m_handle->Valid)
			{
				return m_handle->Close();
			}

			return false;
		}

		bool Diversion::SetParam(DivertParam param, uint64_t value)
		{
			// No. No you may not. Seriously though this shouldn't happen.
			if (m_handle == nullptr ||
				m_handle->UnmanagedHandle == nullptr ||
				m_handle->UnmanagedHandle == INVALID_HANDLE_VALUE
				)
			{				
				return false;
			}

			int result = WinDivertSetParam(m_handle->UnmanagedHandle, static_cast<WINDIVERT_PARAM>(param), value);

			return result == 1;
		}

		bool Diversion::GetParam(DivertParam param, uint64_t% value)
		{
			// No. No you may not. Seriously though this shouldn't happen.
			if (m_handle == nullptr ||
				m_handle->UnmanagedHandle == nullptr ||
				m_handle->UnmanagedHandle == INVALID_HANDLE_VALUE
				)
			{
				return false;
			}

			uint64_t retVal = 0;
			int result = WinDivertGetParam(m_handle->UnmanagedHandle, static_cast<WINDIVERT_PARAM>(param), &retVal);

			return result == 1;
		}

		bool Diversion::ParsePacket(array<System::Byte>^ packetBuffer, IPHeader^ ipHeader, IPv6Header^ ipv6Header, ICMPHeader^ icmpHeader, ICMPv6Header^ icmpv6Header, TCPHeader^ tcpHeader, UDPHeader^ udpHeader)
		{
			// Ewww, code duplication. Oh well, this was the simplest way to provide the full functionality of the WinDivert parse packet function.

			System::Exception^ e = nullptr;

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			unsigned char* pb = static_cast<unsigned char*>(byteArray);

			if (pb == nullptr)
			{
				// This shouldn't ever happen, but TRUST NO 1
				e = gcnew System::Exception(u8"In Diversion::ParsePacket(array<System::Byte>^ packetBuffer, IPHeader^, IPv6Header^, ICMPHeader^, ICMPv6Header^, TCPHeader^, UDPHeader^) - Failed to pin packet buffer.");
				throw e;
			}

			PWINDIVERT_IPHDR umpipV4Header = nullptr;
			PWINDIVERT_IPV6HDR umpipV6Header = nullptr;
			PWINDIVERT_UDPHDR umpudpHeader = nullptr;
			PWINDIVERT_TCPHDR umptcpHeader = nullptr;
			PWINDIVERT_ICMPHDR umpicmpHeader = nullptr;
			PWINDIVERT_ICMPV6HDR umpicmpV6Header = nullptr;

			uint32_t packetDataLength = 0;
			char* packetDataPointer = nullptr;

			int retVal = 0;

			retVal = WinDivertHelperParsePacket(pb, packetBuffer->Length, &umpipV4Header, &umpipV6Header, &umpicmpHeader, &umpicmpV6Header, &umptcpHeader, &umpudpHeader, (PVOID*)(&packetDataPointer), &packetDataLength);

			if (ipHeader != nullptr && umpipV4Header != nullptr)
			{
				ipHeader->UnmanagedHeader = umpipV4Header;
			}

			if (ipv6Header != nullptr && umpipV6Header != nullptr)
			{
				ipv6Header->UnmanagedHeader = umpipV6Header;
			}

			if (icmpHeader != nullptr && umpicmpHeader != nullptr)
			{
				icmpHeader->UnmanagedHeader = umpicmpHeader;
			}

			if (icmpv6Header != nullptr && umpicmpV6Header != nullptr)
			{
				icmpv6Header->UnmanagedHeader = umpicmpV6Header;
			}

			if (tcpHeader != nullptr && umptcpHeader != nullptr)
			{
				tcpHeader->UnmanagedHeader = umptcpHeader;
			}

			if (udpHeader != nullptr && umpudpHeader != nullptr)
			{
				udpHeader->UnmanagedHeader = umpudpHeader;
			}

			return retVal == 1;
		}

		bool Diversion::ParsePacket(array<System::Byte>^ packetBuffer, IPHeader^ ipHeader, IPv6Header^ ipv6Header, ICMPHeader^ icmpHeader, ICMPv6Header^ icmpv6Header, TCPHeader^ tcpHeader, UDPHeader^ udpHeader, array<System::Byte>^% packetData)
		{
			System::Exception^ e = nullptr;

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			unsigned char* pb = static_cast<unsigned char*>(byteArray);

			if (pb == nullptr)
			{
				// This shouldn't ever happen, but TRUST NO 1
				e = gcnew System::Exception(u8"In Diversion::ParsePacket(array<System::Byte>^ packetBuffer, IPHeader^, IPv6Header^, ICMPHeader^, ICMPv6Header^, TCPHeader^, UDPHeader^, array<System::Byte>^%) - Failed to pin packet buffer.");
				throw e;
			}			

			PWINDIVERT_IPHDR umpipV4Header = nullptr;
			PWINDIVERT_IPV6HDR umpipV6Header = nullptr;
			PWINDIVERT_UDPHDR umpudpHeader = nullptr;
			PWINDIVERT_TCPHDR umptcpHeader = nullptr;
			PWINDIVERT_ICMPHDR umpicmpHeader = nullptr;
			PWINDIVERT_ICMPV6HDR umpicmpV6Header = nullptr;

			uint32_t packetDataLength = 0;
			char* packetDataPointer = nullptr;

			int retVal = 0;

			retVal = WinDivertHelperParsePacket(pb, packetBuffer->Length, &umpipV4Header, &umpipV6Header, &umpicmpHeader, &umpicmpV6Header, &umptcpHeader, &umpudpHeader, (PVOID*)(&packetDataPointer), &packetDataLength);

			packetData = gcnew array<System::Byte>(packetDataLength);

			if (packetDataLength > 0 && packetDataPointer != nullptr)
			{
				System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)packetDataPointer), packetData, 0, packetDataLength);
			}	

			if (ipHeader != nullptr && umpipV4Header != nullptr)
			{
				ipHeader->UnmanagedHeader = umpipV4Header;
			}

			if (ipv6Header != nullptr && umpipV6Header != nullptr)
			{
				ipv6Header->UnmanagedHeader = umpipV6Header;
			}

			if (icmpHeader != nullptr && umpicmpHeader != nullptr)
			{
				icmpHeader->UnmanagedHeader = umpicmpHeader;
			}

			if (icmpv6Header != nullptr && umpicmpV6Header != nullptr)
			{
				icmpv6Header->UnmanagedHeader = umpicmpV6Header;
			}

			if (tcpHeader != nullptr && umptcpHeader != nullptr)
			{
				tcpHeader->UnmanagedHeader = umptcpHeader;
			}

			if (udpHeader != nullptr && umpudpHeader != nullptr)
			{
				udpHeader->UnmanagedHeader = umpudpHeader;
			}

			return retVal == 1;
		}

		uint32_t Diversion::CalculateChecksums(array<System::Byte>^ packetBuffer, ChecksumCalculationFlags flags)
		{
			System::Exception^ e = nullptr;

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			unsigned char* pb = static_cast<unsigned char*>(byteArray);

			if (pb == nullptr)
			{
				// This shouldn't ever happen, but TRUST NO 1
				e = gcnew System::Exception(u8"In Diversion::CalculateChecksums(array<System::Byte>^, ChecksumCalculationFlags) - Failed to pin packet buffer.");
				throw e;
			}

			uint64_t flagsInt = (uint64_t)System::Convert::ChangeType(flags, System::UInt64::typeid);

			int retVal = WinDivertHelperCalcChecksums(pb, packetBuffer->Length, flagsInt);

			return retVal == 1;
		}

	} /* namespace Net */
} /* namespace Divert */
