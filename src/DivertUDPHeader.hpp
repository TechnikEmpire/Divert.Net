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

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>
#include <string>
#include <iphlpapi.h>
#include <timeapi.h>
#include <udpmib.h>
#include <psapi.h>
#include <windivert.h>

namespace Divert
{
	namespace Net
	{

		/// <summary>
		/// Represents the UDP header for an intercepted UDP packet.
		/// 
		/// When a packet is intercepted and the headers are parsed, the packet may be an UDP
		/// packet. If such is the case, this structure will be populated with all of the
		/// information in the UDP packet header.
		/// 
		/// More information here:
		/// https://en.wikipedia.org/wiki/User_Datagram_Protocol#Packet_structure
		/// </summary>
		public ref class UDPHeader
		{

		public:

			/// <summary>
			/// Default constructor.
			/// </summary>
			UDPHeader();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~UDPHeader();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!UDPHeader();

			/// <summary>
			/// The source port that the UDP packet originates from. Note when reading/writing this
			/// value, that although WinDivert stores this data in Network Byte Order, conversion is
			/// done automatically on these values to and from Host to Network Byte Order.
			/// 
			/// The purpose of this is to take the burden off the end user of having to sprinkle
			/// conversion code everywhere that these values are read and modified.
			/// 
			/// Supply values in Host Network Order, otherwise face undefined behavior.
			/// </summary>
			property uint16_t SourcePort
			{
				uint16_t get();
				void set(uint16_t value);
			}

			/// <summary>
			/// The port that the UDP packet is destined to. Note when reading/writing this value,
			/// that although WinDivert stores this data in Network Byte Order, conversion is done
			/// automatically on these values to and from Host to Network Byte Order.
			/// 
			/// The purpose of this is to take the burden off the end user of having to sprinkle
			/// conversion code everywhere that these values are read and modified.
			/// 
			/// Supply values in Host Network Order, otherwise face undefined behavior.
			/// </summary>
			property uint16_t DestinationPort
			{
				uint16_t get();
				void set(uint16_t value);
			}

			/// <summary>
			/// Specifies the length in bytes of the UDP header and UDP data.
			/// </summary>
			property uint16_t Length
			{
				uint16_t get();
				void set(uint16_t value);
			}

			/// <summary>
			/// Packet checksum, can be used for error checking. Optional for IPv4, mandatory for
			/// IPv6.
			/// </summary>
			property uint16_t Checksum
			{
				uint16_t get();
				void set(uint16_t value);
			}

			/// <summary>
			/// Check if the object is initialized with valid data. Use this flag to determine if
			/// this header was populated by the ParsePacket method.
			/// </summary>
			property bool Valid
			{
				bool get();
			}

		internal:

			/// <summary>
			/// Allow internal construction with the supplied unmanaged UDP header.
			/// </summary>
			/// <param name="address">
			/// Unmanaged PWINDIVERT_UDPHDR to construct this wrapper around.
			/// </param>
			UDPHeader(PWINDIVERT_UDPHDR UDPHeader);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_UDPHDR object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_UDPHDR member.
			/// </returns>
			property PWINDIVERT_UDPHDR UnmanagedHeader
			{
				PWINDIVERT_UDPHDR get();
				void set(PWINDIVERT_UDPHDR value);
			}

			/// <summary>
			/// Internal accessor to the unmanaged PMIB_UDPTABLE_OWNER_PID object held by this object. The
			/// purpose of the PMIB_UDPTABLE_OWNER_PID object is to allow the user to easily get the
			/// information about the process behind behind a TCP packet flow. Static helper
			/// functions are made available in the Diversion class where the user can query this
			/// type of information by supplying DivertTCPHeader or DivertUDPHeader along with the
			/// appropriate IP header.
			/// 
			/// This object will handle freeing the data this member points to.
			/// </summary>
			/// <returns>
			/// The unmanaged PMIB_UDPTABLE_OWNER_PID member. 
			/// </returns>
			property PMIB_UDPTABLE_OWNER_PID UnmanagedUdpV4Table
			{
				PMIB_UDPTABLE_OWNER_PID get();
				void set(PMIB_UDPTABLE_OWNER_PID value);
			}

			/// <summary>
			/// Internal accessor to the unmanaged PMIB_UDP6TABLE_OWNER_PID object held by this object. The
			/// purpose of the PMIB_UDP6TABLE_OWNER_PID object is to allow the user to easily get the
			/// information about the process behind behind a TCP packet flow. Static helper
			/// functions are made available in the Diversion class where the user can query this
			/// type of information by supplying DivertTCPHeader or DivertUDPHeader along with the
			/// appropriate IP header.
			/// 
			/// This object will handle freeing the data this member points to.
			/// </summary>
			/// <returns>
			/// The unmanaged PMIB_UDP6TABLE_OWNER_PID member. 
			/// </returns>
			property PMIB_UDP6TABLE_OWNER_PID UnmanagedUdpV6Table
			{
				PMIB_UDP6TABLE_OWNER_PID get();
				void set(PMIB_UDP6TABLE_OWNER_PID value);
			}

		private:

			/// <summary>
			/// Privately held PWINDIVERT_UDPHDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_UDPHDR m_udpHeader = nullptr;

			PMIB_UDPTABLE_OWNER_PID m_udpv4Table;

			PMIB_UDP6TABLE_OWNER_PID m_udpv6Table;

		};

	} /* namespace Net */
} /* namespace Divert */
