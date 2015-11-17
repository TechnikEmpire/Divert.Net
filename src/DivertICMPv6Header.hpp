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
		/// Represents the ICMPv6 header for an intercepted ICMPv6 packet.
		/// 
		/// When a packet is intercepted and the headers are parsed, the packet may be an ICMPv6
		/// packet. If such is the case, this structure will be populated with all of the
		/// information in the ICMPv6 packet header.
		/// </summary>
		ref class ICMPv6Header
		{

		public:

			/// <summary>
			/// Default constructor. 
			/// </summary>
			ICMPv6Header();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~ICMPv6Header();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!ICMPv6Header();

		internal:

			/// <summary>
			/// Allow internal construction with the supplied unmanaged icmpv6 header.
			/// </summary>
			/// <param name="address">
			/// Unmanaged PWINDIVERT_ICMPV6HDR to construct this wrapper around.
			/// </param>
			ICMPv6Header(PWINDIVERT_ICMPV6HDR icmpv6Header);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_ICMPV6HDR object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_ICMPV6HDR member.
			/// </returns>
			PWINDIVERT_ICMPV6HDR GetUnmanagedICMPv6Header();

		private:

			/// Privately held PWINDIVERT_ICMPV6HDR member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_ICMPV6HDR m_icmpv6Header = nullptr;

		};

	} /* namespace Net */
} /* namespace Divert */