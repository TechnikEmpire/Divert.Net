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
#include <tcpmib.h>
#include <psapi.h>
#include <windivert.h>

namespace Divert
{
	namespace Net
	{
		/// <summary>
		/// Represents the TCP header for an intercepted TCP packet.
		/// 
		/// When a packet is intercepted and the headers are parsed, the packet may be a TCP
		/// packet. If such is the case, this structure will be populated with all of the
		/// information in the TCP packet header.
		/// 
		/// More information here:
		/// http://en.wikipedia.org/wiki/Transmission_Control_Protocol#TCP_segment_structure
		/// </summary>
		public ref class TCPHeader
		{

		public:

			/// <summary>
			/// Default constructor.
			/// </summary>
			TCPHeader();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~TCPHeader();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!TCPHeader();

			/// <summary>
			/// The source port that the TCP packet originates from. Note when reading/writing this
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
			/// The source port that the TCP packet is destined for. Note when reading/writing this
			/// value, that although WinDivert stores this data in Network Byte Order, conversion is
			/// done automatically on these values to and from Host to Network Byte Order.
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

			property uint32_t SequenceNumber
			{
				uint32_t get();
				void set(uint32_t value);
			}

			property uint32_t AcknowledgmentNumber
			{
				uint32_t get();
				void set(uint32_t value);
			}

			property uint16_t Reserved1
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t HeaderLength
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Fin
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Syn
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Rst
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Psh
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Ack
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Urg
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Reserved2
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t WindowSize
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Checksum
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t UrgentPointer
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
			/// Allow internal construction with the supplied unmanaged TCP header.
			/// </summary>
			/// <param name="address">
			/// Unmanaged PWINDIVERT_TCPHDR to construct this wrapper around.
			/// </param>
			TCPHeader(PWINDIVERT_TCPHDR tcpHeader);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_TCPHDR object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_TCPHDR member.
			/// </returns>
			property PWINDIVERT_TCPHDR UnmanagedHeader
			{
				PWINDIVERT_TCPHDR get();
				void set(PWINDIVERT_TCPHDR value);
			}

			/// <summary>
			/// Internal accessor to the unmanaged PMIB_TCPTABLE2 object held by this object. The
			/// purpose of the PMIB_TCPTABLE2 object is to allow the user to easily get the
			/// information about the process behind behind a TCP packet flow. Static helper
			/// functions are made available in the Diversion class where the user can query this
			/// type of information by supplying DivertTCPHeader or DivertUDPHeader along with the
			/// appropriate IP header.
			/// 
			/// This object will handle freeing the data this member points to.
			/// </summary>
			/// <returns>
			/// The unmanaged PMIB_TCPTABLE2 member. 
			/// </returns>
			property PMIB_TCPTABLE2 UnmanagedTcpV4Table
			{
				PMIB_TCPTABLE2 get();
				void set(PMIB_TCPTABLE2 value);
			}

			/// <summary>
			/// Internal accessor to the unmanaged PMIB_TCP6TABLE2 object held by this object. The
			/// purpose of the PMIB_TCPTABLE2 object is to allow the user to easily get the
			/// information about the process behind behind a TCP packet flow. Static helper
			/// functions are made available in the Diversion class where the user can query this
			/// type of information by supplying DivertTCPHeader or DivertUDPHeader along with the
			/// appropriate IP header.
			/// 
			/// This object will handle freeing the data this member points to.
			/// </summary>
			/// <returns>
			/// The unmanaged PMIB_TCP6TABLE2 member. 
			/// </returns>
			property PMIB_TCP6TABLE2 UnmanagedTcpV6Table
			{
				PMIB_TCP6TABLE2 get();
				void set(PMIB_TCP6TABLE2 value);
			}

		private:

			/// <summary>
			/// Privately held PWINDIVERT_TCPHDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_TCPHDR m_tcpHeader = nullptr;
			
			/// <summary>
			/// Privately held PWINDIVERT_TCPHDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PMIB_TCPTABLE2 m_tcpv4Table = nullptr;

			/// <summary>
			/// Privately held PWINDIVERT_TCPHDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PMIB_TCP6TABLE2 m_tcpv6Table = nullptr;
		};

	} /* namespace Net */
} /* namespace Divert */
