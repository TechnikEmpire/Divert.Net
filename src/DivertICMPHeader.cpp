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

#include "DivertICMPHeader.hpp"

namespace Divert
{
	namespace Net
	{

		ICMPHeader::ICMPHeader()
		{
			
		}

		ICMPHeader::~ICMPHeader()
		{
			this->!ICMPHeader();
		}

		ICMPHeader::!ICMPHeader()
		{
			if (m_icmpHeader != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_icmpHeader = nullptr;
			}
		}

		ICMPHeader::ICMPHeader(PWINDIVERT_ICMPHDR icmpHeader)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(icmpHeader != nullptr, u8"In ICMPHeader::ICMPHeader(PWINDIVERT_ICMPHDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			m_icmpHeader = icmpHeader;
		}

		System::Byte ICMPHeader::Type::get()
		{
			if (m_icmpHeader != nullptr)
			{
				return m_icmpHeader->Type;
			}

			return 0;
		}

		void ICMPHeader::Type::set(System::Byte value)
		{
			if (m_icmpHeader != nullptr)
			{
				m_icmpHeader->Type = value;
			}
		}

		System::Byte ICMPHeader::Code::get()
		{
			if (m_icmpHeader != nullptr)
			{
				return m_icmpHeader->Code;
			}

			return 0;
		}

		void ICMPHeader::Code::set(System::Byte value)
		{
			if (m_icmpHeader != nullptr)
			{
				m_icmpHeader->Code = value;
			}
		}

		uint16_t ICMPHeader::Checksum::get()
		{
			if (m_icmpHeader != nullptr)
			{
				return m_icmpHeader->Checksum;
			}

			return 0;
		}

		void ICMPHeader::Checksum::set(uint16_t value)
		{
			if (m_icmpHeader != nullptr)
			{
				m_icmpHeader->Checksum = value;
			}
		}

		uint32_t ICMPHeader::Body::get()
		{
			if (m_icmpHeader != nullptr)
			{
				return m_icmpHeader->Body;
			}

			return 0;
		}

		void ICMPHeader::Body::set(uint32_t value)
		{
			if (m_icmpHeader != nullptr)
			{
				m_icmpHeader->Body = value;
			}
		}

		PWINDIVERT_ICMPHDR ICMPHeader::UnmanagedHeader::get()
		{
			return m_icmpHeader;
		}

		void ICMPHeader::UnmanagedHeader::set(PWINDIVERT_ICMPHDR value)
		{
			m_icmpHeader = value;
		}

	} /* namespace Net */
} /* namespace Divert */