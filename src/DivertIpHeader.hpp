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

#include <windivert.h>
#include <cstdint>

namespace Divert
{
	namespace Net
	{

		/// <summary>
		/// Represets the IP header for an intercepted IPV4 packet.
		/// 
		/// When a packet is intercepted and the headers are parsed, the packet may be an IPV4
		/// packet. If such is the case, this structure will be populated with all of the
		/// information in the IPV4 packet header, such as the source and destination addresses.
		/// 
		/// More information here: https://en.wikipedia.org/wiki/IPv4#Packet_structure
		/// </summary>
		ref class DivertIpHeader
		{

		public:

			/// <summary>
			/// Default constructor for the DivertIpHeader object. This internally allocates a
			/// WINDIVERT_IPHDR object which is supplied internally to the unmanaged side of
			/// send/recv methods.
			/// </summary>
			DivertIpHeader();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~DivertIpHeader();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!DivertIpHeader();

			property System::Byte HeaderLength
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property System::Byte Version
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property System::Byte TOS
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property uint16_t Length
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property System::Byte Id
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property uint16_t FragOff
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t MF
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t DF
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint16_t Reserved
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property System::Byte TTL
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property System::Byte Protocol
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property uint16_t Checksum
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property System::Net::IPAddress^ SourceAddress
			{
				System::Net::IPAddress^ get();
				void set(System::Net::IPAddress^ value);
			}

			property System::Net::IPAddress^ DestinationAddress
			{
				System::Net::IPAddress^ get();
				void set(System::Net::IPAddress^ value);
			}

		internal:

			/// <summary>
			/// Allow internal construction with the supplied unmanaged ip header.
			/// </summary>
			/// <param name="address">
			/// Unmanaged PWINDIVERT_IPHDR to construct this wrapper around.
			/// </param>
			DivertIpHeader(PWINDIVERT_IPHDR ipheader);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_IPHDR object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_IPHDR member.
			/// </returns>
			PWINDIVERT_IPHDR GetUnmanagedIpHeader();

		private:

			/// <summary>
			/// The source address for the packet that this header belongs to. On the unmanaged
			/// side, the address bytes are stored in network order. Before being sent to the user
			/// of course, these values are converted to host order, then back to network order
			/// if/when the user accesses the setter.
			/// </summary>
			System::Net::IPAddress^ m_sourceAddress;

			/// <summary>
			/// The destination address for the packet that this header belongs to. On the unmanaged
			/// side, the address bytes are stored in network order. Before being sent to the user
			/// of course, these values are converted to host order, then back to network order
			/// if/when the user accesses the setter.
			/// </summary>
			System::Net::IPAddress^ m_destinationAddress;

			/// <summary>
			/// In order to only recreate the System::Net::IPAddress object when the address on the
			/// unmanaged side has actually changed, the current value on the unmanaged side is
			/// stored and checked whenever the Source/Destination addresses are accessed through
			/// the getter/setter methods. On a get, if the address is no longer the same, a managed
			/// IPAddress object is reconstructed to reflect the current value.
			/// </summary>
			UINT32 m_lastDstAddr = 0;

			/// <summary>
			/// In order to only recreate the System::Net::IPAddress object when the address on the
			/// unmanaged side has actually changed, the current value on the unmanaged side is
			/// stored and checked whenever the Source/Destination addresses are accessed through
			/// the getter/setter methods. On a get, if the address is no longer the same, a managed
			/// IPAddress object is reconstructed to reflect the current value.
			/// </summary>
			UINT32 m_lastSrcAddr = 0;

			/// <summary>
			/// Privately held PWINDIVERT_IPHDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_IPHDR m_ipHeader;		

		};

	} /* namespace Net */
} /* namespace Divert */
