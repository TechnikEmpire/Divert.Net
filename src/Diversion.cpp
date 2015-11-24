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

		Diversion^ Diversion::Open(System::String^ filter, DivertLayer layer, int16_t priority, FilterFlags flags)
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

		bool Diversion::ValidateFilter(System::String^ filter, DivertLayer layer, System::String^% errorDetails)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			// Check if the string is null or empty. Empty filter strings are not supported, AFAIK.
			if (System::String::IsNullOrEmpty(filter) || System::String::IsNullOrWhiteSpace(filter))
			{
				errorDetails = gcnew System::String(u8"In Diversion::ValidateFilter(System::String^, DivertLayer, System::String^%) - Supplied filter string is null, empty or whitespace.");
				return false;
			}

			// Marshal the supplied string to a native C string for supplying to WinDivert
			const char* charString = static_cast<const char*>((System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filter)).ToPointer());

			if (charString == nullptr)
			{
				e = gcnew System::Exception(u8"In Diversion::ValidateFilter(System::String^, DivertLayer, System::String^%) - Failed to marshal filter string.");
				throw e;
			}

			const char* errStr = nullptr;
			uint32_t errPos = 0;

			int result = WinDivertHelperCheckFilter(charString, static_cast<WINDIVERT_LAYER>(layer), &errStr, &errPos);

			// Free the marshalled filter string
			System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)charString));

			if (result == 0)
			{
				errorDetails = gcnew System::String(errStr);
			}

			return result == 1;
		}

		bool Diversion::EvaluateFilter(System::String^ filter, DivertLayer layer, array<System::Byte>^ packetBuffer, uint32_t packetLength, Address^ address)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			// Check if the string is null or empty. Empty filter strings are not supported, AFAIK.
			if (System::String::IsNullOrEmpty(filter) || System::String::IsNullOrWhiteSpace(filter))
			{
				e = gcnew System::Exception(u8"In Diversion::EvaluateFilter(System::String^, DivertLayer, array<System::Byte>^, uint32_t, Address^) - Supplied filter string is null, empty or whitespace.");
				throw e;
			}

			// Marshal the supplied string to a native C string for supplying to WinDivert
			const char* charString = static_cast<const char*>((System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filter)).ToPointer());

			if (charString == nullptr)
			{
				e = gcnew System::Exception(u8"In Diversion::EvaluateFilter(System::String^, DivertLayer, array<System::Byte>^, uint32_t, Address^) - Failed to marshal filter string.");
				throw e;
			}

			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			int result = WinDivertHelperEvalFilter(charString, static_cast<WINDIVERT_LAYER>(layer), byteArray, packetLength, address->UnmanagedAddress);

			// Free the marshalled filter string
			System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)charString));

			return result == 1;
		}

		void Diversion::GetPacketProcess(Address^ address, TCPHeader^ tcpHeader, IPHeader^ ipv4Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			ULONG currentTableSize = 0;

			// First, if the table has never been initialized, it needs to be.
			if (tcpHeader->UnmanagedTcpV4Table == nullptr)
			{
				currentTableSize = sizeof(PMIB_TCPTABLE2);
				tcpHeader->UnmanagedTcpV4Table = static_cast<PMIB_TCPTABLE2>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (tcpHeader->UnmanagedTcpV4Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(TCPHeader^, IPHeader^, uint64_t%, System::String^) - Failed to allocate TCPTABLE2.");
					throw e;
				}
			}			

			// The return value will give us information about how to proceed to getting a proper populated
			// table back from Windows. If the return value is ERROR_INSUFFICIENT_BUFFER, then this means we
			// need to increase the size of the table to the size that the failing call set currentTableSize
			// to. That is to say, such a failed call will adjust the value of the parameter (passed here as
			// currentTableSize) to tell the user how much memory to allocate to fit the current table into.
			DWORD dwRetVal = 0;

			if ((dwRetVal = GetTcpTable2(tcpHeader->UnmanagedTcpV4Table, &currentTableSize, FALSE)) == ERROR_INSUFFICIENT_BUFFER)
			{
				// Need to resize
				free(tcpHeader->UnmanagedTcpV4Table);

				// Allocate with returned, sufficient size
				tcpHeader->UnmanagedTcpV4Table = static_cast<PMIB_TCPTABLE2>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (tcpHeader->UnmanagedTcpV4Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(TCPHeader^, IPHeader^, uint64_t%, System::String^) - Failed to reallocate TCPTABLE2.");
					throw e;
				}	

				// Now this call should succeed
				dwRetVal = GetTcpTable2(tcpHeader->UnmanagedTcpV4Table, &currentTableSize, FALSE);

				if (dwRetVal != NOERROR && tcpHeader->UnmanagedTcpV4Table != nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(TCPHeader^, IPHeader^, uint64_t%, System::String^) - Error populating TCPTABLE2.");
					throw e;
				}
			}	

			// Not sure if this is necessary?
			pin_ptr<ULONG> procIdPtr = &processId;

			// If the traffic is outbound, we want to match the remote port. If it's inbound,
			// we want to match the local port.
			bool matchSrcPort = address->Direction == DivertDirection::Outbound;

			// If we made it here, the table is valid and populated.
			for (size_t i = 0; i < tcpHeader->UnmanagedTcpV4Table->dwNumEntries; ++i)
			{
				switch (matchSrcPort)
				{
					case true:
					{
						if (static_cast<u_short>(tcpHeader->UnmanagedTcpV4Table->table[i].dwLocalPort) == tcpHeader->UnmanagedHeader->SrcPort)
						{
							processId = tcpHeader->UnmanagedTcpV4Table->table[i].dwOwningPid;
							break;
						}
					}
					break;
					case false:
					{
						if (static_cast<u_short>(tcpHeader->UnmanagedTcpV4Table->table[i].dwLocalPort) == tcpHeader->UnmanagedHeader->DstPort)
						{
							processId = tcpHeader->UnmanagedTcpV4Table->table[i].dwOwningPid;
							break;
						}
					}
					break;
				}
			}

			std::string procName = GetProcessName(processId);
			processName = gcnew System::String(procName.c_str());

			// We don't need to free the allocated table here. In fact, it's better not to.
			// It's assumed that if the user is interested in the process information, they're
			// going to be interested in the process information more than one time, and since
			// packets can come through a gazillion at a time, the last thing we want to do
			// is destroy and recreate the table every single time. 
			// 
			// So, we'll leave the table alone, it will only ever be freed and reallocated when
			// its size is not sufficient. When all is said and done, the table will ultimately
			// be freed automatically when its owning managed object is garbage collected.
		}

		void Diversion::GetPacketProcess(Address^ address, TCPHeader^ tcpHeader, IPv6Header^ ipv6Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			ULONG currentTableSize = 0;

			// First, if the table has never been initialized, it needs to be.
			if (tcpHeader->UnmanagedTcpV6Table == nullptr)
			{
				currentTableSize = sizeof(PMIB_TCP6TABLE2);
				tcpHeader->UnmanagedTcpV6Table = static_cast<PMIB_TCP6TABLE2>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (tcpHeader->UnmanagedTcpV6Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(TCPHeader^, IPv6Header^, uint64_t%, System::String^) - Failed to allocate TCPTABLE2.");
					throw e;
				}
			}

			// The return value will give us information about how to proceed to getting a proper populated
			// table back from Windows. If the return value is ERROR_INSUFFICIENT_BUFFER, then this means we
			// need to increase the size of the table to the size that the failing call set currentTableSize
			// to. That is to say, such a failed call will adjust the value of the parameter (passed here as
			// currentTableSize) to tell the user how much memory to allocate to fit the current table into.
			DWORD dwRetVal = 0;

			if ((dwRetVal = GetTcp6Table2(tcpHeader->UnmanagedTcpV6Table, &currentTableSize, FALSE)) == ERROR_INSUFFICIENT_BUFFER)
			{
				// Need to resize
				free(tcpHeader->UnmanagedTcpV6Table);

				// Allocate with returned, sufficient size
				tcpHeader->UnmanagedTcpV6Table = static_cast<PMIB_TCP6TABLE2>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (tcpHeader->UnmanagedTcpV6Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(TCPHeader^, IPv6Header^, uint64_t%, System::String^) - Failed to reallocate TCPTABLE2.");
					throw e;
				}

				// Now this call should succeed
				dwRetVal = GetTcp6Table2(tcpHeader->UnmanagedTcpV6Table, &currentTableSize, FALSE);

				if (dwRetVal != NOERROR && tcpHeader->UnmanagedTcpV6Table != nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(TCPHeader^, IPv6Header^, uint64_t%, System::String^) - Error populating TCPTABLE2.");
					throw e;
				}
			}

			// Not sure if this is necessary?
			pin_ptr<ULONG> procIdPtr = &processId;

			// If the traffic is outbound, we want to match the remote port. If it's inbound,
			// we want to match the local port.
			bool matchSrcPort = address->Direction == DivertDirection::Outbound;

			// If we made it here, the table is valid and populated.
			for (size_t i = 0; i < tcpHeader->UnmanagedTcpV6Table->dwNumEntries; ++i)
			{
				switch (matchSrcPort)
				{
				case true:
				{
					if (static_cast<u_short>(tcpHeader->UnmanagedTcpV6Table->table[i].dwLocalPort) == tcpHeader->UnmanagedHeader->SrcPort)
					{
						processId = tcpHeader->UnmanagedTcpV6Table->table[i].dwOwningPid;
						break;
					}
				}
				break;
				case false:
				{
					if (static_cast<u_short>(tcpHeader->UnmanagedTcpV6Table->table[i].dwLocalPort) == tcpHeader->UnmanagedHeader->DstPort)
					{
						processId = tcpHeader->UnmanagedTcpV6Table->table[i].dwOwningPid;
						break;
					}
				}
				break;
				}
			}

			std::string procName = GetProcessName(processId);
			processName = gcnew System::String(procName.c_str());

			// We don't need to free the allocated table here. In fact, it's better not to.
			// It's assumed that if the user is interested in the process information, they're
			// going to be interested in the process information more than one time, and since
			// packets can come through a gazillion at a time, the last thing we want to do
			// is destroy and recreate the table every single time. 
			// 
			// So, we'll leave the table alone, it will only ever be freed and reallocated when
			// its size is not sufficient. When all is said and done, the table will ultimately
			// be freed automatically when its owning managed object is garbage collected.
		}

		void Diversion::GetPacketProcess(Address^ address, UDPHeader^ udpHeader, IPHeader^ ipv4Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			ULONG currentTableSize = 0;

			// First, if the table has never been initialized, it needs to be.
			if (udpHeader->UnmanagedUdpV4Table == nullptr)
			{
				currentTableSize = sizeof(MIB_UDPTABLE_OWNER_PID);
				udpHeader->UnmanagedUdpV4Table = static_cast<PMIB_UDPTABLE_OWNER_PID>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (udpHeader->UnmanagedUdpV4Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(UDPHeader^, IPHeader^, uint64_t%, System::String^) - Failed to allocate MIB_UDPTABLE_OWNER_PID.");
					throw e;
				}
			}

			// The return value will give us information about how to proceed to getting a proper populated
			// table back from Windows. If the return value is ERROR_INSUFFICIENT_BUFFER, then this means we
			// need to increase the size of the table to the size that the failing call set currentTableSize
			// to. That is to say, such a failed call will adjust the value of the parameter (passed here as
			// currentTableSize) to tell the user how much memory to allocate to fit the current table into.
			DWORD dwRetVal = 0;

			if ((dwRetVal = GetExtendedUdpTable(udpHeader->UnmanagedUdpV4Table, &currentTableSize, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0)) == ERROR_INSUFFICIENT_BUFFER)
			{
				// Need to resize
				free(udpHeader->UnmanagedUdpV4Table);

				// Allocate with returned, sufficient size
				udpHeader->UnmanagedUdpV4Table = static_cast<PMIB_UDPTABLE_OWNER_PID>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (udpHeader->UnmanagedUdpV4Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(UDPHeader^, IPHeader^, uint64_t%, System::String^) - Failed to reallocate MIB_UDPTABLE_OWNER_PID.");
					throw e;
				}

				// Now this call should succeed
				dwRetVal = GetExtendedUdpTable(udpHeader->UnmanagedUdpV4Table, &currentTableSize, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0);

				if (dwRetVal != NOERROR && udpHeader->UnmanagedUdpV4Table != nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(UDPHeader^, IPHeader^, uint64_t%, System::String^) - Error populating MIB_UDPTABLE_OWNER_PID.");
					throw e;
				}
			}

			// Not sure if this is necessary?
			pin_ptr<ULONG> procIdPtr = &processId;

			// If the traffic is outbound, we want to match the remote port. If it's inbound,
			// we want to match the local port.
			bool matchSrcPort = address->Direction == DivertDirection::Outbound;

			// If we made it here, the table is valid and populated.
			for (size_t i = 0; i < udpHeader->UnmanagedUdpV4Table->dwNumEntries; ++i)
			{
				switch (matchSrcPort)
				{
				case true:
				{
					if (static_cast<u_short>(udpHeader->UnmanagedUdpV4Table->table[i].dwLocalPort) == udpHeader->UnmanagedHeader->SrcPort)
					{
						processId = udpHeader->UnmanagedUdpV4Table->table[i].dwOwningPid;
						break;
					}
				}
				break;
				case false:
				{
					if (static_cast<u_short>(udpHeader->UnmanagedUdpV4Table->table[i].dwLocalPort) == udpHeader->UnmanagedHeader->DstPort)
					{
						processId = udpHeader->UnmanagedUdpV4Table->table[i].dwOwningPid;
						break;
					}
				}
				break;
				}
			}

			std::string procName = GetProcessName(processId);
			processName = gcnew System::String(procName.c_str());

			// We don't need to free the allocated table here. In fact, it's better not to.
			// It's assumed that if the user is interested in the process information, they're
			// going to be interested in the process information more than one time, and since
			// packets can come through a gazillion at a time, the last thing we want to do
			// is destroy and recreate the table every single time. 
			// 
			// So, we'll leave the table alone, it will only ever be freed and reallocated when
			// its size is not sufficient. When all is said and done, the table will ultimately
			// be freed automatically when its owning managed object is garbage collected.

		}

		void Diversion::GetPacketProcess(Address^ address, UDPHeader^ udpHeader, IPv6Header^ ipv6Header, ULONG% processId, [System::Runtime::InteropServices::Out] System::String^% processName)
		{
			// In case something goes wrong, we'll create this and throw it.
			System::Exception^ e = nullptr;

			ULONG currentTableSize = 0;

			// First, if the table has never been initialized, it needs to be.
			if (udpHeader->UnmanagedUdpV6Table == nullptr)
			{
				currentTableSize = sizeof(MIB_UDP6TABLE_OWNER_PID);
				udpHeader->UnmanagedUdpV6Table = static_cast<PMIB_UDP6TABLE_OWNER_PID>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (udpHeader->UnmanagedUdpV6Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(UDPHeader^, IPv6Header^, uint64_t%, System::String^) - Failed to allocate MIB_UDP6TABLE_OWNER_PID.");
					throw e;
				}
			}

			// The return value will give us information about how to proceed to getting a proper populated
			// table back from Windows. If the return value is ERROR_INSUFFICIENT_BUFFER, then this means we
			// need to increase the size of the table to the size that the failing call set currentTableSize
			// to. That is to say, such a failed call will adjust the value of the parameter (passed here as
			// currentTableSize) to tell the user how much memory to allocate to fit the current table into.
			DWORD dwRetVal = 0;

			if ((dwRetVal = GetExtendedUdpTable(udpHeader->UnmanagedUdpV6Table, &currentTableSize, FALSE, AF_INET6, UDP_TABLE_OWNER_PID, 0)) == ERROR_INSUFFICIENT_BUFFER)
			{
				// Need to resize
				free(udpHeader->UnmanagedUdpV6Table);

				// Allocate with returned, sufficient size
				udpHeader->UnmanagedUdpV6Table = static_cast<PMIB_UDP6TABLE_OWNER_PID>(malloc(currentTableSize));

				// If the table is still null, then we have failed to allocate, something is seriously wrong.
				if (udpHeader->UnmanagedUdpV6Table == nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(UDPHeader^, IPv6Header^, uint64_t%, System::String^) - Failed to reallocate MIB_UDP6TABLE_OWNER_PID.");
					throw e;
				}

				// Now this call should succeed
				dwRetVal = GetExtendedUdpTable(udpHeader->UnmanagedUdpV6Table, &currentTableSize, FALSE, AF_INET6, UDP_TABLE_OWNER_PID, 0);

				if (dwRetVal != NOERROR && udpHeader->UnmanagedUdpV6Table != nullptr)
				{
					e = gcnew System::Exception(u8"In Diversion::GetPacketProcess(UDPHeader^, IPv6Header^, uint64_t%, System::String^) - Error populating MIB_UDP6TABLE_OWNER_PID.");
					throw e;
				}
			}

			// Not sure if this is necessary?
			pin_ptr<ULONG> procIdPtr = &processId;

			// If the traffic is outbound, we want to match the remote port. If it's inbound,
			// we want to match the local port.
			bool matchSrcPort = address->Direction == DivertDirection::Outbound;

			// If we made it here, the table is valid and populated.
			for (size_t i = 0; i < udpHeader->UnmanagedUdpV6Table->dwNumEntries; ++i)
			{
				switch (matchSrcPort)
				{
				case true:
				{
					if (static_cast<u_short>(udpHeader->UnmanagedUdpV6Table->table[i].dwLocalPort) == udpHeader->UnmanagedHeader->SrcPort)
					{
						processId = udpHeader->UnmanagedUdpV6Table->table[i].dwOwningPid;
						break;
					}
				}
				break;
				case false:
				{
					if (static_cast<u_short>(udpHeader->UnmanagedUdpV6Table->table[i].dwLocalPort) == udpHeader->UnmanagedHeader->DstPort)
					{
						processId = udpHeader->UnmanagedUdpV6Table->table[i].dwOwningPid;
						break;
					}
				}
				break;
				}
			}

			std::string procName = GetProcessName(processId);
			processName = gcnew System::String(procName.c_str());

			// We don't need to free the allocated table here. In fact, it's better not to.
			// It's assumed that if the user is interested in the process information, they're
			// going to be interested in the process information more than one time, and since
			// packets can come through a gazillion at a time, the last thing we want to do
			// is destroy and recreate the table every single time. 
			// 
			// So, we'll leave the table alone, it will only ever be freed and reallocated when
			// its size is not sufficient. When all is said and done, the table will ultimately
			// be freed automatically when its owning managed object is garbage collected.
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
			delete m_winDivertHandle;
		}

		DivertHandle^ Diversion::Handle::get()
		{
			return m_winDivertHandle;
		}	

		void Diversion::Handle::set(DivertHandle^ value)
		{
			m_winDivertHandle = value;
		}

		bool Diversion::Receive(array<System::Byte>^ packetBuffer, Address^ address, uint32_t% receiveLength)
		{
			System::Exception^ e = nullptr;

			if (packetBuffer->Length == 0)
			{
				e = gcnew System::Exception(u8"In Diversion::Receive(array<System::Byte>^, Address^, out uint32_t) - Supplied buffer has a length of zero. Not possible to read in to.");
				throw e;
			}

			if (!address->Reset())
			{
				e = gcnew System::Exception(u8"In Diversion::Receive(array<System::Byte>^, Address^, out uint32_t) - Failed to reset Address.");
				throw e;
			}

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];
			
			uint32_t readLen = 0;

			int result = WinDivertRecv(m_winDivertHandle->UnmanagedHandle, byteArray, packetBuffer->Length, address->UnmanagedAddress, &readLen);
			
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

			if (!address->Reset())
			{
				e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to reset Address.");
				throw e;
			}

			uint32_t recvLength = 0;

			pin_ptr<uint32_t> pinnedRecvLen = &receiveLength;

			// If the user did not supply a valid DivertAsyncResult object, then the user does not care to wait for a pending
			// result. In the Receive method, I can't imagine a scenario where this is useful. However, the parameter is
			// optional. If the user has supplied a valid DivertAsyncResult object, then we need to pin the pointer of the 
			// supplied array inside the DivertAsyncResult object, in order to preserve the buffer pointer under the
			// async IO completes.
			if (asyncResult == nullptr)
			{
				// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
				pin_ptr<System::Byte> byteArray = &packetBuffer[0];

				//Attempt a read, if it fails, because the user did not provide an async result object, we just
				// abandon the entire operation.
				if (!WinDivertRecvEx(m_winDivertHandle->UnmanagedHandle, byteArray, packetBuffer->Length, 0, address->UnmanagedAddress, pinnedRecvLen, nullptr))
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
				asyncResult->WinDivertHandle = m_winDivertHandle;

				System::IntPtr bPtr = bufferHandle.AddrOfPinnedObject();

				if (bPtr == System::IntPtr::Zero)
				{
					// Pointer of pinned array is null
					e = gcnew System::Exception(u8"In Diversion::ReceiveAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}			

				if (!WinDivertRecvEx(m_winDivertHandle->UnmanagedHandle, static_cast<void*>(bPtr), packetBuffer->Length, 0, address->UnmanagedAddress, &recvLength, asyncResult->UnmanagedOverlapped))
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

		bool Diversion::Send(array<System::Byte>^ packetBuffer, uint32_t packetLength, Address^ address, uint32_t% sendLength)
		{
			System::Exception^ e = nullptr;

			if (packetBuffer->Length == 0)
			{
				e = gcnew System::Exception(u8"In Diversion::Send(array<System::Byte>^, Address^, out uint32_t) - Supplied buffer has a length of zero. Cannot inject nothing.");
				throw e;
			}

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			uint32_t sendLen = 0;

			int result = WinDivertSend(m_winDivertHandle->UnmanagedHandle, byteArray, packetLength, address->UnmanagedAddress, &sendLen);

			sendLength = sendLen;

			return result == 1;
		}

		bool Diversion::SendAsync(array<System::Byte>^ packetBuffer, uint32_t packetLength, Address^ address, uint32_t% sendLength, [System::Runtime::InteropServices::Optional]DivertAsyncResult^ asyncResult)
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

				// Attempt a Send, if it fails, because the user did not provide an async result object, we just
				// abandon the entire operation.
				if (!WinDivertSendEx(m_winDivertHandle->UnmanagedHandle, byteArray, packetLength, 0, address->UnmanagedAddress, &sendLen, nullptr))
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
				asyncResult->WinDivertHandle = m_winDivertHandle;

				System::IntPtr bPtr = bufferHandle.AddrOfPinnedObject();

				if (bPtr == System::IntPtr::Zero)
				{
					// Pointer of pinned array is null
					e = gcnew System::Exception(u8"In Diversion::SendAsync(array<System::Byte>^, Address^, uint32_t%, DivertAsyncResult^) - Failed to pin packet buffer.");
					throw e;
				}

				if (!WinDivertSendEx(m_winDivertHandle->UnmanagedHandle, static_cast<void*>(bPtr), packetBuffer->Length, 0, address->UnmanagedAddress, &sendLen, asyncResult->UnmanagedOverlapped))
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
			if (m_winDivertHandle && m_winDivertHandle->Valid)
			{
				return m_winDivertHandle->Close();
			}

			return false;
		}

		bool Diversion::SetParam(DivertParam param, uint64_t value)
		{
			// No. No you may not. Seriously though this shouldn't happen.
			if (m_winDivertHandle == nullptr ||
				m_winDivertHandle->UnmanagedHandle == nullptr ||
				m_winDivertHandle->UnmanagedHandle == INVALID_HANDLE_VALUE
				)
			{				
				return false;
			}

			int result = WinDivertSetParam(m_winDivertHandle->UnmanagedHandle, static_cast<WINDIVERT_PARAM>(param), value);

			return result == 1;
		}

		bool Diversion::GetParam(DivertParam param, uint64_t% value)
		{
			// No. No you may not. Seriously though this shouldn't happen.
			if (m_winDivertHandle == nullptr ||
				m_winDivertHandle->UnmanagedHandle == nullptr ||
				m_winDivertHandle->UnmanagedHandle == INVALID_HANDLE_VALUE
				)
			{
				return false;
			}

			uint64_t retVal = 0;
			int result = WinDivertGetParam(m_winDivertHandle->UnmanagedHandle, static_cast<WINDIVERT_PARAM>(param), &retVal);

			return result == 1;
		}

		bool Diversion::ParsePacket(array<System::Byte>^ packetBuffer, uint32_t packetLength, IPHeader^ ipHeader, IPv6Header^ ipv6Header, ICMPHeader^ icmpHeader, ICMPv6Header^ icmpv6Header, TCPHeader^ tcpHeader, UDPHeader^ udpHeader)
		{
			// Ewww, code duplication. Oh well, this was the simplest way to provide the full functionality of the WinDivert parse packet function.

			System::Exception^ e = nullptr;

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			PWINDIVERT_IPHDR umpipV4Header = nullptr;
			PWINDIVERT_IPV6HDR umpipV6Header = nullptr;
			PWINDIVERT_UDPHDR umpudpHeader = nullptr;
			PWINDIVERT_TCPHDR umptcpHeader = nullptr;
			PWINDIVERT_ICMPHDR umpicmpHeader = nullptr;
			PWINDIVERT_ICMPV6HDR umpicmpV6Header = nullptr;

			int retVal = 0;

			retVal = WinDivertHelperParsePacket(byteArray, packetLength, &umpipV4Header, &umpipV6Header, &umpicmpHeader, &umpicmpV6Header, &umptcpHeader, &umpudpHeader, nullptr, nullptr);

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

		bool Diversion::ParsePacket(array<System::Byte>^ packetBuffer, uint32_t packetLength, IPHeader^ ipHeader, IPv6Header^ ipv6Header, ICMPHeader^ icmpHeader, ICMPv6Header^ icmpv6Header, TCPHeader^ tcpHeader, UDPHeader^ udpHeader, array<System::Byte>^% packetData)
		{
			System::Exception^ e = nullptr;

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];			

			PWINDIVERT_IPHDR umpipV4Header = nullptr;
			PWINDIVERT_IPV6HDR umpipV6Header = nullptr;
			PWINDIVERT_UDPHDR umpudpHeader = nullptr;
			PWINDIVERT_TCPHDR umptcpHeader = nullptr;
			PWINDIVERT_ICMPHDR umpicmpHeader = nullptr;
			PWINDIVERT_ICMPV6HDR umpicmpV6Header = nullptr;

			uint32_t packetDataLength = 0;
			char* packetDataPointer = nullptr;

			int retVal = 0;

			retVal = WinDivertHelperParsePacket(byteArray, packetLength, &umpipV4Header, &umpipV6Header, &umpicmpHeader, &umpicmpV6Header, &umptcpHeader, &umpudpHeader, (PVOID*)(&packetDataPointer), &packetDataLength);

			packetData = gcnew array<System::Byte>(packetDataLength);

			if (packetDataLength > 0 && packetDataPointer != nullptr)
			{
				System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)packetDataPointer), packetData, 0, packetDataLength);
			}	

			if (ipHeader != nullptr)
			{
				ipHeader->UnmanagedHeader = umpipV4Header;
			}

			if (ipv6Header != nullptr)
			{
				ipv6Header->UnmanagedHeader = umpipV6Header;
			}

			if (icmpHeader != nullptr)
			{
				icmpHeader->UnmanagedHeader = umpicmpHeader;
			}

			if (icmpv6Header != nullptr)
			{
				icmpv6Header->UnmanagedHeader = umpicmpV6Header;
			}

			if (tcpHeader != nullptr)
			{
				tcpHeader->UnmanagedHeader = umptcpHeader;
			}

			if (udpHeader != nullptr)
			{
				udpHeader->UnmanagedHeader = umpudpHeader;
			}

			return retVal == 1;
		}

		uint32_t Diversion::CalculateChecksums(array<System::Byte>^ packetBuffer, uint32_t packetLength, ChecksumCalculationFlags flags)
		{
			System::Exception^ e = nullptr;

			// Pin the array to ensure the GC leaves the object alone with the unmanaged code uses it
			pin_ptr<System::Byte> byteArray = &packetBuffer[0];

			uint64_t flagsInt = (uint64_t)System::Convert::ChangeType(flags, System::UInt64::typeid);

			int retVal = WinDivertHelperCalcChecksums(byteArray, packetLength, flagsInt);

			return retVal == 1;
		}

		std::string Diversion::GetProcessName(const ULONG processId)
		{
			std::string result("SYSTEM");

			HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

			if (processHandle != nullptr && processHandle != INVALID_HANDLE_VALUE)
			{
				char filename[MAX_PATH];
				DWORD resSize = MAX_PATH;

				if (QueryFullProcessImageNameA(processHandle, 0, filename, &resSize) == 0)
				{
					// Failed to get module filename.
					CloseHandle(processHandle);
					return result;
				}
				else
				{
					// Success
					result = std::string(filename);
					CloseHandle(processHandle);
					return result;
				}
			}

			// Failed to open a valid handle. This is almost surely a SYSTEM process, or protected by AV.
			// Let's not throw any errors. It is expected behavior that this call may fail for reasons such
			// as I listed above, or the process is closed and the ID reassigned before this call is 
			// processed.
			return result;
		}

	} /* namespace Net */
} /* namespace Divert */
