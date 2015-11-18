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

#include "DivertUDPHeader.hpp"

namespace Divert
{
	namespace Net
	{

		UDPHeader::UDPHeader()
		{

		}

		UDPHeader::~UDPHeader()
		{
			this->!UDPHeader();
		}

		UDPHeader::!UDPHeader()
		{
			if (m_udpHeader != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_udpHeader = nullptr;
			}
		}

		UDPHeader::UDPHeader(PWINDIVERT_UDPHDR udpHeader)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(m_udpHeader != nullptr, u8"In UDPHeader::UDPHeader(PWINDIVERT_UDPHDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			m_udpHeader = udpHeader;

		}

		uint16_t UDPHeader::SourcePort::get()
		{
			if (m_udpHeader != nullptr)
			{
				System::Net::IPAddress::NetworkToHostOrder(m_udpHeader->SrcPort);
			}

			return 0;
		}

		void UDPHeader::SourcePort::set(uint16_t value)
		{
			if (m_udpHeader != nullptr)
			{
				m_udpHeader->SrcPort = System::Net::IPAddress::HostToNetworkOrder(value);
			}
		}

		uint16_t UDPHeader::DestinationPort::get()
		{
			if (m_udpHeader != nullptr)
			{
				System::Net::IPAddress::NetworkToHostOrder(m_udpHeader->DstPort);
			}

			return 0;
		}

		void UDPHeader::DestinationPort::set(uint16_t value)
		{
			if (m_udpHeader != nullptr)
			{
				m_udpHeader->DstPort = System::Net::IPAddress::HostToNetworkOrder(value);
			}
		}

		uint16_t UDPHeader::Length::get()
		{
			if (m_udpHeader != nullptr)
			{
				return m_udpHeader->Length;
			}

			return 0;
		}

		void UDPHeader::Length::set(uint16_t value)
		{
			if (m_udpHeader != nullptr)
			{
				m_udpHeader->Length = value;
			}
		}

		uint16_t UDPHeader::Checksum::get()
		{
			if (m_udpHeader != nullptr)
			{
				return m_udpHeader->Checksum;
			}

			return 0;
		}

		void UDPHeader::Checksum::set(uint16_t value)
		{
			if (m_udpHeader != nullptr)
			{
				m_udpHeader->Checksum = value;
			}
		}

		PWINDIVERT_UDPHDR UDPHeader::GetUnmanagedUDPHeader()
		{
			return m_udpHeader;
		}

	} /* namespace Net */
} /* namespace Divert */