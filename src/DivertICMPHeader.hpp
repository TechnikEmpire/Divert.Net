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
		/// Represents the ICMP header for an intercepted ICMP packet.
		/// 
		/// When a packet is intercepted and the headers are parsed, the packet may be an ICMP
		/// packet. If such is the case, this structure will be populated with all of the
		/// information in the ICMP packet header.
		/// 
		/// More information here:
		/// http://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#ICMP_segment_structure
		/// </summary>
		public ref class ICMPHeader
		{

		public:

			/// <summary>
			/// Default constructor.
			/// </summary>
			ICMPHeader();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~ICMPHeader();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!ICMPHeader();

			property System::Byte Type
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property System::Byte Code
			{
				System::Byte get();
				void set(System::Byte value);
			}

			property uint16_t Checksum
			{
				uint16_t get();
				void set(uint16_t value);
			}

			property uint32_t Body
			{
				uint32_t get();
				void set(uint32_t value);
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
			/// Allow internal construction with the supplied unmanaged icmp header.
			/// </summary>
			/// <param name="icmpHeader">
			/// Unmanaged PWINDIVERT_ICMPHDR to construct this wrapper around.
			/// </param>
			ICMPHeader(PWINDIVERT_ICMPHDR icmpHeader);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_ICMPHDR object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_ICMPHDR member.
			/// </returns>
			property PWINDIVERT_ICMPHDR UnmanagedHeader
			{
				PWINDIVERT_ICMPHDR get();
				void set(PWINDIVERT_ICMPHDR value);
			}

		private:

			/// <summary>
			/// Privately held PWINDIVERT_ICMPHDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_ICMPHDR m_icmpHeader = nullptr;

		};

	} /* namespace Net */
} /* namespace Divert */
