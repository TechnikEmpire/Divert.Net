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

#include "DivertICMPv6Header.hpp"

namespace Divert
{
	namespace Net
	{

		ICMPv6Header::ICMPv6Header()
		{

		}

		ICMPv6Header::~ICMPv6Header()
		{
			this->!ICMPv6Header();
		}

		ICMPv6Header::!ICMPv6Header()
		{
			if (m_icmpv6Header != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_icmpv6Header = nullptr;
			}
		}

		ICMPv6Header::ICMPv6Header(PWINDIVERT_ICMPV6HDR icmpv6Header)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(icmpv6Header != nullptr, u8"In ICMPv6Header::ICMPv6Header(PWINDIVERT_ICMPV6HDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			m_icmpv6Header = icmpv6Header;
		}

		System::Byte ICMPv6Header::Type::get()
		{
			if (m_icmpv6Header != nullptr)
			{
				return m_icmpv6Header->Type;
			}

			return 0;
		}

		void ICMPv6Header::Type::set(System::Byte value)
		{
			if (m_icmpv6Header != nullptr)
			{
				m_icmpv6Header->Type = value;
			}
		}

		System::Byte ICMPv6Header::Code::get()
		{
			if (m_icmpv6Header != nullptr)
			{
				return m_icmpv6Header->Code;
			}

			return 0;
		}

		void ICMPv6Header::Code::set(System::Byte value)
		{
			if (m_icmpv6Header != nullptr)
			{
				m_icmpv6Header->Code = value;
			}
		}

		uint16_t ICMPv6Header::Checksum::get()
		{
			if (m_icmpv6Header != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(m_icmpv6Header->Checksum));				
			}

			return 0;
		}

		void ICMPv6Header::Checksum::set(uint16_t value)
		{
			if (m_icmpv6Header != nullptr)
			{
				m_icmpv6Header->Checksum = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
			}
		}

		uint32_t ICMPv6Header::Body::get()
		{
			if (m_icmpv6Header != nullptr)
			{
				return static_cast<uint32_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_icmpv6Header->Body)));				
			}

			return 0;
		}

		void ICMPv6Header::Body::set(uint32_t value)
		{
			if (m_icmpv6Header != nullptr)
			{
				m_icmpv6Header->Body = static_cast<uint32_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		bool ICMPv6Header::Valid::get()
		{
			return UnmanagedHeader != nullptr;
		}

		PWINDIVERT_ICMPV6HDR ICMPv6Header::UnmanagedHeader::get()
		{
			return m_icmpv6Header;
		}

		void ICMPv6Header::UnmanagedHeader::set(PWINDIVERT_ICMPV6HDR value)
		{
			m_icmpv6Header = value;
		}

	} /* namespace Net */
} /* namespace Divert */
