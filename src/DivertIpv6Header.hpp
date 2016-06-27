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
		/// Represents the IP header for an intercepted IPV6 packet.
		/// 
		/// When a packet is intercepted and the headers are parsed, the packet may be an IPV6
		/// packet. If such is the case, this structure will be populated with all of the
		/// information in the IPV6 packet header, such as the source and destination addresses.
		/// 
		/// More information here: https://en.wikipedia.org/wiki/IPv6#Packet_format
		/// </summary>
		public ref class IPv6Header
		{

		public:

			/// <summary>
			/// Default constructor for the IpHeader object. This internally allocates a
			/// WINDIVERT_IPHDR object which is supplied internally to the unmanaged side of
			/// send/recv methods.
			/// </summary>
			IPv6Header();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~IPv6Header();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!IPv6Header();

			property uint16_t Length
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property System::Byte Version
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property uint32_t TrafficClass
			{
				uint32_t get();
				void set(uint32_t value);
			}

			property uint32_t FlowLabel
			{
				uint32_t get();
				void set(uint32_t value);
			}

			property System::Byte NextHeader
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property System::Byte HopLimit
			{
				System::Byte get();
				void set(System::Byte value);
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
			/// Allow internal construction with the supplied unmanaged ip header.
			/// </summary>
			/// <param name="ipv6Header">
			/// Unmanaged PWINDIVERT_IPV6HDR to construct this wrapper around.
			/// </param>
			IPv6Header(PWINDIVERT_IPV6HDR ipv6Header);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_IPV6HDR object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_IPV6HDR member.
			/// </returns>
			property PWINDIVERT_IPV6HDR UnmanagedHeader
			{
				PWINDIVERT_IPV6HDR get();
				void set(PWINDIVERT_IPV6HDR value);
			}

		private:

			/// <summary>
			/// The source address for the packet that this header belongs to. 
			/// </summary>
			System::Net::IPAddress^ m_sourceAddress;

			/// <summary>
			/// The destination address for the packet that this header belongs to.
			/// </summary>
			System::Net::IPAddress^ m_destinationAddress;

			/// <summary>
			/// In order to only recreate the System::Net::IPAddress object when the address on the
			/// unmanaged side has actually changed, the current value on the unmanaged side is
			/// stored and checked whenever the Source/Destination addresses are accessed through
			/// the getter/setter methods. On a get, if the address is no longer the same, a managed
			/// IPAddress object is reconstructed to reflect the current value.
			/// </summary>
			UINT32* m_lastDstAddr = nullptr;

			/// <summary>
			/// In order to only recreate the System::Net::IPAddress object when the address on the
			/// unmanaged side has actually changed, the current value on the unmanaged side is
			/// stored and checked whenever the Source/Destination addresses are accessed through
			/// the getter/setter methods. On a get, if the address is no longer the same, a managed
			/// IPAddress object is reconstructed to reflect the current value.
			/// </summary>
			UINT32* m_lastSrcAddr = nullptr;

			/// <summary>
			/// Privately held PWINDIVERT_IPV6HDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_IPV6HDR m_ipv6Header = nullptr;

			/// <summary>
			/// There's some special initialization required, regardless of constructor. Rather than
			/// duplicate the code, wrap it up in an init method and call it in all constructors.
			/// 
			/// This is primarily for ensuring that the m_lastXAddr members are properly allocated
			/// so we're not doing any illegal access in other members.
			/// </summary>
			void Init();

		};

	} /* namespace Net */
} /* namespace Divert */
