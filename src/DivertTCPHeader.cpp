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

#include "DivertTCPHeader.hpp"

namespace Divert
{
	namespace Net
	{

		TCPHeader::TCPHeader()
		{

		}

		TCPHeader::~TCPHeader()
		{
			this->!TCPHeader();
		}

		TCPHeader::!TCPHeader()
		{
			if (m_tcpHeader != nullptr)
			{
				// This pointer is provided by WinDivert and is not ours to manage.
				m_tcpHeader = nullptr;
			}

			if (m_tcpv4Table != nullptr)
			{
				// This object is exclusively owned by this object and therefore needs to be freed here.
				free(m_tcpv4Table);
			}

			if (m_tcpv6Table != nullptr)
			{
				// This object is exclusively owned by this object and therefore needs to be freed here.
				free(m_tcpv6Table);
			}
		}

		TCPHeader::TCPHeader(PWINDIVERT_TCPHDR tcpHeader)
		{
			#ifndef NDEBUG
			System::Diagnostics::Debug::Assert(tcpHeader != nullptr, u8"In TCPHeader::TCPHeader(PWINDIVERT_TCPHDR) - nullptr provided to constructor expecting non-null pointer argument.");
			#endif

			m_tcpHeader = tcpHeader;
		}

		uint16_t TCPHeader::SourcePort::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(m_tcpHeader->SrcPort));
			}

			return 0;
		}

		void TCPHeader::SourcePort::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->SrcPort = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
			}
		}

		uint16_t TCPHeader::DestinationPort::get()
		{
			if (m_tcpHeader != nullptr)
			{				
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(m_tcpHeader->DstPort));
			}

			return 0;
		}

		void TCPHeader::DestinationPort::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->DstPort = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(value));
			}
		}

		uint32_t TCPHeader::SequenceNumber::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint32_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->SeqNum)));				
			}

			return 0;
		}

		void TCPHeader::SequenceNumber::set(uint32_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->SeqNum = static_cast<uint32_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint32_t TCPHeader::AcknowledgmentNumber::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint32_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->AckNum)));
			}

			return 0;
		}

		void TCPHeader::AcknowledgmentNumber::set(uint32_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->AckNum = static_cast<uint32_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Reserved1::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint32_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Reserved1)));
			}

			return 0;
		}

		void TCPHeader::Reserved1::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Reserved1 = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::HeaderLength::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->HdrLength)));
			}

			return 0;
		}

		void TCPHeader::HeaderLength::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->HdrLength = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Fin::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Fin)));			
			}

			return 0;
		}

		void TCPHeader::Fin::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Fin = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Syn::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Syn)));				
			}

			return 0;
		}

		void TCPHeader::Syn::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Syn = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Rst::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Rst)));				
			}

			return 0;
		}

		void TCPHeader::Rst::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Rst = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Psh::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Psh)));				
			}

			return 0;
		}

		void TCPHeader::Psh::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Psh = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Ack::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Ack)));
			}

			return 0;
		}

		void TCPHeader::Ack::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Ack = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Urg::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Urg)));				
			}

			return 0;
		}

		void TCPHeader::Urg::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Urg = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Reserved2::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Reserved2)));
			}

			return 0;
		}

		void TCPHeader::Reserved2::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Reserved2 = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::WindowSize::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Window)));				
			}

			return 0;
		}

		void TCPHeader::WindowSize::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Window = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::Checksum::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->Checksum)));				
			}

			return 0;
		}

		void TCPHeader::Checksum::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->Checksum = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		uint16_t TCPHeader::UrgentPointer::get()
		{
			if (m_tcpHeader != nullptr)
			{
				return static_cast<uint16_t>(System::Net::IPAddress::NetworkToHostOrder(static_cast<long long>(m_tcpHeader->UrgPtr)));				
			}

			return 0;
		}

		void TCPHeader::UrgentPointer::set(uint16_t value)
		{
			if (m_tcpHeader != nullptr)
			{
				m_tcpHeader->UrgPtr = static_cast<uint16_t>(System::Net::IPAddress::HostToNetworkOrder(static_cast<long long>(value)));
			}
		}

		bool TCPHeader::Valid::get()
		{
			return UnmanagedHeader != nullptr;
		}

		PWINDIVERT_TCPHDR TCPHeader::UnmanagedHeader::get()
		{
			return m_tcpHeader;
		}

		void TCPHeader::UnmanagedHeader::set(PWINDIVERT_TCPHDR value)
		{
			m_tcpHeader = value;
		}

		PMIB_TCPTABLE2 TCPHeader::UnmanagedTcpV4Table::get()
		{
			return m_tcpv4Table;
		}

		void TCPHeader::UnmanagedTcpV4Table::set(PMIB_TCPTABLE2 value)
		{
			m_tcpv4Table = value;
		}

		PMIB_TCP6TABLE2 TCPHeader::UnmanagedTcpV6Table::get()
		{
			return m_tcpv6Table;
		}

		void TCPHeader::UnmanagedTcpV6Table::set(PMIB_TCP6TABLE2 value)
		{
			m_tcpv6Table = value;
		}

	} /* namespace Net */
} /* namespace Divert */
